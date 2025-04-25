#include "Renderer.h"
#include "ShaderUtils.h"
#include <stdexcept>
#include "ecs/TransformComponent.h"
#include "ecs/MeshComponent.h"
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

void Renderer::Init(HWND hwnd, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
#if _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0};

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        &scd,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        &featureLevel,
        m_Context.GetAddressOf());

    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create DX11 device and swap chain.");
    }

    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.FrontCounterClockwise = FALSE;
    rasterDesc.DepthClipEnable = TRUE;
    hr = m_Device->CreateRasterizerState(
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
    hr = m_Device->CreateDepthStencilState(
        &depthDesc,
        m_depthStencilStateDefault.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create depth stencil state.");
    }

    if (!m_GBuffer.Init(m_Device.Get(), width, height))
    {
        throw std::runtime_error("Failed to initialize GBuffer.");
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());

    {
        // imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(m_Device.Get(), m_Context.Get());
    }

    {
        // compile geometry vertex shader
        Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
        if (!CompileShaderFromFile(L"shaders/GeometryVS.hlsl", "main", "vs_5_0", vsBlob))
        {
            throw std::runtime_error("Failed to compile vertex shader.");
        }
        HRESULT hr = m_Device->CreateVertexShader(
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
        hr = m_Device->CreatePixelShader(
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
        };
        hr = m_Device->CreateInputLayout(
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
            HRESULT hr = m_Device->CreateBuffer(&cbDesc, nullptr, m_cbPerFrame.GetAddressOf());
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
            HRESULT hr = m_Device->CreateBuffer(&cbDesc, nullptr, m_cbPerObject.GetAddressOf());
            if (FAILED(hr))
            {
                throw std::runtime_error("Failed to create per-object constant buffer.");
            }
        }
    }
}

void Renderer::UpdatePerFrameConstants(const glm::mat4 &view, const glm::mat4 &proj)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(m_Context->Map(m_cbPerFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        auto *data = reinterpret_cast<PerFrameData *>(mapped.pData);
        data->viewMatrix = view; // transpose if needed
        data->projectionMatrix = proj;
        m_Context->Unmap(m_cbPerFrame.Get(), 0);
    }
}

void Renderer::BeginFrame()
{
    m_GBuffer.Bind(m_Context.Get());
    m_GBuffer.Clear(m_Context.Get());
}

void Renderer::GeometryPass(ECSRegistry &registry, ResourceManager &resources)
{
    m_Context->RSSetState(m_rasterizerStateDefault.Get());
    m_Context->OMSetDepthStencilState(m_depthStencilStateDefault.Get(), 1);

    // bind pipeline state
    m_Context->IASetInputLayout(m_inputLayout.Get());
    m_Context->VSSetShader(m_vsGeometry.Get(), nullptr, 0);
    m_Context->PSSetShader(m_psGeometry.Get(), nullptr, 0);
    m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // bind constant buffers
    m_Context->VSSetConstantBuffers(0, 1, m_cbPerFrame.GetAddressOf());
    m_Context->VSSetConstantBuffers(1, 1, m_cbPerObject.GetAddressOf());

    // loop through entities and draw
    auto view = registry.View<TransformComponent, MeshComponent>();
    OutputDebugStringA("[GeometryPass] Checking for entities to draw...\n");
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
        if (SUCCEEDED(m_Context->Map(m_cbPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        {
            *reinterpret_cast<PerObjectData *>(mapped.pData) = pod;
            m_Context->Unmap(m_cbPerObject.Get(), 0);
        }

        // fetch mesh
        OutputDebugStringA("[GeometryPass] Found Entity. PrimitiveID:\n");
        OutputDebugStringA(meshComp.primitiveID.c_str());
        OutputDebugStringA("\n");
        Mesh *mesh = resources.GetPrimitiveMesh(meshComp.primitiveID);
        if (!mesh)
        {
            OutputDebugStringA("[GeometryPass][ERROR] Mesh not found!\n");
            continue;
        }
        if (!mesh->vertexBuffer || !mesh->indexBuffer || mesh->indexCount == 0)
        {
            OutputDebugStringA("[GeometryPass][ERROR] Mesh buffers not initialized!\n");
            continue;
        }
        OutputDebugStringA("[GeometryPass] Found mesh.\n");

        // bind vertex/index buffers
        UINT stride = sizeof(Vertex), offset = 0;
        m_Context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        m_Context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw call
        OutputDebugStringA("[GeometryPass] Attempting draw call...\n");
        m_Context->DrawIndexed(mesh->indexCount, 0, 0);
        OutputDebugStringA("[GeometryPass] Draw call complete.\n");
    }
}

void Renderer::EndFrame()
{
    // rebind back buffer
    ID3D11RenderTargetView *backRTV = m_BackBufferRTV.Get();
    m_Context->OMSetRenderTargets(1, &backRTV, nullptr);

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

    ID3D11ShaderResourceView *srv = nullptr;
    if (channel == 0)
        srv = m_GBuffer.GetAlbedoSRV();
    else if (channel == 1)
        srv = m_GBuffer.GetNormalSRV();
    else if (channel == 2)
        srv = m_GBuffer.GetOrmSRV();

    if (srv)
    {
        ImTextureID texID = (ImTextureID)srv;
        ImGui::Image(texID, ImGui::GetContentRegionAvail());
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    m_SwapChain->Present(1, 0);
}