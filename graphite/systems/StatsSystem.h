#pragma once

#include "core/ISystem.h"
#include "rendering/Camera.h"
#include <glm/vec3.hpp>
#include <string>

class RenderSystem;

class StatsSystem : public ISystem
{
public:
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void DrawImGui();

    void SetRenderSystem(RenderSystem *renderSystem) { m_renderSystem = renderSystem; }
    void SetCamera(Camera *camera) { m_camera = camera; }

    glm::vec3 GetViewDir() const
    {
        return m_camera ? m_camera->GetForward() : glm::vec3(0.f, 0.f, 1.f);
    }

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