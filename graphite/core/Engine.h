#pragma once

#include "ecs/ECSRegistry.h"
#include "core/SystemManager.h"
#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "systems/RenderSystem.h"
#include "systems/ResizeSystem.h"
#include "systems/CameraController.h"
#include "rendering/Camera.h"
#include "systems/StatsSystem.h"

#include <memory>
#include <Windows.h>
#include <glm/glm.hpp>

class Engine
{
public:
    void Init(HWND hwnd, UINT width, UINT height);
    void Update(float deltaTime);
    void OnResize(int width, int height);
    void Shutdown();

    RenderSystem &GetRenderSystem() const { return *m_RenderSystem; }

private:
    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<SystemManager> m_SystemManager;
    std::unique_ptr<DeviceManager> m_DeviceManager;
    std::unique_ptr<AssetManager> m_AssetManager;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<StatsSystem> m_StatsSystem;
    std::unique_ptr<RenderSystem> m_RenderSystem;
    std::unique_ptr<ResizeSystem> m_ResizeSystem;

    // systems
    CameraController m_cameraController;

    UINT m_Width;
    UINT m_Height;

    // helpers
    void InitCamera(UINT width, UINT height);
    void InitSystems(HWND hwnd);
    void InitScene();
};
