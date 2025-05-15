#include "Renderer.h"
#include "managers/DeviceManager.h"
#include <stdexcept>
#include "ecs/RenderableComponent.h"
#include "RendererSetup.h"
#include "Material.h"
#include "Vertex.h"
#include "utils/Logger.h"
#include "ConstantBuffers.h"
#include <glm/gtc/matrix_transform.hpp>
#include "utils/ImGuiConfig.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

Renderer::~Renderer()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::InitImGui(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *context)
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

void Renderer::InitConstantBuffers(ID3D11Device *device)
{
    HRESULT hr;

    // create per-frame cb
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.ByteWidth = sizeof(PerFrameData);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerFrame.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_CRITICAL("Failed to create per-frame constant buffer");
        throw std::runtime_error("Failed to create per-frame constant buffer");
    }
    LOG_INFO("Created per-frame constant buffer");

    // create per-object cb
    // only the byte width changes, so we can reuse the same buffer description
    cbDesc.ByteWidth = sizeof(PerObjectData);
    hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerObject.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_CRITICAL("Failed to create per-object constant buffer");
        throw std::runtime_error("Failed to create per-object constant buffer");
    }
    LOG_INFO("Created per-object constant buffer");
}

void Renderer::Init(DeviceManager *deviceManager, HWND hwnd, UINT width, UINT height)
{
    m_DeviceManager = deviceManager;
    auto *device = deviceManager->GetDevice();
    auto *context = deviceManager->GetContext();

    RendererSetup::InitStateObjects(
        device,
        m_rasterizerStateDefault,
        m_rasterizerStateWire_NoCull,
        m_depthStencilStateDefault,
        m_samplerStateDefault);

    InitImGui(hwnd, device, context);

    RendererSetup::InitGeometryShadersAndLayout(
        device,
        hwnd,
        m_vsGeometry,
        m_psGeometry,
        m_inputLayout);

    InitConstantBuffers(device);

    RendererSetup::InitLightingShaders(
        device,
        m_vsLighting,
        m_psLighting);

    {
        // create lighting cb
        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.ByteWidth = sizeof(DirectionalLightData);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, m_cbLight.GetAddressOf());
        if (FAILED(hr))
        {
            LOG_CRITICAL("Failed to create directional light constant buffer");
            throw std::runtime_error("Failed to create directional light constant buffer");
        }
        LOG_INFO("Created directional light constant buffer");
    }

    if (!m_GBuffer.Init(device, width, height))
    {
        LOG_CRITICAL("Failed to initialize GBuffer");
        throw std::runtime_error("Failed to initialize GBuffer");
    }
    LOG_INFO("Initialized GBuffer");
}

