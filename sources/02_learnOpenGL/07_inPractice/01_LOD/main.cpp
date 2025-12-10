/**
 * 1.手动切换 LOD
 *
 */

#define TEST1

#ifdef TEST1

#include <common2.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <random>

struct PointCloud
{
    std::vector<float> vertices {};
    std::vector<uint32_t> point_count_of_lod {};
    AABB aabb {};
};

namespace {
PointCloud generate_fixed_random_point_cloud(uint32_t point_count, float min, float max)
{
    PointCloud pc {};
    std::mt19937 generator(42);

    std::uniform_real_distribution<float> coord_distribution(min, max);
    std::uniform_real_distribution<float> color_distribution(0.f, 1.f);

    pc.vertices.reserve(point_count * 6);
    for (uint32_t i = 0; i < point_count; ++i)
    {
        float x = coord_distribution(generator);
        float y = coord_distribution(generator);
        float z = coord_distribution(generator);

        pc.vertices.push_back(x);
        pc.vertices.push_back(y);
        pc.vertices.push_back(z);

        pc.vertices.push_back(std::abs(z / (max - min)));
        pc.vertices.push_back(0.f);
        pc.vertices.push_back(0.f);
    }

    uint32_t level0_count = static_cast<uint32_t>(std::round(point_count * 0.0625f));
    uint32_t level1_count = static_cast<uint32_t>(std::round(point_count * 0.125f));
    uint32_t level2_count = static_cast<uint32_t>(std::round(point_count * 0.25f));
    uint32_t level3_count = static_cast<uint32_t>(std::round(point_count * 0.5f));
    uint32_t level4_count = static_cast<uint32_t>(std::round(point_count * 0.75f));
    uint32_t level5_count = point_count;

    pc.point_count_of_lod.push_back(level0_count);
    pc.point_count_of_lod.push_back(level1_count);
    pc.point_count_of_lod.push_back(level2_count);
    pc.point_count_of_lod.push_back(level3_count);
    pc.point_count_of_lod.push_back(level4_count);
    pc.point_count_of_lod.push_back(level5_count);

    pc.aabb = AABB(min, min, min, max, max, max);

    return pc;
}

} // namespace

int main()
{
    uint32_t total_points = 1'000'000;
    float min_coord       = 0.f;
    float max_coord       = 10.f;

    PointCloud cloud = generate_fixed_random_point_cloud(total_points, min_coord, max_coord);

    Window window("Test Window", 800, 600);
    window.interactor.camera.type = Camera2::Type::Orthographic;

    glfwSwapInterval(0); // 禁用垂直同步，跑满帧数

    ShaderProgram program("shaders/02_07_01_TEST1.vert", "shaders/02_07_01_TEST1.frag");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    Renderer drawable(cloud.vertices, { 3, 3 }, GL_POINTS);
    window.interactor.camera.Reset(cloud.aabb);

    int level {};
    std::vector<std::string> level_of_details {};
    for (size_t i = 0; i < cloud.point_count_of_lod.size(); ++i)
    {
        level_of_details.emplace_back("Level " + std::to_string(i) + ": " + std::to_string(cloud.point_count_of_lod[i]));
    }
    std::vector<const char*> level_of_details_arr {};
    for (auto& lod : level_of_details)
    {
        level_of_details_arr.emplace_back(lod.c_str());
    }

    double last_time                      = glfwGetTime();
    double last_fps_update_time           = last_time;
    constexpr double fps_refresh_interval = 1.; // 每隔1s刷新一次FPS
    std::string fps_str {};

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.interactor.camera.ResetClipRange(cloud.aabb);

        auto proj = window.interactor.camera.projMat;
        auto view = window.interactor.camera.viewMat;

        program.Use();
        program.SetUniformMat4("transform", proj * view);

        // 点云均匀采样，例如前10%的顶点是在整个点云中均匀采样10%，前30%的顶点是在整个点云中均匀采样30%
        // VAO VBO 不变，指定绘制的顶点个数即可达到切换 LOD Level 的效果
        glBindVertexArray(drawable.VAO());
        glDrawArrays(GL_POINTS, 0, cloud.point_count_of_lod[level]);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            double current_time = glfwGetTime();
            if (current_time - last_fps_update_time > fps_refresh_interval)
            {
                int fps              = static_cast<int>(1. / (current_time - last_time));
                fps_str              = "FPS: " + std::to_string(fps);
                last_fps_update_time = current_time;
            }
            last_time = current_time;

            ImGui::Begin("PointCloud");
            ImGui::Combo("Level", &level, level_of_details_arr.data(), static_cast<int>(level_of_details_arr.size()));
            ImGui::Text(fps_str.c_str());
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
}

#endif // TEST1
  