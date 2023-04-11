/*
 * 1. 使用几何着色器将输入的顶点绘制成三角形
 * 2. 将顶点着色器的变量传入到几何着色器再到片段着色器
 * 3. 利用几何着色器在给定位置生成立方体并开启深度测试
 * 4. 将三角形带（GL_TRIANGLE_STRIP）传入几何着色器
 * 5. 几何着色器中变量 gl_PrimitiveIDIn 和 gl_PrimitiveID 的使用
 * 6. gl_Layer的使用，一般可以用于渲染3D纹理、立方体阴影映射、立方体环境映射等场景
 * 7. 几何着色器多次实例化、多视口 gl_ViewportIndex gl_InvocationID
 * 8. 使用几何着色器实现广告牌效果（物体始终朝向相机）
 */

#define TEST8

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

#ifdef TEST6

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST6.fs", "resources/02_04_09_TEST6.gs");

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

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()) / 3);

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

#endif // TEST6

#ifdef TEST7

#include <array>
#include <common.hpp>

constexpr int width = 800;
constexpr int height = 600;
constexpr float wot = 400.f;
constexpr float hot = 300.f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ShaderProgram program("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST2.fs", "resources/02_04_09_TEST7.gs");

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
    // 设置视口索引及视口范围，OpenGL版本必须高于410
    // 参数1对应于几何着色器中的 gl_ViewportIndex
    glViewportIndexedf(0, 0.0f, 0.0f, wot, hot);
    glViewportIndexedf(1, wot, 0.0f, wot, hot);
    glViewportIndexedf(2, 0.0f, hot, wot, hot);
    glViewportIndexedf(3, wot, hot, wot, hot);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 每个视口使用不同的模型矩阵
        program.Use();
        glm::mat4 model[4];
        model[0] = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        model[1] = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.f, 0.f, 0.f));
        model[2] = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));
        model[3] = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        program.SetUniformMat4("model[0]", model[0]);
        program.SetUniformMat4("model[1]", model[1]);
        program.SetUniformMat4("model[2]", model[2]);
        program.SetUniformMat4("model[3]", model[3]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()) / 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers

    glDeleteVertexArrays(1, &VAO);
    program.DeleteProgram();

    glfwTerminate();
    return 0;
}

#endif // TEST7

#ifdef TEST8

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_09_TEST8.vs", "resources/02_04_09_TEST1.fs");
    ShaderProgram billBoardProgram("resources/02_04_09_TEST1.vs", "resources/02_04_09_TEST1.fs", "resources/02_04_09_TEST8.gs");

    // clang-format off
    std::array<GLfloat, 2 * 3> vertices{
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
    };

    std::array<GLfloat, 3 * 3> vertices2{
        -0.5f, -0.5f, 0.0f,    // left
         0.5f, -0.5f, 0.0f,    // right
         0.0f,  0.0f, 0.0f,    // top
    };
    // clang-format on

    // 两个广告牌
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

    // 一个用来和广告牌对比的三角形
    GLuint VAO2 { 0 };
    {
        GLuint VBO { 0 };
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO2);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    //----------------------------------------------------------------------------------
    // 将glDrawArrays输入的GL_POINTS绘制成三角形

    glPointSize(5.f);
    // glm::vec3 cameraPos(0, 0, 5);
    // auto viewMat = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // auto projectionMat = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.f, 1000.f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto time = static_cast<float>(glfwGetTime());
        auto radius = 5.f;
        auto cameraPos = glm::vec3(radius * std::sin(time), 0, radius * std::cos(time));
        auto viewMat = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        auto projectionMat = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.f, 1000.f);

        //-----------------------------------------------------------------
        // 广告牌始终朝向相机，即观察者始终都能看到正面
        billBoardProgram.Use();
        billBoardProgram.SetUniformMat4("MatVP", projectionMat * viewMat);
        billBoardProgram.SetUniform3fv("cameraPos", cameraPos);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()) / 3);
        glBindVertexArray(0);

        //-----------------------------------------------------------------
        // 随着时间旋转的三角形
        program.Use();
        program.SetUniformMat4("MatVP", projectionMat * viewMat);

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices2.size()) / 3);
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

#endif // TEST8