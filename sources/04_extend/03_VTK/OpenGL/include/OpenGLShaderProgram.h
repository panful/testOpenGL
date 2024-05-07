#pragma once

#include "object.h"
#include <array>
#include <string>

class OpenGLShaderProgram : public Object
{
public:
    static OpenGLShaderProgram* New();

    void Use();

    void ReplacePointColor();
    void ReplaceCellColor();
    void ReplacePrimitiveColor(const std::array<double, 3>&);
    void ReplaceLight();
    void ReplaceNormal();
    void ReplaceTexCoord();

    void SetUniform1f(std::string_view, float);
    void SetUniform3f(std::string_view, float, float, float);
    void SetUniformMat4(std::string_view, float*);

protected:
    OpenGLShaderProgram();
    ~OpenGLShaderProgram() override;

private:
    void Build();
    void ReplaceValue(std::string&, const std::string&, const std::string&, const std::string&);
    void ReplaceValue(std::string&, const std::string&, const std::string&);
    int GetUniformLocation(std::string_view);

private:
    uint32_t m_handle { 0 };
    bool m_needBuild { true };
    std::string m_vsCode;
    std::string m_fsCode;
};
