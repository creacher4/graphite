#include "CameraController.h"
#include "core/SceneManager.h"
#include "core/ServiceLocator.h"
#include "rendering/Camera.h"
#include <imgui.h>
#include <glm/glm.hpp>

CameraController::CameraController(SceneManager *sceneManager)
    : m_SceneManager(sceneManager)
{
}

void CameraController::Update(float dt)
{
    if (!m_SceneManager)
        return;

    auto &camera = m_SceneManager->GetCamera();
    auto &input = ServiceLocator::GetInputManager();

    auto &io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        return;

    // mouse look only when rmb hold
    if (input.IsDown(VK_RBUTTON))
    {
        auto md = input.GetMouseDelta();
        float yaw = -md.x * LOOK_SPEED;
        float pitch = md.y * LOOK_SPEED;
        camera.Rotate(yaw, pitch);
    }

    // move camera with WASD
    glm::vec3 dir{0};
    if (input.IsDown('W'))
        dir += glm::vec3{0, 0, 1};
    if (input.IsDown('S'))
        dir += glm::vec3{0, 0, -1};
    if (input.IsDown('A'))
        dir += glm::vec3{-1, 0, 0};
    if (input.IsDown('D'))
        dir += glm::vec3{1, 0, 0};

    if (dir != glm::vec3{0})
    {
        glm::vec3 move = glm::normalize(dir);
        glm::vec3 worldMove =
            camera.GetRight() * move.x +
            camera.GetForward() * move.z; // no y component for now

        camera.Translate(worldMove * MOVE_SPEED * dt);
    }
}