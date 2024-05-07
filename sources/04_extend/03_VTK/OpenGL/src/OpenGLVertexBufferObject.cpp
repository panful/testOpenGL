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

template <typename T>
uint32_t OpenGLVertexBufferObject::ConvertToOpenGLType() const noexcept
{
    LogDebug("-------------------- error ------------------------");
    return GL_FLOAT;
};

template <>
uint32_t OpenGLVertexBufferObject::ConvertToOpenGLType<float>() const noexcept
{
    return GL_FLOAT;
};

template <>
uint32_t OpenGLVertexBufferObject::ConvertToOpenGLType<uint8_t>() const noexcept
{
    return GL_UNSIGNED_BYTE;
};
