#include "Engine.h"
#include "ecs/TransformComponent.h"
#include "ecs/MeshComponent.h"
#include <string>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

void Engine::Init(HWND hwnd, UINT width, UINT height)
{
    m_Width = width;
    m_Height = height;

    // set up temp camera
    m_ViewMatrix = glm::lookAt(
        glm::vec3{0.0f, 0.0f, -3.0f}, // eye position
        glm::vec3{0.0f, 0.0f, 0.0f},  // target
        glm::vec3{0.0f, 1.0f, 0.0f}); // up

    float aspect = float(width) / float(height);
    m_ProjectionMatrix = glm::perspective(
        glm::radians(60.0f),
        aspect,
        0.1f, 100.0f);

    // initialize systems
    m_SystemManager = std::make_unique<SystemManager>();
    m_Registry = std::make_unique<ECSRegistry>();

    // register subsystems
    m_SystemManager->RegisterSystem(&m_InputSystem);
    m_DeviceManager = std::make_unique<DeviceManager>();
    m_DeviceManager->InitDevice(hwnd, width, height);
    m_AssetManager = std::make_unique<AssetManager>();
    m_AssetManager->SetDeviceManager(m_DeviceManager.get());
    if (!m_AssetManager->InitPrimitiveMeshes())
    {
        throw std::runtime_error("AssetManager failed to init primitives");
    }

    m_RenderSystem.SetDeviceManager(m_DeviceManager.get());
    m_RenderSystem.SetAssetManager(m_AssetManager.get());
    m_RenderSystem.SetRegistry(m_Registry.get());
    m_RenderSystem.SetWindowHandle(hwnd);
    m_RenderSystem.SetWindowSize(m_Width, m_Height);
    m_SystemManager->RegisterSystem(&m_RenderSystem);

    m_ResizeSystem.SetDeviceManager(m_DeviceManager.get());
    m_ResizeSystem.SetRenderSystem(&m_RenderSystem);
    m_SystemManager->RegisterSystem(&m_ResizeSystem);

    m_SystemManager->InitAll();

    // create test entity
    auto entity = m_Registry->CreateEntity();
    m_Registry->AddComponent<TransformComponent>(entity); // default transform
    m_Registry->AddComponent<MeshComponent>(entity, MeshComponent{"Cube"});
}

void Engine::Update()
{
    m_SystemManager->UpdateAll();
}

void Engine::Shutdown()
{
    m_SystemManager->ShutdownAll();
    m_AssetManager->Shutdown();
    m_DeviceManager->Shutdown();
}

void Engine::OnResize(int width, int height)
{
    m_ResizeSystem.OnResize(width, height);
}
