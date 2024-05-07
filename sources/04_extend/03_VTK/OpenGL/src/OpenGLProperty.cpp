#include "OpenGLProperty.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLProperty);

OpenGLProperty::OpenGLProperty()
{
    LogDebug("");
}

OpenGLProperty::~OpenGLProperty()
{
    LogDebug("");
}

void OpenGLProperty::Render()
{
    glPointSize(static_cast<GLfloat>(m_pointSize));
    glLineWidth(static_cast<GLfloat>(m_lineWidth));
}