#include "Event.h"
#include "Visitor.h"

void Event::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void KeyEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void MouseEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void KeyPressEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void KeyReleaseEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void MousePressEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void MouseReleaseEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void MouseMoveEvent::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}
