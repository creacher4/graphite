#include "RenderSystem.h"
#include "core/ServiceLocator.h"
#include "core/SceneManager.h"
#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "ecs/ECSRegistry.h"
#include "ecs/RenderableComponent.h"
#include "ecs/TransformComponent.h"
#include "rendering/RendererSetup.h"
#include "rendering/Material.h"
#include "rendering/Vertex.h"
#include "utils/Logger.h"
#include "utils/ImGuiConfig.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <glm/gtc/matrix_transform.hpp>

RenderSystem::RenderSystem(SceneManager *sceneManager)
    : m_SceneManager(sceneManager)
{
}

RenderSystem::~RenderSystem()
{
    if (ImGui::GetCurrentContext())
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}

void RenderSystem::Init()
{
    LOG_INFO("Initializing render system...");
    if (!m_SceneManager)
        throw std::runtime_error("RenderSystem dependencies not set");

    auto &deviceManager = ServiceLocator::GetDeviceManager();
    auto *device = deviceManager.GetDevice();
    auto *context = deviceManager.GetContext();

    m_Width = deviceManager.GetWidth();
    m_Height = deviceManager.GetHeight();

    InitImGui(deviceManager.GetHWND(), device, context);
    InitStateObjectsAndShaders();
    InitConstantBuffers(device);

    if (!m_GBuffer.Init(device, m_Width, m_Height))
    {
        LOG_CRITICAL("Failed to initialize GBuffer");
        throw std::runtime_error("Failed to initialize GBuffer");
    }
    LOG_INFO("Render system initialized");
}

void RenderSystem::Update(float /*dt*/)
{
    UpdatePerFrameConstants();
    BeginFrame();
    GeometryPass();
    LightingPass();
    RenderImGui();

    ServiceLocator::GetDeviceManager().GetSwapChain()->Present(1, 0);
}

void RenderSystem::Shutdown()
{
    LOG_INFO("RenderSystem shutdown.");
}

void RenderSystem::OnResize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    auto *device = ServiceLocator::GetDeviceManager().GetDevice();
    m_GBuffer.Resize(device, width, height);
}

void RenderSystem::UpdateLightingData(const DirectionalLightData &data)
{
    auto *context = ServiceLocator::GetDeviceManager().GetContext();
    context->UpdateSubresource(m_cbLight.Get(), 0, nullptr, &data, 0, 0);
}

void RenderSystem::InitImGui(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGuiConfig::ConfigureStyle();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);
}

void RenderSystem::InitConstantBuffers(ID3D11Device *device)
{
    HRESULT hr;
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    cbDesc.ByteWidth = sizeof(PerFrameData);
    hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerFrame.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("Failed to create per-frame constant buffer");

    cbDesc.ByteWidth = sizeof(PerObjectData);
    hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerObject.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("Failed to create per-object constant buffer");

    cbDesc.ByteWidth = sizeof(DirectionalLightData);
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.CPUAccessFlags = 0;
    hr = device->CreateBuffer(&cbDesc, nullptr, m_cbLight.GetAddressOf());
    if (FAILED(hr)) throw std::runtime_error("Failed to create directional light constant buffer");
}

void RenderSystem::InitStateObjectsAndShaders()
{
    auto &deviceManager = ServiceLocator::GetDeviceManager();
    auto *device = deviceManager.GetDevice();
    RendererSetup::InitStateObjects(
        device,
        m_rasterizerStateDefault,
        m_rasterizerStateWire_NoCull,
        m_depthStencilStateDefault,
        m_samplerStateDefault);

    RendererSetup::InitGeometryShadersAndLayout(
        device,
        deviceManager.GetHWND(),
        m_vsGeometry,
        m_psGeometry,
        m_inputLayout);

    RendererSetup::InitLightingShaders(
        device,
        m_vsLighting,
        m_psLighting);
}

void RenderSystem::BeginFrame()
{
    m_drawCallCount = 0;
    m_triangleCount = 0;

    auto *context = ServiceLocator::GetDeviceManager().GetContext();
    m_GBuffer.Bind(context);
    m_GBuffer.Clear(context);
}

void RenderSystem::GeometryPass()
{
    auto &deviceManager = ServiceLocator::GetDeviceManager();
    auto &assetManager = ServiceLocator::GetAssetManager();
    auto &registry = m_SceneManager->GetRegistry();
    auto *context = deviceManager.GetContext();

    SetGeometryPassState(context);

    auto view = registry.View<RenderableComponent, TransformComponent>();
    for (auto [ent, rc, tc] : view.each())
    {
        auto *model = assetManager.GetModel(rc.modelID);
        if (!model) continue;

        size_t index = std::min(rc.subMeshIndex, model->meshes.size() - 1);
        const auto &mesh = model->meshes[index];

        auto *material = assetManager.GetMaterial(rc.materialID);
        if (!material) continue;

        UpdatePerObjectConstantBuffer(context, tc);
        BindMaterial(context, material);
        DrawMesh(context, &mesh);
        UnbindMaterial(context);

        m_drawCallCount++;
        m_triangleCount += mesh.indexCount / 3;
    }
}

