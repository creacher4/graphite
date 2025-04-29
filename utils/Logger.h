#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <string>

enum class LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERR, // because ERROR is a macro in Windows
    CRITICAL
};

class Logger
{
public:
    static void Init(LogLevel level, const std::string &logFilePath);
    static void Shutdown();
    static void Log(
        LogLevel level,
        const char *file,
        int line,
        const char *fmt,
        ...);

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
    static spdlog::level::level_enum ToSpdLevel(LogLevel level);
};

// macros
#define LOG_TRACE(...) Logger::Log(LogLevel::TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::Log(LogLevel::DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) Logger::Log(LogLevel::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) Logger::Log(LogLevel::WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::Log(LogLevel::ERR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) Logger::Log(LogLevel::CRITICAL, __FILE__, __LINE__, __VA_ARGS__)
