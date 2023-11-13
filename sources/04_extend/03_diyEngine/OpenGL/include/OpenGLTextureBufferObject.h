#pragma once

#include "OpenGLBufferObject.h"

class OpenGLTextureBufferObject : public OpenGLBufferObject
{
public:
    static OpenGLTextureBufferObject* New();

    void SetFormat(uint32_t);
    uint32_t GetFormat() const noexcept;

private:
    uint32_t m_format { 0 };

protected:
    OpenGLTextureBufferObject();
    ~OpenGLTextureBufferObject() override;
};
