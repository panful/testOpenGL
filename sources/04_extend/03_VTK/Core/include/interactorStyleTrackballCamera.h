#pragma once

#include "InteractorStyle.h"

class InteractorStyleTrackballCamera : public InteractorStyle
{
public:
    static InteractorStyleTrackballCamera* New();

public:
    void MouseMoveEvent() override;
    void RightButtonPressEvent() override;
    void RightButtonReleaseEvent() override;
    void LeftButtonPressEvent() override;
    void LeftButtonReleaseEvent() override;
    void MiddleButtonPressEvent() override;
    void MiddleButtonReleaseEvent() override;
    void MouseWheelForwardEvent() override;
    void MouseWheelBackwardEvent() override;

    void EnterEvent() override;
    void LeaveEvent() override;
    void KeyPressEvent() override;
    void KeyReleaseEvent() override;
    void CharEvent() override;

protected:
    InteractorStyleTrackballCamera();
    ~InteractorStyleTrackballCamera() override;
};