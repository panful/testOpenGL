#include "OpenGLTextureBufferObject.h"
#include "log.h"
#include "objectFactory.h"

StandardNewMacro(OpenGLTextureBufferObject);

OpenGLTextureBufferObject::OpenGLTextureBufferObject()
{
    LogDebug("");
    m_internal.Target = GL_TEXTURE_BUFFER;
}

OpenGLTextureBufferObject::~OpenGLTextureBufferObject()
{
    LogDebug("");
}

void OpenGLTextureBufferObject::SetFormat(uint32_t format)
{
    m_format = format;
}

uint32_t OpenGLTextureBufferObject::GetFormat() const noexcept
{
    return m_format;
}