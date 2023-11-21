#pragma once

#include "object.h"
#include <array>

class Interactor;
class Renderer;

enum class InteractorState
{
    None = 0,
    Rotate,
    Pan,
    Zoom,
};

class InteractorStyle : public Object
{
public:
    static InteractorStyle* New();

    void SetInteractor(Interactor*);
    Interactor* GetInteractor() const noexcept;
    void FindPokedRenderer(const std::array<int, 2>&);
    void SetCurrentRenderer(Renderer*);

public:
    virtual void MouseMoveEvent();
    virtual void RightButtonPressEvent();
    virtual void RightButtonReleaseEvent();
    virtual void LeftButtonPressEvent();
    virtual void LeftButtonReleaseEvent();
    virtual void MiddleButtonPressEvent();
    virtual void MiddleButtonReleaseEvent();
    virtual void MouseWheelForwardEvent();
    virtual void MouseWheelBackwardEvent();

    virtual void EnterEvent();
    virtual void LeaveEvent();
    virtual void KeyPressEvent();
    virtual void KeyReleaseEvent();
    virtual void CharEvent();

protected:
    InteractorStyle();
    ~InteractorStyle() override;

protected:
    InteractorState m_state { InteractorState::None };
    Interactor* m_interactor { nullptr };
    Renderer* m_currentRenderer { nullptr };
    std::array<int, 2> m_lastEventPos {};
};