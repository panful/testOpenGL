#pragma once

#include "object.h"
#include <glad/glad.h>
#include <vector>

class OpenGLBufferObject : public Object
{
    struct Internal
    {
        uint32_t Handle { 0 };
        uint32_t Target { 0 };
    };

public:
    static OpenGLBufferObject* New();

    void Bind();
    void UnBind();

    uint32_t GetHandle() const noexcept;

    template <typename T>
    void UpLoad(const std::vector<T>& data);

protected:
    OpenGLBufferObject();
    ~OpenGLBufferObject() override;

    Internal m_internal;
};

template <typename T>
void OpenGLBufferObject::UpLoad(const std::vector<T>& data)
{
    glBufferData(m_internal.Target, sizeof(T) * data.size(), static_cast<const void*>(data.data()), GL_STATIC_DRAW);
}