#pragma once

#include "mapper.h"

class OpenGLIndexBufferObject;
class OpenGLVertexBufferObject;
class OpenGLVertexArrayObject;
class OpenGLShaderProgram;

class OpenGLMapper : public Mapper
{
public:
    static OpenGLMapper* New();

    void Render(Actor*) override;

protected:
    OpenGLMapper();
    ~OpenGLMapper() override;
private:
    void BuildBufferObjects();
    void BuildShaderProgram(Actor*);

private:
    OpenGLIndexBufferObject* m_iboVertex { nullptr };
    OpenGLIndexBufferObject* m_iboLine { nullptr };
    OpenGLIndexBufferObject* m_iboTriangle { nullptr };
    OpenGLVertexBufferObject* m_vbo { nullptr };
    OpenGLVertexArrayObject* m_vao { nullptr };
    OpenGLShaderProgram* m_shaderProgram { nullptr };
};
