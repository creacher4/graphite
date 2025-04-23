#pragma once

#include <Windows.h>
#include <string>

class PlatformWindow
{
public:
    PlatformWindow(HINSTANCE hInstance, int width, int height, const std::wstring &title);
    ~PlatformWindow();

    HWND GetHWND() const { return m_HWND; }
    bool ProcessMessages();

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    HWND m_HWND;
    HINSTANCE m_HInstance;
};
