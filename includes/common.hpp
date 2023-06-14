///-------------------------------------------------------
///  @file      公共库hpp文件
///  @brief     初始化glfw，glad，shader，shaderProgram
///  @version   V1.0
///  @copyright Leo
///  @author    Leo
///  @date      2023-2-27
///-------------------------------------------------------

#pragma once

// glad需要在glfw之前包含
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

class Camera
{
public:
    constexpr Camera() noexcept = default;
    ~Camera() noexcept          = default;

    Camera(const std::array<float, 3>& pos, const std::array<float, 3>& vp, const std::array<float, 3>& fp) noexcept
        : m_position({ pos[0], pos[1], pos[2] })
        , m_viewUp({ vp[0], vp[1], vp[2] })
        , m_focalPoint({ fp[0], fp[1], fp[2] })
        , m_resetPosition({ pos[0], pos[1], pos[2] })
        , m_resetViewUp({ vp[0], vp[1], vp[2] })
        , m_resetFocalPoint({ fp[0], fp[1], fp[2] })
    {
    }

    Camera(const float* pos, const float* vp, const float* fp) noexcept
        : m_position({ pos[0], pos[1], pos[2] })
        , m_viewUp({ vp[0], vp[1], vp[2] })
        , m_focalPoint({ fp[0], fp[1], fp[2] })
        , m_resetPosition({ pos[0], pos[1], pos[2] })
        , m_resetViewUp({ vp[0], vp[1], vp[2] })
        , m_resetFocalPoint({ fp[0], fp[1], fp[2] })
    {
    }

    Camera(const std::array<float, 3>& pos) noexcept
        : m_position({ pos[0], pos[1], pos[2] })
        , m_resetPosition({ pos[0], pos[1], pos[2] })
    {
    }

    Camera(const float* pos) noexcept
        : m_position({ pos[0], pos[1], pos[2] })
        , m_resetPosition({ pos[0], pos[1], pos[2] })
    {
    }

    // 缩放（拉近或拉远相机）
    void Dolly(float value)
    {
        if (value <= 0.f)
        {
            return;
        }

        auto direction = glm::normalize(m_position - m_focalPoint);
        auto distance  = direction * (value - 1.f);
        auto x         = m_position[0] + distance.x;
        auto y         = m_position[1] + distance.y;
        auto z         = m_position[2] + distance.z;

        this->SetPosition({ x, y, z });
    }

    // 绕viewUp旋转（y轴）
    void Azimuth(float angle)
    {
        auto mat   = glm::translate(glm::mat4(1.f), m_focalPoint);
        mat        = glm::rotate(mat, glm::radians(angle), m_viewUp);
        mat        = glm::translate(mat, -m_focalPoint);
        auto pos   = mat * glm::vec4(m_position, 1.f);
        m_position = glm::vec3(pos);
    }

    // 绕viewUp和相机位置到焦点的向量的正交轴旋转（x轴）
    void Elevation(float angle)
    {
        auto axis = glm::normalize(glm::cross(m_viewUp, m_position - m_focalPoint));
        auto mat  = glm::translate(glm::mat4(1.f), m_focalPoint);
        mat       = glm::rotate(mat, glm::radians(angle), axis);
        mat       = glm::translate(mat, -m_focalPoint);

        auto pos   = mat * glm::vec4(m_position, 1.f);
        m_position = glm::vec3(pos);

        // 调整viewUp，不能使用矩阵对viewUp做变换
        m_viewUp = glm::normalize(glm::cross(axis, m_focalPoint - m_position));

        // 使用矩阵变换viewUp的错误示例
        // auto vp = mat * glm::vec4(m_viewUp, 1.f);
        // m_viewUp = -glm::normalize(glm::vec3(vp));
    }

    void Reset()
    {
        m_position   = m_resetPosition;
        m_focalPoint = m_resetFocalPoint;
        m_viewUp     = m_resetViewUp;
    }

    std::array<float, 3> GetPosition() const
    {
        return { m_position.x, m_position.y, m_position.z };
    }

    void SetPosition(const std::array<float, 3>& pos)
    {
        m_position = { pos[0], pos[1], pos[2] };
    }

    std::array<float, 3> GetViewUp() const
    {
        return { m_viewUp.x, m_viewUp.y, m_viewUp.z };
    }

    void SetViewUp(const std::array<float, 3>& vp)
    {
        m_viewUp = { vp[0], vp[1], vp[2] };
    }