void RenderSystem::LightingPass()
{
    auto &deviceManager = ServiceLocator::GetDeviceManager();
    auto *context = deviceManager.GetContext();
    auto *backRTV = deviceManager.GetBackBufferRTV();

    context->OMSetRenderTargets(1, &backRTV, nullptr);
    context->ClearRenderTargetView(backRTV, Config::ClearColors::BACKBUFFER_CLEAR.data());

    context->RSSetState(m_rasterizerStateDefault.Get());
    context->IASetInputLayout(nullptr);
    context->OMSetDepthStencilState(nullptr, 0);

    ID3D11ShaderResourceView *srvs[] = {
        m_GBuffer.GetAlbedoSRV(),
        m_GBuffer.GetNormalSRV(),
        m_GBuffer.GetOrmSRV(),
        m_GBuffer.GetDepthSRV()};
    context->PSSetShaderResources(0, _countof(srvs), srvs);

    context->VSSetShader(m_vsLighting.Get(), nullptr, 0);
    context->PSSetShader(m_psLighting.Get(), nullptr, 0);
    context->PSSetConstantBuffers(0, 1, m_cbLight.GetAddressOf());
    context->PSSetSamplers(0, 1, m_samplerStateDefault.GetAddressOf());

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(3, 0);

    ID3D11ShaderResourceView *nulls[_countof(srvs)] = {};
    context->PSSetShaderResources(0, _countof(nulls), nulls);
}

void RenderSystem::RenderImGui()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void RenderSystem::UpdatePerFrameConstants()
{
    auto *context = ServiceLocator::GetDeviceManager().GetContext();
    auto &camera = m_SceneManager->GetCamera();

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(m_cbPerFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        auto *data = reinterpret_cast<PerFrameData *>(mapped.pData);
        data->viewMatrix = camera.GetView();
        data->projectionMatrix = camera.GetProjection();
        context->Unmap(m_cbPerFrame.Get(), 0);
    }
}

void RenderSystem::SetGeometryPassState(ID3D11DeviceContext *context)
{
    context->RSSetState(m_useWire_NoCull ? m_rasterizerStateWire_NoCull.Get() : m_rasterizerStateDefault.Get());
    context->OMSetDepthStencilState(m_depthStencilStateDefault.Get(), 1);
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vsGeometry.Get(), nullptr, 0);
    context->PSSetShader(m_psGeometry.Get(), nullptr, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->VSSetConstantBuffers(0, 1, m_cbPerFrame.GetAddressOf());
    context->PSSetSamplers(0, 1, m_samplerStateDefault.GetAddressOf());
}

void RenderSystem::UpdatePerObjectConstantBuffer(ID3D11DeviceContext *context, const TransformComponent &transform)
{
    PerObjectData pod{};
    glm::mat4 world(1.0f);
    world = glm::translate(world, transform.position);
    world = glm::rotate(world, transform.rotation.y, glm::vec3{0, 1, 0});
    world = glm::rotate(world, transform.rotation.x, glm::vec3{1, 0, 0});
    world = glm::rotate(world, transform.rotation.z, glm::vec3{0, 0, 1});
    world = glm::scale(world, transform.scale);
    pod.worldMatrix = world;

    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(m_cbPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        *reinterpret_cast<PerObjectData *>(mapped.pData) = pod;
        context->Unmap(m_cbPerObject.Get(), 0);
    }
    context->VSSetConstantBuffers(1, 1, m_cbPerObject.GetAddressOf());
}

void RenderSystem::BindMaterial(ID3D11DeviceContext *context, const Material *material)
{
    auto &assetManager = ServiceLocator::GetAssetManager();
    ID3D11ShaderResourceView *srvs[3] = {};
    if (material)
    {
        auto *albedo = assetManager.GetTexture(material->albedo);
        if (albedo) srvs[0] = albedo->srv.Get();
        auto *normal = assetManager.GetTexture(material->normal);
        if (normal) srvs[1] = normal->srv.Get();
        auto *orm = assetManager.GetTexture(material->orm);
        if (orm) srvs[2] = orm->srv.Get();
    }
    context->PSSetShaderResources(0, 3, srvs);
}

void RenderSystem::DrawMesh(ID3D11DeviceContext *context, const AssetManager::MeshResource *mesh)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(mesh->indexCount, 0, 0);
}

void RenderSystem::UnbindMaterial(ID3D11DeviceContext *context)
{
    ID3D11ShaderResourceView *nulls[3] = {};
    context->PSSetShaderResources(0, 3, nulls);
}