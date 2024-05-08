#include "Node.h"
#include "Visitor.h"

void Node::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}
