/*
 * 1. 使用几何着色器将输入的顶点绘制成三角形
 */

#define TEST1

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST1.fs", "resources/02_04_09_TEST1.gs");

    // clang-format off
    std::array<GLfloat, 3 * 3> vertices{
        -0.5f, -0.5f, 0.0f,    // left
         0.5f, -0.5f, 0.0f,    // right
         0.0f,  0.5f, 0.0f,    // top
    };
    // clang-format on

    GLuint VAO { 0 };
    {
        GLuint VBO { 0 };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    //----------------------------------------------------------------------------------
    // 将glDrawArrays输入的GL_POINTS绘制成三角形

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()) / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers

    glDeleteVertexArrays(1, &VAO);
    program.DeleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST1