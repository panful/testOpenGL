#include "Trackball.h"
#include "Event.h"
#include "Log.h"

void Trackball::SetView(View* view)
{
    m_view = view;
}

void Trackball::Apply(Event& event)
{
}

void Trackball::Apply(KeyEvent& keyEvent)
{
}

void Trackball::Apply(KeyPressEvent& keyPressEvent)
{
}

void Trackball::Apply(KeyReleaseEvent& keyReleaseEvent)
{
}

void Trackball::Apply(MouseEvent& mouseEvent)
{
}

void Trackball::Apply(MousePressEvent& mousePressEvent)
{
    Log::GetInstance()->Trace();
}

void Trackball::Apply(MouseReleaseEvent& mouseReleaseEvent)
{
    Log::GetInstance()->Trace();
}

void Trackball::Apply(MouseMoveEvent& mouseMoveEvent)
{
}