    std::array<float, 3> GetFocalPoint() const
    {
        return { m_focalPoint[0], m_focalPoint[1], m_focalPoint[2] };
    }

    void SetFocalPoint(const std::array<float, 3>& fp)
    {
        m_focalPoint = { fp[0], fp[1], fp[2] };
    }

    void SetAspect(float aspect)
    {
        m_aspect = aspect;
    }

    glm::mat4 GetVPMatrix() const
    {
        auto view       = glm::lookAt(m_position, m_focalPoint, m_viewUp);
        auto projection = glm::perspective(glm::radians(m_viewAngle), m_aspect, m_nearPlane, m_farPlane);

        // std::cout << "---------------------------------------------\n"
        //           << "position:\t" << m_position.x << '\t' << m_position.x << '\t' << m_position.x << '\n'
        //           << "focalPoint:\t" << m_focalPoint.x << '\t' << m_focalPoint.x << '\t' << m_focalPoint.x << '\n'
        //           << "viewUp:\t" << m_viewUp.x << '\t' << m_viewUp.x << '\t' << m_viewUp.x << '\n'
        //           << "viewAngle:\t" << m_viewAngle << '\n'
        //           << "aspect:\t" << m_aspect << '\n'
        //           << "nearFarPlane:\t" << m_nearPlane << '\t' << m_farPlane << '\n';

        return projection * view;
    }

    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(m_position, m_focalPoint, m_viewUp);
    }

    glm::mat4 GetProjectionMatrix() const
    {
        return glm::perspective(glm::radians(m_viewAngle), m_aspect, m_nearPlane, m_farPlane);
    }

private:
    glm::vec3 m_position { 0.f, 0.f, 1.f };
    glm::vec3 m_viewUp { 0.f, 1.f, 0.f };
    glm::vec3 m_focalPoint { 0.f, 0.f, 0.f };

    glm::vec3 m_resetPosition { 0.f, 0.f, 1.f };
    glm::vec3 m_resetFocalPoint { 0.f, 0.f, 0.f };
    glm::vec3 m_resetViewUp { 0.f, 1.f, 0.f };

    float m_nearPlane { 0.1f };
    float m_farPlane { 100.0f };
    float m_aspect { 1.f };
    float m_viewAngle { 30.f };
};

class Interactor
{
    friend class InitOpenGL;

private:
    enum class Interaction
    {
        NONE     = 0,
        PANNING  = 1,
        ZOOMING  = 2,
        ROTATING = 3,
    };

protected:
    void OnLeftButtonDown()
    {
        if (Interaction::NONE == m_interaction)
        {
            m_interaction = Interaction::PANNING;
        }
    }

    void OnLeftButtonUp()
    {
        m_interaction = Interaction::NONE;
    }

    void OnRightButtonDown()
    {
        if (Interaction::NONE == m_interaction)
        {
            m_interaction = Interaction::ROTATING;
        }
    }

    void OnRightButtonUp()
    {
        m_interaction = Interaction::NONE;
    }

    void OnMiddleButtonDown()
    {
        if (m_midButtonCallback)
        {
            m_midButtonCallback(m_cursorPosition[0], m_cursorPosition[1]);
        }
    }

    void OnMiddleButtonUp()
    {
    }

    void OnMouseMove()
    {
        auto dx = m_cursorPosition[0] - m_lastCursorPosition[0];
        auto dy = m_cursorPosition[1] - m_lastCursorPosition[1];

        switch (m_interaction)
        {
        case Interaction::PANNING:
        {
            auto viewMat = m_camera.GetViewMatrix();

            // 平移的距离
            auto x = static_cast<float>(dx) / static_cast<float>(m_windowSize[0]) * 2.f;
            auto y = static_cast<float>(dy) / static_cast<float>(m_windowSize[1]) * 2.f;

            // 平移只对x和y进行变化（当+x朝右、+y朝上、+z朝外时直接对x和y进行加减就行）
            // 当朝向不是默认方向时，需要先求出改变朝向后的平移方向
            auto xVec = glm::vec4(1.f, 0.f, 0.f, 1.f) * viewMat;
            auto yVec = glm::vec4(0.f, 1.f, 0.f, 1.f) * viewMat;

            auto xx = xVec * x;
            auto yy = yVec * y;

            auto lastPosition   = m_camera.GetPosition();
            auto lastFocalPoint = m_camera.GetFocalPoint();

            m_camera.SetPosition({ lastPosition[0] - xx.x + yy.x, lastPosition[1] - xx.y + yy.y, lastPosition[2] - xx.z + yy.z });
            m_camera.SetFocalPoint({ lastFocalPoint[0] - xx.x + yy.x, lastFocalPoint[1] - xx.y + yy.y, lastFocalPoint[2] - xx.z + yy.z });
        }
        break;
        case Interaction::ROTATING:
        {
            auto delta_elevation = -20.f / m_windowSize[1];
            auto delta_azimuth   = -20.f / m_windowSize[0];
            auto rx              = dx * delta_azimuth * m_motionFactor;
            auto ry              = dy * delta_elevation * m_motionFactor;

            m_camera.Azimuth(rx);
            m_camera.Elevation(ry);
        }
        break;
        case Interaction::ZOOMING:
            break;
        default:
            break;
        }
    }

