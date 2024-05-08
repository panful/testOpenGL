#pragma once

#include "Object.h"

class Visitor;

class Node : public Object
{
public:
    virtual void Accept(Visitor& visitor);
};
