/*
 * 1. 使用几何着色器将输入的顶点绘制成三角形
 * 2. 将顶点着色器的变量传入到几何着色器再到片段着色器
 * 3. 利用几何着色器在给定位置生成立方体并开启深度测试
 * 4. 将三角形带（GL_TRIANGLE_STRIP）传入几何着色器
 * 5. 几何着色器中变量 gl_PrimitiveIDIn 和 gl_PrimitiveID 的使用
 */

#define TEST5

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

#ifdef TEST2

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST2.vs", "resources/02_04_09_TEST2.fs", "resources/02_04_09_TEST2.gs");

    // clang-format off
    std::array<GLfloat, 3 * 2 * 6> vertices{
        -0.6f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
        -0.2f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,

         0.2f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.6f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,

        -0.2f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.2f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    //-----------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);

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
    ShaderProgram program("resources/02_04_09_TEST3.vs", "resources/02_04_09_TEST2.fs", "resources/02_04_09_TEST3.gs");

    // clang-format off
    std::array<GLfloat, 2 * 7> vertices{
        -2.0f, -2.0f,  0.0f,  1.0f,     1.0f, 0.0f, 0.0f,
         2.0f,  2.0f,  0.0f,  1.0f,     1.0f, 0.0f, 0.0f,
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

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }

    //-----------------------------------------------------------------------
    // 给两个顶点，在这两个顶点处各生成一个立方体，开启深度测试
    // 透视除法在几何着色器之后

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto modleMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.Use();
        program.SetUniformMat4("model", modleMat);
        program.SetUniformMat4("view", viewMat);
        program.SetUniformMat4("projection", projectiongMat);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size() / 7));
        glBindVertexArray(0);

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

#endif // TEST3

#ifdef TEST4

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST2.fs", "resources/02_04_09_TEST4.gs");

    // clang-format off
    std::array<GLfloat, 3 * 4> vertices{
        -0.5f, -0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,
         0.5f,  0.5f,  0.0f,
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
    // GL_TRIANGLE_STRIP类型的图元，传入到几何着色器的数据是每一个三角形数据
    // 即不会将这个三角形带的所有三角形一次传入到几何着色器

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()) / 3);

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

#endif // TEST4

#ifdef TEST5

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST2.fs", "resources/02_04_09_TEST5.gs");

    // clang-format off
    std::array<GLfloat, 3 * 8> vertices{
        -0.5f,  0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
        -0.2f,  0.5f,  0.0f,
        -0.2f, -0.5f,  0.0f,

         0.2f,  0.5f,  0.0f,
         0.2f, -0.5f,  0.0f,

         0.5f,  0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
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
    // GL_TRIANGLE_STRIP类型的图元，传入到几何着色器的数据是每一个三角形数据
    // 即不会将这个三角形带的所有三角形一次传入到几何着色器

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()) / 3);

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

#endif // TEST5