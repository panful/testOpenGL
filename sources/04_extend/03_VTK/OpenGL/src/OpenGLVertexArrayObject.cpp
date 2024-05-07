#include "OpenGLVertexArrayObject.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLVertexArrayObject);

OpenGLVertexArrayObject::OpenGLVertexArrayObject()
{
    LogDebug("");
    glGenVertexArrays(1, &m_handle);
}

OpenGLVertexArrayObject::~OpenGLVertexArrayObject()
{
    LogDebug("");
    glDeleteVertexArrays(1, &m_handle);
}

void OpenGLVertexArrayObject::Bind()
{
    glBindVertexArray(m_handle);
}

void OpenGLVertexArrayObject::UnBind()
{
    glBindVertexArray(0);
}
