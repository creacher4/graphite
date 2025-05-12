#include "PlatformWindow.h"
#include "input/InputManager.h"
#include <stdexcept>
#include <imgui_impl_win32.h>
#include "utils/Logger.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK PlatformWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // handle input messages first
    InputManager::Get().HandleWin32Message(msg, wParam, lParam);

    // handle ImGui messages next
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    // recover pointer
    PlatformWindow *window = reinterpret_cast<PlatformWindow *>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_SIZE:
        if (window)
        {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            window->HandleResize(w, h);
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

PlatformWindow::PlatformWindow(HINSTANCE hInstance, int width, int height, const std::wstring &title)
    : m_HInstance(hInstance), m_Width(width), m_Height(height)
{
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, L"DX11Win", NULL};
    RegisterClassEx(&wc);

    RECT wr = {0, 0, width, height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_HWND = CreateWindowW(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW,
                           100, 100, wr.right - wr.left, wr.bottom - wr.top,
                           NULL, NULL, hInstance, NULL);

    if (!m_HWND)
    {
        LOG_ERROR("Failed to create window.");
        UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    SetWindowLongPtr(m_HWND, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(m_HWND, SW_SHOWDEFAULT);
    UpdateWindow(m_HWND);
}

void PlatformWindow::HandleResize(int width, int height)
{
    m_Width = width;
    m_Height = height;
    if (m_OnResize)
    {
        m_OnResize(width, height);
    }
}

PlatformWindow::~PlatformWindow()
{
    DestroyWindow(m_HWND);
}

bool PlatformWindow::ProcessMessages() const
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            LOG_INFO("Received WM_QUIT message. Exiting application.");
            return false;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
