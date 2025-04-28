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

    RenderSystem &GetRenderSystem() { return m_RenderSystem; }

private:
    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<SystemManager> m_SystemManager;
    std::unique_ptr<DeviceManager> m_DeviceManager;
    std::unique_ptr<AssetManager> m_AssetManager;
    std::unique_ptr<Camera> m_Camera;

    // systems
    RenderSystem m_RenderSystem;
    ResizeSystem m_ResizeSystem;
    StatsSystem m_StatsSystem;
    CameraController m_cameraController;

    UINT m_Width;
    UINT m_Height;

    // helpers
    void SetupCamera(UINT width, UINT height);
    // void RegisterSystems(HWND hwnd);
};
