#pragma once

#include "ecs/ECSRegistry.h"
#include "rendering/Camera.h"
#include <memory>
#include <Windows.h>

// manages scene-specific data like entities, components, and the camera
class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void Init(UINT width, UINT height);
    void InitScene();
    void OnResize(UINT width, UINT height);

    ECSRegistry &GetRegistry() { return *m_Registry; }
    Camera &GetCamera() { return *m_Camera; }

private:
    void InitCamera(UINT width, UINT height);

    std::unique_ptr<ECSRegistry> m_Registry;
    std::unique_ptr<Camera> m_Camera;
};