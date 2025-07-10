#pragma once

#include "core/ISystem.h"
#include <glm/vec3.hpp>
#include <string>

class RenderSystem;
class SceneManager;

class StatsSystem : public ISystem
{
public:
    StatsSystem(RenderSystem *renderSystem, SceneManager *sceneManager);
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    // main ui drawing function, called each frame
    void DrawDebugUI();

    glm::vec3 GetViewDir() const;

private:
    void DrawEngineStatsWindow();
    void DrawLightingDebugWindow();
    void DrawGBufferViewerWindow();

    RenderSystem *m_renderSystem = nullptr;
    SceneManager *m_sceneManager = nullptr;

    // gathered each frame
    float m_frameTimeMs = 0.f;
    float m_fps = 0.f;
    int m_drawCalls = 0;
    int m_triCount = 0;
    glm::vec3 m_camPos;
    float m_camYaw, m_camPitch;

    // UI state
    bool m_showEngineStats = true;
    bool m_showLightingDebug = true;
    bool m_showGBufferViewer = true;
};