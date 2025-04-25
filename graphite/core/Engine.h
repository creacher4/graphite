#pragma once

#include "rendering/Renderer.h"
#include "core/ECSRegistry.h"
#include "input/InputManager.h"
#include "resources/ResourceManager.h"

#include <memory>
#include <Windows.h>
#include <glm/glm.hpp>

class Engine
{
public:
    void Init(HWND hwnd, UINT width, UINT height);
    void Update();
    void Render();

private:
    // temp camera
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ProjectionMatrix;

    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<InputManager> m_Input;
    std::unique_ptr<ResourceManager> m_ResourceManager;

    UINT m_Width;
    UINT m_Height;
};
