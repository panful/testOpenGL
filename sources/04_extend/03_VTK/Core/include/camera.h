#pragma once

#include "object.h"
#include <glm/glm.hpp>

class Camera : public Object
{
public:
    static Camera* New();

    glm::mat4 GetViewMatrix() const noexcept;
    glm::mat4 GetProjectMatrix() const noexcept;
    glm::vec3 GetViewUp() const noexcept;
    glm::vec3 GetEyePos() const noexcept;
    glm::vec3 GetFocalPos() const noexcept;

    void Zoom(const glm::mat4&);
    void Rotate(const glm::mat4&);
    void Translate(const glm::mat4&);

protected:
    Camera();
    ~Camera() override;

    glm::vec3 m_viewUp { glm::vec3(0.f, 1.f, 0.) };
    glm::vec3 m_eyePos { glm::vec3(0.f, 0.f, 1.f) };
    glm::vec3 m_focalPos { glm::vec3(0.f, 0.f, 0.f) };
};