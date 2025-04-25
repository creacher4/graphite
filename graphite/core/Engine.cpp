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

    // initialize subsystems
    m_Renderer = std::make_unique<Renderer>();
    m_Registry = std::make_unique<ECSRegistry>();
    m_Input = std::make_unique<InputManager>();

    m_Renderer->Init(hwnd, width, height);

    m_ResourceManager = std::make_unique<ResourceManager>();
    if (!m_ResourceManager->InitPrimitiveMeshes(m_Renderer->GetDevice()))
    {
        throw std::runtime_error("Failed to initialize primitive meshes.");
    }

    // create test entity
    auto entity = m_Registry->CreateEntity();
    m_Registry->AddComponent<TransformComponent>(entity); // default transform
    m_Registry->AddComponent<MeshComponent>(entity, MeshComponent{"Cube"});
}

void Engine::Update()
{
    auto view = m_Registry->View<TransformComponent, MeshComponent>();

    for (auto [entity, transform, mesh] : view.each())
    {
        std::ostringstream oss;
        oss << "[ECS Debug] Entity found with MeshID: " << mesh.primitiveID
            << " | Position: (" << transform.position.x
            << ", " << transform.position.y
            << ", " << transform.position.z << ")\n";

        OutputDebugStringA(oss.str().c_str());
    }
}

void Engine::Render()
{
    m_Renderer->UpdatePerFrameConstants(m_ViewMatrix, m_ProjectionMatrix);
    m_Renderer->BeginFrame();
    m_Renderer->GeometryPass(*m_Registry, *m_ResourceManager);
    m_Renderer->EndFrame();
}
