#pragma once

#include <iomanip>
#include <iostream>

#define LogTrace(str) std::clog << "TRACE\t" <<  __FUNCTION__ << " : " << str << std::endl;
#define LogDebug(str) std::clog << "DEBUG\t" <<  __FUNCTION__ << " : " << str << std::endl;
#define LogInfo(str) std::clog << "INFO\t" <<  __FUNCTION__ << " : " << str << std::endl;
#define LogWarn(str) std::clog << "WARN\t" <<  __FUNCTION__ << " : " << str << std::endl;
#define LogError(str) std::clog << "ERROR\t" <<  __FUNCTION__ << " : " << str << std::endl;