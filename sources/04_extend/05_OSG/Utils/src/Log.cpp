#include "Log.h"
#include <format>
#include <iostream>

Log* Log::GetInstance()
{
    static Log log {};
    return &log;
}

void Log::Trace(const std::string_view message, const std::source_location location) const
{
    // std::clog << std::format(
    //     "[Trace] File: {}, Line: {}, Function: {}, Message: {}\n", location.file_name(), location.line(), location.function_name(), message);

    std::clog << std::format("[Trace] Function: {}, Message: {}\n", location.function_name(), message);
}

void Log::Debug(const std::string_view message, const std::source_location location) const
{
    std::clog << std::format(
        "[Debug] File: {}, Line: {}, Function: {}, Message: {}\n", location.file_name(), location.line(), location.function_name(), message);
}

void Log::Info(const std::string_view message, const std::source_location location) const
{
}

void Log::Warn(const std::string_view message, const std::source_location location) const
{
}

void Log::Error(const std::string_view message, const std::source_location location) const
{
}
