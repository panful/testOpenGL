#include "OpenGLBufferObject.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(OpenGLBufferObject);

OpenGLBufferObject::OpenGLBufferObject()
{
    LogDebug("");
    glGenBuffers(1, &m_internal.Handle);
}

OpenGLBufferObject::~OpenGLBufferObject()
{
    LogDebug("");
    glDeleteBuffers(1, &m_internal.Handle);
}

void OpenGLBufferObject::Bind()
{
    glBindBuffer(m_internal.Target, m_internal.Handle);
}

void OpenGLBufferObject::UnBind()
{
    glBindBuffer(m_internal.Target, 0);
}