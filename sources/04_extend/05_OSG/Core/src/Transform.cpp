#include "Transform.h"
#include "Log.h"
#include "Visitor.h"

void Transform::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void Transform::SetMatrix(const glm::mat4& mat)
{
    m_matrix = mat;
}

const glm::mat4& Transform::GetMatrix() const noexcept
{
    return m_matrix;
}
