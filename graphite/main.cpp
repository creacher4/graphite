#include "core/Application.h"
#include "utils/Logger.h"

#include <Windows.h>

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    (void)hPrevInstance;

    Logger::Init(LogLevel::DEBUG, "logs/Graphite.log");
    LOG_INFO("Starting up Graphite...");

    try
    {
        Application app(hInstance);
        LOG_INFO("Entering main application loop");
        app.Run();
        LOG_INFO("Exited main application loop");
    }
    catch (const std::exception &e)
    {
        LOG_CRITICAL("Unhandled exception: {}", e.what());
    }

    Logger::Shutdown();
    return EXIT_SUCCESS;
}
