/*
 * 1. glDepthFunc 深度测试函数的设置
 * 2. 对不同的图元使用不同的深度测试函数
 * 3. glDepthMask 设置深度掩码是否更新深度缓冲
 * 4. 对深度值进行可视化 gl_FragCoord.z
 * 5. 
 */

#define TEST4

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

        auto modleMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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

        auto modleMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        // 接下来绘制的图元的深度值如果小于深度缓冲中的深度值则保留
        // 因为之前的深度值被glClear()清除了，所以接下来绘制的图元都会保留，直到下一次修改深度测试函数
        // 可以将调用glClear()清除深度缓冲之后，深度缓冲中的深度值认为是一个无穷大的值
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

        auto modleMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        // 从观察坐标到裁剪坐标（应用投影矩阵）的过程就已经将变换z值的非线性方程嵌入进去了，即透视投影矩阵已经包含了这个非线性方程
        auto perspectiveMat = glm::perspective(glm::radians(30.f), 800.f / 800.f, 0.1f, 100.f);
        auto orthoMat = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

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

