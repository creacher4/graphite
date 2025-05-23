#include "StatsSystem.h"
#include "systems/RenderSystem.h"
#include "rendering/Camera.h"
#include "input/InputManager.h"
#include <imgui.h>
#include <Windows.h>
#include <cmath>

StatsSystem::StatsSystem(RenderSystem *renderSystem, Camera *camera)
    : m_renderSystem(renderSystem), m_camera(camera)
{
}

void StatsSystem::Init()
{
}

void StatsSystem::Shutdown()
{
}

void StatsSystem::Update(float dt)
{
    m_frameTimeMs = dt * 1000.0f;
    m_fps = dt > 0.f ? 1.0f / dt : 0.f;

    m_drawCalls = m_renderSystem->GetDrawCallCount();
    m_triCount = m_renderSystem->GetTriangleCount();

    m_camPos = m_camera->GetPosition();
    auto yawp = m_camera->GetYawPitch();

    float rawYaw = yawp.x;
    float wrappedYaw = std::fmod(rawYaw, glm::two_pi<float>());
    if (wrappedYaw < 0.f)
        wrappedYaw += glm::two_pi<float>();

    m_camYaw = wrappedYaw;
    m_camPitch = yawp.y;

    if (InputManager::Get().WasPressed(VK_F1))
        m_showWindow = !m_showWindow;
}

void StatsSystem::DrawImGui()
{
    if (!m_showWindow)
        return;

    ImGui::Begin("Engine Stats", &m_showWindow);

    ImGui::Text("Frame Time: %.2f ms", m_frameTimeMs);
    ImGui::Text("FPS:        %.1f", m_fps);

    ImGui::Separator();
    ImGui::Text("Draw Calls: %d", m_drawCalls);
    ImGui::Text("Triangles:  %d", m_triCount);

    ImGui::Separator();
    ImGui::Text("Cam Pos:    %.2f, %.2f, %.2f",
                m_camPos.x, m_camPos.y, m_camPos.z);
    ImGui::Text("Yaw / Pitch: %.1f°, %.1f°",
                glm::degrees(m_camYaw),
                glm::degrees(m_camPitch));

    ImGui::Separator();
    bool wire = m_renderSystem->GetWireframeMode();
    ImGui::Checkbox("Wireframe: No-Cull", &wire);
    {
        m_renderSystem->SetWireframeMode(wire);
    }

    ImGui::End();
}

glm::vec3 StatsSystem::GetViewDir() const
{
    return m_camera ? m_camera->GetForward() : glm::vec3(0.f, 0.f, 1.f);
}