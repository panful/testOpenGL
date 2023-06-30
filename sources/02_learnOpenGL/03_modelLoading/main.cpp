
/*
 * 1. learnOpenGL教程中的代码
 */

#define TEST1

#ifdef TEST1

#include <model.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 10 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_03_TEST1.vs", "resources/02_03_TEST1.fs");

    ModelLoading::Model ourModel("resources/backpack.obj");

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
        model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        program.SetUniformMat4("model", model);

        ourModel.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1