    void OnMouseWheelForward()
    {
        m_camera.Dolly(1.1f);
    }

    void OnMouseWheelBackward()
    {
        m_camera.Dolly(0.9f);
    }

    void OnChar()
    {
        switch (m_keyCode)
        {
        case 'r':
        case 'R':
            m_camera.Reset();
            break;
        default:
            break;
        }

        if (m_keyCallback)
        {
            m_keyCallback(m_keyCode);
        }
    }

    void SetCursorPosition(const std::array<int, 2>& pos)
    {
        static std::once_flag initLastPos;
        std::call_once(initLastPos,
            [this, pos]()
            {
                this->m_lastCursorPosition = pos;
                this->m_cursorPosition     = pos;
            });

        m_lastCursorPosition = m_cursorPosition;
        m_cursorPosition     = pos;
    }

    void SetKeyCode(char key)
    {
        m_keyCode = key;
    }

    void SetWindowSize(const std::array<int, 2>& size)
    {
        assert(0 != size[1]);

        m_windowSize = size;
        m_camera.SetAspect(static_cast<float>(size[0]) / static_cast<float>(size[1]));
    }

    Camera GetCamera() const
    {
        return m_camera;
    }

    void SetCamera(const Camera& camera)
    {
        m_camera = camera;
    }

    void SetKeyCallback(std::function<void(char)>&& cb)
    {
        m_keyCallback = cb;
    }

    void SetMiddleButtonCallback(std::function<void(int, int)>&& cb)
    {
        m_midButtonCallback = cb;
    }

private:
    Camera m_camera {};
    Interaction m_interaction { Interaction::NONE };
    std::array<int, 2> m_lastCursorPosition { 0, 0 };
    std::array<int, 2> m_cursorPosition { 0, 0 };
    std::array<int, 2> m_windowSize { 0, 0 };
    float m_motionFactor { 10.f };
    char m_keyCode { 0 };
    std::function<void(char)> m_keyCallback {};
    std::function<void(int, int)> m_midButtonCallback {};
};

class InitOpenGL
{
public:
    InitOpenGL() noexcept
    {
        m_interactor.SetCamera(Camera());
        m_interactor.SetWindowSize({ m_windowWidth, m_windowHeight });
        Init(m_windowName, m_windowWidth, m_windowHeight);
        InitEvent();
    }

    InitOpenGL(const Camera& camera) noexcept
    {
        m_interactor.SetCamera(camera);
        m_interactor.SetWindowSize({ m_windowWidth, m_windowHeight });
        Init(m_windowName, m_windowWidth, m_windowHeight);
        InitEvent();
    }

    InitOpenGL(const std::string_view& name, uint32_t w, uint32_t h) noexcept
        : m_windowName(name)
        , m_windowWidth(w)
        , m_windowHeight(h)
    {
        m_interactor.SetCamera(Camera());
        m_interactor.SetWindowSize({ m_windowWidth, m_windowHeight });
        Init(m_windowName, m_windowWidth, m_windowHeight);
        InitEvent();
    }

    InitOpenGL(const std::string_view& name, uint32_t w, uint32_t h, const Camera& camera) noexcept
        : m_windowName(name)
        , m_windowWidth(w)
        , m_windowHeight(h)
    {
        m_interactor.SetCamera(camera);
        m_interactor.SetWindowSize({ m_windowWidth, m_windowHeight });
        Init(m_windowName, m_windowWidth, m_windowHeight);
        InitEvent();
    }

    GLFWwindow* GetWindow() const
    {
        return m_window;
    }

