#ifndef _SHADER_HPP_
#define _SHADER_HPP_

#include <glad/glad.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

class Shader
{
private:
    enum class ShaderType
    {
        Vertex = 0,
        Fragment = 1,
        Geometry = 2,
        Program = 3
    };

public:
    Shader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath = std::string());
    ~Shader() = default;

public:
    uint32_t GetProgram();

private:
    void CheckCompileErrors(uint32_t shader, ShaderType type);

private:
    uint32_t m_vertexShader;
    uint32_t m_fragmentShader;
    uint32_t m_geometryShader;

    uint32_t m_shaderProgram;
};

Shader::Shader(const std::string& vsPath, const std::string& fsPath, const std::string& gsPath)
    : m_vertexShader(0)
    , m_fragmentShader(0)
    , m_geometryShader(0)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // 确保fstream可以抛出异常
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // 加载着色器资源
    try
    {
        vShaderFile.open(vsPath);
        fShaderFile.open(fsPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // 如果几何着色器资源不为空
        if (!gsPath.empty())
        {
            gShaderFile.open(gsPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "---------------------------------------\n";
        std::cout << "       Load shader source failed!      \n";
        std::cout << std::filesystem::current_path() << '\n';
        std::cout << e.what() << '\n';
        std::cout << "---------------------------------------\n\n";
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 编译着色器源码
    uint32_t vertex = 0, fragment = 0, geometry = 0;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, ShaderType::Vertex);

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, ShaderType::Fragment);

    // geometry shader
    if (!geometryCode.empty())
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        CheckCompileErrors(geometry, ShaderType::Geometry);
    }

    // shader Program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertex);
    glAttachShader(m_shaderProgram, fragment);
    if (!geometryCode.empty())
        glAttachShader(m_shaderProgram, geometry);
    glLinkProgram(m_shaderProgram);
    CheckCompileErrors(m_shaderProgram, ShaderType::Program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (!geometryCode.empty())
        glDeleteShader(geometry);
}

uint32_t Shader::GetProgram()
{
    return m_shaderProgram;
}

void Shader::CheckCompileErrors(uint32_t shader, ShaderType type)
{
    int32_t success = 0;
    char infoLog[1024];

    switch (type)
    {
    case Shader::ShaderType::Vertex:
    case Shader::ShaderType::Fragment:
    case Shader::ShaderType::Geometry:
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "------------ Shader Error  ------------\n";
            std::cout << infoLog << '\n';
            std::cout << "------------ Shader Error  ------------\n\n";
        }
        break;
    case Shader::ShaderType::Program:
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "------------ Program Error ------------\n";
            std::cout << infoLog << '\n';
            std::cout << "------------ Program Error ------------\n\n";
        }
        break;
    default:
        break;
    }
}

#endif // !_SHADER_HPP_
