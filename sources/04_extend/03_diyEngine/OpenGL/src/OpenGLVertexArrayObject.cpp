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

void OpenGLVertexArrayObject::VertexAttrib()
{
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
