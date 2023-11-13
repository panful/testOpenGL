#pragma once

#include "object.h"

class OpenGLTextureBufferObject;

class OpenGLTexture : public Object
{
    struct Internal
    {
        uint32_t Handle { 0 };
        uint32_t Target { 0 };
    };

public:
    static OpenGLTexture* New();

    void SetTarget(uint32_t);

    void LoadBuffer(OpenGLTextureBufferObject*);

    void Bind();
    void UnBind();

protected:
    OpenGLTexture();
    ~OpenGLTexture() override;

private:
    Internal m_internal {};
    OpenGLTextureBufferObject* m_textureBufferObject { nullptr };
};
