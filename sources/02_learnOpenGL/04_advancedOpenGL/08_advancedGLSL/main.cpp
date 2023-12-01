/*
 * 1. 着色器只会对有图元部分的gl_FragCoord进行遍历
 * 2. 在顶点着色器中设置 gl_PointSize
 * 3. 顶点着色器当前处理的顶点ID gl_VertexID
 * 4. 片段着色器判断当前片段是属于正向面的一部分还是背向面的一部分 gl_FrontFacing
 * 5. 使用提前深度测试时，设置片段的深度值 gl_FragDepth
 * 6. 着色器中接口块（Interface Block）的使用，就是把着色器之间传输的变量包裹起来形成一个结构体
 * 7. 多个着色器中使用同一个全局Uniform块，Uniform Buffer Object的使用
 */

#define TEST7

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
    ShaderProgram program("resources/02_04_08_TEST1.vs", "resources/02_04_08_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
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
    // 需要绘制的图元只有窗口的中间有一部分，并且绕y轴旋转
    // 着色器只会对属于这一部分的gl_FragCoord进行遍历
    // 只有背景色的部分就不会在着色器中对gl_FragCoord进行遍历

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绕y轴旋转
        program.Use();
        auto rotateMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
        program.SetUniformMat4("transform", rotateMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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
    ShaderProgram program("resources/02_04_08_TEST2.vs", "resources/02_04_08_TEST2.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.1f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.3f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.8f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 4> indices{
        0, 1, 2, 3
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

    // 启用在顶点着色器设置顶点的大小，即修改 gl_PointSize 的值
    glEnable(GL_PROGRAM_POINT_SIZE);

    // 直接在代码中修改顶点的大小
    // glPointSize(5.f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAO);
        // 这两种绘制方式都行
        // glDrawElements(GL_POINTS, 4, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

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

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_08_TEST3.vs", "resources/02_04_08_TEST2.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.2f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.2f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
    };

    std::array<GLfloat, 4 * 6> vertices2{
        // pos                  // color
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f, -0.2f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  0.2f, 0.0f,     1.0f, 0.0f, 0.0f,
        0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
    };

    std::array<GLuint, 4> indices{
        0, 1, 2, 3
    };

    std::array<GLuint, 3> indices2{
        3, 2, 1
    };
    // clang-format on

    GLuint VAO { 0 };
    {
        GLuint VBO { 0 }, EBO { 0 };
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

        glBindVertexArray(0);
    }

    GLuint VAO2 { 0 };
    {
        GLuint VBO { 0 }, EBO { 0 };
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices2.size(), indices2.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }
    //----------------------------------------------------------------------------------

    glEnable(GL_PROGRAM_POINT_SIZE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        //------------------------------------------------------
        glBindVertexArray(VAO);
        glDrawElements(GL_POINTS, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

        //------------------------------------------------------
        glBindVertexArray(VAO2);
        glDrawElements(GL_POINTS, static_cast<GLsizei>(indices2.size()), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

        //------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO2);
    

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
    ShaderProgram program("resources/02_04_08_TEST1.vs", "resources/02_04_08_TEST4.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f, // 后左下
         0.5f, -0.5f, -.5f,     1.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 1.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     1.0f, 1.0f, 1.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint, 6 * 2 * 3> indices{
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

    unsigned int plane1VAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &plane1VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(plane1VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    // 关闭背面剔除(默认关闭)
    glDisable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto distance = static_cast<float>(glfwGetTime() / 2.);
        distance = distance >= 3.f ? 2.99f : distance;
        std::cout << distance << '\n';

        // 将相机一直靠近立方体，直到相机进入立方体内部
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f - distance), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.Use();
        program.SetUniformMat4("transform", projectiongMat * viewMat);

        glBindVertexArray(plane1VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

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
    ShaderProgram program("resources/02_04_08_TEST1.vs", "resources/02_04_08_TEST5.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> verticesPlane{
        // pos                  // color
        -0.5f, -0.5f, -1.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f,  1.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f,  1.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, -1.0f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indicesPlane{
        0, 1, 3,
        1, 2, 3,
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
    // 然而，由我们自己设置深度值有一个很大的缺点，只要我们在片段着色器中对gl_FragDepth进行写入，
    // OpenGL就会（像深度测试小节中讨论的那样）禁用所有的提前深度测试(Early Depth Testing)。
    // 它被禁用的原因是，OpenGL无法在片段着色器运行之前得知片段将拥有的深度值，因为片段着色器可能会完全修改这个深度值。
    // 从OpenGL 4.2起，我们仍可以对两者进行一定的调和，具体使用方法请看片段着色器

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 设置深度测试函数
    glDepthFunc(GL_LESS);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearDepth(0.5f); // 将深度缓冲的值全部置为0.5，默认为1.0
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        glBindVertexArray(planeVAO);
        program.SetUniformMat4("transform", glm::mat4(1.f));
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
    ShaderProgram program("resources/02_04_08_TEST6.vs", "resources/02_04_08_TEST6.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
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

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绕y轴旋转
        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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

#endif // TEST6

#ifdef TEST7

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLuint UBO { 0 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program_red("resources/02_04_08_TEST7.vs", "resources/02_04_08_TEST7.fs");
    ShaderProgram program_green("resources/02_04_08_TEST7.vs", "resources/02_04_08_TEST7.fs");
    ShaderProgram program_blue("resources/02_04_08_TEST7.vs", "resources/02_04_08_TEST7.fs");
    ShaderProgram program_white("resources/02_04_08_TEST7.vs", "resources/02_04_08_TEST7.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos
        -0.5f, -0.5f, 0.5f,    // 前左下
         0.5f, -0.5f, 0.5f,    // 前右下
         0.5f,  0.5f, 0.5f,    // 前右上
        -0.5f,  0.5f, 0.5f,    // 前左上

        -0.5f, -0.5f, -.5f,    // 后左下
         0.5f, -0.5f, -.5f,    // 后右下
         0.5f,  0.5f, -.5f,    // 后右上
        -0.5f,  0.5f, -.5f,    // 后左上
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
    // clang-format on

    GLuint cubeVAO { 0 };
    {
        GLuint VBO { 0 }, EBO { 0 };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // 设置Uniform块的绑定点，也可以在着色器中使用"binging=x"指定（OpenGL4.2及以上版本支持）
    {
        // 获取块的序号，和绑定点不一样，注意区分
        auto index_Test = glGetUniformBlockIndex(program_red.GetProgram(), "Test");
        auto index_MatVP = glGetUniformBlockIndex(program_red.GetProgram(), "MatVP");

        std::cout << "index: " << index_Test << '\t' << index_MatVP << '\n';

        // 设置Uniform块的绑定点
        // glUniformBlockBinding(program_red.GetProgram(), index_red, 6);
        // glUniformBlockBinding(program_green.GetProgram(), index_green, 6);
        // glUniformBlockBinding(program_blue.GetProgram(), index_blue, 6);
        // glUniformBlockBinding(program_white.GetProgram(), index_white, 6);
    }

    // 创建一个UBO，并给uniform buffer预分配内存空间
    {
        glGenBuffers(1, &UBO);

        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // 1. 绑定目标
        // 2. 绑定点索引，和着色器中的 "binding = x" 中的x数值对应，或者和glUniformBlockBinding设置的绑定点对应
        // 3. 偏移量
        // 4. 大小
        // glBindBufferRange(GL_UNIFORM_BUFFER, 6, UBO, 0, 2 * sizeof(glm::mat4));

        // glBindBufferBase和glBindBufferRange效果是一样的，只是前者比后者少两个参数
        // 后者通过偏移量和大小两个参数可以绑定Uniform缓冲的特定一部分到绑定点中
        // 后者也可以让多个不同的Uniform块绑定到同一个Uniform缓冲上
        glBindBufferBase(GL_UNIFORM_BUFFER, 6, UBO);
    }

    // 给uniform buffer填充数据
    {
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        // glBufferSubData的使用可以看 02_04_07_TEST1
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewMat));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projectiongMat));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    //----------------------------------------------------------------------------------
    // Uniform缓冲对象（Uniform Buffer Object）可以让多个不同的着色器使用相同的全局Uniform变量
    // 比如我有多个着色器，但是每个着色器中都有一个viewMat和projectionMat，他们的数据完全相同，
    // 这时候就可以使用UBO，只需要对这两个矩阵设置一次就可以达到多个着色器中共享这两个矩阵

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(cubeVAO);

        // 四个立方体使用相同的缩放旋转矩阵
        auto rotateMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto scaleMat = glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f));

        // 右上红色立方体
        {
            auto translateMat = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.f));
            program_red.Use();
            program_red.SetUniformMat4("model", translateMat * rotateMat * scaleMat);
            program_red.SetUniform3f("myColor", 1.f, 0.f, 0.f);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        }

        // 左上绿色立方体
        {
            auto translateMat = glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, 0.5f, 0.f));
            program_green.Use();
            program_green.SetUniformMat4("model", translateMat * rotateMat * scaleMat);
            program_green.SetUniform3f("myColor", 0.f, 1.f, 0.f);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        }

        // 左下蓝色立方体
        {
            auto translateMat = glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, -0.5f, 0.f));
            program_blue.Use();
            program_blue.SetUniformMat4("model", translateMat * rotateMat * scaleMat);
            program_blue.SetUniform3f("myColor", 0.f, 0.f, 1.f);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        }

        // 右下白色立方体
        {
            auto translateMat = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, -0.5f, 0.f));
            program_white.Use();
            program_white.SetUniformMat4("model", translateMat * rotateMat * scaleMat);
            program_white.SetUniform3f("myColor", 1.f, 1.f, 1.f);
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers and programs

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