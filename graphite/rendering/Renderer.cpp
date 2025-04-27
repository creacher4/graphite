#include "Renderer.h"
#include "managers/DeviceManager.h"
#include "ShaderUtils.h"
#include <stdexcept>
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "Vertex.h"
#include "ConstantBuffers.h"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

Renderer::~Renderer()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::Init(DeviceManager *deviceManager, HWND hwnd, UINT width, UINT height)
{
    m_DeviceManager = deviceManager;
    auto *device = deviceManager->GetDevice();
    auto *context = deviceManager->GetContext();

    HRESULT hr;

    // rasterizer state
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;
    hr = device->CreateRasterizerState(
        &rasterDesc,
        m_rasterizerStateDefault.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create rasterizer state.");
    }

    // depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
    hr = device->CreateDepthStencilState(
        &depthDesc,
        m_depthStencilStateDefault.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create depth stencil state.");
    }

    // sampler state
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    device->CreateSamplerState(
        &samplerDesc,
        m_samplerStateDefault.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create sampler state.");
    }

    if (!m_GBuffer.Init(device, width, height))
    {
        throw std::runtime_error("Failed to initialize GBuffer.");
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

    {
        // imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device, context);
    }

    {
        // compile geometry vertex shader
        Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
        if (!CompileShaderFromFile(L"shaders/GeometryVS.hlsl", "main", "vs_5_0", vsBlob))
        {
            throw std::runtime_error("Failed to compile vertex shader.");
        }
        HRESULT hr = device->CreateVertexShader(
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            nullptr, m_vsGeometry.GetAddressOf());
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create vertex shader.");
        }

        // compile geometry pixel shader
        Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
        if (!CompileShaderFromFile(L"shaders/GeometryPS.hlsl", "main", "ps_5_0", psBlob))
        {
            throw std::runtime_error("Failed to compile pixel shader.");
        }
        hr = device->CreatePixelShader(
            psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
            nullptr, m_psGeometry.GetAddressOf());
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create pixel shader.");
        }

        // create input layout
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}};
        hr = device->CreateInputLayout(
            layoutDesc, ARRAYSIZE(layoutDesc),
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            m_inputLayout.GetAddressOf());
        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create input layout.");
        }

        {
            // create per-frame cb
            D3D11_BUFFER_DESC cbDesc = {};
            cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbDesc.ByteWidth = sizeof(PerFrameData);
            cbDesc.Usage = D3D11_USAGE_DYNAMIC;
            cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerFrame.GetAddressOf());
            if (FAILED(hr))
            {
                throw std::runtime_error("Failed to create per-frame constant buffer.");
            }
        }
        {
            // create per-object cb
            D3D11_BUFFER_DESC cbDesc = {};
            cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbDesc.ByteWidth = sizeof(PerObjectData);
            cbDesc.Usage = D3D11_USAGE_DYNAMIC;
            cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, m_cbPerObject.GetAddressOf());
            if (FAILED(hr))
            {
                throw std::runtime_error("Failed to create per-object constant buffer.");
            }
        }
    }
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
    auto *context = m_DeviceManager->GetContext();
    if (!m_GBuffer.Init(device, width, height))
    {
        throw std::runtime_error("Failed to reinitialize GBuffer on resize.");
    }
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

    context->RSSetState(m_rasterizerStateDefault.Get());
    context->OMSetDepthStencilState(m_depthStencilStateDefault.Get(), 1);

    // bind pipeline state
    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vsGeometry.Get(), nullptr, 0);
    context->PSSetShader(m_psGeometry.Get(), nullptr, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // bind constant buffers
    context->VSSetConstantBuffers(0, 1, m_cbPerFrame.GetAddressOf());
    context->VSSetConstantBuffers(1, 1, m_cbPerObject.GetAddressOf());

    // loop through entities and draw
    auto view = registry.View<TransformComponent, RenderableComponent>();
    // OutputDebugStringA("[GeometryPass] Checking for entities to draw...\n");
    for (auto [ent, transform, meshComp] : view.each())
    {
        PerObjectData pod{};
        glm::mat4 world(1.0f);
        world = glm::translate(world, transform.position);
        world = glm::rotate(world, transform.rotation.y, glm::vec3{0, 1, 0});
        world = glm::rotate(world, transform.rotation.x, glm::vec3{1, 0, 0});
        world = glm::rotate(world, transform.rotation.z, glm::vec3{0, 0, 1});
        world = glm::scale(world, glm::vec3{2.0f});
        world = glm::scale(world, transform.scale);
        pod.worldMatrix = world;

        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(context->Map(m_cbPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            *reinterpret_cast<PerObjectData *>(mapped.pData) = pod;
            context->Unmap(m_cbPerObject.Get(), 0);
        }

        // fetch mesh
        // OutputDebugStringA("[GeometryPass] Found Entity. PrimitiveID:\n");
        // OutputDebugStringA(meshComp.primitiveID.c_str());
        // OutputDebugStringA("\n");
        auto *mesh = assets.GetMesh(meshComp.meshID);
        auto *material = assets.GetMaterial(meshComp.materialID);
        if (!mesh || !material)
        {
            OutputDebugStringA("[GeometryPass][ERROR] Mesh or material not found!\n");
            continue;
        }
        if (!mesh->vertexBuffer || !mesh->indexBuffer || mesh->indexCount == 0)
        {
            OutputDebugStringA("[GeometryPass][ERROR] Mesh buffers not initialized!\n");
            continue;
        }
        // OutputDebugStringA("[GeometryPass] Found mesh.\n");

        // bind textures
        ID3D11ShaderResourceView *srvs[3] = {nullptr, nullptr, nullptr};
        if (auto *t = assets.GetTexture(material->albedo))
            srvs[0] = t->srv.Get();
        if (auto *t = assets.GetTexture(material->normal))
            srvs[1] = t->srv.Get();
        if (auto *t = assets.GetTexture(material->orm))
            srvs[2] = t->srv.Get();
        context->PSSetShaderResources(0, 3, srvs);
        context->PSSetSamplers(0, 1, m_samplerStateDefault.GetAddressOf());

        // bind vertex/index buffers
        UINT stride = sizeof(Vertex), offset = 0;
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw call
        // OutputDebugStringA("[GeometryPass] Attempting draw call...\n");
        context->DrawIndexed(mesh->indexCount, 0, 0);
        // OutputDebugStringA("[GeometryPass] Draw call complete.\n");

        // ui stats
        m_drawCallCount++;
        m_triangleCount += mesh->indexCount / 3;

        // unbind textures
        ID3D11ShaderResourceView *nulls[3] = {nullptr, nullptr, nullptr};
        context->PSSetShaderResources(0, 3, nulls);
    }
}

void Renderer::EndFrame(StatsSystem *stats)
{
    // rebind back buffer
    auto *context = m_DeviceManager->GetContext();
    auto *backRTV = m_DeviceManager->GetBackBufferRTV();
    context->OMSetRenderTargets(1, &backRTV, nullptr);

    // clear back buffer
    // completely forgot about this, lol
    // this is why the window was black and multiple instances of the imgui window were appearing
    const float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    context->ClearRenderTargetView(backRTV, clearColor);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

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
        ImGui::Image(texID, ImGui::GetContentRegionAvail());
    }
    ImGui::End();

    // STATS viewer
    if (stats)
        stats->DrawImGui();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_DeviceManager->GetSwapChain()->Present(1, 0);
}