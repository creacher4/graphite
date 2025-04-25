#include "ResizeSystem.h"
#include "managers/DeviceManager.h"
#include "RenderSystem.h"

void ResizeSystem::OnResize(int width, int height)
{
    OutputDebugStringA("[ResizeSystem] OnResize\n");

    m_DeviceManager->ResizeSwapChain(width, height);
    m_RenderSystem->OnResize(width, height);
}
