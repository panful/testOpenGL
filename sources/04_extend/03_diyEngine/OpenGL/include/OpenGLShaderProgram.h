#pragma once

#include "object.h"
#include <array>
#include <string>

class OpenGLShaderProgram : public Object
{
public:
    static OpenGLShaderProgram* New();

    void Use();

    void SetAttribute();
    void ReplaceColor();
    void ReplaceColor(const std::array<double, 3>&);
    void ReplaceLight();
    void ReplaceNormal();
    void ReplaceTexCoord();

protected:
    OpenGLShaderProgram();
    ~OpenGLShaderProgram() override;

private:
    void Build();
    void ReplaceValue(std::string&, const std::string&, const std::string&, const std::string&);
    void ReplaceValue(std::string&, const std::string&, const std::string&);

private:
    uint32_t m_handle { 0 };
    bool m_needBuild { true };
    std::string m_vsCode;
    std::string m_fsCode;
};
