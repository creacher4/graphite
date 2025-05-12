#include "Application.h"
#include "cfg/Config.h"
#include <chrono>
#include <string>

Application::Application(HINSTANCE hInstance)
{
    m_Window = std::make_unique<PlatformWindow>(
        hInstance,
        Config::Window::WIDTH,
        Config::Window::HEIGHT,
        Config::Window::TITLE);
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
