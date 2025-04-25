#include "DeviceManager.h"

void DeviceManager::InitDevice(HWND hwnd, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createFlags = 0;
#if defined(_DEBUG)
    createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL levels[] = {D3D_FEATURE_LEVEL_11_0};

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createFlags,
        levels, _countof(levels),
        D3D11_SDK_VERSION,
        &scd,
        m_SwapChain.GetAddressOf(),
        m_Device.GetAddressOf(),
        &featureLevel,
        m_Context.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("DeviceManager::InitDevice failed");
    }

    // create back buffer render target view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        throw std::runtime_error("DeviceManager::GetBuffer failed");
    hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());
    if (FAILED(hr))
        throw std::runtime_error("DeviceManager::CreateRTV failed");

    OutputDebugStringA("DeviceManager::InitDevice succeeded\n");
}

void DeviceManager::ResizeSwapChain(UINT width, UINT height)
{
    m_BackBufferRTV.Reset();
    HRESULT hr = m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
        throw std::runtime_error("DeviceManager::ResizeBuffers failed");

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        throw std::runtime_error("DeviceManager::GetBuffer after resize failed");
    hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());
    if (FAILED(hr))
        throw std::runtime_error("DeviceManager::CreateRTV after resize failed");

    OutputDebugStringA("DeviceManager::ResizeSwapChain succeeded\n");
}

void DeviceManager::Shutdown()
{
    m_BackBufferRTV.Reset();
    m_SwapChain.Reset();
    m_Context.Reset();
    m_Device.Reset();
    OutputDebugStringA("DeviceManager::Shutdown completed\n");
}