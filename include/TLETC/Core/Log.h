#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace TLETC
{

enum class LogLevel
{
    Trace,
    Info,
    Warning,
    Error,
    Fatal
};

class Log
{
public:
    template<typename... Args>
    static void Trace(Args&&... args)
    {
        Print(LogLevel::Trace, "[TRACE] ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Info(Args&&... args)
    {
        Print(LogLevel::Info, "[INFO] ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Warning(Args&&... args)
    {
        Print(LogLevel::Warning, "[WARNING] ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Error(Args&&... args)
    {
        Print(LogLevel::Error, "[ERROR] ", std::forward<Args>(args)...);
    }

private:
    template<typename... Args>
    static void Print(LogLevel level, const char* prefix, Args&&... args)
    {
        (void)level;
        std::ostringstream oss;
        oss << prefix;
        (oss << ... << args);  // C++17 fold expression
        oss << "\n";
        
        std::cout << oss.str();
        
        // TODO: Write to file, send to profiler, etc.
    }
};

} // namespace TLETC

// Convenience macros
#define TLETC_TRACE(...)   TLETC::Log::Trace(__VA_ARGS__)
#define TLETC_INFO(...)    TLETC::Log::Info(__VA_ARGS__)
#define TLETC_WARN(...)    TLETC::Log::Warning(__VA_ARGS__)
#define TLETC_ERROR(...)   TLETC::Log::Error(__VA_ARGS__)