#pragma once

#include "ecs/ECSRegistry.h"
#include "core/SystemManager.h"
#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "systems/InputSystem.h"
#include "systems/RenderSystem.h"
#include "systems/ResizeSystem.h"

#include <memory>
#include <Windows.h>
#include <glm/glm.hpp>

class Engine
{
public:
    void Init(HWND hwnd, UINT width, UINT height);
    void Update();
    void OnResize(int width, int height);
    void Shutdown();

    const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
    RenderSystem &GetRenderSystem() { return m_RenderSystem; }

private:
    // temp camera
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<SystemManager> m_SystemManager;
    std::unique_ptr<DeviceManager> m_DeviceManager;
    std::unique_ptr<AssetManager> m_AssetManager;

    // systems
    InputSystem m_InputSystem;
    RenderSystem m_RenderSystem;
    ResizeSystem m_ResizeSystem;

    UINT m_Width;
    UINT m_Height;
};
