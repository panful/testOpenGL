#pragma once

#include "Node.h"
#include <glm/glm.hpp>

class Camera : public Node
{
public:
    void Accept(Visitor&) override;

    const glm::mat4 GetViewMatrix() const noexcept;
    const glm::mat4 GetProjectMatrix() const noexcept;

private:
    glm::vec3 m_lookAt { 0.f, 0.f, 0.f };
    glm::vec3 m_viewUp { 0.f, 1.f, 0.f };
    glm::vec3 m_position { 0.f, 0.f, 2.f };
};
