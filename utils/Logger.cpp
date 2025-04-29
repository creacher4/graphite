#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstdarg>
#include <spdlog/common.h>
#include <algorithm>
#include <cctype>

// singleton
std::shared_ptr<spdlog::logger> Logger::s_Logger = nullptr;

void Logger::Init(LogLevel level, const std::string &logFilePath)
{
    // create file sink
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);

    // [2023-10-01T12:34:56.789] [1234] | INFO | This is a log message
    fileSink->set_pattern(
        "[%Y-%m-%dT%H:%M:%S.%e] [%P] %v");

    // register logger with sinks
    s_Logger = std::make_shared<spdlog::logger>(
        "Graphite",
        spdlog::sinks_init_list{fileSink});
    spdlog::register_logger(s_Logger);
    s_Logger->set_level(ToSpdLevel(level));  // global log level
    s_Logger->flush_on(spdlog::level::info); // flush on info level and above
}

void Logger::Shutdown()
{
    if (s_Logger)
    {
        s_Logger->flush();
        spdlog::drop("Graphite");
        s_Logger.reset();
    }
}

spdlog::level::level_enum Logger::ToSpdLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::TRACE:
        return spdlog::level::trace;
    case LogLevel::DEBUG:
        return spdlog::level::debug;
    case LogLevel::INFO:
        return spdlog::level::info;
    case LogLevel::WARN:
        return spdlog::level::warn;
    case LogLevel::ERR:
        return spdlog::level::err;
    case LogLevel::CRITICAL:
        return spdlog::level::critical;
    default:
        return spdlog::level::info;
    }
}

void Logger::Log(LogLevel level, const char *file, int line, const char *fmt, ...)
{
    if (!s_Logger)
        return;

    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    // id rather the log levels be uppercase so
    auto lvl = ToSpdLevel(level);
    auto lvlView = spdlog::level::to_string_view(lvl);
    std::string lvlName{lvlView.begin(), lvlView.end()};
    std::transform(lvlName.begin(), lvlName.end(), lvlName.begin(), [](unsigned char c)
                   { return std::toupper(c); });

    std::string msg = "| " + lvlName + " | " + buffer;

    spdlog::source_loc loc{file, line, /*func=*/""};
    s_Logger->log(loc, lvl, "{}", msg);
}
