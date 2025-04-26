#include "StatsSystem.h"
#include <imgui.h>
#include <Windows.h>

#include "systems/RenderSystem.h"
#include "rendering/Camera.h"

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
    m_camYaw = yawp.x;
    m_camPitch = yawp.y;

    if (GetAsyncKeyState(VK_F1) & 1)
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

    ImGui::End();
}