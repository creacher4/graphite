#include "Engine.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "rendering/Material.h"
#include "utils/Logger.h"
#include "cfg/Config.h"
#include <string>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
    // default camera settings
    inline constexpr float DEFAULT_FOV_Y_RAD = glm::radians(45.0f);
    inline constexpr float DEFAULT_NEAR_Z = 0.1f;
    inline constexpr float DEFAULT_FAR_Z = 100.0f;
}

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
    m_ResizeSystem->OnResize(width, height);

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
        DEFAULT_FOV_Y_RAD,
        float(width) / float(height), // aspect
        DEFAULT_NEAR_Z,
        DEFAULT_FAR_Z);
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

    // render system
    m_RenderSystem = std::make_unique<RenderSystem>(
        m_DeviceManager.get(),
        m_AssetManager.get(),
        m_Registry.get(),
        m_Camera.get(),
        hwnd,
        m_Width,
        m_Height);

    // stats system (which currently relies on render system)
    m_StatsSystem = std::make_unique<StatsSystem>(
        m_RenderSystem.get(),
        m_Camera.get());
    m_SystemManager->RegisterSystem(m_StatsSystem.get());

    // inject stats back into renderer, then register it
    m_RenderSystem->SetStatsSystem(m_StatsSystem.get());
    m_SystemManager->RegisterSystem(m_RenderSystem.get());

    // then resize
    m_ResizeSystem = std::make_unique<ResizeSystem>(
        m_DeviceManager.get(),
        m_RenderSystem.get());
    m_SystemManager->RegisterSystem(m_ResizeSystem.get());

    // initialize all systems
    m_SystemManager->InitAll();
}

void Engine::InitScene()
{
    LOG_INFO("Initializing scene...");
    try
    {
        namespace SA = Config::SceneAssets;
        AssetID modelPath = SA::MODEL_PATH;
        AssetID materialID = SA::MATERIAL_ID;
        AssetID albedoPath = SA::ALBEDO_PATH;
        AssetID normalPath = SA::NORMAL_PATH;
        AssetID ormPath = SA::ORM_PATH;

        // load model
        if (!m_AssetManager->LoadModel(modelPath))
            LOG_ERROR("Failed to load model");

        // load textures
        if (!m_AssetManager->LoadTexture(albedoPath) ||
            !m_AssetManager->LoadTexture(normalPath) ||
            !m_AssetManager->LoadTexture(ormPath))
            LOG_ERROR("Failed to load textures");

        // create material
        Material mat{albedoPath, normalPath, ormPath};
        if (!m_AssetManager->AddMaterial(materialID, mat))
            LOG_ERROR("Failed to add material");

        auto entity = m_Registry->CreateEntity();
        // set at origin with no rotation and scale of 1
        TransformComponent t{};
        t.position = glm::vec3(0, 0, 0);
        t.rotation = glm::vec3(glm::pi<float>(), 0, 0);
        t.scale = glm::vec3(1, 1, 1);
        m_Registry->AddComponent<TransformComponent>(entity, t);
        m_Registry->AddComponent<RenderableComponent>(entity, RenderableComponent{modelPath, 0, materialID});
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Error initializing scene: {}", e.what());
        throw;
    }
}