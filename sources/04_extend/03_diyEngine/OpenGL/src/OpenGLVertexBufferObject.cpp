#include "OpenGLVertexBufferObject.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(OpenGLVertexBufferObject);

OpenGLVertexBufferObject::OpenGLVertexBufferObject()
{
    LogDebug("");
    m_internal.Target = GL_ARRAY_BUFFER;
}

OpenGLVertexBufferObject::~OpenGLVertexBufferObject()
{
    LogDebug("");
}
