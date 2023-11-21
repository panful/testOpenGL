#pragma once

#include "mapper.h"
#include <array>
#include <vector>

class OpenGLIndexBufferObject;
class OpenGLVertexBufferObjectGroup;
class OpenGLVertexArrayObject;
class OpenGLShaderProgram;
class OpenGLTexture;
class DataArray;

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

    void Render(Renderer*, Actor*) override;

protected:
    OpenGLMapper();
    ~OpenGLMapper() override;

private:
    void BuildBufferObjects();
    void BuildAllIBOs();
    void BuildShaderProgram(Actor*);
    void BuildColorBufferObject();
    void BuildCellColorTextures(PrimitiveTypes, const std::vector<float>&);
    void SetShaderParameters(Renderer*);

private:
    std::array<OpenGLIndexBufferObject*, PT_Number> m_primitives {};
    OpenGLVertexBufferObjectGroup* m_vbos { nullptr };
    OpenGLVertexArrayObject* m_vao { nullptr };
    OpenGLShaderProgram* m_shaderProgram { nullptr };
    std::array<OpenGLTexture*, PT_Number> m_internalColorTextures {};
};
