#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <Windows.h>
#include <stdexcept>

// manages the DirectX device and swap chain
class DeviceManager
{
public:
    DeviceManager() = default;
    ~DeviceManager() = default;

    void InitDevice(HWND hwnd, UINT width, UINT height);
    void ResizeSwapChain(UINT width, UINT height);

    ID3D11Device *GetDevice() const { return m_Device.Get(); }
    ID3D11DeviceContext *GetContext() const { return m_Context.Get(); }
    ID3D11RenderTargetView *GetBackBufferRTV() const { return m_BackBufferRTV.Get(); }
    IDXGISwapChain *GetSwapChain() const { return m_SwapChain.Get(); }
    HWND GetHWND() const { return m_Hwnd; }
    UINT GetWidth() const { return m_Width; }
    UINT GetHeight() const { return m_Height; }

    void Shutdown();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackBufferRTV;

    HWND m_Hwnd = nullptr;
    UINT m_Width = 0, m_Height = 0;
};