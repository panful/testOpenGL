/*
 * 1. 使用glm::lookAt设置观察矩阵
 * 2. 相机绕y轴随时间旋转
 * 3. 键盘控制相机位置方向，即对物体缩放，平移
 */

#define TEST3

#ifdef TEST1

#include <array>
#include <common.hpp>
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_05_TEST1.vs", "resources/02_01_05_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    // 开启深度测试，默认关闭
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 模型矩阵，将模型坐标转换为世界坐标
        // 此处不做任何变换，即模型坐标就是世界坐标
        auto m = glm::mat4(1.0f);
        // 观察矩阵，第一个是摄像机的位置，第二个是焦点位置，第三个是摄像机向上的方向
        auto v = glm::lookAt(glm::vec3(2, 2, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // 投影矩阵
        auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 100.0f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_05_TEST1.vs", "resources/02_01_05_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    // 开启深度测试，默认关闭
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto time = static_cast<float>(glfwGetTime());
        auto radius = 5.0f;
        auto cameraX = std::cos(time) * radius;
        auto cameraZ = std::sin(time) * radius;

        // 模型矩阵
        auto m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        // 观察矩阵
        // 绕着y轴半径为5的圆旋转
        auto v = glm::lookAt(glm::vec3(cameraX, 0, cameraZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        // 投影矩阵
        // auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 100.0f);
        auto p = glm::ortho(-5.f, 5.f, -5.f / (8 / 6.f), 5.f / (8 / 6.f), -5.f, 5.f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_05_TEST1.vs", "resources/02_01_05_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    // 开启深度测试，默认关闭
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 模型矩阵
        auto m = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        // 观察矩阵
        // cameraFront是一个始终指向屏幕内的向量，摄像机位置向量 + 摄像机方向向量可以让摄像机始终注视着目标
        auto v = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // 投影矩阵
        auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 100.0f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    program.DeleteProgram();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.05f; // adjust accordingly

    // w摄像机向上，物体向下
    // s摄像机向下，物体向上
    // a摄像机向左，物体向右
    // d摄像机向右，物体向左
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;

    // glm::cross(cameraFront, cameraUp); 返回的是右向量
    // glm::normalize()标准化可以让摄像机匀速移动
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // z摄像机向里，物体放大
    // x摄像机向外，物体缩小
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST3
