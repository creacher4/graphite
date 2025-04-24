#include "Engine.h"
#include "rendering/Renderer.h"
#include "ecs/ECSRegistry.h"
#include "input/InputManager.h"
#include "ecs/TransformComponent.h"
#include "ecs/MeshComponent.h"
#include <string>
#include <sstream>

void Engine::Init(HWND hwnd)
{
    m_Renderer = std::make_unique<Renderer>();
    m_Registry = std::make_unique<ECSRegistry>();
    m_Input = std::make_unique<InputManager>();

    m_Renderer->Init(hwnd);

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
    m_Renderer->BeginFrame();
    m_Renderer->EndFrame();
}
