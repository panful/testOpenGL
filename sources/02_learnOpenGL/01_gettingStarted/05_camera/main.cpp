/*
 * 1. 使用glm::lookAt设置观察矩阵
 * 2. 相机绕y轴随时间旋转
 * 3. 键盘控制相机平移，即对物体缩放，平移
 * 4. 键盘控制相机观察的方向，不修改相机位置（俯仰角pitch偏航角yaw）
 * 5. 键盘控制相机绕(0,0,0)旋转，本质是修改相机位置
 * 6. 鼠标控制相机绕(0,0,0)旋转，本质是修改相机位置
 * 7. 鼠标控制相机观察的方向，俯仰角，偏航角
 * 8. 测试自定义相机
 * 9. 正交投影 透视投影互相转换
 * 10. Camera2 测试轨道相机
 * 11. Camera2 测试 FPS 相机
 */

// 万向节死锁 https://zhuanlan.zhihu.com/p/344050856

#define TEST11

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
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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

        auto time    = static_cast<float>(glfwGetTime());
        auto radius  = 5.0f;
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

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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
        // 如果照相机位置始终在z=3这个平面，照相机观察方向始终和y平行，那么照相机没有在z轴上时看到的就是物体的侧面
        // 当照相机观察方向 = cameraPos + cameraFront时，因为cameraFront的z分量是-1，就可以保证照相机始终观察的是物体，且观察方向不会反向
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

#ifdef TEST4

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

auto pitch = 0.0f;
auto yaw   = -90.0f; // 保证cameraFront起始值是(0,0,-1)

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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
        // cameraFront是摄像机的方向，摄像机位置向量 + 摄像机方向向量可以让摄像机始终注视着目标
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

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // pitch 俯仰角 绕x轴旋转 可以理解为抬头和低头看
    // yaw   偏航角 绕y轴旋转 可以理解为往左边或右边转头看
    // roll  滚转角 绕z轴旋转

    float cameraSpeed = 0.5f; // adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        pitch -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        pitch += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        yaw += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        yaw -= cameraSpeed;

    glm::vec3 front(1.0f);
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);
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

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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
        // 如果照相机位置始终在z=3这个平面，照相机观察方向始终和y平行，那么照相机没有在z轴上时看到的就是物体的侧面
        // 当照相机观察方向 = cameraPos + cameraFront时，因为cameraFront的z分量是-1，就可以保证照相机始终观察的是物体，且观察方向不会反向
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

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto tempMat4 = glm::mat4(1.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        tempMat4 = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(1.f, 0.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        tempMat4 = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(1.f, 0.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        tempMat4 = glm::rotate(glm::mat4(1.0f), glm::radians(5.0f), glm::vec3(0.f, 1.f, 0.f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        tempMat4 = glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f), glm::vec3(0.f, 1.f, 0.f));

    auto tempVec4 = tempMat4 * glm::vec4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);

    cameraPos   = glm::vec3(tempVec4.x, tempVec4.y, tempVec4.z);
    cameraFront = glm::normalize(glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// 鼠标是否第一次进入窗口
auto firstMouse = true;

// 上一次鼠标的位置
auto lastPosX = 0.0f;
auto lastPosY = 0.0f;

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetCursorPosCB(mouse_callback);
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // 鼠标第一次进入窗口不做变换
    if (firstMouse)
    {
        lastPosX   = static_cast<float>(xpos);
        lastPosY   = static_cast<float>(ypos);
        firstMouse = false;
        return;
    }

    auto offsetX = static_cast<float>(xpos) - lastPosX;
    auto offsetY = static_cast<float>(ypos) - lastPosY;

    lastPosX = static_cast<float>(xpos);
    lastPosY = static_cast<float>(ypos);

    auto sensitivity = 0.5f;

    auto rotateX = glm::rotate(glm::mat4(1.0f), glm::radians(offsetY / sensitivity), glm::vec3(1.0f, 0.f, 0.f));
    auto rotateY = glm::rotate(glm::mat4(1.0f), glm::radians(offsetX / sensitivity), glm::vec3(0.0f, 1.f, 0.f));

    auto tempCameraPos = rotateX * rotateY * glm::vec4(cameraPos, 1.0f);
    cameraPos          = glm::vec3(tempCameraPos.x, tempCameraPos.y, tempCameraPos.z);
    cameraFront        = glm::normalize(glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z));
}

#endif // TEST6

#ifdef TEST7

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// 鼠标是否第一次进入窗口
auto firstMouse = true;

// 上一次鼠标的位置
auto lastPosX = 0.0f;
auto lastPosY = 0.0f;

auto pitch = 0.0f;
auto yaw   = -90.0f;

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetCursorPosCB(mouse_callback);
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastPosX   = static_cast<float>(xpos);
        lastPosY   = static_cast<float>(ypos);
        firstMouse = false;
        return;
    }

    auto offsetX = static_cast<float>(xpos) - lastPosX;
    auto offsetY = lastPosY - static_cast<float>(ypos);

    lastPosX = static_cast<float>(xpos);
    lastPosY = static_cast<float>(ypos);

    auto sensitivity = 0.05f;

    offsetX *= sensitivity;
    offsetY *= sensitivity;

    pitch += offsetY;
    yaw += offsetX;

    glm::vec3 front(1.0f);
    front.x     = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y     = sin(glm::radians(pitch));
    front.z     = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

