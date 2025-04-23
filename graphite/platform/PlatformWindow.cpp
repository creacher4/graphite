#include "PlatformWindow.h"
#include <stdexcept>

LRESULT CALLBACK PlatformWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

PlatformWindow::PlatformWindow(HINSTANCE hInstance, int width, int height, const std::wstring &title)
    : m_HInstance(hInstance)
{
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, L"DX11Win", NULL};
    RegisterClassEx(&wc);

    RECT wr = {0, 0, width, height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    m_HWND = CreateWindowW(wc.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW,
                           100, 100, wr.right - wr.left, wr.bottom - wr.top,
                           NULL, NULL, hInstance, NULL);

    if (!m_HWND)
        throw std::runtime_error("Failed to create window.");

    ShowWindow(m_HWND, SW_SHOWDEFAULT);
    UpdateWindow(m_HWND);
}

PlatformWindow::~PlatformWindow()
{
    DestroyWindow(m_HWND);
}

bool PlatformWindow::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}
