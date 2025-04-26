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
    m_Camera = std::make_unique<Camera>();
    m_Camera->LookAt(
        glm::vec3(0, 0, -3), // eye
        glm::vec3(0, 0, 0),  // center
        glm::vec3(0, 1, 0)); // up
    m_Camera->SetPerspective(
        glm::radians(45.0f),                // fovY
        static_cast<float>(width) / height, // aspect
        0.1f,                               // nearZ
        100.0f);                            // farZ

    // initialize systems
    m_SystemManager = std::make_unique<SystemManager>();
    m_Registry = std::make_unique<ECSRegistry>();

    // register subsystems
    m_SystemManager->RegisterSystem(&m_InputSystem);

    m_cameraController.SetCamera(m_Camera.get());
    m_SystemManager->RegisterSystem(&m_cameraController);

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
    m_RenderSystem.SetCamera(m_Camera.get());
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

void Engine::Update(float deltaTime)
{
    m_SystemManager->UpdateAll(deltaTime);
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
