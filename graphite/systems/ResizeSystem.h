#pragma once

#include "core/ISystem.h"
#include <Windows.h>

class DeviceManager;
class RenderSystem;

class ResizeSystem : public ISystem
{
public:
    void Init() override {}
    void Update(float deltaTime) override {}
    void Shutdown() override {}

    void OnResize(int width, int height);

    void SetDeviceManager(DeviceManager *deviceManager) { m_DeviceManager = deviceManager; }
    void SetRenderSystem(RenderSystem *renderSystem) { m_RenderSystem = renderSystem; }

private:
    DeviceManager *m_DeviceManager = nullptr;
    RenderSystem *m_RenderSystem = nullptr;
};