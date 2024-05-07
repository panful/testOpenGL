#include "OpenGLIndexBufferObject.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(OpenGLIndexBufferObject);

OpenGLIndexBufferObject::OpenGLIndexBufferObject()
{
    LogDebug("");
    m_internal.Target = GL_ELEMENT_ARRAY_BUFFER;
}

OpenGLIndexBufferObject::~OpenGLIndexBufferObject()
{
    LogDebug("");
}
