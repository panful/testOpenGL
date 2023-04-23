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

    // 窗口缩放
    void SetFramebufferSizeCB(GLFWframebuffersizefun fun) const
    {
        glfwSetFramebufferSizeCallback(m_window, fun);
    }

    // 光标位置
    void SetCursorPosCB(GLFWcursorposfun fun) const
    {
        glfwSetCursorPosCallback(m_window, fun);
    }

    // 键盘按下抬起
    void SetKeyCB(GLFWkeyfun fun) const
    {
        glfwSetKeyCallback(m_window, fun);
    }

    // 鼠标滚轮
    void SetScrollCB(GLFWscrollfun fun) const
    {
        glfwSetScrollCallback(m_window, fun);
    }

    // 鼠标按下抬起
    void SetMouseCB(GLFWmousebuttonfun fun) const
    {
        glfwSetMouseButtonCallback(m_window, fun);
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
        GLint success { 0 };
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLint logLength { 0 };
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

            auto infoLog = new char[logLength]();
            glGetShaderInfoLog(shader, logLength, nullptr, infoLog);
            std::clog << "---------------------------------------\n" << infoLog << '\n';
            delete[] infoLog;
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

    GLuint GetProgram() const
    {
        return m_program;
    }

    void DeleteProgram() const
    {
        glDeleteProgram(m_program);
    }

    void Use() const
    {
        glUseProgram(m_program);
    }

    // GLfloat
    //----------------------------------------------------------------------
    void SetUniform1f(const std::string_view& name, GLfloat v) const
    {
        glUniform1f(Location(name), v);
    }

    void SetUniform2f(const std::string_view& name, GLfloat v1, GLfloat v2) const
    {
        glUniform2f(Location(name), v1, v2);
    }

    void SetUniform2fv(const std::string_view& name, glm::vec2 v) const
    {
        glUniform2f(Location(name), v.x, v.y);
    }

    void SetUniform3f(const std::string_view& name, GLfloat v1, GLfloat v2, GLfloat v3) const
    {
        glUniform3f(Location(name), v1, v2, v3);
    }

    void SetUniform3fv(const std::string_view& name, glm::vec3 v) const
    {
        glUniform3f(Location(name), v.x, v.y, v.z);
    }

    void SetUniform4f(const std::string_view& name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4) const
    {
        glUniform4f(Location(name), v1, v2, v3, v4);
    }

    void SetUniform4fv(const std::string_view& name, glm::vec4 v) const
    {
        glUniform4f(Location(name), v.x, v.y, v.z, v.w);
    }

    // GLint
    //----------------------------------------------------------------------
    void SetUniform1i(const std::string_view& name, GLint v) const
    {
        glUniform1i(Location(name), v);
    }

    // Matrix
    //----------------------------------------------------------------------
    void SetUniformMat4(const std::string_view& name, const glm::mat4& m) const
    {
        // glUniformMatrix4fv的参数：
        // 1.uniform的位置值。
        // 2.告诉OpenGL我们将要发送多少个矩阵
        // 3.是否对矩阵进行置换(Transpose)，也就是说交换矩阵的行和列。
        //      OpenGL开发者通常使用一种内部矩阵布局，叫做列主序(Column-major Ordering)布局。
        //      GLM的默认布局就是列主序，所以并不需要置换矩阵，我们填GL_FALSE。
        // 4.矩阵数据，但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，因此要先用GLM的自带的函数value_ptr来变换这些数据。
        glUniformMatrix4fv(Location(name), 1, GL_FALSE, glm::value_ptr(m));
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
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLint logLength { 0 };
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

            auto infoLog = new char[logLength]();
            glGetProgramInfoLog(m_program, logLength, nullptr, infoLog);
            std::clog << "---------------------------------------\n"
                      << "Link shader program failed\n"
                      << infoLog << '\n';
            delete[] infoLog;
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

    GLint Location(const std::string_view& name) const
    {
        auto location = glGetUniformLocation(m_program, name.data());

        if (-1 == location)
        {
            throw std::runtime_error("Wrong of uniform name");
        }

        return location;
    }

private:
    uint32_t m_program;
};

class Texture
{
public:
    Texture(const std::string_view& path, GLuint textureUnit = 0)
        : m_texture(0)
        , m_width(0)
        , m_height(0)
        , m_colorFormat(0)
        , m_textureUnit(textureUnit)
    {
        glGenTextures(1, &m_texture);

        // 默认激活纹理单元0
        Active(m_textureUnit);

        // 默认2D纹理
        Bind();

        // 默认环绕、过滤方式
        SetWarpParameter(GL_REPEAT, GL_REPEAT);
        SetFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        // 将图像数据加载到纹理中
        LoadImage(path);

        // 取消纹理绑定
        Release();
    }

    Texture(GLsizei w, GLsizei h, GLenum format = GL_RGB, GLuint textureUnit = 0)
        : m_texture(0)
        , m_width(w)
        , m_height(h)
        , m_colorFormat(format)
        , m_textureUnit(textureUnit)
    {
        glGenTextures(1, &m_texture);

        // 默认激活纹理单元0
        Active(m_textureUnit);

        // 默认2D纹理
        Bind();

        // 创建一个图像数据为空的纹理
        CreateNullTexture();

        // 默认环绕、过滤方式
        SetWarpParameter(GL_REPEAT, GL_REPEAT);
        SetFilterParameter(GL_LINEAR, GL_LINEAR);

        // 取消纹理绑定
        Release();
    }

    ~Texture() = default;

private:
    GLuint m_texture;
    GLsizei m_width;
    GLsizei m_height;
    GLenum m_colorFormat;
    GLuint m_textureUnit;

public:
    GLuint Get() const
    {
        return m_texture;
    }

    void Bind(GLuint textureUnit = 0) const
    {
        Active(m_textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void Release() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void LoadImage(const std::string_view& path, bool bFlip = true)
    {
        stbi_set_flip_vertically_on_load(bFlip);
        int channels { 0 };
        if (auto data = stbi_load(path.data(), &m_width, &m_height, &channels, 0))
        {
            if (channels == 3)
            {
                m_colorFormat = GL_RGB;
            }
            else if (channels == 4)
            {
                m_colorFormat = GL_RGBA;
            }
            else
            {
                std::clog << "Image channels error!\n";
            }

            glTexImage2D(GL_TEXTURE_2D, 0, m_colorFormat, m_width, m_height, 0, m_colorFormat, GL_UNSIGNED_BYTE, data);
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

    void ResetSize(GLsizei w, GLsizei h)
    {
        m_width = w;
        m_height = h;

        CreateNullTexture();
    }

    GLsizei GetWidth() const
    {
        return m_width;
    }
    GLsizei GetHeight() const
    {
        return m_height;
    }
    GLenum GetColorFormat() const
    {
        return m_colorFormat;
    }

private:
    void CreateNullTexture() const
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_colorFormat, m_width, m_height, 0, m_colorFormat, GL_UNSIGNED_BYTE, NULL);
    }

    void Active(GLuint texUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + texUnit);
    }
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
