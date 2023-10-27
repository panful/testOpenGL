#pragma once

#include "object.h"

class OpenGLShaderProgram : public Object
{
public:
    static OpenGLShaderProgram* New();

    void SetAttribute();
    void Use();

protected:
    OpenGLShaderProgram();
    ~OpenGLShaderProgram();
private:
    void Build();

private:
    uint32_t m_handle { 0 };
    bool m_needBuild { true };
};
