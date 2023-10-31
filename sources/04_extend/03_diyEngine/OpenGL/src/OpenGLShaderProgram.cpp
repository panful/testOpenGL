#include "OpenGLShaderProgram.h"
#include "log.h"
#include "objectFactory.h"
#include <format>
#include <glad/glad.h>
#include <string>

StandardNewMacro(OpenGLShaderProgram);

OpenGLShaderProgram::OpenGLShaderProgram()
{
    LogDebug("");
    m_vsCode = R"(
        #version 330 core                     
        layout (location = 0) in vec3 aPos;                    
        void main()
        {                                                     
            gl_Position = vec4(aPos, 1.f);    
        }
    )";

    m_fsCode = R"(
        #version 330 core

        // Shader::Color Start
        const vec3 color = vec3(0.f, 1.f, 0.f);
        // Shader::Color End

        void main()
        {
            gl_FragColor = vec4(color, 1.f);
        }
    )";
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    LogDebug("");
}

void OpenGLShaderProgram::Use()
{
    if (m_needBuild)
    {
        Build();
        m_needBuild = false;
    }
    glUseProgram(m_handle);
}

void OpenGLShaderProgram::SetAttribute()
{
}

void OpenGLShaderProgram::ReplaceColor(const std::array<double, 3>& color)
{
    auto strColor = std::format("const vec3 color = vec3({}, {}, {});\n", color[0], color[1], color[2]);
    ReplaceValue(m_fsCode, "// Shader::Color Start\n", "// Shader::Color End\n", strColor);
}

void OpenGLShaderProgram::ReplaceLight()
{
}

void OpenGLShaderProgram::ReplaceNormal()
{
}

void OpenGLShaderProgram::ReplaceTexCoord()
{
}

void OpenGLShaderProgram::ReplaceValue(std::string& str, const std::string& start, const std::string& end, const std::string& replacement)
{
    size_t startPos = str.find(start);
    size_t endPos   = str.find(end);

    if (startPos != std::string::npos && endPos != std::string::npos)
    {
        size_t replaceLength = endPos - startPos + end.length();
        str.replace(startPos, replaceLength, start + replacement + end);
    }
    else
    {
        LogError("Shader text replacement error");
    }
}

void OpenGLShaderProgram::Build()
{
    auto vsSource = m_vsCode.data();
    auto fsSource = m_fsCode.data();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsSource, NULL);
    glCompileShader(vertexShader);

    int success { 0 };
    char infoLog[512] {};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::runtime_error(std::string(infoLog));
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw std::runtime_error(std::string(infoLog));
    }

    m_handle = glCreateProgram();
    glAttachShader(m_handle, vertexShader);
    glAttachShader(m_handle, fragmentShader);
    glLinkProgram(m_handle);

    glGetProgramiv(m_handle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_handle, 512, NULL, infoLog);
        throw std::runtime_error(std::string(infoLog));
    }

    glDetachShader(m_handle, vertexShader);
    glDetachShader(m_handle, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}