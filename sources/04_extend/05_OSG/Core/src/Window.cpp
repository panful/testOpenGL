#include "Window.h"

#include "CompileTraversal.h"
#include "Event.h"
#include "Log.h"
#include "RecordTraversal.h"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdexcept>

Window::Window()
{
    if (GLFW_FALSE == glfwInit())
    {
        throw std::runtime_error("glfw init failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_oglMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_oglMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_windowwidth, m_windowHeight, m_windowName.c_str(), nullptr, nullptr);
    if (nullptr == m_window)
    {
        glfwTerminate();
        throw std::runtime_error("create glfw window failed");
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize glad");
    }
}

void Window::Run() noexcept
{
    glfwSetWindowUserPointer(m_window, const_cast<Window*>(this));
    glfwSetCursorPosCallback(m_window, CursorPosCallback);
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);
    glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);

    while (!glfwWindowShouldClose(m_window))
    {
        for (const auto& event : m_events)
        {
            for (const auto& eventHandle : m_eventHandlers)
            {
                event->Accept(*eventHandle);
            }
        }
        m_events.clear();

        RecordTraversal recordTraversal {};
        Traverse(&recordTraversal);

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Window::Compile()
{
    CompileTraversal compileTraversal {};
    Traverse(&compileTraversal);
}

void Window::AddEventHandler(Visitor* eventHandler)
{
    m_eventHandlers.emplace_back(eventHandler);
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (auto pInstance = static_cast<Window*>(glfwGetWindowUserPointer(window)))
    {
    }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (auto pInstance = static_cast<Window*>(glfwGetWindowUserPointer(window)))
    {
        switch (action)
        {
            case GLFW_PRESS:
                pInstance->m_events.emplace_back(new MousePressEvent());
                // switch (button)
                // {
                //     case GLFW_MOUSE_BUTTON_LEFT:
                //     case GLFW_MOUSE_BUTTON_MIDDLE:
                //     case GLFW_MOUSE_BUTTON_RIGHT:;
                // }
                break;
            case GLFW_RELEASE:
                pInstance->m_events.emplace_back(new MouseReleaseEvent());
                // switch (button)
                // {
                //     case GLFW_MOUSE_BUTTON_LEFT:
                //     case GLFW_MOUSE_BUTTON_MIDDLE:
                //     case GLFW_MOUSE_BUTTON_RIGHT:;
                // }
                break;
        }
    }
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (auto pInstance = static_cast<Window*>(glfwGetWindowUserPointer(window)))
    {
        if (yoffset > 0.0)
        {
        }
        else if (yoffset < 0.0)
        {
        }
    }
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (auto pInstance = static_cast<Window*>(glfwGetWindowUserPointer(window)))
    {
        switch (action)
        {
            case GLFW_PRESS:
            case GLFW_RELEASE:
            case GLFW_REPEAT:;
        }

        switch (key)
        {
            case GLFW_KEY_0:
            case GLFW_KEY_9:
            case GLFW_KEY_A:
            case GLFW_KEY_Z:;
        }

        switch (mods)
        {
            case GLFW_MOD_SHIFT:
            case GLFW_MOD_CONTROL:
            case GLFW_MOD_ALT:
            case GLFW_MOD_SUPER:
            case GLFW_MOD_CAPS_LOCK:
            case GLFW_MOD_NUM_LOCK:;
        }
    }
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (auto pInstance = static_cast<Window*>(glfwGetWindowUserPointer(window)))
    {
    }
}
