#pragma once

#include "core/ISystem.h"
#include <Windows.h>

class DeviceManager;
class RenderSystem;

class ResizeSystem : public ISystem
{
public:
    ResizeSystem(
        DeviceManager *deviceManager,
        RenderSystem *renderSystem);
    void Init() override {}
    void Update(float deltaTime) override {}
    void Shutdown() override {}
    void OnResize(int width, int height);

private:
    DeviceManager *m_DeviceManager = nullptr;
    RenderSystem *m_RenderSystem = nullptr;
};