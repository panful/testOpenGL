#pragma once

#include "Object.h"

class Node;
class Group;
class Window;
class View;
class Transform;
class State;
class GraphicsPipeline;
class Geometry;
class Animation;

class Event;
class KeyEvent;
class MouseEvent;
class KeyPressEvent;
class KeyReleaseEvent;
class MousePressEvent;
class MouseReleaseEvent;
class MouseMoveEvent;

class Visitor : public Object
{
public:
    virtual void Apply(Node&);
    virtual void Apply(Group&);
    virtual void Apply(Window&);
    virtual void Apply(View&);
    virtual void Apply(Transform&);
    virtual void Apply(State&);
    virtual void Apply(GraphicsPipeline&);
    virtual void Apply(Geometry&);
    virtual void Apply(Animation&);

    virtual void Apply(Event&);
    virtual void Apply(KeyEvent&);
    virtual void Apply(MouseEvent&);
    virtual void Apply(KeyPressEvent&);
    virtual void Apply(KeyReleaseEvent&);
    virtual void Apply(MousePressEvent&);
    virtual void Apply(MouseReleaseEvent&);
    virtual void Apply(MouseMoveEvent&);
};
