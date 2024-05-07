#include "interactorStyle.h"
#include "interactor.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(InteractorStyle);

InteractorStyle::InteractorStyle()
{
    LogDebug("");
}

InteractorStyle::~InteractorStyle()
{
    LogDebug("");
}

void InteractorStyle::SetInteractor(Interactor* interactor)
{
    // 不能增加interactor的引用计数，避免循环引用导致无法释放
    if (m_interactor != interactor)
    {
        m_interactor = interactor;
    }
}

Interactor* InteractorStyle::GetInteractor() const noexcept
{
    return m_interactor;
}

void InteractorStyle::FindPokedRenderer(const std::array<int, 2>& eventPos)
{
    SetCurrentRenderer(m_interactor->FindPokedRenderer(eventPos));
}

void InteractorStyle::SetCurrentRenderer(Renderer* renderer)
{
    m_currentRenderer = renderer;
}

void InteractorStyle::MouseMoveEvent()
{
    LogTrace("");
}

void InteractorStyle::RightButtonPressEvent()
{
    LogTrace("");
}

void InteractorStyle::RightButtonReleaseEvent()
{
    LogTrace("");
}

void InteractorStyle::LeftButtonPressEvent()
{
    LogTrace("");
}

void InteractorStyle::LeftButtonReleaseEvent()
{
    LogTrace("");
}

void InteractorStyle::MiddleButtonPressEvent()
{
    LogTrace("");
}

void InteractorStyle::MiddleButtonReleaseEvent()
{
    LogTrace("");
}

void InteractorStyle::MouseWheelForwardEvent()
{
    LogTrace("");
}

void InteractorStyle::MouseWheelBackwardEvent()
{
    LogTrace("");
}

void InteractorStyle::EnterEvent()
{
    LogTrace("");
}

void InteractorStyle::LeaveEvent()
{
    LogTrace("");
}

void InteractorStyle::KeyPressEvent()
{
    LogTrace("");
}

void InteractorStyle::KeyReleaseEvent()
{
    LogTrace("");
}

void InteractorStyle::CharEvent()
{
    LogTrace("");
}