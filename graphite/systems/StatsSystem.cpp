#include "StatsSystem.h"
#include "core/SceneManager.h"
#include "core/ServiceLocator.h"
#include "systems/RenderSystem.h"
#include "rendering/Camera.h"
#include "rendering/Lighting.h"
#include "input/InputManager.h"
#include <imgui.h>
#include <Windows.h>
#include <cmath>

StatsSystem::StatsSystem(RenderSystem *renderSystem, SceneManager *sceneManager)
    : m_renderSystem(renderSystem), m_sceneManager(sceneManager)
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
    // gather stats
    m_frameTimeMs = dt * 1000.0f;
    m_fps = dt > 0.f ? 1.0f / dt : 0.f;

    m_drawCalls = m_renderSystem->GetDrawCallCount();
    m_triCount = m_renderSystem->GetTriangleCount();

    auto &camera = m_sceneManager->GetCamera();
    m_camPos = camera.GetPosition();
    auto yawp = camera.GetYawPitch();

    float rawYaw = yawp.x;
    float wrappedYaw = std::fmod(rawYaw, glm::two_pi<float>());
    if (wrappedYaw < 0.f)
        wrappedYaw += glm::two_pi<float>();

    m_camYaw = wrappedYaw;
    m_camPitch = yawp.y;

    // handle input for toggling debug windows
    auto &input = ServiceLocator::GetInputManager();
    if (input.WasPressed(VK_F1))
        m_showEngineStats = !m_showEngineStats;
    if (input.WasPressed(VK_F2))
        m_showLightingDebug = !m_showLightingDebug;
    if (input.WasPressed(VK_F3))
        m_showGBufferViewer = !m_showGBufferViewer;

    // draw ui
    DrawDebugUI();
}

void StatsSystem::DrawDebugUI()
{
    if (m_showEngineStats)
        DrawEngineStatsWindow();
    if (m_showLightingDebug)
        DrawLightingDebugWindow();
    if (m_showGBufferViewer)
        DrawGBufferViewerWindow();
}

void StatsSystem::DrawEngineStatsWindow()
{
    if (!ImGui::Begin("Engine Stats (F1)", &m_showEngineStats))
    {
        ImGui::End();
        return;
    }

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
    if (ImGui::Checkbox("Wireframe: No-Cull", &wire))
    {
        m_renderSystem->SetWireframeMode(wire);
    }

    ImGui::End();
}

void StatsSystem::DrawLightingDebugWindow()
{
    if (!ImGui::Begin("Lighting Debug (F2)", &m_showLightingDebug))
    {
        ImGui::End();
        return;
    }

    static bool cbAlbedo = true;
    static bool cbNormals = true;
    static bool cbAO = true;
    static bool cbRoughness = true;
    static bool cbMetallic = true;
    static bool cbFresnel = true;
    static bool cbRim = true;

    ImGui::Checkbox("Albedo", &cbAlbedo);
    ImGui::Checkbox("Normals", &cbNormals);
    ImGui::Checkbox("AO", &cbAO);
    ImGui::Checkbox("Roughness", &cbRoughness);
    ImGui::Checkbox("Metallic", &cbMetallic);
    ImGui::Checkbox("Fresnel", &cbFresnel);
    ImGui::Checkbox("Rim", &cbRim);

    // build and upload light data
    DirectionalLightData lightData{};
    lightData.dir = glm::normalize(glm::vec3{0.0f, -1.0f, -0.1f});
    lightData.color = glm::vec3{1.0f, 0.95f, 0.85f};
    lightData.useAlbedo = cbAlbedo ? 1 : 0;
    lightData.useNormals = cbNormals ? 1 : 0;
    lightData.useAO = cbAO ? 1 : 0;
    lightData.viewDir = GetViewDir();
    lightData.useRoughness = cbRoughness ? 1 : 0;
    lightData.useMetallic = cbMetallic ? 1 : 0;
    lightData.useFresnel = cbFresnel ? 1 : 0;
    lightData.useRim = cbRim ? 1 : 0;

    m_renderSystem->UpdateLightingData(lightData);

    ImGui::End();
}

void StatsSystem::DrawGBufferViewerWindow()
{
    if (!ImGui::Begin("GBuffer Viewer (F3)", &m_showGBufferViewer))
    {
        ImGui::End();
        return;
    }

    static int channel = 0;
    ImGui::RadioButton("Albedo", &channel, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Normal", &channel, 1);
    ImGui::SameLine();
    ImGui::RadioButton("ORM", &channel, 2);
    ImGui::SameLine();
    ImGui::RadioButton("Depth", &channel, 3);

    ID3D11ShaderResourceView *srv = nullptr;
    switch (channel)
    {
    case 0: srv = m_renderSystem->GetGBuffer().GetAlbedoSRV(); break;
    case 1: srv = m_renderSystem->GetGBuffer().GetNormalSRV(); break;
    case 2: srv = m_renderSystem->GetGBuffer().GetOrmSRV(); break;
    case 3: srv = m_renderSystem->GetGBuffer().GetDepthSRV(); break;
    default: srv = m_renderSystem->GetGBuffer().GetAlbedoSRV(); break;
    }

    if (srv)
    {
        ImTextureID texID = (ImTextureID)srv;
        auto size = ImGui::GetContentRegionAvail();
        ImGui::Image(texID, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
    }
    ImGui::End();
}

glm::vec3 StatsSystem::GetViewDir() const
{
    return m_sceneManager ? m_sceneManager->GetCamera().GetForward() : glm::vec3(0.f, 0.f, 1.f);
}