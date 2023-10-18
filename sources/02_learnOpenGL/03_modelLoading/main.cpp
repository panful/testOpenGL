
/*
 * 1. learnOpenGL教程中的代码
 */

#define TEST1

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
