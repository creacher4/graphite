#include "Renderer.h"
#include <stdexcept>

void Renderer::Init(HWND hwnd)
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

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());

    ID3D11RenderTargetView *rtv = m_BackBufferRTV.Get();
    m_Context->OMSetRenderTargets(1, &rtv, nullptr);
}

void Renderer::BeginFrame()
{
    float clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f}; // some dark gray color
    m_Context->ClearRenderTargetView(m_BackBufferRTV.Get(), clearColor);
}

void Renderer::EndFrame()
{
    m_SwapChain->Present(1, 0);
}