    // 窗口缩放
    [[deprecated]] void SetFramebufferSizeCB(GLFWframebuffersizefun fun) const
    {
        glfwSetFramebufferSizeCallback(m_window, fun);
    }

    // 光标位置
    [[deprecated]] void SetCursorPosCB(GLFWcursorposfun fun) const
    {
        glfwSetCursorPosCallback(m_window, fun);
    }

    // 键盘按下抬起
    [[deprecated]] void SetKeyCB(GLFWkeyfun fun) const
    {
        glfwSetKeyCallback(m_window, fun);
    }

    // 鼠标滚轮
    [[deprecated]] void SetScrollCB(GLFWscrollfun fun) const
    {
        glfwSetScrollCallback(m_window, fun);
    }

    // 鼠标按下抬起
    [[deprecated]] void SetMouseCB(GLFWmousebuttonfun fun) const
    {
        glfwSetMouseButtonCallback(m_window, fun);
    }

    glm::mat4 GetVPMatrix() const
    {
        return m_interactor.GetCamera().GetVPMatrix();
    }

    glm::mat4 GetViewMatrix() const
    {
        return m_interactor.GetCamera().GetViewMatrix();
    }

    glm::mat4 GetProjectionMatrix() const
    {
        return m_interactor.GetCamera().GetProjectionMatrix();
    }

    glm::vec3 GetViewPosition() const
    {
        return glm::vec3(
            m_interactor.GetCamera().GetPosition()[0], m_interactor.GetCamera().GetPosition()[1], m_interactor.GetCamera().GetPosition()[2]);
    }

    // 临时用来测试交互
    // 后面可以考虑将Interactor作为基类，重写此类实现自定义交互
    void SetKeyCallback(std::function<void(char)>&& cb) const
    {
        m_interactor.SetKeyCallback(std::move(cb));
    }

    void SetMiddleButtonCallback(std::function<void(int, int)>&& cb) const
    {
        m_interactor.SetMiddleButtonCallback(std::move(cb));
    }

private:
    // 绑定默认的事件处理函数
    void InitEvent() const
    {
        glfwSetCursorPosCallback(m_window, CursorPosCallback);
        glfwSetKeyCallback(m_window, KeyCallback);
        glfwSetScrollCallback(m_window, ScrollCallback);
        glfwSetMouseButtonCallback(m_window, MouseButtonCallback);
        glfwSetFramebufferSizeCallback(m_window, FrameBufferSizeCallback);
    }

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        m_interactor.SetCursorPosition({ static_cast<int>(xpos), static_cast<int>(ypos) });
        m_interactor.OnMouseMove();
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (key)
        {
        case GLFW_KEY_0:
        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
        case GLFW_KEY_4:
        case GLFW_KEY_5:
        case GLFW_KEY_6:
        case GLFW_KEY_7:
        case GLFW_KEY_8:
        case GLFW_KEY_9:
            m_interactor.SetKeyCode(key);
            break;
        case GLFW_KEY_A:
        case GLFW_KEY_B:
        case GLFW_KEY_C:
        case GLFW_KEY_D:
        case GLFW_KEY_E:
        case GLFW_KEY_F:
        case GLFW_KEY_G:
        case GLFW_KEY_H:
        case GLFW_KEY_I:
        case GLFW_KEY_J:
        case GLFW_KEY_K:
        case GLFW_KEY_L:
        case GLFW_KEY_M:
        case GLFW_KEY_N:
        case GLFW_KEY_O:
        case GLFW_KEY_P:
        case GLFW_KEY_Q:
        case GLFW_KEY_R:
        case GLFW_KEY_S:
        case GLFW_KEY_T:
        case GLFW_KEY_U:
        case GLFW_KEY_V:
        case GLFW_KEY_W:
        case GLFW_KEY_X:
        case GLFW_KEY_Y:
        case GLFW_KEY_Z:
            m_interactor.SetKeyCode(mods == GLFW_MOD_CAPS_LOCK ? key : key + 32);
            break;
        default:
            // 只对数字和大小写字母进行处理，其他字符一律按'#'处理
            m_interactor.SetKeyCode('#');
            break;
        }

