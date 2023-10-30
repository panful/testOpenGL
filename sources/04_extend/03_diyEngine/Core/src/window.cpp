#include "window.h"
#include "log.h"
#include "objectFactory.h"
#include "renderer.h"
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <mutex>

StandardNewMacro(Window);

Window::Window()
{
    LogDebug("");
}

Window::~Window()
{
    LogDebug("");
    glfwDestroyWindow(m_window);
    glfwTerminate();
    for (const auto renderer : m_renderers)
    {
        renderer->UnRegister(this);
    }
}

void Window::Render()
{
    static std::once_flag once_flag;
    std::call_once(once_flag,
        [this]()
        {
            this->Init();
            this->CreateWindow();
        });

    for (const auto ren : m_renderers)
    {
        ren->Render();
    }
}

void Window::SetSize(int w, int h)
{
    m_windowwidth  = w;
    m_windowHeight = h;
}

std::array<int, 2> Window::GetSize() const noexcept
{
    return { m_windowwidth, m_windowHeight };
}

void Window::SetWindowName(const std::string& name)
{
    m_windowName = name;
}

void Window::Init() const
{
    if (GLFW_FALSE == glfwInit())
    {
        throw std::runtime_error("glfw init failed");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_oglMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_oglMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::CreateWindow()
{
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

GLFWwindow* Window::GetGlfwWindow() const noexcept
{
    return m_window;
}

void Window::SetGlfwWindow(GLFWwindow* w)
{
    m_window = w;
}

void Window::AddRenderer(Renderer* renderer)
{
    if (!HasRenderer(renderer))
    {
        renderer->SetWindow(this);
        m_renderers.emplace_back(renderer);
        renderer->Register(this);
    }
}

void Window::RemoveRenderer(Renderer* renderer)
{
    if (HasRenderer(renderer))
    {
        m_renderers.remove(renderer);
        renderer->UnRegister(this);
    }
}

bool Window::HasRenderer(Renderer* renderer) const noexcept
{
    return !(std::find(std::begin(m_renderers), std::end(m_renderers), renderer) == m_renderers.end());
}