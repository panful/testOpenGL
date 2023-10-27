#pragma once

#include "mapper.h"
#include "smartpointer.h"

class OpenGLIndexBufferObject;
class OpenGLVertexBufferObject;
class OpenGLVertexArrayObject;
class OpenGLShaderProgram;

class OpenGLMapper : public Mapper
{
public:
    static OpenGLMapper* New();

    void Render() override;

protected:
    OpenGLMapper();
    ~OpenGLMapper();
private:
    void BuildBufferObjects();
    void BuildShaderProgram();

private:
    SmartPointer<OpenGLIndexBufferObject> m_iboVertex { nullptr };
    SmartPointer<OpenGLIndexBufferObject> m_iboLine { nullptr };
    SmartPointer<OpenGLIndexBufferObject> m_iboTriangle { nullptr };
    SmartPointer<OpenGLVertexBufferObject> m_vbo { nullptr };
    SmartPointer<OpenGLVertexArrayObject> m_vao { nullptr };
    SmartPointer<OpenGLShaderProgram> m_shaderProgram { nullptr };
};
