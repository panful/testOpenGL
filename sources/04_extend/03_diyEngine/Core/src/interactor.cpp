
#include "interactor.h"
#include "interactorStyle.h"
#include "log.h"
#include "objectFactory.h"
#include "window.h"
#include <GLFW/glfw3.h>

StandardNewMacro(Interactor);

Interactor::Interactor()
{
    LogDebug("");
}

Interactor::~Interactor()
{
    LogDebug("");
    DestructObjectMemberMacro(m_window);
    DestructObjectMemberMacro(m_interactorStyle);
}

void Interactor::Start()
{
    glfwSetWindowUserPointer(m_window->GetGlfwWindow(), this);
    glfwSetCursorPosCallback(m_window->GetGlfwWindow(), CursorPosCallback);
    glfwSetKeyCallback(m_window->GetGlfwWindow(), KeyCallback);
    glfwSetScrollCallback(m_window->GetGlfwWindow(), ScrollCallback);
    glfwSetMouseButtonCallback(m_window->GetGlfwWindow(), MouseButtonCallback);
    glfwSetFramebufferSizeCallback(m_window->GetGlfwWindow(), FramebufferSizeCallback);

    while (!glfwWindowShouldClose(m_window->GetGlfwWindow()))
    {
        ProcessInput(m_window->GetGlfwWindow());
        Render();

        glfwSwapBuffers(m_window->GetGlfwWindow());
        glfwPollEvents();
    }
}

void Interactor::Render()
{
    if (m_window)
    {
        m_window->Render();
    }
}

void Interactor::SetWindow(Window* window)
{
    SetObjectBodyMacro(m_window, window);
}

void Interactor::SetInteractorStyle(InteractorStyle* interactorStyle)
{
    SetObjectBodyMacro(m_interactorStyle, interactorStyle);
}

void Interactor::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Interactor::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (auto pInstance = static_cast<Interactor*>(glfwGetWindowUserPointer(window)))
    {
        pInstance->m_eventPos = { static_cast<int>(xpos), static_cast<int>(xpos) };
    }
}

void Interactor::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (auto pInstance = static_cast<Interactor*>(glfwGetWindowUserPointer(window)))
    {
        switch (action)
        {
        case GLFW_PRESS:
            switch (button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                pInstance->m_interactorStyle->LeftButtonPressEvent();
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                pInstance->m_interactorStyle->MiddleButtonPressEvent();
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                pInstance->m_interactorStyle->RightButtonPressEvent();
                break;
            default:
                break;
            }
            break;
        case GLFW_RELEASE:
            switch (button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                pInstance->m_interactorStyle->LeftButtonReleaseEvent();
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                pInstance->m_interactorStyle->MiddleButtonReleaseEvent();
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                pInstance->m_interactorStyle->RightButtonReleaseEvent();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void Interactor::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (auto pInstance = static_cast<Interactor*>(glfwGetWindowUserPointer(window)))
    {
        if (yoffset > 0.0)
        {
            pInstance->m_interactorStyle->MouseWheelForwardEvent();
        }
        else if (yoffset < 0.0)
        {
            pInstance->m_interactorStyle->MouseWheelBackwardEvent();
        }
    }
}

void Interactor::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (auto pInstance = static_cast<Interactor*>(glfwGetWindowUserPointer(window)))
    {
        std::string strAction;
        std::string strKey;
        std::string strMods;

        switch (action)
        {
        case GLFW_PRESS:
            strAction = "press\t";
            break;
        case GLFW_RELEASE:
            strAction = "release";
            break;
        case GLFW_REPEAT:
            strAction = "repeat";
            break;
        default:
            strAction = "error";
            break;
        }

        switch (key)
        {
        case GLFW_KEY_0:
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
        case GLFW_KEY_9:
            strKey = "number";
            break;
        case GLFW_KEY_A:
        case GLFW_KEY_B:
        case GLFW_KEY_C:
        case GLFW_KEY_D:
        case GLFW_KEY_E:
        // ...
        case GLFW_KEY_Z:
            strKey = "alpha";
            break;
        case GLFW_KEY_RIGHT:
        case GLFW_KEY_LEFT:
        case GLFW_KEY_DOWN:
        case GLFW_KEY_UP:
            strKey = "direction";
            break;
        default:
            strKey = "other";
            break;
        }

        switch (mods)
        {
        case GLFW_MOD_SHIFT:
            strMods = "shift";
            break;
        case GLFW_MOD_CONTROL:
            strMods = "control";
            break;
        case GLFW_MOD_ALT:
            strMods = "alt";
            break;
        case GLFW_MOD_SUPER:
            strMods = "super";
            break;
        case GLFW_MOD_CAPS_LOCK:
            strMods = "caps";
            break;
        case GLFW_MOD_NUM_LOCK:
            strMods = "num";
            break;
        default:
            break;
        }

        switch (key)
        {
        case GLFW_KEY_0:
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
        case GLFW_KEY_9:
            break;
        case GLFW_KEY_A:
        case GLFW_KEY_B:
        case GLFW_KEY_C:
        case GLFW_KEY_D:
        case GLFW_KEY_E:
        case GLFW_KEY_F:
        case GLFW_KEY_G:
        case GLFW_KEY_H:
        case GLFW_KEY_I:
        case GLFW_KEY_J:
        case GLFW_KEY_K:
        case GLFW_KEY_L:
        case GLFW_KEY_M:
        case GLFW_KEY_N:
        case GLFW_KEY_O:
        case GLFW_KEY_P:
        case GLFW_KEY_Q:
        case GLFW_KEY_R:
        case GLFW_KEY_S:
        case GLFW_KEY_T:
        case GLFW_KEY_U:
        case GLFW_KEY_V:
        case GLFW_KEY_W:
        case GLFW_KEY_X:
        case GLFW_KEY_Y:
        case GLFW_KEY_Z:
            pInstance->m_interactorStyle->CharEvent();
            pInstance->m_key = mods == GLFW_MOD_CAPS_LOCK ? key : key + 32;
            break;
        default:
            break;
        }
    }
}

void Interactor::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (auto pInstance = static_cast<Interactor*>(glfwGetWindowUserPointer(window)))
    {
        pInstance->m_window->SetSize(width, height);
    }
}