void Renderer::UpdatePerFrameConstants(const glm::mat4 &view, const glm::mat4 &proj)
{
    auto *context = m_DeviceManager->GetContext();
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(context->Map(m_cbPerFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        auto *data = reinterpret_cast<PerFrameData *>(mapped.pData);
        data->viewMatrix = view;
        data->projectionMatrix = proj;
        context->Unmap(m_cbPerFrame.Get(), 0);
    }
}

void Renderer::OnResize(UINT width, UINT height)
{
    // reinitialize just the GBuffer
    auto *device = m_DeviceManager->GetDevice();
    m_GBuffer.Resize(device, width, height);
}

void Renderer::BeginFrame()
{
    m_drawCallCount = 0;
    m_triangleCount = 0;

    auto *context = m_DeviceManager->GetContext();
    m_GBuffer.Bind(context);
    m_GBuffer.Clear(context);
}

void Renderer::GeometryPass(ECSRegistry &registry, AssetManager &assets)
{
    auto *context = m_DeviceManager->GetContext();
    SetPassState(context);

    // loop through entities and draw
    auto view = registry.View<RenderableComponent, TransformComponent>();
    for (auto [ent, rc, tc] : view.each())
    {
        // fetch model
        auto *model = assets.GetModel(rc.modelID);
        if (!model)
        {
            LOG_WARN("Skipping entity {} due to missing model", ent);
            continue;
        }

        // clamp index
        size_t index = std::min(rc.subMeshIndex, model->meshes.size() - 1);
        const auto &mesh = model->meshes[index];

        // fetch material
        auto *material = assets.GetMaterial(rc.materialID);
        if (!material)
        {
            LOG_WARN("Skipping entity {} due to missing material", ent);
            continue;
        }

        // if here, model and materials are valid
        UpdatePerObjectConstantBuffer(context, tc);
        BindMaterial(context, assets, material);
        DrawMesh(context, &mesh); // draw call
        UnbindMaterial(context);  // helpful unbind in case its used again

        // update stats
        m_drawCallCount++;
        m_triangleCount += mesh.indexCount / 3;
    }
}

void Renderer::LightingPass()
{
    auto *context = m_DeviceManager->GetContext();

    context->RSSetState(m_rasterizerStateDefault.Get());
    context->IASetInputLayout(nullptr);
    context->OMSetDepthStencilState(nullptr, 0);

    // bind gbuffer srvs
    ID3D11ShaderResourceView *srvs[4] = {
        m_GBuffer.GetAlbedoSRV(),
        m_GBuffer.GetNormalSRV(),
        m_GBuffer.GetOrmSRV(),
        m_GBuffer.GetDepthSRV()};
    context->PSSetShaderResources(0, 4, srvs);

    // bind lighting shaders
    context->VSSetShader(m_vsLighting.Get(), nullptr, 0);
    context->PSSetShader(m_psLighting.Get(), nullptr, 0);

    // bind lighting constant buffer
    context->PSSetConstantBuffers(0, 1, m_cbLight.GetAddressOf());
    context->PSSetSamplers(0, 1, m_samplerStateDefault.GetAddressOf());

    // draw fullscreen quad
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(3, 0); // assuming a fullscreen quad is set up in the vertex buffer (which it is)

    // unbind gbuffer srvs
    ID3D11ShaderResourceView *nulls[4] = {nullptr, nullptr, nullptr, nullptr};
    context->PSSetShaderResources(0, 4, nulls);
}

void Renderer::EndFrame(StatsSystem *stats)
{
    // rebind back buffer
    auto *context = m_DeviceManager->GetContext();
    auto *backRTV = m_DeviceManager->GetBackBufferRTV();
    context->OMSetRenderTargets(1, &backRTV, nullptr);

    // clear back buffer
    context->ClearRenderTargetView(backRTV, CC::BACKBUFFER_CLEAR.data());

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // debug ui toggles
    static bool cbAlbedo = true;
    static bool cbNormals = true;
    static bool cbAO = true;
    static bool cbRoughness = true;
    static bool cbMetallic = true;
    static bool cbFresnel = true;
    static bool cbRim = true;

    ImGui::Begin("Lighting Debug");
    ImGui::Checkbox("Albedo", &cbAlbedo);
    ImGui::Checkbox("Normals", &cbNormals);
    ImGui::Checkbox("AO", &cbAO);
    ImGui::Checkbox("Roughness", &cbRoughness);
    ImGui::Checkbox("Metallic", &cbMetallic);
    ImGui::Checkbox("Fresnel", &cbFresnel);
    ImGui::Checkbox("Rim", &cbRim);
    ImGui::End();

    // build and upload light data
    DirectionalLightData lightData{};
    // from the top
    lightData.dir = glm::normalize(glm::vec3{0.0f, 1.0f, 0.0f});
    lightData.color = glm::vec3{1.0f, 0.95f, 0.85f};
    lightData.useAlbedo = cbAlbedo ? 1 : 0;
    lightData.useNormals = cbNormals ? 1 : 0;
    lightData.useAO = cbAO ? 1 : 0;
    lightData.viewDir = stats->GetViewDir();
    lightData.useRoughness = cbRoughness ? 1 : 0;
    lightData.useMetallic = cbMetallic ? 1 : 0;
    lightData.useFresnel = cbFresnel ? 1 : 0;
    lightData.useRim = cbRim ? 1 : 0;
    context->UpdateSubresource(
        m_cbLight.Get(), 0, nullptr, &lightData, 0, 0);

    // lighting pass
    LightingPass();

    // ImGui_ImplDX11_NewFrame();
    // ImGui_ImplWin32_NewFrame();
    // ImGui::NewFrame();

    // GBUFFER viewer
    ImGui::Begin("GBuffer Viewer");
    static int channel = 0;
    ImGui::RadioButton("Albedo", &channel, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Normal", &channel, 1);
    ImGui::SameLine();
    ImGui::RadioButton("ORM", &channel, 2);
    ImGui::SameLine();
    ImGui::RadioButton("Depth", &channel, 3);

    ID3D11ShaderResourceView *srv = nullptr;
    switch (channel)
    {
    case 0:
        srv = m_GBuffer.GetAlbedoSRV();
        break;
    case 1:
        srv = m_GBuffer.GetNormalSRV();
        break;
    case 2:
        srv = m_GBuffer.GetOrmSRV();
        break;
    case 3:
        srv = m_GBuffer.GetDepthSRV();
        break;
    default:
        srv = m_GBuffer.GetAlbedoSRV();
        break;
    }

    if (srv)
    {
        ImTextureID texID = (ImTextureID)srv;
        auto size = ImGui::GetContentRegionAvail();
        ImGui::Image(
            texID,
            size,
            ImVec2(0.0f, 1.0f), // top-left in UV space
            ImVec2(1.0f, 0.0f)  // bottom-right in UV space
        );
    }
    ImGui::End();

    // STATS viewer
    if (stats)
        stats->DrawImGui();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_DeviceManager->GetSwapChain()->Present(1, 0);
}

void Renderer::SetPassState(ID3D11DeviceContext *context)
{
    // set rasterizer state
    context->RSSetState(m_useWire_NoCull ? m_rasterizerStateWire_NoCull.Get() : m_rasterizerStateDefault.Get());

    // set other common states
    context->OMSetDepthStencilState(m_depthStencilStateDefault.Get(), 1);
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vsGeometry.Get(), nullptr, 0);
    context->PSSetShader(m_psGeometry.Get(), nullptr, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // bind per-frame constant buffer
    context->VSSetConstantBuffers(0, 1, m_cbPerFrame.GetAddressOf());
    // bind default sampler state
    context->PSSetSamplers(0, 1, m_samplerStateDefault.GetAddressOf());
}

void Renderer::UpdatePerObjectConstantBuffer(ID3D11DeviceContext *context, const TransformComponent &transform)
{
    PerObjectData pod{};
    // apply transform in world space
    glm::mat4 world(1.0f);
    world = glm::translate(world, transform.position);

    // build world matrix: translate, then apply rotations in yxz order
    // yaw
    // pitch
    // roll
    // matches Camera and avoids unexpected twisting when combining rotations

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

    // bind the updated per-object constant buffer
    context->VSSetConstantBuffers(1, 1, m_cbPerObject.GetAddressOf());
}

void Renderer::BindMaterial(ID3D11DeviceContext *context, AssetManager &assets, const Material *material)
{
    ID3D11ShaderResourceView *srvs[3] = {nullptr, nullptr, nullptr};

    if (material)
    {
        auto *albedo = assets.GetTexture(material->albedo);
        if (albedo)
            srvs[0] = albedo->srv.Get();
        auto *normal = assets.GetTexture(material->normal);
        if (normal)
            srvs[1] = normal->srv.Get();
        auto *orm = assets.GetTexture(material->orm);
        if (orm)
            srvs[2] = orm->srv.Get();
    }

    context->PSSetShaderResources(0, 3, srvs);
    // sampler binding is done in SetPassState
}

void Renderer::DrawMesh(ID3D11DeviceContext *context, const AssetManager::MeshResource *mesh)
{
    // assumes mesh is valid and buffers are bound
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(mesh->indexCount, 0, 0);
}

void Renderer::UnbindMaterial(ID3D11DeviceContext *context)
{
    ID3D11ShaderResourceView *nulls[3] = {nullptr, nullptr, nullptr};
    context->PSSetShaderResources(0, 3, nulls);
}