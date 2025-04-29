#include "Engine.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "rendering/Material.h"
#include <string>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

void Engine::Init(HWND hwnd, UINT width, UINT height)
{
    // used elsewhere for window resizing
    m_Width = width;
    m_Height = height;

    InitCamera(width, height);
    InitSystems(hwnd);
    InitScene();
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

    {
        float aspect = float(width) / float(height);
        m_Camera->SetPerspective(
            m_Camera->GetFovY(), // fovY
            aspect,
            m_Camera->GetNearZ(), // nearZ
            m_Camera->GetFarZ()); // farZ
    }
}

void Engine::InitCamera(UINT width, UINT height)
{
    m_Camera = std::make_unique<Camera>();
    m_Camera->LookAt(
        glm::vec3(0, 0, -3), // eye
        glm::vec3(0, 0, 0),  // center
        glm::vec3(0, 1, 0)); // up
    m_Camera->SetPerspective(
        glm::radians(45.0f),          // fovY
        float(width) / float(height), // aspect
        0.1f,                         // nearZ
        100.0f);                      // farZ
}

void Engine::InitSystems(HWND hwnd)
{
    // system-manager and registry
    m_SystemManager = std::make_unique<SystemManager>();
    m_Registry = std::make_unique<ECSRegistry>();

    // camera controller
    m_cameraController.SetCamera(m_Camera.get());
    m_SystemManager->RegisterSystem(&m_cameraController);

    // device and asset managers
    m_DeviceManager = std::make_unique<DeviceManager>();
    m_DeviceManager->InitDevice(hwnd, m_Width, m_Height);
    m_AssetManager = std::make_unique<AssetManager>();
    m_AssetManager->SetDeviceManager(m_DeviceManager.get());

    // render and stats system
    m_RenderSystem.SetDeviceManager(m_DeviceManager.get());
    m_RenderSystem.SetAssetManager(m_AssetManager.get());
    m_RenderSystem.SetRegistry(m_Registry.get());
    m_RenderSystem.SetCamera(m_Camera.get());

    m_StatsSystem.SetRenderSystem(&m_RenderSystem);
    m_StatsSystem.SetCamera(m_Camera.get());
    m_SystemManager->RegisterSystem(&m_StatsSystem);

    m_RenderSystem.SetStatsSystem(&m_StatsSystem);
    m_RenderSystem.SetWindowHandle(hwnd);
    m_RenderSystem.SetWindowSize(m_Width, m_Height);
    m_SystemManager->RegisterSystem(&m_RenderSystem);

    m_ResizeSystem.SetDeviceManager(m_DeviceManager.get());
    m_ResizeSystem.SetRenderSystem(&m_RenderSystem);
    m_SystemManager->RegisterSystem(&m_ResizeSystem);

    // initialize all systems
    m_SystemManager->InitAll();
}

void Engine::InitScene()
{
    OutputDebugStringA("[Engine] Loading scene assets...\n");
    try
    {
        AssetID modelPath = "assets/models/boulder/boulder_01_8k.gltf";
        AssetID materialID = "assets/models/boulder/boulder_01_8k";
        AssetID albedoPath = "assets/models/boulder/boulder_01_albedo.png";
        AssetID normalPath = "assets/models/boulder/boulder_01_normals.png";
        AssetID ormPath = "assets/models/boulder/boulder_01_orm.png";

        // load model
        if (!m_AssetManager->LoadModel(modelPath))
            throw std::runtime_error("Failed to load model.");
        OutputDebugStringA("[Engine] Model loaded successfully.\n");

        // load textures
        if (!m_AssetManager->LoadTexture(albedoPath) ||
            !m_AssetManager->LoadTexture(normalPath) ||
            !m_AssetManager->LoadTexture(ormPath))
            throw std::runtime_error("Failed to load textures.");

        // create material
        Material mat{albedoPath, normalPath, ormPath};
        if (!m_AssetManager->AddMaterial(materialID, mat))
            throw std::runtime_error("Failed to add material.");

        auto entity = m_Registry->CreateEntity();
        // set at origin with no rotation and scale of 1
        TransformComponent t{glm::vec3{0, 0, 0}, {}, glm::vec3{1, 1, 1}};
        m_Registry->AddComponent<TransformComponent>(entity, t);
        m_Registry->AddComponent<RenderableComponent>(entity, RenderableComponent{modelPath, materialID});
    }
    catch (const std::exception &e)
    {
        OutputDebugStringA(std::string("[Engine] InitScene error: " + std::string(e.what()) + "\n").c_str());
        throw;
    }
}