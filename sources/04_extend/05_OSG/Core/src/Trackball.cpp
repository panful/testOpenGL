#include "Trackball.h"
#include "Camera.h"
#include "Event.h"
#include "Log.h"
#include "View.h"

void Trackball::AddView(View* view)
{
    m_views.emplace_back(view);
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

    // TODO Set m_pokedView
    m_pokedView = *m_views.begin();

    static float radians { 0.f };
    radians += 0.1f;
    m_pokedView->GetCamera()->SetViewUp({ std::sin(radians), std::cos(radians), 0.f });
}

void Trackball::Apply(MouseReleaseEvent& mouseReleaseEvent)
{
    Log::GetInstance()->Trace();
}

void Trackball::Apply(MouseMoveEvent& mouseMoveEvent)
{
}
