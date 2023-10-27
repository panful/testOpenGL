#include "OpenGLRenderer.h"
#include "actor.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLRenderer);

OpenGLRenderer::OpenGLRenderer()
{
    LogDebug("");
}

OpenGLRenderer::~OpenGLRenderer()
{
    LogDebug("");
}

void OpenGLRenderer::Render()
{
    glClearColor(.1f, .2f, .3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto actor : m_actors)
    {
        actor->Render();
    }
}