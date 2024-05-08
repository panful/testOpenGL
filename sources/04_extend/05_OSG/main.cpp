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
    Viewer viewer {};
    Window window {};
    View view {};

    GraphicsPipeline graphicsPipeline {};
    graphicsPipeline.SetShaderCode(VS, FS);

    GraphicsPipeline graphicsPipeline2 {};
    graphicsPipeline2.SetShaderCode(VS2, FS2);

    Geometry geometry {};
    geometry.SetVertices(vertices);
    geometry.SetIndices(indices);
    geometry.SetAttributes({ 2 });

    Geometry geometry2 {};
    geometry2.SetData(vertices2, indices2, { 2 });

    Geometry geometry3 {};
    geometry3.SetData(vertices3, indices3, { 3 });

    view.AddChild(&graphicsPipeline);
    view.AddChild(&graphicsPipeline2);

    graphicsPipeline.AddChild(&geometry);
    graphicsPipeline.AddChild(&geometry2);

    graphicsPipeline2.AddChild(&geometry3);

    window.AddChild(&view);
    viewer.AddWindow(&window);

    viewer.Compile();
    viewer.Run();

    return 0;
}
