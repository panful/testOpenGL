#pragma once

#include "common.hpp"
#include <limits>
#include <numbers>

class NoCopyable
{
public:
    NoCopyable(const NoCopyable&) = delete;

    NoCopyable& operator=(const NoCopyable&) = delete;

    NoCopyable() noexcept = default;

    virtual ~NoCopyable() noexcept = default;
};

struct AABB
{
    float min_x { std::numeric_limits<float>::max() };
    float min_y { std::numeric_limits<float>::max() };
    float min_z { std::numeric_limits<float>::max() };
    float max_x { std::numeric_limits<float>::lowest() };
    float max_y { std::numeric_limits<float>::lowest() };
    float max_z { std::numeric_limits<float>::lowest() };

    float operator[](size_t index) const
    {
        switch (index)
        {
            case 0:
                return min_x;
            case 1:
                return max_x;
            case 2:
                return min_y;
            case 3:
                return max_y;
            case 4:
                return min_z;
            case 5:
                return max_z;
            default:
                throw std::runtime_error("error index");
        }
    }

    glm::vec3 GetCenter() const
    {
        return glm::vec3(std::midpoint(min_x, max_x), std::midpoint(min_y, max_y), std::midpoint(min_z, max_z));
    }
};

class AABBTool
{
public:
    static AABB ComputeAABB(const std::vector<float>& vertices, size_t vertexStride = 3)
    {
        AABB aabb {};
        for (size_t i = 0; i < vertices.size(); i += vertexStride)
        {
            // 顶点数据的前3个是位置
            aabb.min_x = std::min(aabb.min_x, vertices[i]);
            aabb.min_y = std::min(aabb.min_y, vertices[i + 1]);
            aabb.min_z = std::min(aabb.min_z, vertices[i + 2]);
            aabb.max_x = std::max(aabb.max_x, vertices[i]);
            aabb.max_y = std::max(aabb.max_y, vertices[i + 1]);
            aabb.max_z = std::max(aabb.max_z, vertices[i + 2]);
        }
        return aabb;
    }
    
    static AABB MergeAABB(const std::vector<AABB>& aabbs)
    {
        AABB aabb {};
        for (auto& elem : aabbs)
        {
            aabb.min_x = std::min(aabb.min_x, elem.min_x);
            aabb.min_y = std::min(aabb.min_y, elem.min_y);
            aabb.min_z = std::min(aabb.min_z, elem.min_z);
            aabb.max_x = std::max(aabb.max_x, elem.max_x);
            aabb.max_y = std::max(aabb.max_y, elem.max_y);
            aabb.max_z = std::max(aabb.max_z, elem.max_z);
        }
        return aabb;
    }
};

class Camera2 : public NoCopyable
{
public:
    enum class Type : uint8_t
    {
        Orthographic,
        Perspective
    };

    enum class Behavior : uint8_t
    {
        FPS,      // 第一人称
        Orbit,    // 轨道
        FreeLook, // 自由观察
    };
    
public:
    Type type { Type::Perspective };
    Behavior behavior { Behavior::Orbit };

    float aspectRatio { 1.f };                                    // 视口的宽高比
    float viewAngle { std::numbers::pi_v<float> / 180.f * 30.f }; // 视角角度默认30度
    float orthographicScale { 1.f };                              // 正交投影的缩放比例   

    glm::vec4 viewRange { -1., 1., -1., 1. };                     // 视野宽高范围
    glm::vec2 clipRange { -1., 1. };                              // 深度裁剪范围

    glm::mat4 viewMat { glm::mat4(1.f) };
    glm::mat4 projMat { glm::mat4(1.f) };

    glm::vec3 viewUp { 0.f, -1.f, 0.f };  // 相机向上的方向
    glm::vec3 eyePos { 0.f, 0.f, 1.f };   // 相机的位置
    glm::vec3 focalPos { 0.f, 0.f, 0.f }; // 焦点的位置

public:
    Camera2()
    {
        Update();
    }

    void Print() const
    {
        std::cout << std::format("Eye: ({:.3f}, {:.3f}, {:.3f})\n", eyePos.x, eyePos.y, eyePos.z);
        std::cout << std::format("Focal: ({:.3f}, {:.3f}, {:.3f})\n", focalPos.x, focalPos.y, focalPos.z);
        std::cout << std::format("Up: ({:.3f}, {:.3f}, {:.3f})\n", viewUp.x, viewUp.y, viewUp.z);
        std::cout << std::format("ClipRange: ({:.3f}, {:.3f})\n", clipRange.x, clipRange.y);
        std::cout << std::format("ViewAngle: {:.3f}\n", viewAngle / (std::numbers::pi_v<float> / 180.f));
        std::cout << std::format("OrthographicScale: {:.3f}\n", orthographicScale);
    }

