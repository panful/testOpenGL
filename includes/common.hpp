///-------------------------------------------------------
///  @file      公共库hpp文件
///  @brief     初始化glfw，glad，shader，shaderProgram
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2023-2-27
///-------------------------------------------------------

#ifndef _COMMOND_HPP_
#define _COMMOND_HPP_

// glad需要在glfw之前包含

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
// clang-format on

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

class InitOpenGL
{
public:
    InitOpenGL(const std::string_view& name = "Learn OpenGL", uint32_t w = 800, uint32_t h = 600)
        : m_windowName(name)
        , m_windowWidth(w)
        , m_windowHeight(h)
        , m_window(nullptr)
    {
        Init(m_windowName, m_windowWidth, m_windowHeight);
    }

    GLFWwindow* GetWindow() const
    {
        return m_window;
    }

    void SetFramebufferSizeCB(GLFWframebuffersizefun fun) const
    {
        glfwSetFramebufferSizeCallback(m_window, fun);
    }

    void SetCursorPosCB(GLFWcursorposfun fun) const
    {
        glfwSetCursorPosCallback(m_window, fun);
    }

private:
    void Init(const std::string_view& name, uint32_t w, uint32_t h)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(w, h, name.data(), nullptr, nullptr);

        if (!m_window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

private:
    std::string_view m_windowName;
    uint32_t m_windowWidth;
    uint32_t m_windowHeight;
    GLFWwindow* m_window;
};

class Shader
{
public:
    enum class ShaderType
    {
        Vertex = 0,
        Fragment = 1,
        Geometry = 2,
    };

public:
    Shader(const std::string_view& filePath, ShaderType type)
        : m_shader(0)
    {
        CreateShader(filePath, type);
    }

    ~Shader() = default;

    GLuint GetShader() const
    {
        return m_shader;
    }
    void DeleteShader() const
    {
        glDeleteShader(m_shader);
    }

private:
    void CheckErrors(uint32_t shader) const
    {
        GLint success = 0;
        GLchar infoLog[1024] { 0 };

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::clog << "---------------------------------------\n";
            std::clog << infoLog << '\n';
        }
    }

    void CreateShader(const std::string_view& path, ShaderType t)
    {
        if (std::filesystem::exists(std::filesystem::path(path)))
        {
            std::ifstream ifs(path.data());
            std::ostringstream iss;
            iss << ifs.rdbuf();
            auto str = iss.str();
            auto shaderCode = str.c_str();

            switch (t)
            {
            case Shader::ShaderType::Vertex:
                m_shader = glCreateShader(GL_VERTEX_SHADER);
                break;
            case Shader::ShaderType::Fragment:
                m_shader = glCreateShader(GL_FRAGMENT_SHADER);
                break;
            case Shader::ShaderType::Geometry:
                m_shader = glCreateShader(GL_GEOMETRY_SHADER);
                break;
            default:
                std::clog << "shder type is invalid\n";
                break;
            }

            glShaderSource(m_shader, 1, &shaderCode, nullptr);
            glCompileShader(m_shader);
            CheckErrors(m_shader);
        }
        else
        {
            std::clog << "--------------------------------------\n"
                      << "file path is invalid\n"
                      << "current path: " << std::filesystem::current_path() << '\n'
                      << "file path: " << path << '\n';
        }
    }

private:
    GLuint m_shader;
};

class ShaderProgram
{
public:
    ShaderProgram(const std::string_view& vsPath, const std::string_view& fsPath, const std::string_view& gsPath = std::string_view())
        : m_program(0)
    {
        CreateProgram();
        AttachShader(vsPath, Shader::ShaderType::Vertex);
        AttachShader(fsPath, Shader::ShaderType::Fragment);
        AttachShader(gsPath, Shader::ShaderType::Geometry);
        LinkShader();
        CheckErrors();
    }

    GLuint GetProgram() const { return m_program; }

    void DeleteProgram() const { glDeleteProgram(m_program); }

    void Use() const { glUseProgram(m_program); }

    // GLfloat
    //----------------------------------------------------------------------
    void SetUniform1f(const std::string_view& name, GLfloat v) const
    {
        glUniform1f(glGetUniformLocation(m_program, name.data()), v);
    }

