/**
 * 1. 多个着色器、多个图元
 * 2. 使用 Transform 节点
 * 3. 动画
 * 4. 交互、相机
 * 5. 设置图形管线的属性
 */

#define TEST5

#ifdef TEST1

#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "View.h"
#include "Viewer.h"
#include "Window.h"

namespace {
std::string VS { R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos, 0.f, 1.f);
        }
    )" };

std::string FS { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        }
    )" };

std::string VS2 { R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.f);
        }
    )" };

std::string FS2 { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 1.f, 0.f, 1.f);
        }
    )" };

std::vector<float> vertices { -.8f, -.5f, .2f, -.5f, -.3f, .5f };
std::vector<uint32_t> indices { 0, 1, 2 };

std::vector<float> vertices2 { .3f, -.3f, .9f, -.3f, .9f, .3f, .3f, .3f };
std::vector<uint32_t> indices2 { 0, 1, 2, 0, 2, 3 };

std::vector<float> vertices3 { .5f, .5f, 0.f, .9f, .5f, 0.f, .7f, .8f, 0.f };
std::vector<uint32_t> indices3 { 0, 1, 2 };
} // namespace

int main()
{
    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);
    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);
    graphicsPipeline2.AddChild(&geometry3);

    View view {};
    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    Window window {};
    window.AddChild(&view);

    Viewer viewer {};
    viewer.AddWindow(&window);
    viewer.Compile();
    viewer.Run();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Transform.h"
#include "View.h"
#include "Viewer.h"
#include "Window.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

namespace {
std::string VS { R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos, 0.f, 1.f);
        }
    )" };

std::string FS { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        }
    )" };

std::string VS2 { R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 transform;
        void main() {
            gl_Position = transform * vec4(aPos, 1.f);
        }
    )" };

std::string FS2 { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 1.f, 0.f, 1.f);
        }
    )" };

std::vector<float> vertices { -.8f, -.5f, .2f, -.5f, -.3f, .5f };
std::vector<uint32_t> indices { 0, 1, 2 };

std::vector<float> vertices2 { .3f, -.3f, .9f, -.3f, .9f, .3f, .3f, .3f };
std::vector<uint32_t> indices2 { 0, 1, 2, 0, 2, 3 };

std::vector<float> vertices3 { .5f, .5f, 0.f, .9f, .5f, 0.f, .7f, .8f, 0.f };
std::vector<uint32_t> indices3 { 0, 1, 2 };
} // namespace

int main()
{
    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    Transform transform {};
    transform.SetMatrix(glm::translate(glm::mat4(1.f), glm::vec3(-.7f, 0.f, 0.f)));
    transform.AddChild(&geometry3);

    Transform transform2 {};
    transform2.SetMatrix(glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.2f, 0.f)));
    transform2.AddChild(&transform);

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);

    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);
    graphicsPipeline2.AddChild(&transform2);

    View view {};
    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    Window window {};
    window.AddChild(&view);

    Viewer viewer {};
    viewer.AddWindow(&window);
    viewer.Compile();
    viewer.Run();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include "Animation.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Transform.h"
#include "View.h"
#include "Viewer.h"
#include "Window.h"

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

namespace {
std::string VS { R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos, 0.f, 1.f);
        }
    )" };

std::string FS { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        }
    )" };

std::string VS2 { R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 transform;
        void main() {
            gl_Position = transform * vec4(aPos, 1.f);
        }
    )" };

std::string FS2 { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 1.f, 0.f, 1.f);
        }
    )" };

std::vector<float> vertices { -.8f, -.5f, .2f, -.5f, -.3f, .5f };
std::vector<uint32_t> indices { 0, 1, 2 };

std::vector<float> vertices2 { .3f, -.3f, .9f, -.3f, .9f, .3f, .3f, .3f };
std::vector<uint32_t> indices2 { 0, 1, 2, 0, 2, 3 };

std::vector<float> vertices3 { .5f, .5f, 0.f, .9f, .5f, 0.f, .7f, .8f, 0.f };
std::vector<uint32_t> indices3 { 0, 1, 2 };
} // namespace

int main()
{
    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    Animation animation {};
    animation.SetUpdateCallback(
        [](glm::mat4& mat)
        {
            static float radians { 0.02f };
            mat = glm::rotate(mat, radians, glm::vec3(0.f, 0.f, 1.f));
        });
    animation.AddChild(&geometry3);

    Transform transform {};
    transform.SetMatrix(glm::translate(glm::mat4(1.f), glm::vec3(-.7f, 0.f, 0.f)));
    transform.AddChild(&animation);

    Transform transform2 {};
    transform2.SetMatrix(glm::translate(glm::mat4(1.f), glm::vec3(0.f, -.7f, 0.f)));
    transform2.AddChild(&transform);

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);

    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);
    graphicsPipeline2.AddChild(&transform2);

    View view {};
    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    Window window {};
    window.AddChild(&view);

    Viewer viewer {};
    viewer.AddWindow(&window);
    viewer.Compile();
    viewer.Run();

    return 0;
}

