#pragma once

#include "rendering/GBuffer.h"
#include "rendering/ConstantBuffers.h"
#include "core/ECSRegistry.h"
#include "resources/ResourceManager.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    ~Renderer();

    void Init(HWND hwnd, UINT width, UINT height);
    void BeginFrame();
    void GeometryPass(ECSRegistry &registry, ResourceManager &resources);
    void EndFrame();
    // void OnResize(UINT width, UINT height);
    void UpdatePerFrameConstants(const glm::mat4 &view, const glm::mat4 &proj);

    ID3D11Device *GetDevice() const { return m_Device.Get(); }

private:
    GBuffer m_GBuffer;

    Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackBufferRTV;

    // rasterizer state
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateDefault;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateDefault;

    // shader objects
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vsGeometry;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psGeometry;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    // buffer members
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerFrame;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerObject;
};
