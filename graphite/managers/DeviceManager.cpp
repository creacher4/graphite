#include "DeviceManager.h"
#include "utils/Logger.h"

void DeviceManager::InitDevice(HWND hwnd, UINT width, UINT height)
{
    m_Hwnd = hwnd;
    m_Width = width;
    m_Height = height;

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 2; // or 3 for triple buffering
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

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
        LOG_CRITICAL("Failed to create D3D11 device and swap chain. HRESULT: {}", hr);
        throw std::runtime_error("Failed to create D3D11 device and swap chain");
    }
    LOG_INFO("Created D3D11 device and swap chain");

    // create back buffer render target view
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        LOG_ERROR("Failed to get back buffer. HRESULT: {}", hr);
    LOG_INFO("Got back buffer");
    hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());
    if (FAILED(hr))
        LOG_ERROR("Failed to create render target view. HRESULT: {}", hr);
    LOG_INFO("Created render target view");

    LOG_INFO("D3D11 device initialization completed");
}

void DeviceManager::ResizeSwapChain(UINT width, UINT height)
{
    m_Width = width;
    m_Height = height;

    m_Context->OMSetRenderTargets(0, nullptr, nullptr);

    m_BackBufferRTV.Reset();
    HRESULT hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
        LOG_ERROR("Failed to resize swap chain. HRESULT: {}", hr);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_SwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        LOG_ERROR("Failed to get back buffer after resize. HRESULT: {}", hr);

    hr = m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_BackBufferRTV.GetAddressOf());
    if (FAILED(hr))
        LOG_ERROR("Failed to create render target view after resize. HRESULT: {}", hr);

    LOG_INFO("Resized swap chain and created new render target view");
}

void DeviceManager::Shutdown()
{
    m_BackBufferRTV.Reset();
    m_SwapChain.Reset();
    m_Context.Reset();
    m_Device.Reset();
    LOG_INFO("D3D11 device and swap chain shutdown completed");
}