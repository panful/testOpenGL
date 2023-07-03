
/*
 * 1. 渲染*.dae格式的模型文件
 * 2. 对*.dae格式的模型以动画渲染
 */

#define TEST2

#ifdef TEST1

#include <animation.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_08_01_TEST1.vs", "resources/02_08_01_TEST1.fs");

    ModelLoading::Model ourModel("resources/dancing_vampire.dae", false);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto model = glm::scale(glm::mat4(1.f), glm::vec3(0.01f, 0.01f, 0.01f));
        model      = glm::translate(model, glm::vec3(0.0f, -100.f, 0.0f));
        program.SetUniformMat4("model", model);
        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        ourModel.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <animation.hpp>

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program("resources/02_08_01_TEST2.vs", "resources/02_08_01_TEST1.fs");

    ModelLoading::Model ourModel("resources/dancing_vampire.dae", false);
    Animation::Animation danceAnimation("resources/dancing_vampire.dae", &ourModel);
    Animation::Animator animator(&danceAnimation);

    glEnable(GL_DEPTH_TEST);

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime          = currentFrame - lastFrame;
        lastFrame          = currentFrame;

        animator.UpdateAnimation(deltaTime);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto transforms = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            program.SetUniformMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }

        glm::mat4 model = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.5f, 0.0f));
        program.SetUniformMat4("model", model);
        program.SetUniformMat4("projection", init.GetProjectionMatrix());
        program.SetUniformMat4("view", init.GetViewMatrix());

        ourModel.Draw(program);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2