#pragma once
#include "glad/glad.h"

#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
private:
    unsigned int m_vertexShader, m_fragmentShader, m_shaderProgram;
    int m_success;
    char m_infoLog[512];
public:
    Shader(/*const std::string& vs, const std::string& fs*/);
    ~Shader();

    void use() const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const glm::mat4& mat)const;
    //set Uniform ...

private:
    void checkShaderError(unsigned int shader, std::string type);
};

Shader::Shader(/*const std::string& vs, const std::string& fs*/)
    :m_vertexShader(0),
    m_fragmentShader(0),
    m_shaderProgram(0)
{
    //const char* tempVS = vs.c_str();
    //const char* tempFS = fs.c_str();

    const char* tempVS = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 fColor;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 model;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        //"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   fColor = aColor;\n"
        "}\0";
    const char* tempFS = "#version 330 core\n"
        "in vec3 fColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor =  vec4(fColor.r, fColor.g, fColor.b, 1.0f);\n"
        "}\n\0";

    //vertex shader
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &tempVS, nullptr);
    glCompileShader(m_vertexShader);
    checkShaderError(m_vertexShader, "VERTEX");

    //fragment shader
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &tempFS, nullptr);
    glCompileShader(m_fragmentShader);
    checkShaderError(m_fragmentShader, "FRAGMENT");

    //Program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);
    checkShaderError(m_shaderProgram, "PROGRAM");

    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderProgram);
}

void Shader::checkShaderError(unsigned int shader, std::string type)
{
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &m_success);
        if (!m_success)
        {
            glGetShaderInfoLog(shader, 512, NULL, m_infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << m_infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &m_success);
        if (!m_success)
        {
            glGetProgramInfoLog(shader, 512, NULL, m_infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << m_infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Shader::use() const
{
    glUseProgram(m_shaderProgram);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUseProgram(m_shaderProgram);
    glUniform3f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y, z);
}

inline void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
