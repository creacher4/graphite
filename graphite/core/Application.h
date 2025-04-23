#pragma once

#include "Engine.h"
#include "platform/PlatformWindow.h"
#include <memory>
#include <Windows.h>

class Application
{
public:
    Application(HINSTANCE hInstance);
    void Run();

private:
    std::unique_ptr<PlatformWindow> m_Window;
    std::unique_ptr<Engine> m_Engine;
};
