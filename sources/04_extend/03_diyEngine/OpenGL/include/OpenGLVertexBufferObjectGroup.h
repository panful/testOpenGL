#pragma once

#include "OpenGLVertexBufferObject.h"
#include "object.h"
#include "objectFactory.h"
#include <glad/glad.h>
#include <map>
#include <vector>

class OpenGLVertexBufferObject;

enum class OpenGLVertexAttributes
{
    Pos = 0,
    Color,
    Normal,
    TCoord,
};

class OpenGLVertexBufferObjectGroup : public Object
{
public:
    static OpenGLVertexBufferObjectGroup* New();

    template <typename T>
    void CacheVertexAttribute(OpenGLVertexAttributes, const std::vector<T>&, bool normalized = false);

private:
    int GetAttributeSize(OpenGLVertexAttributes) const noexcept;

    uint8_t ConvertToOpenGLBoolType(bool) const noexcept;

protected:
    OpenGLVertexBufferObjectGroup();
    ~OpenGLVertexBufferObjectGroup() override;

private:
    std::map<OpenGLVertexAttributes, OpenGLVertexBufferObject*> m_vertexAttributes {};
};

template <typename T>
void OpenGLVertexBufferObjectGroup::CacheVertexAttribute(OpenGLVertexAttributes vertexAttribute, const std::vector<T>& data, bool normalized)
{
    if (m_vertexAttributes.contains(vertexAttribute))
    {
        DestructObjectMemberMacro(m_vertexAttributes[vertexAttribute]);
    }

    ConstructObjectMemberMacro(m_vertexAttributes[vertexAttribute], OpenGLVertexBufferObject);

    m_vertexAttributes[vertexAttribute]->Bind();
    m_vertexAttributes[vertexAttribute]->UpLoad(data);
    m_vertexAttributes[vertexAttribute]->EnableVertexAttributePointer<T>(
        static_cast<uint32_t>(vertexAttribute), GetAttributeSize(vertexAttribute), ConvertToOpenGLBoolType(normalized));
}
