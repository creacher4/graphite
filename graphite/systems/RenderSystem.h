#pragma once

#include "core/ISystem.h"
#include "managers/AssetManager.h"
#include "rendering/GBuffer.h"
#include "rendering/ConstantBuffers.h"
#include "rendering/Lighting.h"
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <glm/glm.hpp>
#include <array>

class SceneManager;
class ECSRegistry;
class StatsSystem;
struct Material;
struct TransformComponent;

class RenderSystem : public ISystem
{
public:
    explicit RenderSystem(SceneManager *sceneManager);
    ~RenderSystem();

    // isystem interface
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void OnResize(int width, int height) override;

    // state changers
    void SetWireframeMode(bool enable) { m_useWire_NoCull = enable; }
    void UpdateLightingData(const DirectionalLightData &data);

    // getters
    int GetDrawCallCount() const { return m_drawCallCount; }
    int GetTriangleCount() const { return m_triangleCount; }
    bool GetWireframeMode() const { return m_useWire_NoCull; }
    GBuffer &GetGBuffer() { return m_GBuffer; }

private:
    // dependencies
    SceneManager *m_SceneManager = nullptr;
    StatsSystem *m_stats = nullptr; // note: this dependency could be removed with an event bus
    UINT m_Width = 0, m_Height = 0;

    // rendering resources
    GBuffer m_GBuffer;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateDefault;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStateWire_NoCull;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateDefault;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerStateDefault;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vsGeometry;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psGeometry;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vsLighting;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psLighting;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerFrame;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbPerObject;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbLight;

    bool m_useWire_NoCull = false;
    int m_drawCallCount = 0;
    int m_triangleCount = 0;

    // private methods
    void InitImGui(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *context);
    void InitConstantBuffers(ID3D11Device *device);
    void InitStateObjectsAndShaders();

    void BeginFrame();
    void GeometryPass();
    void LightingPass();
    void RenderImGui();

    void UpdatePerFrameConstants();
    void UpdatePerObjectConstantBuffer(ID3D11DeviceContext *context, const TransformComponent &transform);
    void SetGeometryPassState(ID3D11DeviceContext *context);
    void BindMaterial(ID3D11DeviceContext *context, const Material *material);
    void UnbindMaterial(ID3D11DeviceContext *context);
    void DrawMesh(ID3D11DeviceContext *context, const AssetManager::MeshResource *mesh);
};