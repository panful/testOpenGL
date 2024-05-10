#include "Camera.h"
#include "Visitor.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

void Camera::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void Camera::SetLookAt(glm::vec3&& lookAt) noexcept
{
    m_lookAt = std::move(lookAt);
}

void Camera::SetViewUp(glm::vec3&& viewUp) noexcept
{
    m_viewUp = std::move(viewUp);
}

void Camera::SetPosition(glm::vec3&& position) noexcept
{
    m_position = std::move(position);
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