        m_interactor.OnChar();
    }

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (yoffset > 0.0)
        {
            m_interactor.OnMouseWheelForward();
        }
        else if (yoffset < 0.0)
        {
            m_interactor.OnMouseWheelBackward();
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        switch (action)
        {
        case GLFW_PRESS:
            switch (button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_interactor.OnLeftButtonDown();
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                m_interactor.OnMiddleButtonDown();
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                m_interactor.OnRightButtonDown();
                break;
            default:
                break;
            }
            break;
        case GLFW_RELEASE:
            switch (button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                m_interactor.OnLeftButtonUp();
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                m_interactor.OnMiddleButtonUp();
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                m_interactor.OnRightButtonUp();
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        m_interactor.SetWindowSize({ width, height });
        glViewport(0, 0, width, height);
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
    std::string_view m_windowName { "Test" };
    int m_windowWidth { 800 };
    int m_windowHeight { 600 };
    GLFWwindow* m_window { nullptr };
    inline static Interactor m_interactor {};
};

class Shader
{
public:
    enum class ShaderType
    {
        Vertex   = 0,
        Fragment = 1,
        Geometry = 2,
    };

public:
    Shader(const std::string_view& filePath, ShaderType type)
        : m_shader(0)
    {
        CreateShader(filePath, type);
    }

    ~Shader()
    {
        glDeleteShader(m_shader);
    }

    GLuint GetShader() const
    {
        return m_shader;
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

            auto infoLog = new char[logLength + 1]();
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
            auto str        = iss.str();
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

    ~ShaderProgram()
    {
        glDeleteProgram(m_program);
    }

    GLuint GetProgram() const
    {
        return m_program;
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

    void SetUniformMat3(const std::string_view& name, const glm::mat3& m) const
    {
        glUniformMatrix3fv(Location(name), 1, GL_FALSE, glm::value_ptr(m));
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

            auto infoLog = new char[logLength + 1]();
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
        }
    };

    GLint Location(const std::string_view& name) const
    {
        auto location = glGetUniformLocation(m_program, name.data());

        if (-1 == location)
        {
            std::clog << "Wrong of uniform name\t" << name << '\n';
        }

        return location;
    }

private:
    uint32_t m_program;
};

class Texture
{
public:
    Texture(const std::string_view& path, GLuint textureUnit = 0, bool gamma = false)
        : m_texture(0)
        , m_width(0)
        , m_height(0)
        , m_colorFormat(0)
        , m_internalFormat(0)
        , m_dataType(GL_UNSIGNED_BYTE)
        , m_textureUnit(textureUnit)
        , m_gamma(gamma)
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

    Texture(GLsizei w, GLsizei h, GLenum internalformat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, GLuint textureUnit = 0)
        : m_texture(0)
        , m_width(w)
        , m_height(h)
        , m_colorFormat(format)
        , m_internalFormat(internalformat)
        , m_dataType(type)
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

    Texture(const void* image, GLsizei w, GLsizei h, GLenum internalformat = GL_RGB, GLenum format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE,
        GLuint textureUnit = 0, bool gamma = false)
        : m_texture(0)
        , m_width(w)
        , m_height(h)
        , m_colorFormat(format)
        , m_internalFormat(internalformat)
        , m_dataType(type)
        , m_textureUnit(textureUnit)
        , m_gamma(gamma)
    {
        glGenTextures(1, &m_texture);

        // 默认激活纹理单元0
        Active(m_textureUnit);

        // 默认2D纹理
        Bind();

        // 默认环绕、过滤方式
        SetWarpParameter(GL_REPEAT, GL_REPEAT);
        SetFilterParameter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

        LoadImageData(image);

        // 取消纹理绑定
        Release();
    }

    ~Texture()
    {
        glDeleteTextures(1, &m_texture);
    }

private:
    GLuint m_texture;
    GLsizei m_width;
    GLsizei m_height;
    GLenum m_colorFormat;
    GLenum m_internalFormat;
    GLenum m_dataType;
    GLuint m_textureUnit;
    bool m_gamma;

public:
    GLuint Get() const
    {
        return m_texture;
    }

    void Bind() const
    {
        Active(m_textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void Use(GLuint unit = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
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
                m_internalFormat = m_gamma ? GL_SRGB : GL_RGB;
                m_colorFormat    = GL_RGB;
            }
            else if (channels == 4)
            {
                m_internalFormat = m_gamma ? GL_SRGB_ALPHA : GL_RGBA;
                m_colorFormat    = GL_RGBA;
            }
            else
            {
                std::clog << "Image channels error!\n";
            }

            glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_colorFormat, m_dataType, data);
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
        m_width  = w;
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

    // GLenum GetColorFormat() const
    // {
    //     return m_colorFormat;
    // }

    GLenum GetTarget() const
    {
        // 暂时先只做2D，1D和3D后面再扩展
        return GL_TEXTURE_2D;
    }

private:
    void CreateNullTexture() const
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_colorFormat, m_dataType, NULL);
    }

    void LoadImageData(const void* data) const
    {
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_colorFormat, m_dataType, data);
    }

    void Active(GLuint texUnit) const
    {
        glActiveTexture(GL_TEXTURE0 + texUnit);
    }
};

class Renderer
{
private:
    enum class DrawType
    {
        Arrays   = 0,
        Elements = 1
    };

public:
    Renderer(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, std::initializer_list<GLuint>&& layout)
        : m_vao(0)
        , m_vbo(0)
        , m_ebo(0)
        , m_count(static_cast<GLsizei>(indices.size()))
        , m_drawType(DrawType::Elements)
    {
        glGenVertexArrays(1, &m_vao);

        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        GLuint stride = std::accumulate(layout.begin(), layout.end(), 0);
        GLuint index { 0 }, offset { 0 };
        for (auto& elem : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, elem, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(sizeof(GLfloat) * offset));
            offset += elem;
            index++;
        }

        glBindVertexArray(0);
    }

    Renderer(const std::vector<GLfloat>& vertices, std::initializer_list<GLsizei>&& layout)
        : m_vao(0)
        , m_vbo(0)
        , m_ebo(0)
        , m_count(0)
        , m_drawType(DrawType::Arrays)
    {
        glGenVertexArrays(1, &m_vao);

        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        auto stride = std::accumulate(layout.begin(), layout.end(), 0);
        m_count     = static_cast<GLsizei>(vertices.size()) / stride;
        GLuint index { 0 }, offset { 0 };
        for (auto& elem : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, elem, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(sizeof(GLfloat) * offset));
            offset += elem;
            index++;
        }

        glBindVertexArray(0);
    }

    ~Renderer()
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        if (0 != m_ebo)
        {
            glDeleteBuffers(1, &m_ebo);
        }
    }

    void Draw(GLenum mode) const
    {
        glBindVertexArray(m_vao);

        switch (m_drawType)
        {
        case DrawType::Arrays:
            glDrawArrays(mode, 0, m_count);
            break;
        case DrawType::Elements:
            glDrawElements(mode, m_count, GL_UNSIGNED_INT, 0);
            break;
        default:
            assert(false);
        }

        glBindVertexArray(0);
    }

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLsizei m_count;
    DrawType m_drawType;
};