    void Reset(const AABB& aabb)
    {
        auto radius = std::hypot(aabb.max_x - aabb.min_x, aabb.max_y - aabb.min_y, aabb.max_z - aabb.min_z) * 0.5f;
        radius      = radius == 0.f ? .5f : radius;

        auto distance = radius / std::sin(viewAngle * .5f);
        auto vpNormal = glm::normalize(eyePos - focalPos); // view plane noraml

        focalPos          = aabb.GetCenter();
        eyePos            = focalPos + distance * vpNormal;
        orthographicScale = radius;
        clipRange         = glm::vec2(distance - radius * 1.1f, distance + radius * 1.1f);

        Update();
    }
    
    void ResetClipRange(const AABB& aabb)
    {
        auto vpNormal = glm::normalize(eyePos - focalPos); // view plane noraml

        auto a = -vpNormal[0];
        auto b = -vpNormal[1];
        auto c = -vpNormal[2];
        auto d = -(a * eyePos[0] + b * eyePos[1] + c * eyePos[2]);
        
        std::array<double, 2> range {};

        range[0] = a * aabb[0] + b * aabb[2] + c * aabb[4] + d;
        range[1] = std::numeric_limits<double>::min();

        for (auto k = 0u; k < 2; ++k)
        {
            for (auto j = 0u; j < 2; ++j)
            {
                for (auto i = 0u; i < 2; ++i)
                {
                    auto dist = a * aabb[i] + b * aabb[2 + j] + c * aabb[4 + k] + d;
                    range[0]  = range[0] > dist ? dist : range[0];
                    range[1]  = range[1] < dist ? dist : range[1];
                }
            }
        }

        static constexpr double expandRadio { 0.1 }; // 留出来一点空间，不要让前后裁剪平面紧贴着包围盒球体
        range[0] = range[0] - range[0] * expandRadio;
        range[1] = range[1] + range[1] * expandRadio;

        // 保证近裁剪面在相机的前面
        if (range[0] <= 0.0)
        {
            static constexpr double nearClipPlaneTolerance { 0.01 }; // 可以根据深度缓冲的字节数调整
            range[0] = range[1] * nearClipPlaneTolerance;
        }

        clipRange = glm::vec2(range[0], range[1]);
        Update();
    }

    /// @brief FPS 俯仰，绕相机的X轴旋转，以相机位置为旋转中心，弧度
    void Pitch(float angle) noexcept
    {
        glm::vec3 axis = glm::normalize(glm::cross(focalPos - eyePos, viewUp));

        glm::mat4 transform(1.f);
        transform = glm::translate(transform, eyePos);
        transform = glm::rotate(transform, angle, axis);
        transform = glm::translate(transform, -eyePos);

        focalPos = glm::vec3(transform * glm::vec4(focalPos, 1.f));
        viewUp   = glm::normalize(glm::cross(axis, focalPos - eyePos));
        Update();
    }

    /// @brief FPS 偏航，绕相机的Y轴旋转，以相机位置为旋转中心，弧度
    void Yaw(float angle) noexcept
    {
        glm::vec3 axis = glm::normalize(viewUp);

        glm::mat4 transform(1.f);
        transform = glm::translate(transform, eyePos);
        transform = glm::rotate(transform, angle, axis);
        transform = glm::translate(transform, -eyePos);

        focalPos = glm::vec3(transform * glm::vec4(focalPos, 1.f));
        Update();
    }

    /// @brief FPS 滚转，绕相机的Z轴旋转，以相机位置为旋转中心，弧度
    void Roll(float angle) noexcept
    {
        glm::vec3 axis = glm::normalize(focalPos - eyePos);

        glm::mat4 transform(1.f);
        transform = glm::translate(transform, eyePos);
        transform = glm::rotate(transform, angle, axis);
        transform = glm::translate(transform, -eyePos);

        viewUp = glm::vec3(transform * glm::vec4(viewUp, 1.f));
        Update();
    }

    /// @brief 绕相机的Y轴旋转，以焦点为旋转中心，弧度
    void Azimuth(float angle) noexcept
    {
        glm::vec3 axis = glm::normalize(viewUp);

        glm::mat4 transform(1.f);
        transform = glm::translate(transform, focalPos);
        transform = glm::rotate(transform, angle, axis);
        transform = glm::translate(transform, -focalPos);

        eyePos = glm::vec3(transform * glm::vec4(eyePos, 1.));
        Update();
    }

