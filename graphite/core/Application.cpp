#include "Application.h"

Application::Application(HINSTANCE hInstance)
{
    m_Window = std::make_unique<PlatformWindow>(hInstance, 1280, 720, L"Graphite");
    m_Engine = std::make_unique<Engine>();
    m_Engine->Init(m_Window->GetHWND());
}

void Application::Run()
{
    while (m_Window->ProcessMessages())
    {
        m_Engine->Update();
        m_Engine->Render();
    }
}
