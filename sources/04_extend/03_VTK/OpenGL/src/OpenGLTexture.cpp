#include "OpenGLTexture.h"
#include "OpenGLTextureBufferObject.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLTexture);

OpenGLTexture::OpenGLTexture()
{
    LogDebug("");
    glGenTextures(1, &m_internal.Handle);
}

OpenGLTexture::~OpenGLTexture()
{
    LogDebug("");
    glDeleteTextures(1, &m_internal.Handle);
    DestructObjectMemberMacro(m_textureBufferObject);
}

void OpenGLTexture::SetTarget(uint32_t target)
{
    m_internal.Target = target;
}

void OpenGLTexture::Bind()
{
    glBindTexture(m_internal.Target, m_internal.Handle);
}

void OpenGLTexture::UnBind()
{
    glBindTexture(m_internal.Target, 0);
}

void OpenGLTexture::LoadBuffer(OpenGLTextureBufferObject* tbo)
{
    SetObjectBodyMacro(m_textureBufferObject, tbo);
    if (m_textureBufferObject)
    {
        glTexBuffer(m_internal.Target, m_textureBufferObject->GetFormat(), m_textureBufferObject->GetHandle());
    }
}