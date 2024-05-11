#include "State.h"
#include "Visitor.h"

void State::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void InputAssemblyState::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}
