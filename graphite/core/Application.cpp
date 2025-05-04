#include "Application.h"
#include <chrono>
#include <string>

namespace
{
    // window config defaults
    static constexpr int DEFAULT_WINDOW_WIDTH = 1280;
    static constexpr int DEFAULT_WINDOW_HEIGHT = 720;
    static const std::wstring DEFAULT_WINDOW_TITLE{L"Graphite"};
}

Application::Application(HINSTANCE hInstance)
{
    m_Window = std::make_unique<PlatformWindow>(
        hInstance,
        DEFAULT_WINDOW_WIDTH,
        DEFAULT_WINDOW_HEIGHT,
        DEFAULT_WINDOW_TITLE);
    m_Engine = std::make_unique<Engine>();
    m_Engine->Init(
        m_Window->GetHWND(),
        static_cast<UINT>(m_Window->GetWidth()),
        static_cast<UINT>(m_Window->GetHeight()));

    m_Window->SetResizeCallback([&](int w, int h)
                                { m_Engine->OnResize(w, h); });
}

int Application::Run()
{
    LARGE_INTEGER freq, prev, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    while (true)
    {
        InputManager::Get().NewFrame();

        if (!m_Window->ProcessMessages())
            break;

        // compute dt
        QueryPerformanceCounter(&now);
        float dt = float(now.QuadPart - prev.QuadPart) / float(freq.QuadPart);
        prev = now;

        // update engine
        m_Engine->Update(dt);
    }

    m_Engine->Shutdown();
    return 0;
}
