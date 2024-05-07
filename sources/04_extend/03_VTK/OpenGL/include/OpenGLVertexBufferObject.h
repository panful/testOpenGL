#pragma once

#include "OpenGLBufferObject.h"
#include "log.h"

class OpenGLVertexBufferObject : public OpenGLBufferObject
{
public:
    static OpenGLVertexBufferObject* New();

    template <typename T>
    void EnableVertexAttributePointer(uint32_t index, int size, uint8_t normalized) const noexcept;

private:
    template <typename T>
    uint32_t ConvertToOpenGLType() const noexcept;

protected:
    OpenGLVertexBufferObject();
    ~OpenGLVertexBufferObject() override;
};

template <typename T>
void OpenGLVertexBufferObject::EnableVertexAttributePointer(uint32_t index, int size, uint8_t normalized) const noexcept
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, ConvertToOpenGLType<T>(), normalized, size * sizeof(T), static_cast<void*>(0));
}