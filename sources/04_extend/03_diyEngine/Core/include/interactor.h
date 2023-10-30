#pragma once

#include "object.h"
#include <array>

class Window;
class InteractorStyle;
struct GLFWwindow;

class Interactor : public Object
{
public:
    static Interactor* New();

    virtual void Render();
    virtual void Start();

    void SetWindow(Window*);
    void SetInteractorStyle(InteractorStyle*);
    void ProcessInput(GLFWwindow*);

protected:
    Interactor();
    ~Interactor() override;

private:
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    Window* m_window { nullptr };
    InteractorStyle* m_interactorStyle { nullptr };
    std::array<int, 2> m_eventPos {};
    uint16_t m_key { 0 };
};