class RenderBufferObject
{
public:
    constexpr RenderBufferObject(GLenum format, GLsizei w, GLsizei h) noexcept
        : m_format(format)
    {
        glGenRenderbuffers(1, &m_rbo);
        Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
        Release();
    }

    ~RenderBufferObject() noexcept
    {
        glDeleteRenderbuffers(1, &m_rbo);
    }

    constexpr GLuint GetHandle() const noexcept
    {
        return m_rbo;
    }

    constexpr void Resize(GLsizei w, GLsizei h) const noexcept
    {
        Bind();
        glRenderbufferStorage(GL_RENDERBUFFER, m_format, w, h);
        Release();
    }

private:
    constexpr void Bind() const noexcept
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    }

    constexpr void Release() const noexcept
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

private:
    GLuint m_rbo { 0 };
    GLenum m_format { 0 };
};

class FrameBufferObject
{
public:
    constexpr FrameBufferObject() noexcept
    {
        glGenFramebuffers(1, &m_fbo);
    }

    ~FrameBufferObject() noexcept
    {
        glDeleteFramebuffers(1, &m_fbo);
    }

    constexpr void AddAttachment(GLuint attachment, const Texture& texture, GLint level = 0) const
    {
        Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texture.GetTarget(), texture.Get(), level);
        Release();
    }

    constexpr void AddAttachment(GLuint attachment, const RenderBufferObject& rbo) const
    {
        Bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.GetHandle());
        Release();
    }

    constexpr void SetDrawBuffers(const std::vector<GLenum> buffers) const
    {
        Bind();
        glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
        Release();
    }

    constexpr void Bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    constexpr void Release() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    constexpr bool Check() const
    {
        Bind();
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        Release();
    }

    constexpr GLuint GetHandle() const
    {
        return m_fbo;
    }

private:
    GLuint m_fbo { 0 };
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
