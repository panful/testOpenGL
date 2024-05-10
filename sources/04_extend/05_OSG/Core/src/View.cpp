#include "View.h"
#include "Log.h"
#include "Visitor.h"

#include <glad/glad.h>

void View::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void View::SetCamera(Camera* camera)
{
    m_camera = camera;
}

Camera* View::GetCamera() const noexcept
{
    return m_camera;
}

void View::Clear() const noexcept
{
    Log::GetInstance()->Trace();

    glClearColor(.3f, .2f, .1f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}
