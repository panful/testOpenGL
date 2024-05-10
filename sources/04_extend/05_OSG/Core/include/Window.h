#pragma once

#include "Group.h"
#include <list>
#include <string>
#include <vector>

struct GLFWwindow;
class Event;

class Window : public Group
{
public:
    Window();

    void Run() noexcept;
    void Compile();
    void AddEventHandler(Visitor* eventHandler);

private:
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_window { nullptr };
    std::string m_windowName { "Engine" };
    int m_windowwidth { 800 };
    int m_windowHeight { 600 };
    int m_oglMajor { 3 };
    int m_oglMinor { 3 };

    std::list<Event*> m_events {};
    std::vector<Visitor*> m_eventHandlers {};
};
