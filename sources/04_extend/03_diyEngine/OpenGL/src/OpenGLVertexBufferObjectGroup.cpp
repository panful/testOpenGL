#include "OpenGLVertexBufferObjectGroup.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>

StandardNewMacro(OpenGLVertexBufferObjectGroup);

OpenGLVertexBufferObjectGroup::OpenGLVertexBufferObjectGroup()
{
    LogDebug("");
}

OpenGLVertexBufferObjectGroup::~OpenGLVertexBufferObjectGroup()
{
    LogDebug("");
    for (auto [tmp, vbo] : m_vertexAttributes)
    {
        DestructObjectMemberMacro(vbo);
    }
}

int OpenGLVertexBufferObjectGroup::GetAttributeSize(OpenGLVertexAttributes vertexAttribute) const noexcept
{
    switch (vertexAttribute)
    {
    case OpenGLVertexAttributes::Pos:
        [[fallthrough]];
    case OpenGLVertexAttributes::Color:
        [[fallthrough]];
    case OpenGLVertexAttributes::Normal:
        return 3;
    case OpenGLVertexAttributes::TCoord:
        return 2;
    default:
        LogError("Vertex attribute error") return 0;
    }
}

uint8_t OpenGLVertexBufferObjectGroup::ConvertToOpenGLBoolType(bool value) const noexcept
{
    return value ? GL_TRUE : GL_FALSE;
}