#endif // TEST7

#ifdef TEST8

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 5.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback([](char key) { std::cout << "key: " << key << '\n'; });
    initOpenGL.SetMiddleButtonCallback([](int x, int y) { std::cout << "x: " << x << '\t' << "y: " << y << '\n'; });
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices{
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

    std::vector<GLuint> indices{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    Renderer renderer(vertices, indices, { 3, 3 });

    //----------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", initOpenGL.GetVPMatrix());

        renderer.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST8

#ifdef TEST9

#include <common.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <numbers>

// @brief 创建一个球心在(0,0,0)的球体
// @param [in] longitude 经线上的顶点个数
// @param [in] latitude 纬线上的顶点个数
// @param [in] radius 半径
Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = 10.f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3 }, GL_TRIANGLES);
}

int main()
{
    InitOpenGL initOpenGL("Parallel Camera", 800, 600);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_01_05_TEST9.vert", "shaders/02_01_05_TEST9.frag");

    //---------------------------------------------------------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    auto sphere = CreateSphere();

    //---------------------------------------------------------------------
    int indexCameraType { 0 };
    const char* const strCameraType[] { "Ortho", "Perspective", "Frustum" };

    float cameraPosZ { 50.f };

    float clipNear { .1f }, clipFar { 100.f };
    float viewAngle { 30.f };

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto aspect = 800.f / 600.f;
        auto tmp    = std::tan(glm::radians(viewAngle / 2.f));
        auto width  = cameraPosZ * tmp * aspect;
        auto height = cameraPosZ * tmp;

        // NearZ 和 FarZ 需要额外注意，太小可能会导致场景被裁剪
        auto frustProj = glm::frustum(-width, width, -height, height, -cameraPosZ, cameraPosZ);
        auto perspProj = glm::perspective(glm::radians(viewAngle), aspect, clipNear, clipFar);
        auto orthoProj = glm::ortho(-width, width, -height, height, -cameraPosZ, cameraPosZ);

        auto model = glm::mat4(1.f);
        auto view  = glm::lookAt(glm::vec3(0.f, 0.f, cameraPosZ), glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        auto proj  = indexCameraType == 0 ? orthoProj : indexCameraType == 1 ? perspProj : frustProj;

        program.Use();
        program.SetUniformMat4("model", model);
        program.SetUniformMat4("view", view);
        program.SetUniformMat4("proj", proj);
        sphere.Draw();

        //---------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            ImGui::Combo("Camera type", &indexCameraType, strCameraType, 3);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <common2.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main()
{
    Window window("Test Window", 800, 600);
    window.interactor.camera.type = Camera2::Type::Orthographic;

    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    // clang-format off
    std::vector<GLfloat> vertices {
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

    std::vector<GLuint> indices {
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    Renderer drawable(vertices, indices, { 3, 3 }, GL_TRIANGLES);

    auto aabb = AABBTool::ComputeAABB(vertices, 6);
    window.interactor.camera.Reset(aabb);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.interactor.camera.ResetClipRange(aabb);

        auto proj = window.interactor.camera.projMat;
        auto view = window.interactor.camera.viewMat;
        
        program.Use();
        program.SetUniformMat4("transform", proj * view);

        drawable.Draw();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            if (ImGui::Button("Print"))
            {
                window.interactor.camera.Print();
            }
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
}

#endif // TEST10

#ifdef TEST11

#include <common2.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main()
{
    Window window("Test Window", 800, 600);
    window.interactor.camera.eyePos    = glm::vec3(0.f);
    window.interactor.camera.focalPos  = glm::vec3(0.f, 0.f, 1.f);
    window.interactor.camera.viewUp    = glm::vec3(0.f, 1.f, 0.f);
    window.interactor.camera.clipRange = glm::vec2(.01f, 1.f);
    window.interactor.camera.type      = Camera2::Type::Perspective;
    window.interactor.camera.behavior  = Camera2::Behavior::FPS;
    window.interactor.camera.Update();
    
    ShaderProgram program("shaders/02_01_05_TEST1.vs", "shaders/02_01_05_TEST1.fs");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    // clang-format off
    std::vector<GLfloat> vertices {
        // pos                // color
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
    };
    // clang-format on

    Renderer drawable(vertices, { 3, 3 }, GL_TRIANGLES);
    
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        auto proj = window.interactor.camera.projMat;
        auto view = window.interactor.camera.viewMat;

        program.Use();
        program.SetUniformMat4("transform", proj * view);

        drawable.Draw();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            if (ImGui::Button("Print"))
            {
                window.interactor.camera.Print();
            }
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
}

#endif // TEST11
