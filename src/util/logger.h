#pragma once

#include <cstdio>
#include <exception>

#ifdef _WIN64
#undef TRACE
#undef DEBUG
#undef INFO
#undef WARN
#undef ERROR
#undef FATAL
#endif

#define LOG_LEVEL 1

#if LOG_LEVEL == 0
#define TRACE_LOG(...) Logger::Log(stdout, Logger::TRACE, __VA_ARGS__)
#else
#define TRACE_LOG(...)
#endif

#if LOG_LEVEL <= 1
#define DEBUG_LOG(...) Logger::Log(stdout, Logger::DEBUG, __VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#if LOG_LEVEL <= 2
#define INFO_LOG(...) Logger::Log(stdout, Logger::INFO, __VA_ARGS__)
#else
#define INFO_LOG(...)
#endif

#if LOG_LEVEL <= 3
#define WARN_LOG(...) Logger::Log(stdout, Logger::WARN, __VA_ARGS__)
#else
#define WARN_LOG(...)
#endif

#if LOG_LEVEL <= 4
#define ERROR_LOG(...) Logger::Log(stderr, Logger::ERROR, __VA_ARGS__)
#else
#define ERROR_LOG(...)
#endif

#if LOG_LEVEL <= 5
#define FATAL_LOG(...) Logger::Log(stderr, Logger::FATAL, __VA_ARGS__)
#else
#define FATAL_LOG(...)
#endif

class Logger final {
public:
    enum LogLevel : char {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };

    template <typename... Args>
    static void Log(FILE* stream,
                    LogLevel log_level,
                    const char* format,
                    Args... args);

private:
    static constexpr const char* const COLOR_CODE_STR[] = {
        "\033[0m", "\033[35m", "\033[34m", "\033[33m", "\033[31m", "\033[31m"
    };

    static constexpr const char* const LOG_LEVEL_STR[] = {
        "Trace: ", "Debug: ", "Info:  ", "Warn:  ", "Error: ", "Fatal: "
    };
};

#include "logger_imp.h"
