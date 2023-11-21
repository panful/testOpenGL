#include "interactorStyleTrackballCamera.h"
#include "camera.h"
#include "interactor.h"
#include "log.h"
#include "objectFactory.h"
#include "renderer.h"
#include "window.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <numbers>

StandardNewMacro(InteractorStyleTrackballCamera);

InteractorStyleTrackballCamera::InteractorStyleTrackballCamera()
{
    LogDebug("");
}

InteractorStyleTrackballCamera::~InteractorStyleTrackballCamera()
{
    LogDebug("");
}

void InteractorStyleTrackballCamera::MouseMoveEvent()
{
    FindPokedRenderer(m_interactor->GetEventPos());

    double xOffset = m_interactor->GetEventPos()[0] - m_lastEventPos[0];
    double yOffset = m_interactor->GetEventPos()[1] - m_lastEventPos[1];

    switch (m_state)
    {
    case InteractorState::Rotate:
    {
        constexpr auto sensitivity = 0.5 / 180.0 * std::numbers::pi_v<double>;
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        glm::vec3 eyePos = m_currentRenderer->GetCamera()->GetEyePos();
        glm::vec3 viewUp = m_currentRenderer->GetCamera()->GetViewUp();

        glm::vec3 xAxis        = glm::normalize(glm::cross(viewUp, eyePos));
        glm::vec3 yAxis        = glm::normalize(viewUp);
        glm::quat xRotation    = glm::angleAxis(static_cast<float>(-yOffset), xAxis);
        glm::quat yRotation    = glm::angleAxis(static_cast<float>(-xOffset), yAxis);
        glm::quat rotationQuat = xRotation * yRotation;

        m_currentRenderer->GetCamera()->Rotate(glm::mat4(rotationQuat));
        m_lastEventPos = m_interactor->GetEventPos();
    }
    break;
    case InteractorState::Pan:
    {
        xOffset /= (m_currentRenderer->GetWindow()->GetSize()[0] / 2);
        yOffset /= (m_currentRenderer->GetWindow()->GetSize()[1] / 2);

        glm::vec3 eyePos = m_currentRenderer->GetCamera()->GetEyePos();
        glm::vec3 viewUp = m_currentRenderer->GetCamera()->GetViewUp();

        glm::vec3 xAxis        = glm::normalize(glm::cross(viewUp, eyePos));
        glm::vec3 yAxis        = glm::normalize(viewUp);
        glm::mat4 xTranslation = glm::translate(glm::mat4(1.f), xAxis * static_cast<float>(-xOffset));
        glm::mat4 yTranslation = glm::translate(glm::mat4(1.f), yAxis * static_cast<float>(yOffset));
        glm::mat4 translation  = xTranslation * yTranslation;
        m_currentRenderer->GetCamera()->Translate(translation);
        m_lastEventPos = m_interactor->GetEventPos();
    }
    break;
    case InteractorState::Zoom:
        break;
    default:
        break;
    }
}

void InteractorStyleTrackballCamera::RightButtonPressEvent()
{
}

void InteractorStyleTrackballCamera::RightButtonReleaseEvent()
{
}

void InteractorStyleTrackballCamera::LeftButtonPressEvent()
{
    if (m_state == InteractorState::None)
    {
        m_state = InteractorState::Rotate;
    }
    m_lastEventPos = m_interactor->GetEventPos();
}

void InteractorStyleTrackballCamera::LeftButtonReleaseEvent()
{
    m_state = InteractorState::None;
}

void InteractorStyleTrackballCamera::MiddleButtonPressEvent()
{
    if (m_state == InteractorState::None)
    {
        m_state = InteractorState::Pan;
    }
    m_lastEventPos = m_interactor->GetEventPos();
}

void InteractorStyleTrackballCamera::MiddleButtonReleaseEvent()
{
    m_state = InteractorState::None;
}

void InteractorStyleTrackballCamera::MouseWheelForwardEvent()
{
    FindPokedRenderer(m_interactor->GetEventPos());

    glm::vec3 eyePos    = m_currentRenderer->GetCamera()->GetEyePos();
    glm::vec3 focalPos  = m_currentRenderer->GetCamera()->GetFocalPos();
    glm::vec3 direction = glm::normalize(eyePos - focalPos);
    float distance      = glm::distance(eyePos, focalPos);
    glm::vec3 translate = direction * distance * 0.1f;

    m_currentRenderer->GetCamera()->Zoom(glm::translate(glm::mat4(1.f), translate));
}

void InteractorStyleTrackballCamera::MouseWheelBackwardEvent()
{
    FindPokedRenderer(m_interactor->GetEventPos());

    glm::vec3 eyePos    = m_currentRenderer->GetCamera()->GetEyePos();
    glm::vec3 focalPos  = m_currentRenderer->GetCamera()->GetFocalPos();
    glm::vec3 direction = glm::normalize(eyePos - focalPos);
    float distance      = glm::distance(eyePos, focalPos);
    glm::vec3 translate = direction * distance * -0.1f;

    m_currentRenderer->GetCamera()->Zoom(glm::translate(glm::mat4(1.f), translate));
}

void InteractorStyleTrackballCamera::EnterEvent()
{
}

void InteractorStyleTrackballCamera::LeaveEvent()
{
}

void InteractorStyleTrackballCamera::KeyPressEvent()
{
}

void InteractorStyleTrackballCamera::KeyReleaseEvent()
{
}

void InteractorStyleTrackballCamera::CharEvent()
{
}