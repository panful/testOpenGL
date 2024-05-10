#include "Camera.h"
#include "Visitor.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

void Camera::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

const glm::mat4 Camera::GetViewMatrix() const noexcept
{
    return glm::lookAt(m_position, m_lookAt, m_viewUp);
}

const glm::mat4 Camera::GetProjectMatrix() const noexcept
{
    // TODO
    return glm::perspective(glm::radians(45.f), 8.f / 6.f, 0.1f, 100.f);
}
