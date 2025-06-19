/**
 * 1. 通过在片段着色器中计算模长绘制虚线
 * 2. 通过将直线看作矩形绘制任意宽度的直线，使用几何着色器
 * 3. 绘制任意宽度的虚线
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 2.f, 2.f, 8.f }));
    auto window = opengl.GetWindow();

    ShaderProgram program("shaders/02_07_04_TEST1.vert", "shaders/02_07_04_TEST1.frag");

    // clang-format off
    std::vector<GLfloat> vertices = {
        0.f, 0.f, 0.f,
        1.f, 1.f, 1.f
    };
    std::vector<GLuint> indices   = { 0, 1 };
    // clang-format on

    Renderer line(vertices, indices, { 3 }, GL_LINES);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view_matrix = opengl.GetViewMatrix();
        auto proj_matrix = opengl.GetProjectionMatrix();
        auto window_size = opengl.GetWindowSize();

        glm::vec3 startPos = { 0.f, 0.f, 0.f };
        glm::vec4 clipPos  = proj_matrix * view_matrix * glm::vec4(startPos, 1.f);

        glm::vec2 screenPos = { clipPos.x / clipPos.w, clipPos.y / clipPos.w };
        screenPos           = screenPos * 0.5f + 0.5f;
        screenPos           = screenPos * glm::vec2(window_size[0], window_size[1]);

        program.Use();
        program.SetUniform2f("uResolution", static_cast<float>(window_size[0]), static_cast<float>(window_size[1]));
        program.SetUniform2f("uStartPos", screenPos.x, screenPos.y);
        program.SetUniformMat4("uModel", glm::mat4(1.f));
        program.SetUniformMat4("uView", view_matrix);
        program.SetUniformMat4("uProj", proj_matrix);

        line.Draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 2.f, 2.f, 8.f }));
    auto window = opengl.GetWindow();

    ShaderProgram program("shaders/02_07_04_TEST2.vert", "shaders/02_07_04_TEST2.frag", "shaders/02_07_04_TEST2.geom");

    // clang-format off
    std::vector<GLfloat> vertices = {
        0.f, 0.f, 0.f,
        1.f, 1.f, 1.f
    };
    std::vector<GLuint> indices   = { 0, 1 };
    // clang-format on

    Renderer line(vertices, indices, { 3 }, GL_LINES);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view_matrix = opengl.GetViewMatrix();
        auto proj_matrix = opengl.GetProjectionMatrix();
        auto window_size = opengl.GetWindowSize();

        program.Use();
        program.SetUniform1f("uLineWidth", 10.f);
        program.SetUniform2f("uResolution", static_cast<float>(window_size[0]), static_cast<float>(window_size[1]));
        program.SetUniformMat4("uModel", glm::mat4(1.f));
        program.SetUniformMat4("uView", view_matrix);
        program.SetUniformMat4("uProj", proj_matrix);

        line.Draw();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 2.f, 2.f, 8.f }));
    auto window = opengl.GetWindow();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    ShaderProgram program("shaders/02_07_04_TEST3.vert", "shaders/02_07_04_TEST3.frag", "shaders/02_07_04_TEST3.geom");

    // clang-format off
    std::vector<GLfloat> vertices = {
        0.f, 0.f, 0.f,
        1.f, 1.f, 1.f
    };
    std::vector<GLuint> indices   = { 0, 1 };
    // clang-format on

    struct LinePattern
    {
        const char* name;
        std::vector<float> segments; // 单位长度（屏幕像素）
    };

    // clang-format off
    std::vector<LinePattern> line_patterns = {
        { "CONTINUOUS", {} },
        { "ISO02W100", { 10.0f, 5.0f } },                                                 // __ __ __
        { "ISO02W100", { 10.0f, 5.0f } },                                                 // __ __ __
        { "ISO03W100", { 10.0f, 10.0f } },                                                // __    __
        { "ISO04W100", { 15.0f, 5.0f, 1.0f, 5.0f } },                                     // ____ . ____ .
        { "ISO05W100", { 15.0f, 5.0f, 1.0f, 2.0f, 1.0f, 5.0f } },                         // ____ .. ____
        { "ISO06W100", { 15.0f, 5.0f, 1.0f, 2.0f, 1.0f, 2.0f, 1.0f, 5.0f } },             // ____ ... ____
        { "ISO07W100", { 1.0f, 2.0f } },                                                  // . . . . .
        { "ISO08W100", { 15.0f, 5.0f, 5.0f, 5.0f } },                                     // ____ __ __
        { "ISO09W100", { 15.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f } },                         // ____ __ __ __
        { "ISO10W100", { 10.0f, 5.0f, 1.0f, 5.0f } },                                     // __ . __ .
        { "ISO11W100", { 10.0f, 5.0f, 10.0f, 5.0f, 1.0f, 5.0f } },                        // __ __ . __ __ .
        { "ISO12W100", { 10.0f, 5.0f, 1.0f, 2.0f, 1.0f, 5.0f } },                         // __ . . __ . .
        { "ISO13W100", { 10.0f, 5.0f, 10.0f, 5.0f, 1.0f, 2.0f, 1.0f, 5.0f } },            // __ __ . . __ __ . .
        { "ISO14W100", { 10.0f, 5.0f, 1.0f, 2.0f, 1.0f, 2.0f, 1.0f, 5.0f } },             // __ . . . __ . . .
        { "ISO15W100", { 10.0f, 5.0f, 10.0f, 5.0f, 1.0f, 2.0f, 1.0f, 2.0f, 1.0f, 5.0f } } // __ __ . . . __ __ . . .
    };
    // clang-format on

    Renderer line(vertices, indices, { 3 }, GL_LINES);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Select LineType");
        static int current_pattern = 0;
        std::vector<const char*> pattern_names;
        for (const auto& pattern : line_patterns)
        {
            pattern_names.push_back(pattern.name);
        }
        ImGui::Combo("Line Type", &current_pattern, pattern_names.data(), static_cast<int>(pattern_names.size()));
        static float line_width = 2.0f;
        ImGui::SliderFloat("Line Width", &line_width, 1.0f, 20.0f, "%.1f");
        ImGui::End();

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view_matrix = opengl.GetViewMatrix();
        auto proj_matrix = opengl.GetProjectionMatrix();
        auto window_size = opengl.GetWindowSize();

        auto&& segment = line_patterns[current_pattern];

        program.Use();
        program.SetUniform1f("uLineWidth", line_width);
        program.SetUniform2f("uResolution", static_cast<float>(window_size[0]), static_cast<float>(window_size[1]));
        program.SetUniform1i("uPatternSize", static_cast<int>(segment.segments.size()));
        program.SetUniform1fv("uPattern", static_cast<int>(segment.segments.size()), segment.segments.data());
        program.SetUniformMat4("uModel", glm::mat4(1.f));
        program.SetUniformMat4("uView", view_matrix);
        program.SetUniformMat4("uProj", proj_matrix);

        line.Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif // TEST3
