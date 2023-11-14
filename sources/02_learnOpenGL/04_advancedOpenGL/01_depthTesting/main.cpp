/**
 * 1. glDepthFunc 深度测试函数的设置
 * 2. 对不同的图元使用不同的深度测试函数
 * 3. glDepthMask 设置深度掩码是否更新深度缓冲
 * 4. 对深度值进行可视化 gl_FragCoord.z
 * 5. 提前深度测试演示
 * 6. glDepthRange设置深度值映射范围
 * 7. GL_DEPTH_CLAMP 将视锥体内的所有顶点都显示，不考虑近裁剪平面
 * 8. 图层，后面绘制的图层始终在之前的图层上面
 * 9. 遮挡查询遮挡剔除 Hierachical Z-Buffer(HZB)
 */

#define TEST9

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
    ShaderProgram program("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右上
        -0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.8f, -0.8f,  0.8f,     0.0f, 1.0f, 0.0f, // 左下
         0.8f, -0.8f,  0.8f,     0.0f, 1.0f, 0.0f, // 右下
         0.8f,  0.8f, -0.8f,     0.0f, 1.0f, 0.0f, // 右上
        -0.8f,  0.8f, -0.8f,     0.0f, 1.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置深度测试函数
    glDepthFunc(GL_ALWAYS);

    // glDepthFunc parameter:
    // default: GL_LESS
    // GL_ALWAYS	永远通过深度测试
    // GL_NEVER	    永远不通过深度测试
    // GL_LESS	    在片段深度值小于缓冲的深度值时通过测试
    // GL_EQUAL	    在片段深度值等于缓冲区的深度值时通过测试
    // GL_LEQUAL	在片段深度值小于等于缓冲区的深度值时通过测试
    // GL_GREATER	在片段深度值大于缓冲区的深度值时通过测试
    // GL_NOTEQUAL	在片段深度值不等于缓冲区的深度值时通过测试
    // GL_GEQUAL	在片段深度值大于等于缓冲区的深度值时通过测试

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modleMat       = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        // 如果使用 GL_ALWAYS 永远通过深度测试，则后面渲染的图元的片段始终都会保存，之前渲染的都会丢弃
        // 本例中的绿色平面始终在红色立方体的上面

        // 红色立方体
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        // 绿色平面
        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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
    ShaderProgram program("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右上
        -0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.8f, -0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 左下
         0.8f, -0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.8f,  0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 右上
        -0.8f,  0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modleMat       = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        // 接下来绘制的图元的深度值如果小于深度缓冲中的深度值则保留
        // 因为之前的深度值被glClear()清除了，所以接下来绘制的图元都会保留，直到下一次修改深度测试函数
        // 调用glClear()清除深度缓冲之后，深度缓冲中的深度值都是1.0，深度值范围是：[0.0, 1.0]
        glDepthFunc(GL_LESS);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);

        // 接下来绘制的图元的深度值如果大于深度缓冲中的深度值则保留
        glDepthFunc(GL_GREATER);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);

        //-----------------------------------------------------------------------------
        // 渲染出来的效果会将绿色平面X,Y处于红色立方体范围内的部分都保存下来
        // 因为第一次渲染立方体的时候，会将立方体距离眼睛更近的顶点的深度值保存到深度缓冲
        // 第二次渲染平面的时候，深度值都是大于深度缓冲中的值的
        //-----------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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
    ShaderProgram program("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右上
        -0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.8f, -0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 左下
         0.8f, -0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.8f,  0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 右上
        -0.8f,  0.8f,  0.0f,     0.0f, 1.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modleMat       = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        // 对所有片段都执行深度测试，并将不符合深度测试函数的片段丢弃
        // 但是不把保留片段的深度值更新到深度缓冲中
        // 即深度缓冲中的深度值一直都是调用该函数之前的深度值，
        // 此时的深度缓冲是一个只读的缓冲
        // 默认是 GL_TRUE 既深度测试又更新深度缓冲
        // 最好不要在glClear()之前将深度掩码设置为 GL_FALSE
        glDepthMask(GL_FALSE);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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
    InitOpenGL initOpenGL("Depth Test", 800, 800);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_01_TEST4.vs", "resources/02_04_01_TEST4.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 0.0f, // 左下
         0.5f, -0.5f,  -.5f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f,  -.5f,     0.0f, 1.0f, 0.0f, // 右上
        -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------
    // 示例需要等一会才会显示渲染结果

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 使用modelMat让物体随着时间远离相机，即z值越来越大
        auto modelMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f - glfwGetTime() / 2.f));
        auto viewMat  = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        // 从观察坐标到裁剪坐标（应用投影矩阵）的过程就已经将变换z值的非线性方程嵌入进去了，即透视投影矩阵已经包含了这个非线性方程
        auto perspectiveMat = glm::perspective(glm::radians(30.f), 800.f / 800.f, 0.1f, 100.f);
        auto orthoMat       = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

        // 透视投影
        program.SetUniformMat4("transform", perspectiveMat * viewMat * modelMat);
        // 正交投影
        // program.SetUniformMat4("transform", orthoMat * viewMat * modelMat);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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
    ShaderProgram program("resources/02_04_01_TEST5.vs", "resources/02_04_01_TEST5.fs");

    // clang-format off
    std::array<GLfloat, 4 * 3> verticesPlane{
        // pos
        -0.5f, -0.5f, 0.0f,   // 左下
         0.5f, -0.5f, 0.0f,   // 右下
         0.5f,  0.5f, 0.0f,   // 右上
        -0.5f,  0.5f, 0.0f,   // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    //----------------------------------------------------------------------------------
    // 不同的硬件可能会有不同的结果，深度测试原理请看着色器文件

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置深度测试函数
    glDepthFunc(GL_LESS);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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
    ShaderProgram program("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后右上
        -0.5f,  0.5f, -.5f,     1.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.8f, -0.8f,  0.8f,     0.0f, 1.0f, 0.0f, // 左下
         0.8f, -0.8f,  0.8f,     0.0f, 1.0f, 0.0f, // 右下
         0.8f,  0.8f, -0.8f,     0.0f, 1.0f, 0.0f, // 右上
        -0.8f,  0.8f, -0.8f,     0.0f, 1.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int planeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane.size(), verticesPlane.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesPlane.size(), indicesPlane.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------
    // glDepthRange(nearNormDepth, farNormalDepth) 设置深度值映射范围
    // 参数可以取0.0到1.0范围内的任意值，甚至可以让nearNormDepth > farNormalDepth
    // 可以通过该函数可以实现：设置渲染顺序、半透明效果和精度控制（z-fighting)等

    // 默认的深度值映射范围：[0.0, 1.0]
    double range[2] { 0. };
    glGetDoublev(GL_DEPTH_RANGE, range);
    std::cout << "default depth range:\t" << range[0] << '\t' << range[1] << '\n';

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modleMat       = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        // 红色立方体，将深度值映射到[0.5, 1.0]
        glDepthRange(.5, 1.);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 绿色平面，将深度值映射到[0.0, 0.5]
        glDepthRange(.0, .5);
        glBindVertexArray(planeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesPlane.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        // 因为红色立方体的深度值范围为[0.5, 1.0]，绿色平面深度值范围为[0.0, 0.5]，
        // 所以绿色平面的深度值永远不会大于红色立方体的深度值，所以绿色平面永远都在上面

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> verticesCube{
        // pos                  // color
        .0f,  .0f,  .5f,        1.f, 0.f, 0.f,
        .0f, -.5f,  .0f,        1.f, 0.f, 0.f,
        .0f,  .0f, -.5f,        1.f, 0.f, 0.f,
        .0f,  .5f,  .0f,        1.f, 0.f, 0.f,
    };
    // clang-format on

    unsigned int rectVAO;
    {
        unsigned int VBO;

        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------
    // GL_DEPTH_CLAMP 可以让不考虑近裁剪平面的视锥体内的所有顶点都映射到GL_DEPTH_RANGE的范围内
    // 此时的视锥体就相当于是一个金字塔，相机到视锥体远平面之间的所有顶点都可以渲染
    // NDC坐标z值小于-1.0时可以显示，大于1.0仍然不能显示，因为大于1.0时不在视锥体内部
    glEnable(GL_DEPTH_CLAMP);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modelMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0, 1, 0));
        auto viewMat  = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        // 距离相机2.7到3.3之间的片段才可以显示，启用GL_DEPTH_CLAMP之后，0.0到3.3之间的都可以显示
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 2.7f, 3.3f);
        program.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        //----------------------------------------------------------------------------------
        // 将模型向屏幕外移动0.7个单位，即一部分图元的z值会大于1.0，GL_DEPTH_CLAMP使能，z值大于1.0的部分也不会显示
        // program.SetUniformMat4("transform", glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, .7f)) * modelMat);
        // 将模型向屏幕内移动0.7个单位，即使z值小于-1.0，仍然可以显示
        // program.SetUniformMat4("transform", glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -.7f)) * modelMat);

        glBindVertexArray(rectVAO);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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

#endif // TEST7

#ifdef TEST8

#include <common.hpp>

Renderer CreateAboveGrid()
{
    // clang-format off
    std::vector<float> vertices {
        -0.5f, -0.5f,  0.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,     1.0f, 0.0f, 0.0f,
         0.0f,  0.5f,  0.0f,     1.0f, 0.0f, 0.0f,
    };
    // clang-format on

    return Renderer(vertices, { 3, 3 });
}

Renderer CreateBelowGrid()
{
    // clang-format off
    std::vector<float> vertices {
        -0.5f, -0.5f,  0.1f,     0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.1f,     0.0f, 1.0f, 0.0f,
         0.0f,  0.5f,  0.1f,     0.0f, 1.0f, 0.0f,
    };
    // clang-format on

    return Renderer(vertices, { 3, 3 });
}

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    auto above  = CreateAboveGrid();
    auto below  = CreateBelowGrid();

    ShaderProgram shader("resources/02_04_01_TEST1.vs", "resources/02_04_01_TEST1.fs");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------
        // 图层1
        glClearColor(0.f, .5f, .5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();
        auto m = glm::mat4(1.f);
        auto v = init.GetViewMatrix();
        auto p = init.GetProjectionMatrix();
        shader.SetUniformMat4("transform", p * v * m);
        below.Draw(GL_TRIANGLES);

        //----------------------------------------------------------------------------
        // 图层2
        // 将深度缓冲清除，颜色缓冲保留，后面绘制的图元就会显示在之前绘制的图元上面
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.Use();
        shader.SetUniformMat4("transform", p * v * m);
        above.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#endif // TEST8

#ifdef TEST9

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_01_TEST9.vs", "resources/02_04_01_TEST9.fs");

    // clang-format off
    std::vector<GLfloat> vertices1 {
        -0.5f, -0.5f, 0.f,   1.f, 0.f, 0.f,
         0.5f, -0.5f, 0.f,   1.f, 0.f, 0.f,
         0.5f,  0.5f, 0.f,   1.f, 0.f, 0.f,
        -0.5f,  0.5f, 0.f,   1.f, 0.f, 0.f,
    };

    std::vector<GLfloat> vertices2 {
        -0.4f, -0.4f, -.1f,   0.f, 1.f, 0.f,
         0.4f, -0.4f, -.1f,   0.f, 1.f, 0.f,
         0.4f,  0.4f, -.1f,   0.f, 1.f, 0.f,
        -0.4f,  0.4f, -.1f,   0.f, 1.f, 0.f,
    };

    std::vector<GLfloat> vertices3 {
        -0.4f, -0.4f, .1f,   0.f, 0.f, 1.f,
         0.4f, -0.4f, .1f,   0.f, 0.f, 1.f,
         0.4f,  0.4f, .1f,   0.f, 0.f, 1.f,
        -0.4f,  0.4f, .1f,   0.f, 0.f, 1.f,
    };

    std::vector<GLuint> indices { 0, 1, 2, 0, 2, 3 };
    // clang-format on

    Renderer ren1(vertices1, indices, { 3, 3 }, GL_TRIANGLES);
    Renderer ren2(vertices2, indices, { 3, 3 }, GL_TRIANGLES);
    Renderer ren3(vertices3, indices, { 3, 3 }, GL_TRIANGLES);

    GLuint queryId { 0 };
    glGenQueries(1, &queryId);
    GLint queryResult { 0 };

    //----------------------------------------------------------------------------------
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        ren1.Draw();

        // queryResult表示通过深度测试的片段数量，如果为0则表示这个物体被完全遮挡
        // 可以先绘制一次需要查询的物体包围盒，如果该包围盒被遮挡，则不进行绘制(剔除)，没有被遮挡再进行实际绘制
        {
            // 开启遮挡查询
            glBeginQuery(GL_SAMPLES_PASSED, queryId);
            // 绘制需要查询的物体
            ren2.Draw();
            // 结束遮挡查询
            glEndQuery(GL_SAMPLES_PASSED);
            // 获取查询结果
            glGetQueryObjectiv(queryId, GL_QUERY_RESULT, &queryResult);
            std::cout << "green result:\t" << queryResult << std::endl;
        }

        {
            glBeginQuery(GL_SAMPLES_PASSED, queryId);
            ren3.Draw();
            glEndQuery(GL_SAMPLES_PASSED);

            glGetQueryObjectiv(queryId, GL_QUERY_RESULT, &queryResult);
            std::cout << "blue result:\t" << queryResult << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer
    glDeleteQueries(1, &queryId);

    glfwTerminate();
    return 0;
}

#endif // TEST9