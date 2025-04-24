#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>

class Renderer
{
public:
    void Init(HWND hwnd);
    void BeginFrame();
    void EndFrame();

    ID3D11Device *GetDevice() const { return m_Device.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackBufferRTV;
};
