/**
 * 1. glTF加载模型
 * 2. glTF加载glb类型的模型
 * 3. glTF加载带有纹理的gltf类型模型(stb_image加载纹理)
 * 4. 测试相机正交投影和透视投影互相转换
 */

#define TEST4

#ifdef TEST1

#include <glTFModel.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 30 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("shaders/02_03_TEST1.vs", "shaders/02_03_TEST1_color.fs");

    ModelglTFLoading glTFModle("models/cube.gltf");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glTFModle.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <glTFModel.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("shaders/02_03_TEST1.vs", "shaders/02_03_TEST1_color.fs");

    ModelglTFLoading glTFModle("models/CesiumMan.glb");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glTFModle.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <glTFModel.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("shaders/02_03_TEST1.vs", "shaders/02_03_TEST3_texture.fs");

    ModelglTFLoading glTFModle("models/FlightHelmet/FlightHelmet.gltf");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glTFModle.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <glTFModel.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("shaders/02_03_TEST1.vs", "shaders/02_03_TEST3_texture.fs");

    //---------------------------------------------------------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    ModelglTFLoading glTFModle("models/FlightHelmet/FlightHelmet.gltf");

    int indexCameraType { 0 };
    const char* const strCameraType[] { "Parallel", "Perspective" };

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        init.SetParallel(indexCameraType == 0 ? true : false);

        program.Use();
        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glTFModle.Draw(program);

        //---------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            ImGui::Combo("Camera type", &indexCameraType, strCameraType, 2);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4
