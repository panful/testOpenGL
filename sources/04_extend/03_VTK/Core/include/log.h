#pragma once

#include <iomanip>
#include <iostream>
#include <string>

class Log
{
public:
    static Log* GetInstance()
    {
        static int count { 0 };
        static Log log;
        std::cout << count++ << '\t';
        return &log;
    }

    void Trace(const std::string& str)
    {
        std::cout << "[TRACE] : " << str << std::endl;
    }

    void Debug(const std::string& str)
    {
        std::cout << "[DEBUG] : " << str << std::endl;
    }

    void Info(const std::string& str)
    {
    }

    void Warn(const std::string& str)
    {
    }

    void Error(const std::string& str)
    {
    }

private:
};

#define LogTrace(str) std::clog << "TRACE\t" << __FUNCTION__ << " : " << str << std::endl;

#define LogDebug(str)                                           \
    auto logStr = std::string(__FUNCTION__) + std::string(str); \
    Log::GetInstance()->Debug(logStr);

#define LogInfo(str) std::clog << "INFO\t" << __FUNCTION__ << " : " << str << std::endl;

#define LogWarn(str) std::clog << "WARN\t" << __FUNCTION__ << " : " << str << std::endl;

#define LogError(str) std::clog << "ERROR\t" << __FUNCTION__ << " : " << str << std::endl;