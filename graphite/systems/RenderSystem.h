#pragma once

#include "core/ISystem.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"
#include <Windows.h>

class DeviceManager;
class AssetManager;
class ECSRegistry;
class StatsSystem;

class RenderSystem : public ISystem
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void OnResize(UINT width, UINT height);

    void SetDeviceManager(DeviceManager *deviceManager) { m_DeviceManager = deviceManager; }
    void SetAssetManager(AssetManager *assetManager) { m_AssetManager = assetManager; }
    void SetRegistry(ECSRegistry *registry) { m_Registry = registry; }
    void SetWindowHandle(HWND hwnd) { m_Hwnd = hwnd; }
    void SetWindowSize(UINT width, UINT height)
    {
        m_Width = width;
        m_Height = height;
    }

    void SetCamera(Camera *camera) { m_Camera = camera; }
    void SetStatsSystem(StatsSystem *s) { m_stats = s; }

    int GetDrawCallCount() const { return m_Renderer.GetDrawCallCount(); }
    int GetTriangleCount() const { return m_Renderer.GetTriangleCount(); }

    void EnableWireframeNoCull(bool e) { m_Renderer.EnableWireframeNoCull(e); }
    bool IsWireframeNoCullEnabled() const { return m_Renderer.IsWireframeNoCullEnabled(); }

private:
    DeviceManager *m_DeviceManager = nullptr;
    AssetManager *m_AssetManager = nullptr;
    ECSRegistry *m_Registry = nullptr;
    Camera *m_Camera = nullptr;
    StatsSystem *m_stats = nullptr;

    HWND m_Hwnd = nullptr;
    UINT m_Width = 0;
    UINT m_Height = 0;

    Renderer m_Renderer;
};