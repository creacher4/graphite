#pragma once

#include "rendering/GBuffer.h"
#include "rendering/ConstantBuffers.h"
#include "ecs/ECSRegistry.h"
#include "managers/AssetManager.h"
#include "systems/StatsSystem.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <glm/glm.hpp>

class DeviceManager;

class Renderer
{
public:
    ~Renderer();

    void Init(DeviceManager *deviceManager, HWND hwnd, UINT width, UINT height);
    void OnResize(UINT width, UINT height);
    void BeginFrame();
    void GeometryPass(ECSRegistry &registry, AssetManager &assetManager);
    void EndFrame(StatsSystem *stats);
    void UpdatePerFrameConstants(const glm::mat4 &view, const glm::mat4 &proj);

    int GetDrawCallCount() const { return m_drawCallCount; }
    int GetTriangleCount() const { return m_triangleCount; }

private:
    GBuffer m_GBuffer;

    DeviceManager *m_DeviceManager = nullptr;

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

    // bla bla
    int m_drawCallCount = 0;
    int m_triangleCount = 0;
};
