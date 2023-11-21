#include "camera.h"
#include "log.h"
#include "objectFactory.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

StandardNewMacro(Camera);

Camera::Camera()
{
    LogDebug("");
}

Camera::~Camera()
{
    LogDebug("");
}

glm::mat4 Camera::GetViewMatrix() const noexcept
{
    return glm::lookAt(m_eyePos, m_focalPos, m_viewUp);
}

glm::mat4 Camera::GetProjectMatrix() const noexcept
{
    return glm::perspective(glm::radians(45.f), 8.f / 6.f, 0.1f, 100.f);
}

glm::vec3 Camera::GetViewUp() const noexcept
{
    return m_viewUp;
}

glm::vec3 Camera::GetEyePos() const noexcept
{
    return m_eyePos;
}

glm::vec3 Camera::GetFocalPos() const noexcept
{
    return m_focalPos;
}

void Camera::Zoom(const glm::mat4& matrix)
{
    m_eyePos = glm::vec3(matrix * glm::vec4(m_eyePos, 1.f));
}

void Camera::Rotate(const glm::mat4& matrix)
{
    m_eyePos = glm::vec3(matrix * glm::vec4(m_eyePos, 1.f));
    m_viewUp = glm::vec3(matrix * glm::vec4(m_viewUp, 1.f));
}

void Camera::Translate(const glm::mat4& matrix)
{
    m_eyePos   = glm::vec3(matrix * glm::vec4(m_eyePos, 1.f));
    m_focalPos = glm::vec3(matrix * glm::vec4(m_focalPos, 1.f));
}