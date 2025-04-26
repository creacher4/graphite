#include "Application.h"
#include <chrono>

Application::Application(HINSTANCE hInstance)
{
    m_Window = std::make_unique<PlatformWindow>(hInstance, 1280, 720, L"Graphite");
    m_Engine = std::make_unique<Engine>();
    m_Engine->Init(
        m_Window->GetHWND(),
        static_cast<UINT>(m_Window->GetWidth()),
        static_cast<UINT>(m_Window->GetHeight()));

    m_Window->SetResizeCallback([&](int w, int h)
                                { m_Engine->OnResize(w, h); });
}

void Application::Run()
{
    // using clock = std::chrono::high_resolution_clock;
    // auto last = clock::now();

    // while (m_Window->ProcessMessages())
    // {
    //     auto now = clock::now();
    //     float dt = std::chrono::duration<float>(now - last).count();
    //     last = now;

    //     m_Engine->Update(dt);
    // }

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
}
