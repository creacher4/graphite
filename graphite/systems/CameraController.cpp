#include "CameraController.h"
#include <imgui.h>
#include <glm/glm.hpp>

void CameraController::Update(float dt)
{
    if (!m_Camera)
        return;

    auto &io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard)
        return;

    // mouse look only when rmb hold
    if (InputManager::Get().IsDown(VK_RBUTTON))
    {
        // OutputDebugStringA(">> CameraController sees RMB held\n");
        auto md = InputManager::Get().GetMouseDelta();
        // char buf[64];
        // sprintf(buf, ">> MouseDelta = %d, %d\n", md.x, md.y);
        // OutputDebugStringA(buf);

        float yaw = -md.x * m_lookSpeed;
        float pitch = -md.y * m_lookSpeed;
        m_Camera->Rotate(yaw, pitch);
    }

    // move camera with WASD
    glm::vec3 dir{0};
    if (InputManager::Get().IsDown('W'))
        dir += glm::vec3{0, 0, 1};
    if (InputManager::Get().IsDown('S'))
        dir += glm::vec3{0, 0, -1};
    if (InputManager::Get().IsDown('A'))
        dir += glm::vec3{-1, 0, 0};
    if (InputManager::Get().IsDown('D'))
        dir += glm::vec3{1, 0, 0};

    if (dir != glm::vec3{0})
    {
        glm::vec3 move = glm::normalize(dir);
        glm::vec3 worldMove =
            m_Camera->GetRight() * move.x +
            m_Camera->GetForward() * move.z; // no y component for now

        m_Camera->Translate(worldMove * m_moveSpeed * dt);
    }
}
