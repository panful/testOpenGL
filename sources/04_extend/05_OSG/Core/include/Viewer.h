#pragma once

#include "Object.h"
#include <vector>

class Window;

class Viewer : public Object
{
public:
    void Run() const noexcept;

    void Compile() const noexcept;

    void AddWindow(Window* window);

private:
    std::vector<Window*> m_windows;
};
