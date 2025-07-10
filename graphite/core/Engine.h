#pragma once

#include "core/SystemManager.h"
#include "core/SceneManager.h"

#include <memory>
#include <Windows.h>
#include <glm/glm.hpp>

// forward declare managers and systems
class DeviceManager;
class AssetManager;
class InputManager;
class RenderSystem;
class StatsSystem;
class CameraController;

class Engine
{
public:
    ~Engine();
    void Init(HWND hwnd, UINT width, UINT height);
    void Update(float deltaTime);
    void OnResize(int width, int height);
    void Shutdown();

private:
    // managers/services - owned by Engine, provided to ServiceLocator
    std::unique_ptr<DeviceManager> m_DeviceManager;
    std::unique_ptr<AssetManager> m_AssetManager;
    std::unique_ptr<InputManager> m_InputManager;

    // core components
    std::unique_ptr<SceneManager> m_SceneManager;
    std::unique_ptr<SystemManager> m_SystemManager;

    UINT m_Width;
    UINT m_Height;

    // helpers
    void InitServices(HWND hwnd, UINT width, UINT height);
    void InitSystems();
};