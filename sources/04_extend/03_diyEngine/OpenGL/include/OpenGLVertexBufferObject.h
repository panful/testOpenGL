#pragma once

#include "OpenGLBufferObject.h"

class OpenGLVertexBufferObject : public OpenGLBufferObject
{
public:
    static OpenGLVertexBufferObject* New();

protected:
    OpenGLVertexBufferObject();
    ~OpenGLVertexBufferObject() override;
};