    void SetUniform2f(const std::string_view& name, GLfloat v1, GLfloat v2) const
    {
        glUniform2f(glGetUniformLocation(m_program, name.data()), v1, v2);
    }

    void SetUniform3f(const std::string_view& name, GLfloat v1, GLfloat v2, GLfloat v3) const
    {
        glUniform3f(glGetUniformLocation(m_program, name.data()), v1, v2, v3);
    }

    void SetUniform4f(const std::string_view& name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const
    {
        glUniform4f(glGetUniformLocation(m_program, name.data()), v1, v2, v3, v4);
    }

    // GLint
    //----------------------------------------------------------------------
    void SetUniform1i(const std::string_view& name, GLint v) const
    {
        glUniform1i(glGetUniformLocation(m_program, name.data()), v);
    }

    // Matrix
    //----------------------------------------------------------------------
    void SetUniformMat4(const std::string_view& name, const glm::mat4& m) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_program, name.data()), 1, GL_FALSE, glm::value_ptr(m));
    }

private:
    void CreateProgram()
    {
        m_program = glCreateProgram();
    }

    void LinkShader() const
    {
        glLinkProgram(m_program);
    }

    void CheckErrors() const
    {
        GLint success { 0 };
        GLchar infoLog[512] { 0 };
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_program, 512, NULL, infoLog);
            std::clog << "link shader program failed\n"
                      << infoLog << '\n';
        }
    }

    void AttachShader(const std::string_view& filePath, Shader::ShaderType type) const
    {
        if (!filePath.empty())
        {
            Shader vs(filePath, type);
            glAttachShader(m_program, vs.GetShader());
            vs.DeleteShader();
        }
    };

private:
    uint32_t m_program;
};

class Texture
{
public:
    Texture(const std::string_view& path)
        : m_texture(0)
        , m_width(0)
        , m_height(0)
        , m_channels(0)
    {
        glGenTextures(1, &m_texture);

        // 默认激活纹理单元0
        Active(GL_TEXTURE0);

        // 默认2D纹理
        Bind();

        // 默认环绕、过滤方式
        SetWarpParameter(GL_REPEAT, GL_REPEAT);
        SetFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        LoadImage(path);
    }
    ~Texture() = default;

public:
    void Active(GLenum texUnit) const
    {
        glActiveTexture(texUnit);
    }

    void Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void Release() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LoadImage(const std::string_view& path, bool bFlip = true)
    {
        stbi_set_flip_vertically_on_load(bFlip);

        if (auto data = stbi_load(path.data(), &m_width, &m_height, &m_channels, 0))
        {
            GLenum colorFormat { 0 };

            if (m_channels == 3)
            {
                colorFormat = GL_RGB;
            }
            else if (m_channels == 4)
            {
                colorFormat = GL_RGBA;
            }
            else
            {
                std::clog << "Image channels error!\n";
            }

            glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, m_width, m_height, 0, colorFormat, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
        else
        {
            std::clog << "Failed to load texture\n";
        }
    }

    void SetWarpParameter(GLint s, GLint t) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
    }

    void SetFilterParameter(GLint min, GLint mag) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    }

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannels() const { return m_channels; }

private:
    GLuint m_texture;
    int m_width;
    int m_height;
    int m_channels;
};

namespace ErrorImpl {
static void CheckErrorImpl(const char* info, const char* file, int line, const char* func)
{
    std::string_view strError;

    switch (glGetError())
    {
    case GL_NO_ERROR:
        strError = "NO_ERROR";
        break;
    case GL_INVALID_ENUM:
        strError = "INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        strError = "INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        strError = "INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        strError = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        strError = "OUT_OF_MEMORY";
        break;
    default:
        strError = "SOMETHING_WRONG";
        break;
    }

    std::cout << "-----------------------------------------\n"
              << "-- Infomation:\t" << info << '\n'
              << "-- File:\t" << file << '\n'
              << "-- Line:\t" << line << '\n'
              << "-- Function:\t" << func << '\n'
              << "-- Error:\t" << strError.data() << "\n\n";
}
} // namespace ErrorImpl

#define CheckError() ErrorImpl::CheckErrorImpl("", __FILE__, __LINE__, __FUNCTION__)
#define CheckErrorWithInfo(str) ErrorImpl::CheckErrorImpl(str, __FILE__, __LINE__, __FUNCTION__)

#endif // !_COMMOND_HPP_
