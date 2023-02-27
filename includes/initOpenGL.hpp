#ifndef _INIT_OPENGL_HPP_
#define _INIT_OPENGL_HPP_

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string_view>

class InitOpenGL
{
public:
    InitOpenGL(const std::string_view& name = "Learn OpenGL", uint32_t w = 800, uint32_t h = 600)
        : m_windowName(name)
        , m_windowWidth(w)
        , m_windowHeight(h)
        , m_window(nullptr)
    {
        Init(m_windowName, m_windowWidth, m_windowHeight);
    }

    GLFWwindow* GetWindow() const
    {
        return m_window;
    }

private:
    Init(const std::string_view& name, uint32_t w, uint32_t h)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);

        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw("Failed to initialize GLAD");
        }
    }

private:
    std::string_view m_windowName;
    uint32_t m_windowWidth;
    uint32_t m_windowHeight;
    GLFWwindow* m_window;
}

#endif // !_INIT_OPENGL_HPP_
