#include "GraphicsPipeline.h"
#include "Log.h"
#include "Visitor.h"

#include <glad/glad.h>

void GraphicsPipeline::Accept(Visitor& visitor)
{
    visitor.Apply(*this);
}

void GraphicsPipeline::SetShaderCode(const std::string_view vert, const std::string_view frag)
{
    m_vertCode = vert;
    m_fragCode = frag;
}

void GraphicsPipeline::SetShaderCode(const std::string_view vert, const std::string_view frag, const std::string_view geom)
{
    m_vertCode = vert;
    m_fragCode = frag;
    m_geomCode = geom;
}

void GraphicsPipeline::Bind() const noexcept
{
    Log::GetInstance()->Trace();

    glUseProgram(m_shaderProgramHandle);
}

void GraphicsPipeline::UnBind() const noexcept
{
    glUseProgram(0);
}

void GraphicsPipeline::Compile()
{
    Log::GetInstance()->Trace();

    auto vsSource = m_vertCode.data();
    auto fsSource = m_fragCode.data();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsSource, NULL);
    glCompileShader(vertexShader);

    int success { 0 };
    char infoLog[512] {};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        Log::GetInstance()->Error(infoLog);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        Log::GetInstance()->Error(infoLog);
    }

    m_shaderProgramHandle = glCreateProgram();
    glAttachShader(m_shaderProgramHandle, vertexShader);
    glAttachShader(m_shaderProgramHandle, fragmentShader);
    glLinkProgram(m_shaderProgramHandle);

    glGetProgramiv(m_shaderProgramHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shaderProgramHandle, 512, NULL, infoLog);
        Log::GetInstance()->Error(infoLog);
    }

    glDetachShader(m_shaderProgramHandle, vertexShader);
    glDetachShader(m_shaderProgramHandle, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
