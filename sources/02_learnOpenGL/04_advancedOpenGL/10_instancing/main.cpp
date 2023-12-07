/*
 * 1. 实例化，一组顶点绘制多个同样的图形 gl_InstanceID
 * 2. 使用多个VBO的方式给每个实例设置不同的属性 glVertexAttribDivisor
 * 3. 对 GL_TRIANGLE_STRIP 类型的图元进行实例化
 */

#define TEST3

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
    ShaderProgram program("shaders/02_04_10_TEST1.vs", "shaders/02_04_10_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 2 * 3> vertices {
        -0.9f, -0.1f, 
        -0.8f, -0.1f,
        -0.85f, 0.1f,
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    program.Use();
    GLfloat offset = 0.11f;
    for (size_t i = 0; i < 10; i++)
    {
        program.SetUniform1f("offsets[" + std::to_string(i) + "]", i * offset);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        // 最后一个参数表示实例的大小，会影响顶点着色器中的 gl_InstanceID
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 10);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

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

#ifdef TEST2

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

constexpr GLsizei numOfInstances { 10 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("shaders/02_04_10_TEST2.vs", "shaders/02_04_10_TEST2.fs");

    // clang-format off
    std::array<GLfloat, 5 * 4> vertices {
        -0.9f, -0.1f,      1.0f,  0.0f,  0.0f,
        -0.8f, -0.1f,      0.0f,  1.0f,  0.0f,
        -0.8f,  0.1f,      0.0f,  0.0f,  1.0f,
        -0.9f,  0.1f,      1.0f,  1.0f,  1.0f,
    };
    std::array<GLuint, 3 * 2> indices {
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    GLfloat offset = 0.11f;
    std::array<GLfloat, numOfInstances> offsets;
    for (size_t i = 0; i < numOfInstances; i++)
    {
        // 相对于第一个实例的偏移量，array的大小应该和实例个数一致
        offsets[i] = i * offset;
    }

    //------------------------------------------------------------------------
    // 生成多个Buffer(VBO EBO)，并映射数据
    GLuint instanceVBO { 0 };
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint VBO { 0 };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint EBO { 0 };
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //------------------------------------------------------------------------
    // 关于多个VBO绑定至一个VAO的用法请看 02_04_07_TEST5
    GLuint VAO { 0 };
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // 参数1：顶点属性，和顶点着色器中的location对应。参数2：属性除数(Attribute Divisor)。
    // 默认情况下，属性除数是0，告诉OpenGL我们需要在顶点着色器的每次迭代（遍历每个顶点）时更新顶点属性(location对应的数据)。
    // 设置为1时，我们告诉OpenGL我们希望在渲染一个新实例的时候更新顶点属性，即该顶点属性的数组大小应该和实例的个数一致
    // 而设置为2时，我们希望每2个实例更新一次属性，即该顶点属性的数组大小应该为实例个数的一半
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);

    //------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        program.Use();
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0, numOfInstances);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

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

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("shaders/02_04_10_TEST1.vs", "shaders/02_04_10_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 2 * 4> vertices {
        -0.9f, -0.1f, 
        -0.8f, -0.1f,
        -0.9f,  0.1f,
        -0.8f,  0.1f,
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

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    program.Use();
    GLfloat offset = 0.11f;
    for (size_t i = 0; i < 10; i++)
    {
        program.SetUniform1f("offsets[" + std::to_string(i) + "]", i * offset);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        // 实例化是对每一个drawcall使用，即glDrawArrays或glDrawElements，而不是最基本的线段或三角面
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 10);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

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

#endif // TEST3