    /// @brief 绕相机的X轴旋转，以焦点为旋转中心，弧度
    void Elevation(float angle) noexcept
    {
        glm::vec3 axis = glm::normalize(glm::cross(focalPos - eyePos, viewUp));

        glm::mat4 transform(1.f);
        transform = glm::translate(transform, focalPos);
        transform = glm::rotate(transform, angle, axis);
        transform = glm::translate(transform, -focalPos);

        eyePos = glm::vec3(transform * glm::vec4(eyePos, 1.f));
        viewUp = glm::normalize(glm::cross(axis, focalPos - eyePos));
        Update();
    }

    glm::vec3 NDCToWorld(const glm::vec3& ndcPoint) noexcept
    {
        glm::vec4 worldPos = glm::inverse(viewMat) * glm::inverse(projMat) * glm::vec4(ndcPoint, 1.f);
        return { worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w };
    }

    glm::vec3 WorldToNDC(const glm::vec3& worldPoint) noexcept
    {
        glm::vec4 viewPos = projMat * viewMat * glm::vec4(worldPoint, 1.f);
        return { viewPos.x / viewPos.w, viewPos.y / viewPos.w, viewPos.z / viewPos.w };
    }

    void Update()
    {
        viewMat = glm::lookAt(eyePos, focalPos, viewUp);
        switch (type)
        {
            case Type::Orthographic:
            {
                auto left   = orthographicScale * viewRange[0] * aspectRatio;
                auto right  = orthographicScale * viewRange[1] * aspectRatio;
                auto top    = orthographicScale * viewRange[2];
                auto bottom = orthographicScale * viewRange[3];
                auto zNear  = clipRange[0];
                auto zFar   = clipRange[1];

                projMat = glm::ortho(left, right, bottom, top, zNear, zFar);
                break;
            }
            case Type::Perspective:
            {
                auto tmp    = std::tan(viewAngle / 2.f);
                auto width  = clipRange[0] * tmp * aspectRatio;
                auto height = clipRange[0] * tmp;

                auto left   = width * viewRange[0];
                auto right  = width * viewRange[1];
                auto top    = height * viewRange[2];
                auto bottom = height * viewRange[3];
                auto near   = clipRange[0];
                auto far    = clipRange[1];

                projMat = glm::frustum(left, right, bottom, top, near, far);
                break;
            }
            default:
                break;
        }
    }
};

class Interactor2 : public NoCopyable
{
    enum class State
    {
        None,
        Rotate,
        Pan,
        Zoom,
    };

public:
    State state {};
    Camera2 camera {};
    glm::vec2 windowSize {};
    glm::vec2 mousePosition {};
    glm::vec2 lastMousePosition {};
    std::function<void(const glm::vec2&)> windowResizeCallback {};

public:
    glm::vec3 DisplayToWorld(const glm::vec2& displayPoint)
    {
        auto ndc = glm::vec3((displayPoint * 2.f - windowSize) / windowSize, 0.f);
        return camera.NDCToWorld(ndc);
    }

    glm::vec2 WorldToDisplay(const glm::vec3& worldPoint)
    {
        auto ndc = glm::vec2(camera.WorldToNDC(worldPoint));
        return (ndc + glm::vec2(1.f)) / 2.f * windowSize;
    }

    void Dolly(float factor)
    {
        if (factor <= 0.f)
            return;
        switch (camera.type)
        {
            case Camera2::Type::Orthographic:
            {
                camera.orthographicScale /= factor;
                camera.Update();
                break;
            }
            case Camera2::Type::Perspective:
            {
                camera.eyePos = camera.focalPos - (camera.focalPos - camera.eyePos) / factor;
                camera.Update();
                break;
            }
            default:
                break;
        }
    }

    void MouseMoveEvent()
    {
        switch (state)
        {
            case State::Rotate:
            {
                auto dx = mousePosition[0] - lastMousePosition[0];
                auto dy = mousePosition[1] - lastMousePosition[1];
                
                if (camera.behavior == Camera2::Behavior::Orbit)
                {
                    static constexpr auto ratio = std::numbers::pi_v<float> / -180.f; // 逆时针旋转

                    camera.Azimuth(dx * ratio);
                    camera.Elevation(dy * ratio);
                }
                else if (camera.behavior == Camera2::Behavior::FPS)
                {
                    static constexpr auto ratio = std::numbers::pi_v<float> / -180.f; // 逆时针旋转

                    camera.Pitch(dy * ratio);
                    camera.Yaw(dx * ratio);
                }

                break;
            }
            case State::Pan:
            {
                auto curr = DisplayToWorld(mousePosition);
                auto last = DisplayToWorld(lastMousePosition);

                glm::vec3 motionVector = last - curr;
                camera.eyePos += motionVector;
                camera.focalPos += motionVector;
                camera.Update();

                break;
            }
            default:
                break;
        }
    }
    