#endif // TEST3

#ifdef TEST4

#include "Camera.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "Trackball.h"
#include "View.h"
#include "Viewer.h"
#include "Window.h"

namespace {
std::string VS { R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 default_view_proj;
        void main() {
            gl_Position = default_view_proj * vec4(aPos, 0.f, 1.f);
        }
    )" };

std::string FS { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        }
    )" };

std::string VS2 { R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 default_view_proj;
        uniform mat4 model;
        void main() {
            gl_Position = default_view_proj * model * vec4(aPos, 1.f);
        }
    )" };

std::string FS2 { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 1.f, 0.f, 1.f);
        }
    )" };

std::vector<float> vertices { -.8f, -.5f, .2f, -.5f, -.3f, .5f };
std::vector<uint32_t> indices { 0, 1, 2 };

std::vector<float> vertices2 { .3f, -.3f, .9f, -.3f, .9f, .3f, .3f, .3f };
std::vector<uint32_t> indices2 { 0, 1, 2, 0, 2, 3 };

std::vector<float> vertices3 { .5f, .5f, 0.f, .9f, .5f, 0.f, .7f, .8f, 0.f };
// std::vector<float> vertices3 { -.5f, -.5f, 0.f, .5f, -.5f, 0.f, 0.f, .5f, 0.f };
std::vector<uint32_t> indices3 { 0, 1, 2 };
} // namespace

int main()
{
    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);
    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);
    graphicsPipeline2.AddChild(&geometry3);

    Camera camera {};

    View view {};
    view.SetCamera(&camera);
    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    Trackball trackball {};
    trackball.AddView(&view);

    Window window {};
    window.AddChild(&view);
    window.AddEventHandler(&trackball);

    Viewer viewer {};
    viewer.AddWindow(&window);
    viewer.Compile();
    viewer.Run();

    return 0;
}

#endif // TEST4

#ifdef TEST5

#include "Camera.h"
#include "Geometry.h"
#include "GraphicsPipeline.h"
#include "State.h"
#include "Trackball.h"
#include "View.h"
#include "Viewer.h"
#include "Window.h"

#include <glad/glad.h>

namespace {
std::string VS { R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 default_view_proj;
        void main() {
            gl_Position = default_view_proj * vec4(aPos, 0.f, 1.f);
        }
    )" };

std::string FS { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 0.f, 0.f, 1.f);
        }
    )" };

std::string VS2 { R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 default_view_proj;
        uniform mat4 model;
        void main() {
            gl_Position = default_view_proj * model * vec4(aPos, 1.f);
        }
    )" };

std::string FS2 { R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.f, 1.f, 0.f, 1.f);
        }
    )" };

std::vector<float> vertices { -.8f, -.5f, .2f, -.5f, -.3f, .5f };
std::vector<uint32_t> indices { 0, 1, 2 };

std::vector<float> vertices2 { .3f, -.3f, .9f, -.3f, .9f, .3f, .3f, .3f };
std::vector<uint32_t> indices2 { 0, 1, 2, 0, 2, 3 };

std::vector<float> vertices3 { .5f, .5f, 0.f, .9f, .5f, 0.f, .7f, .8f, 0.f };
// std::vector<float> vertices3 { -.5f, -.5f, 0.f, .5f, -.5f, 0.f, 0.f, .5f, 0.f };
std::vector<uint32_t> indices3 { 0, 1, 2 };
} // namespace

int main()
{
    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    InputAssemblyState inputAssemblyState {};
    inputAssemblyState.m_face = static_cast<uint32_t>(GL_FRONT_AND_BACK);
    inputAssemblyState.m_mode = static_cast<uint32_t>(GL_LINE);

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);
    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);
    graphicsPipeline.AddState(&inputAssemblyState);

    InputAssemblyState inputAssemblyState2 {};
    inputAssemblyState2.m_face = static_cast<uint32_t>(GL_FRONT_AND_BACK);
    inputAssemblyState2.m_mode = static_cast<uint32_t>(GL_FILL);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);
    graphicsPipeline2.AddChild(&geometry3);
    graphicsPipeline2.AddState(&inputAssemblyState2);

    Camera camera {};

    View view {};
    view.SetCamera(&camera);
    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    Trackball trackball {};
    trackball.AddView(&view);

    Window window {};
    window.AddChild(&view);
    window.AddEventHandler(&trackball);

    Viewer viewer {};
    viewer.AddWindow(&window);
    viewer.Compile();
    viewer.Run();

    return 0;
}

#endif // TEST5
