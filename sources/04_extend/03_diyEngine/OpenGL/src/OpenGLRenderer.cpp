#include "OpenGLRenderer.h"
#include "actor.h"
#include "log.h"
#include "objectFactory.h"
#include "window.h"
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
    auto winSize = m_window->GetSize();

    auto x = static_cast<int>(m_viewport[0] * winSize[0]);
    auto y = static_cast<int>(m_viewport[1] * winSize[1]);
    auto w = static_cast<int>((m_viewport[2] - m_viewport[0]) * winSize[0]);
    auto h = static_cast<int>((m_viewport[3] - m_viewport[1]) * winSize[1]);

    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    glViewport(x, y, w, h);

    auto r = static_cast<float>(m_background[0]);
    auto g = static_cast<float>(m_background[1]);
    auto b = static_cast<float>(m_background[2]);

    glClearColor(r, g, b, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const auto actor : m_actors)
    {
        actor->Render();
    }
}