/**
 * 1. 通过在片段着色器中计算模长绘制虚线
 * 
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL opengl;
    auto window = opengl.GetWindow();

    ShaderProgram program("shaders/02_07_04_TEST1.vs", "shaders/02_07_04_TEST1.fs");

    // clang-format off
    std::vector<float> line_vertices {
        -0.8f, -0.8f,  0.0f,    1.f, 0.f, 0.f,
         0.8f,  0.8f,  0.0f,    1.f, 0.f, 0.f,
    };
    // clang-format on

    Renderer lines(line_vertices, { 3, 3 });

    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 一次只能绘制一条虚线
        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));
        program.SetUniform3f("startPos", -0.8f, -0.8f, 0.0f);

        lines.Draw(GL_LINES);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // TEST1
