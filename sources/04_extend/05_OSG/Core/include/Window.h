#pragma once

#include "Group.h"
#include <string>

struct GLFWwindow;

class Window : public Group
{
public:
    Window();

    void Run() const noexcept;
    void Compile();

private:
    GLFWwindow* m_window { nullptr };
    std::string m_windowName { "Engine" };
    int m_windowwidth { 800 };
    int m_windowHeight { 600 };
    int m_oglMajor { 3 };
    int m_oglMinor { 3 };
};
