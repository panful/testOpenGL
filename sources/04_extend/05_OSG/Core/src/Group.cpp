#include "Group.h"

void Group::AddChild(Node* child)
{
    m_children.emplace_back(child);
}

void Group::Traverse(Visitor* visitor) const noexcept
{
    for (auto& child : m_children)
    {
        child->Accept(*visitor);
    }
}
