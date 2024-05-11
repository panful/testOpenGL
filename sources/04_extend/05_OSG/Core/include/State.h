#pragma once

#include "Node.h"
#include <cstdint>

class State : public Node
{
public:
    void Accept(Visitor&) override;
};

class RasterizationState : public State
{
};

class InputAssemblyState : public State
{
public:
    void Accept(Visitor&) override;

    uint32_t m_face { 0x0408 }; // GL_FRONT_AND_BACK
    uint32_t m_mode { 0x1B02 }; // GL_FILL
};

class ColorBlendState : public State
{
};

class DepthStencilState : public State
{
};
