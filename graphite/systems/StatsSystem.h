#pragma once

#include "core/ISystem.h"
#include <glm/vec3.hpp>
#include <string>

class RenderSystem;
class Camera;

class StatsSystem : public ISystem
{
public:
    StatsSystem(RenderSystem *renderSystem, Camera *camera);
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void DrawImGui();

    glm::vec3 GetViewDir() const;

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