#pragma once

#include "object.h"
#include <array>
#include <list>
#include <string>

class Renderer;
struct GLFWwindow;

class Window : public Object
{
public:
    static Window* New();

    void Render();
    void AddRenderer(Renderer*);
    void RemoveRenderer(Renderer*);

    void SetSize(int, int);
    std::array<int, 2> GetSize() const noexcept;

    void SetWindowName(const std::string&);
    GLFWwindow* GetGlfwWindow() const noexcept;
    void SetGlfwWindow(GLFWwindow*);

protected:
    Window();
    ~Window() override;

private:
    void Init() const;
    void CreateWindow();
    bool HasRenderer(Renderer*) const noexcept;

private:
    GLFWwindow* m_window { nullptr };
    std::string m_windowName { "Engine" };
    int m_windowwidth { 800 };
    int m_windowHeight { 600 };
    int m_oglMajor { 3 };
    int m_oglMinor { 3 };

    std::list<Renderer*> m_renderers;
};