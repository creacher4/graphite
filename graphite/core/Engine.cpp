#include "Engine.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "rendering/Material.h"
#include <string>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

void Engine::Init(HWND hwnd, UINT width, UINT height)
{
    m_Width = width;
    m_Height = height;

    SetupCamera(width, height);

    // initialize systems
    m_SystemManager = std::make_unique<SystemManager>();
    m_Registry = std::make_unique<ECSRegistry>();

    // register subsystems
    m_cameraController.SetCamera(m_Camera.get());
    m_SystemManager->RegisterSystem(&m_cameraController);

    m_DeviceManager = std::make_unique<DeviceManager>();
    m_DeviceManager->InitDevice(hwnd, width, height);
    m_AssetManager = std::make_unique<AssetManager>();
    m_AssetManager->SetDeviceManager(m_DeviceManager.get());
    // if (!m_AssetManager->InitPrimitiveMeshes())
    // {
    //     throw std::runtime_error("AssetManager failed to init primitives");
    // }

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

    m_SystemManager->InitAll();

    // create test entity
    // auto entity = m_Registry->CreateEntity();
    // m_Registry->AddComponent<TransformComponent>(entity); // default transform
    // m_Registry->AddComponent<RenderableComponent>(entity, RenderableComponent{"Cube"});
    OutputDebugStringA("[Engine] Loading scene assets...\n");
    try
    {
        AssetID modelPath = "assets/models/boulder/boulder_01_8k.gltf";
        AssetID materialID = "assets/models/boulder/boulder_01_8k";

        // texture paths
        AssetID albedoPath = "assets/models/boulder/boulder_01_albedo.png";
        AssetID normalPath = "assets/models/boulder/boulder_01_normals.png";
        AssetID ormPath = "assets/models/boulder/boulder_01_orm.png";

        // load model
        if (!m_AssetManager->LoadModel(modelPath))
        {
            throw std::runtime_error("Failed to load model.");
        }
        else
        {
            OutputDebugStringA("[Engine] Model loaded successfully.\n");
        }

        // load textures
        if (!m_AssetManager->LoadTexture(albedoPath))
        {
            throw std::runtime_error("Failed to load albedo texture.");
        }
        if (!m_AssetManager->LoadTexture(normalPath))
        {
            throw std::runtime_error("Failed to load normal texture.");
        }
        if (!m_AssetManager->LoadTexture(ormPath))
        {
            throw std::runtime_error("Failed to load ORM texture.");
        }

        // create material
        Material catMaterial;
        catMaterial.albedo = albedoPath;
        catMaterial.normal = normalPath;
        catMaterial.orm = ormPath;

        if (!m_AssetManager->AddMaterial(materialID, catMaterial))
        {
            throw std::runtime_error("Failed to add material.");
        }

        // create entity
        auto entity = m_Registry->CreateEntity();

        TransformComponent transform;
        transform.position = glm::vec3(0, 0, 0);
        transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_Registry->AddComponent<TransformComponent>(entity, transform);

        // add renderable component
        m_Registry->AddComponent<RenderableComponent>(entity, RenderableComponent{modelPath, materialID});
    }
    catch (const std::exception &e)
    {
        std::string errorMsg = "[Engine] EXCEPTION during asset loading/entity creation: ";
        errorMsg += e.what();
        errorMsg += "\n";
        OutputDebugStringA(errorMsg.c_str());
        throw;
    }
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

void Engine::SetupCamera(UINT width, UINT height)
{
    m_Camera = std::make_unique<Camera>();
    m_Camera->LookAt(
        glm::vec3(0, 0, -3), // eye
        glm::vec3(0, 0, 0),  // center
        glm::vec3(0, 1, 0)); // up
    m_Camera->SetPerspective(
        glm::radians(45.0f),                    // fovY
        static_cast<float>(m_Width) / m_Height, // aspect
        0.1f,                                   // nearZ
        100.0f);                                // farZ
}