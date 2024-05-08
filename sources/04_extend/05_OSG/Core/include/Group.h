#pragma once

#include "Node.h"
#include <vector>

class Visitor;

class Group : public Node
{
public:
    void Traverse(Visitor* visitor) const noexcept;

    void AddChild(Node* child);

private:
    std::vector<Node*> m_children {};
};
