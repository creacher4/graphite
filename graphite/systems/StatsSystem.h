#pragma once

#include "core/ISystem.h"
#include <glm/vec3.hpp>
#include <string>

class RenderSystem;
class Camera;

class StatsSystem : public ISystem
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void DrawImGui();

    void SetRenderSystem(RenderSystem *renderSystem) { m_renderSystem = renderSystem; }
    void SetCamera(Camera *camera) { m_camera = camera; }

private:
    RenderSystem *m_renderSystem = nullptr;
    Camera *m_camera = nullptr;

    // gathered each frame
    float m_frameTimeMs = 0.f;
    float m_fps = 0.f;
    int m_drawCalls = 0;
    int m_triCount = 0;
    glm::vec3 m_camPos;
    float m_camYaw, m_camPitch;

    bool m_showWindow = true;
};