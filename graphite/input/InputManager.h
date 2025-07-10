#pragma once

#include <Windows.h>
#include <glm/vec2.hpp>
#include <bitset>

class InputManager
{
public:
    InputManager() = default;
    void HandleWin32Message(UINT msg, WPARAM w, LPARAM l);
    void NewFrame();

    bool IsDown(int vk) const;
    bool WasPressed(int vk) const;

    glm::ivec2 GetMouseDelta() const;

private:
    std::bitset<256> m_current, m_previous, m_justPressed;
    POINT m_lastMouse{0, 0}, m_curMouse{0, 0};
};