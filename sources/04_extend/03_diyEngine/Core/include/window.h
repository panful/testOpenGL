#pragma once

#include "object.h"
#include <string>
#include <list>

class Renderer;
struct GLFWwindow;

class Window : public Object
{
public:
    static Window* New()
    {
        return new Window();
    }

    void Render();
    void AddRenderer(Renderer*);
    void RemoveRenderer(Renderer*);

    void SetSize(int, int);
    void SetWindowName(const std::string&);
    GLFWwindow* GetGlfwWindow() const;
    void SetGlfwWindow(GLFWwindow*);

protected:
    Window();
    ~Window() override;

private:
    void Init() const;
    void CreateWindow();

private:
    GLFWwindow* m_window { nullptr };
    std::string m_windowName { "Engine" };
    int m_windowwidth { 800 };
    int m_windowHeight { 600 };
    int m_oglMajor { 3 };
    int m_oglMinor { 3 };

    std::list<Renderer*> m_renderers;
};