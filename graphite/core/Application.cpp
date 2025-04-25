#include "Application.h"

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
    while (m_Window->ProcessMessages())
    {
        auto view = m_Engine->GetViewMatrix();
        auto proj = m_Engine->GetProjectionMatrix();
        m_Engine->GetRenderSystem().SetViewProjection(view, proj);

        m_Engine->Update();
    }

    m_Engine->Shutdown();
}
