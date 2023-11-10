#pragma once

#include "object.h"

class OpenGLVertexArrayObject : public Object
{
public:
    static OpenGLVertexArrayObject* New();

    void Bind();
    void UnBind();

protected:
    OpenGLVertexArrayObject();
    ~OpenGLVertexArrayObject() override;

private:
    uint32_t m_handle { 0 };
};
