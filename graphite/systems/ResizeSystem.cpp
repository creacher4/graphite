#include "ResizeSystem.h"
#include "managers/DeviceManager.h"
#include "RenderSystem.h"

ResizeSystem::ResizeSystem(
    DeviceManager *deviceManager,
    RenderSystem *renderSystem)
    : m_DeviceManager(deviceManager),
      m_RenderSystem(renderSystem)
{
}

void ResizeSystem::OnResize(int width, int height)
{
    m_DeviceManager->ResizeSwapChain(width, height);
    m_RenderSystem->OnResize(width, height);
}
