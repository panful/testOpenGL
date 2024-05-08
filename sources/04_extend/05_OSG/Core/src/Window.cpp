#include "Window.h"
#include <stdexcept>
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "CompileTraversal.h"
#include "RecordTraversal.h"

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

void Window::Run() const noexcept
{
    while (!glfwWindowShouldClose(m_window))
    {
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
