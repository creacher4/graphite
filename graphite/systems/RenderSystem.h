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
    RenderSystem(
        DeviceManager *deviceManager,
        AssetManager *assetManager,
        ECSRegistry *registry,
        Camera *camera,
        HWND hwnd,
        UINT width,
        UINT height);
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void OnResize(UINT width, UINT height);

    // only this remains injected via setter to avoid circular ctor dependency
    void SetStatsSystem(StatsSystem *s) { m_stats = s; }

    // getters
    int GetDrawCallCount() const { return m_Renderer.GetDrawCallCount(); }
    int GetTriangleCount() const { return m_Renderer.GetTriangleCount(); }

    // wireframe mode
    void SetWireframeMode(bool e) { m_Renderer.SetWireframeMode(e); }
    bool GetWireframeMode() const { return m_Renderer.GetWireframeMode(); }

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