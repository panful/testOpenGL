
/**
 * 1. learnOpenGL教程中的代码 assimp加载模型
 */

#define TEST0

#ifdef TEST0

#include <iostream>

int main()
{
    std::cout << "Please download Assimp from https://kimkulling.itch.io/the-asset-importer-lib\n";
}

#endif // TEST0

#ifdef TEST1

#include <model.hpp>

int main()
{
    InitOpenGL init(Camera({ 5, 5, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("shaders/02_03_01_TEST1.vs", "shaders/02_03_01_TEST1.fs");

    ModelLoading::Model ourModel("models/airplane/11803_Airplane_v1_l1.obj", false);

    glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("proj", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());
        program.SetUniformMat4("model", model);

        ourModel.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1
