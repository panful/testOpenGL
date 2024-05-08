#pragma once

#include <source_location>
#include <string_view>

class Log
{
public:
    static Log* GetInstance();

    void Trace(const std::string_view message = {}, const std::source_location location = std::source_location::current()) const;

    void Debug(const std::string_view message = {}, const std::source_location location = std::source_location::current()) const;

    void Info(const std::string_view message = {}, const std::source_location location = std::source_location::current()) const;

    void Warn(const std::string_view message = {}, const std::source_location location = std::source_location::current()) const;

    void Error(const std::string_view message = {}, const std::source_location location = std::source_location::current()) const;

private:
    Log()                      = default;
    ~Log() noexcept            = default;
    Log(const Log&)            = delete;
    Log& operator=(const Log&) = delete;
};