    void RightButtonPressEvent()
    {
        state = State::Rotate;
    }

    void RightButtonReleaseEvent()
    {
        state = State::None;
    }

    void LeftButtonPressEvent()
    {
        state = State::Pan;
    }

    void LeftButtonReleaseEvent()
    {
        state = State::None;
    }

    void MiddleButtonPressEvent()
    {
    }

    void MiddleButtonReleaseEvent()
    {
    }

    void MouseWheelForwardEvent()
    {
        Dolly(1.1f);
    }

    void MouseWheelBackwardEvent()
    {
        Dolly(0.9f);
    }

    void CharEvent()
    {
    }

    void KeyPressEvent()
    {
    }

    void KeyReleaseEvent()
    {
    }
    
    void WindowResizedEvent(const glm::vec2& size)
    {
        windowSize         = size;
        camera.aspectRatio = size[0] / size[1];
        if (windowResizeCallback)
        {
            windowResizeCallback(size);
        }
    }
};

class Window : public NoCopyable
{
public:
    GLFWwindow* window {};
    Interactor2 interactor {};

public:
    Window(const std::string_view name = "OpenGL", int width = 800, int height = 600) noexcept
    {
        interactor.windowSize         = glm::vec2(width, height);
        interactor.camera.aspectRatio = static_cast<float>(width) / height;

        glfwSetErrorCallback([](int error, const char* description) { std::cerr << std::format("GLFW Error: [{}] {}\n", error, description); });

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // 禁止缩放窗口

        window = glfwCreateWindow(width, height, name.data(), NULL, NULL);
        if (window == NULL)
        {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);

        glfwSetWindowUserPointer(window, &interactor);
        glfwSetCursorPosCallback(window, CursorPosCallback);
        glfwSetKeyCallback(window, KeyCallback);
        glfwSetScrollCallback(window, ScrollCallback);
        glfwSetMouseButtonCallback(window, MouseButtonCallback);
        glfwSetWindowSizeCallback(window, WindowSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
        }
    }
    
    ~Window() noexcept
    {
        CheckErrorWithInfo("Application exit");

        if (window)
        {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

private:
    static void WindowSizeCallback(GLFWwindow* window, int width, int height) noexcept
    {
        if (width <= 0 || height <= 0)
        {
            return;
        }
        
        if (auto ptr = static_cast<Interactor2*>(glfwGetWindowUserPointer(window)))
        {
            ptr->WindowResizedEvent(glm::vec2(width, height));
        }
    }

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) noexcept
    {
        if (auto ptr = static_cast<Interactor2*>(glfwGetWindowUserPointer(window)))
        {
            ptr->lastMousePosition = ptr->mousePosition;
            ptr->mousePosition     = { static_cast<int>(xpos), static_cast<int>(-ypos) };
            ptr->MouseMoveEvent();
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) noexcept
    {
        if (auto ptr = static_cast<Interactor2*>(glfwGetWindowUserPointer(window)))
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    switch (button)
                    {
                        case GLFW_MOUSE_BUTTON_LEFT:
                            ptr->LeftButtonPressEvent();
                            break;
                        case GLFW_MOUSE_BUTTON_MIDDLE:
                            ptr->MiddleButtonPressEvent();
                            break;
                        case GLFW_MOUSE_BUTTON_RIGHT:
                            ptr->RightButtonPressEvent();
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case GLFW_RELEASE:
                {
                    switch (button)
                    {
                        case GLFW_MOUSE_BUTTON_LEFT:
                            ptr->LeftButtonReleaseEvent();
                            break;
                        case GLFW_MOUSE_BUTTON_MIDDLE:
                            ptr->MiddleButtonReleaseEvent();
                            break;
                        case GLFW_MOUSE_BUTTON_RIGHT:
                            ptr->RightButtonReleaseEvent();
                            break;
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }              

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) noexcept
    {
        if (auto ptr = static_cast<Interactor2*>(glfwGetWindowUserPointer(window)))
        {
            if (yoffset > 0.0)
            {
                ptr->MouseWheelForwardEvent();
            }
            else if (yoffset < 0.0)
            {
                ptr->MouseWheelBackwardEvent();
            }
        }
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
    {
        if (auto ptr = static_cast<Interactor2*>(glfwGetWindowUserPointer(window)))
        {
        {
        }
    }
};
