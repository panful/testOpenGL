#include "OpenGLShaderProgram.h"
#include "log.h"
#include "objectFactory.h"
#include <glad/glad.h>
#include <string>

StandardNewMacro(OpenGLShaderProgram);

OpenGLShaderProgram::OpenGLShaderProgram()
{
    LogDebug("");
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

void OpenGLShaderProgram::Build()
{
    std::string vsCode = R"(
        #version 330 core                     
        layout (location = 0) in vec3 aPos;                    
        void main()
        {                                                     
            gl_Position = vec4(aPos, 1.f);    
        }
    )";

    std::string fsCode = R"(
        #version 330 core
        void main()
        {
            gl_FragColor = vec4(0.f, 1.f, 0.f, 1.f);
        }
    )";

    auto vsSource = vsCode.data();
    auto fsSource = fsCode.data();

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