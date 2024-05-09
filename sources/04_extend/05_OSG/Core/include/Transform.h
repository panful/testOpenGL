#pragma once

#include "Group.h"
#include <glm/mat4x4.hpp>

class Transform : public Group
{
public:
    void Accept(Visitor& visitor) override;

    void SetMatrix(const glm::mat4& mat);
    const glm::mat4& GetMatrix() const noexcept;

protected:
    glm::mat4 m_matrix { glm::mat4(1.f) };
};
