/*
 * 1. 使用glm::lookAt设置观察矩阵
 * 2. 相机绕y轴随时间旋转
 * 3. 键盘控制相机平移，即对物体缩放，平移
 * 4. 键盘控制相机观察的方向，不修改相机位置（俯仰角pitch偏航角yaw）
 * 5. 键盘控制相机绕(0,0,0)旋转，本质是修改相机位置
 * 6. 鼠标控制相机绕(0,0,0)旋转，本质是修改相机位置
 * 7. 鼠标控制相机观察的方向，俯仰角，偏航角
 * 8. 测试自定义相机
 */

// 万向节死锁 https://zhuanlan.zhihu.com/p/344050856

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
        0, 1, 3,        1, 2, 3,
        1, 5, 2,        5, 6, 2,
        5, 4, 6,        4, 7, 6,
        4, 0, 7,        0, 3, 7, 
        3, 2, 7,        2, 6, 7,
        0, 1, 4,        1, 5, 4,
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
