#include "SceneManager.h"
#include "core/ServiceLocator.h"
#include "managers/AssetManager.h"
#include "ecs/TransformComponent.h"
#include "ecs/RenderableComponent.h"
#include "rendering/Material.h"
#include "utils/Logger.h"
#include "cfg/Config.h"
#include <glm/gtc/matrix_transform.hpp>

namespace
{
    // default camera settings
    inline constexpr float DEFAULT_FOV_Y_RAD = glm::radians(45.0f);
    inline constexpr float DEFAULT_NEAR_Z = 0.1f;
    inline constexpr float DEFAULT_FAR_Z = 100.0f;
}

SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::Init(UINT width, UINT height)
{
    m_Registry = std::make_unique<ECSRegistry>();
    InitCamera(width, height);
}

void SceneManager::OnResize(UINT width, UINT height)
{
    if (m_Camera)
    {
        float aspect = float(width) / float(height);
        m_Camera->SetPerspective(
            m_Camera->GetFovY(),
            aspect,
            m_Camera->GetNearZ(),
            m_Camera->GetFarZ());
    }
}

void SceneManager::InitCamera(UINT width, UINT height)
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

void SceneManager::InitScene()
{
    LOG_INFO("Initializing scene...");
    try
    {
        auto &assetManager = ServiceLocator::GetAssetManager();

        namespace SA = Config::SceneAssets;
        AssetID modelPath = SA::MODEL_PATH;
        AssetID materialID = SA::MATERIAL_ID;
        AssetID albedoPath = SA::ALBEDO_PATH;
        AssetID normalPath = SA::NORMAL_PATH;
        AssetID ormPath = SA::ORM_PATH;

        // load model
        if (!assetManager.LoadModel(modelPath))
            LOG_ERROR("Failed to load model");

        // load textures
        if (!assetManager.LoadTexture(albedoPath) ||
            !assetManager.LoadTexture(normalPath) ||
            !assetManager.LoadTexture(ormPath))
            LOG_ERROR("Failed to load textures");

        // create material
        Material mat{albedoPath, normalPath, ormPath};
        if (!assetManager.AddMaterial(materialID, mat))
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