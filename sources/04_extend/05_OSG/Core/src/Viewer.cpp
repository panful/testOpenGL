#include "Viewer.h"
#include "Log.h"
#include "Window.h"

void Viewer::Run() const noexcept
{
    Log::GetInstance()->Trace();

    for (auto& window : m_windows)
    {
        window->Run();
    }
}

void Viewer::Compile() const noexcept
{
    Log::GetInstance()->Trace();

    for (auto& window : m_windows)
    {
        window->Compile();
    }
}

void Viewer::AddEventHandler(Visitor* eventHandler)
{
    m_eventHandlers.emplace_back(eventHandler);
}

void Viewer::AddWindow(Window* window)
{
    m_windows.emplace_back(window);
}
