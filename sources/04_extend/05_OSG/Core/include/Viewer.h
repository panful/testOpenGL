#pragma once

#include "Object.h"
#include <vector>

class Window;
class Visitor;

class Viewer : public Object
{
public:
    void Run() const noexcept;

    void Compile() const noexcept;

    void AddWindow(Window* window);

    void AddEventHandler(Visitor* eventHandler);

private:
    std::vector<Window*> m_windows;
    std::vector<Visitor*> m_eventHandlers {};
};
