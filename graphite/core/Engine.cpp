#include "Engine.h"
#include "rendering/Renderer.h"
#include "ecs/ECSRegistry.h"
#include "input/InputManager.h"
#include <iostream>

struct NameComponent
{
    std::string name;
};

void Engine::Init(HWND hwnd)
{
    m_Renderer = std::make_unique<Renderer>();
    m_Registry = std::make_unique<ECSRegistry>();
    m_Input = std::make_unique<InputManager>();

    m_Renderer->Init(hwnd);

    // create test entity
    auto entity = m_Registry->CreateEntity();
    m_Registry->AddComponent<NameComponent>(entity, NameComponent{"TestEntity"});
}

void Engine::Update()
{
    auto view = m_Registry->View<NameComponent>();
    for (auto entity : view)
    {
        auto &name = view.get<NameComponent>(entity);
        OutputDebugStringA(("[ECS] Entity Name: " + name.name + "\n").c_str());
    }
}

void Engine::Render()
{
    m_Renderer->BeginFrame();
    m_Renderer->EndFrame();
}
