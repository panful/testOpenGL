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
};
