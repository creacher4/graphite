#pragma once

#include "core/ISystem.h"
#include "rendering/Renderer.h"
#include <Windows.h>

class DeviceManager;
class AssetManager;
class ECSRegistry;

class RenderSystem : public ISystem
{
public:
    void Init() override;
    void Update() override;
    void Shutdown() override;
    void OnResize(UINT width, UINT height);

    void SetDeviceManager(DeviceManager *deviceManager) { m_DeviceManager = deviceManager; }
    void SetAssetManager(AssetManager *assetManager) { m_AssetManager = assetManager; }
    void SetRegistry(ECSRegistry *registry) { m_Registry = registry; }
    void SetWindowHandle(HWND hwnd) { m_Hwnd = hwnd; }
    void SetWindowSize(UINT width, UINT height)
    {
        m_Width = width;
        m_Height = height;
    }

    void SetViewProjection(const glm::mat4 &view, const glm::mat4 &proj)
    {
        m_ViewMatrix = view;
        m_ProjectionMatrix = proj;
    }

private:
    DeviceManager *m_DeviceManager = nullptr;
    AssetManager *m_AssetManager = nullptr;
    ECSRegistry *m_Registry = nullptr;
    HWND m_Hwnd = nullptr;
    UINT m_Width = 0;
    UINT m_Height = 0;

    Renderer m_Renderer;

    // temp camera matrices
    glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
    glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
};