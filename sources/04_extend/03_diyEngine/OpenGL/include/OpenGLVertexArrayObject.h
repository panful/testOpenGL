#pragma once

#include "object.h"

class OpenGLVertexArrayObject : public Object
{
public:
    static OpenGLVertexArrayObject* New();

    void Bind();
    void UnBind();
    void VertexAttrib();

protected:
    OpenGLVertexArrayObject();
    ~OpenGLVertexArrayObject();

private:
    uint32_t m_handle { 0 };
};
