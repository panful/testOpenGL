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

        // Layout Color

        // Layout Normal

        // Layout TCoord

        void main()
        {
            // Shader::VSOUT Start
            gl_Position = vec4(aPos, 1.f);
            // Shader::VSOUT End
        }
    )";

    m_fsCode = R"(
        #version 330 core

        // FS_IN

        // Shader::FSColor Start
        const vec3 color = vec3(0.f, 1.f, 0.f);
        // Shader::FSColor End

        out vec4 FragColor;

        void main()
        {
            // Shader::FSOUT Start
            FragColor = vec4(color, 1.f);
            // Shader::FSOUT End
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

/// @brief 设置图元每个顶点的颜色
void OpenGLShaderProgram::ReplaceColor()
{
    ReplaceValue(m_vsCode, "// Layout Color", "layout (location = 1) in vec3 aColor;\nout vec3 vsOutColor;");
    ReplaceValue(m_vsCode, "// Shader::VSOUT Start\n", "// Shader::VSOUT End", "gl_Position = vec4(aPos, 1.f);\nvsOutColor = aColor;\n");
    ReplaceValue(m_fsCode, "// Shader::FSColor Start\n", "// Shader::FSColor End", "in vec3 vsOutColor;\n");
    ReplaceValue(m_fsCode, "// Shader::FSOUT Start\n", "// Shader::FSOUT End", "FragColor = vec4(vsOutColor, 1.f);\n");
}

/// @brief 设置整个Actor的颜色
/// @param color
void OpenGLShaderProgram::ReplaceColor(const std::array<double, 3>& color)
{
    auto strColor = std::format("const vec3 color = vec3({}, {}, {});", color[0], color[1], color[2]);
    ReplaceValue(m_fsCode, "// Shader::FSColor Start\n", "// Shader::FSColor End", strColor);
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

/// @brief 将str中start到end之间的字符全部使用replacement替换，start和end保留
/// @param str
/// @param start
/// @param end
/// @param replacement
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

/// @brief 将str中的oldStr使用newStr替换
/// @param str
/// @param oldStr
/// @param newStr
void OpenGLShaderProgram::ReplaceValue(std::string& str, const std::string& oldStr, const std::string& newStr)
{
    if (auto pos = str.find(oldStr); pos != std::string::npos)
    {
        str.replace(pos, oldStr.length(), newStr);
    }
}

void OpenGLShaderProgram::Build()
{
    //LogTrace(m_vsCode);
    //LogTrace(m_fsCode);

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
        LogError(infoLog);
        throw std::runtime_error(std::string(infoLog));
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LogError(infoLog);
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
        LogError(infoLog);
        throw std::runtime_error(std::string(infoLog));
    }

    glDetachShader(m_handle, vertexShader);
    glDetachShader(m_handle, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}