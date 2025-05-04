#include "core/Application.h"
#include "utils/Logger.h"

#include <Windows.h>

namespace
{
    // logging cfg
    static constexpr const char LOG_FILE_PATH[] = "logs/Graphite.log";
    static constexpr LogLevel DEFAULT_LOG_LEVEL = LogLevel::DEBUG;
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    (void)hPrevInstance;

    Logger::Init(DEFAULT_LOG_LEVEL, LOG_FILE_PATH);
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
