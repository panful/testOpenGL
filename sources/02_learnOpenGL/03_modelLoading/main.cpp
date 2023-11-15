
/**
 * 1. learnOpenGL教程中的代码 assimp加载模型
 * 2. glTF加载模型
 * 3. glTF加载带有纹理的模型(stb_image加载纹理)
 */

#define TEST3

#ifdef TEST1

#include <model.hpp>

int main()
{
    InitOpenGL init(Camera({ 5, 5, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_03_TEST1.vs", "resources/02_03_TEST1.fs");

    // ModelLoading::Model ourModel("resources/backpack/backpack.obj");
    ModelLoading::Model ourModel("resources/airplane/11803_Airplane_v1_l1.obj", false);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model           = glm::scale(model, glm::vec3(0.001f, 0.001f, 0.001f));
        // model           = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
        program.SetUniformMat4("model", model);

        ourModel.Draw(program);

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
    InitOpenGL init(Camera({ 0, 0, 30 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_03_TEST2.vs", "resources/02_03_TEST2.fs");

    ModelglTFLoading glTFModle("./resources/glTF/cube.gltf");

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
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_03_TEST2.vs", "resources/02_03_TEST3.fs");

    ModelglTFLoading glTFModle("./resources/glTF/FlightHelmet/FlightHelmet.gltf");

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