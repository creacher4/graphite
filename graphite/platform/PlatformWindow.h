#pragma once

#include <Windows.h>
#include <string>
#include <functional>

class PlatformWindow
{
public:
    using ResizeCallback = std::function<void(int /*width*/, int /*height*/)>;

    PlatformWindow(HINSTANCE hInstance, int width, int height, const std::wstring &title);
    ~PlatformWindow();

    HWND GetHWND() const { return m_HWND; }
    bool ProcessMessages() const;

    void SetResizeCallback(ResizeCallback cb) { m_OnResize = std::move(cb); }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    void HandleResize(int width, int height);

    HWND m_HWND;
    HINSTANCE m_HInstance;
    int m_Width, m_Height;

    ResizeCallback m_OnResize;
};
