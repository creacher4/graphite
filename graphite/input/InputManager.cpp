#include "InputManager.h"
#include <stdexcept>

InputManager &InputManager::Get()
{
    static InputManager instance;
    return instance;
}

void InputManager::HandleWin32Message(UINT msg, WPARAM w, LPARAM l)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        m_current[w & 0xFF] = true;
        break;
    case WM_KEYUP:
        m_current[w & 0xFF] = false;
        break;
    case WM_RBUTTONDOWN:
        m_current[VK_RBUTTON] = true;
        // OutputDebugStringA(">> InputManager got WM_RBUTTONDOWN\n");
        break;
    case WM_RBUTTONUP:
        m_current[VK_RBUTTON] = false;
        // OutputDebugStringA(">> InputManager got WM_RBUTTONUP\n");
        break;
    case WM_MOUSEMOVE:
        m_curMouse.x = LOWORD(l);
        m_curMouse.y = HIWORD(l);
        break;
    }
}

void InputManager::NewFrame()
{
    m_justPressed = m_current & ~m_previous;
    m_previous = m_current;
    m_lastMouse = m_curMouse;
}

bool InputManager::IsDown(int vk) const
{
    return m_current[vk & 0xFF];
}

bool InputManager::WasPressed(int vk) const
{
    return m_justPressed[vk & 0xFF];
}

glm::ivec2 InputManager::GetMouseDelta() const
{
    return {m_curMouse.x - m_lastMouse.x,
            m_curMouse.y - m_lastMouse.y};
}