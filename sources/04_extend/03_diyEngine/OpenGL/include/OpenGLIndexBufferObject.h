#pragma once

#include "OpenGLBufferObject.h"

class OpenGLIndexBufferObject : public OpenGLBufferObject
{
public:
    static OpenGLIndexBufferObject* New();

protected:
    OpenGLIndexBufferObject();
    ~OpenGLIndexBufferObject() override;
};
