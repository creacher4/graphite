#pragma once

#include "rendering/Renderer.h"
#include "ecs/ECSRegistry.h"
#include "input/InputManager.h"
#include "resources/ResourceManager.h"

#include <memory>
#include <Windows.h>

class Engine
{
public:
    void Init(HWND hwnd);
    void Update();
    void Render();

private:
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<InputManager> m_Input;
    std::unique_ptr<ResourceManager> m_ResourceManager;
};
