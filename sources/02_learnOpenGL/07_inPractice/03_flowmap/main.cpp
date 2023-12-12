/**
 * 1. flowmap 的简单示例
 * 
 * 生成flowmap https://teckartist.com/
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();

    //---------------------------------------------------------------------------------
    // 初始化Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    ShaderProgram program("shaders/02_07_03_TEST1.vert", "shaders/02_07_03_TEST1.frag");

    // clang-format off
    std::vector<float> rect{
        -.5f, -.5f,  0.f,   0.f, 0.f,
         .5f, -.5f,  0.f,   1.f, 0.f,
        -.5f,  .5f,  0.f,   0.f, 1.f,
         .5f,  .5f,  0.f,   1.f, 1.f,
    };
    // clang-format on

    Renderer renderRect(rect, { 3, 2 }, GL_TRIANGLE_STRIP);

    // clang-format off
    std::vector<Texture*> water{ 
        new Texture("textures/flowmap/water.jpg"), 
        new Texture("textures/flowmap/water2.jpg"),
        new Texture("textures/flowmap/water3.jfif"),
    };
    // clang-format on
    Texture flowmap("textures/flowmap/flowmap.png");

    float flowstrength { .5f };
    int comboIndex { 0 };
    const char* const waterselect[] { "water1", "water2", "water3" };
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //--------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("FlowMap");
            ImGui::SliderFloat("flow strength", &flowstrength, 0.f, 1.f);
            ImGui::Combo("water", &comboIndex, waterselect, 3);
            ImGui::End();
        }

        //--------------------------------------------------
        program.Use();

        program.SetUniformMat4("proj", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniform1i("tex_water", 0);
        program.SetUniform1i("tex_flowmap", 1);

        program.SetUniform1f("time", static_cast<float>(glfwGetTime()));
        program.SetUniform1f("flowstrength", flowstrength);

        water[comboIndex]->Use(0);
        flowmap.Use(1);

        renderRect.Draw();

        //--------------------------------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1