#pragma once

#include "managers/AssetManager.h"
#include "managers/DeviceManager.h"
#include "input/InputManager.h"
#include <memory>
#include <stdexcept>

// a simple static service locator for global access to managers
class ServiceLocator
{
public:
    static void Provide(DeviceManager *deviceManager) { s_DeviceManager = deviceManager; }
    static void Provide(AssetManager *assetManager) { s_AssetManager = assetManager; }
    static void Provide(InputManager *inputManager) { s_InputManager = inputManager; }

    static DeviceManager &GetDeviceManager()
    {
        if (!s_DeviceManager)
            throw std::runtime_error("DeviceManager not provided.");
        return *s_DeviceManager;
    }
    static AssetManager &GetAssetManager()
    {
        if (!s_AssetManager)
            throw std::runtime_error("AssetManager not provided.");
        return *s_AssetManager;
    }
    static InputManager &GetInputManager()
    {
        if (!s_InputManager)
            throw std::runtime_error("InputManager not provided.");
        return *s_InputManager;
    }

    static bool IsInputManagerProvided() { return s_InputManager != nullptr; }

private:
    static DeviceManager *s_DeviceManager;
    static AssetManager *s_AssetManager;
    static InputManager *s_InputManager;
};