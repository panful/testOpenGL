#pragma once

#include "mapper.h"
#include <array>

class OpenGLIndexBufferObject;
class OpenGLVertexBufferObjectGroup;
class OpenGLVertexArrayObject;
class OpenGLShaderProgram;

class OpenGLMapper : public Mapper
{
    enum PrimitiveTypes : size_t
    {
        PT_Point = 0,
        PT_Line,
        PT_Triangle,
        PT_Number,
    };

public:
    static OpenGLMapper* New();

    void Render(Actor*) override;

protected:
    OpenGLMapper();
    ~OpenGLMapper() override;

private:
    void BuildBufferObjects();
    void BuildAllIBOs();
    void BuildShaderProgram(Actor*);
    void BuildColorBufferObject();

private:
    std::array<OpenGLIndexBufferObject*, PT_Number> m_primitives {};
    OpenGLVertexBufferObjectGroup* m_vbos { nullptr };
    OpenGLVertexArrayObject* m_vao { nullptr };
    OpenGLShaderProgram* m_shaderProgram { nullptr };
};
