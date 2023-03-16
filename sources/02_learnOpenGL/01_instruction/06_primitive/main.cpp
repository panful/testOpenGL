/*
 * 1. 设置图元填充模式，点的大小，线的粗细
 * 2. 对多个图元设置不同的填充方式
 * 3. glDrawArrays glDrawElements 绘制方式：点、线、三角形等
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
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 左上
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

    // 设置图元填充模式
    // 第一个参数：
    // 设定多边形的两面，如果是顺时针绘制则是反面，逆时针绘制则是正面；
    // 第二个参数：
    // GL_FILL  默认填充方式
    // GL_LINE  呈现线框模型
    // GL_POINT 用点绘制图形
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 设置点的大小
    glPointSize(5.0f);

    // 设置线的粗细
    glLineWidth(5.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.9f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        -0.1f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    std::array<GLfloat, 3 * 6> vertices2{
        // pos                  // color
         0.1f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.9f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    std::array<GLuint, 1 * 3> indices{
        0, 1, 2
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    {
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
    }

    unsigned int VBO2, VAO2, EBO2;
    {
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        // 左边的三角形
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(5.0f);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        }

        // 右边的三角形
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindVertexArray(VAO2);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

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
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 左上
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

        program.Use();

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glDrawArrays:当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
        // glDrawArrays(GLenum mode, GLint first, GLsizei count) 参数：
        // 1. mode  绘制方式，GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN
        // 2. first 从数组缓存中的哪一位开始绘制，表示从指定数据数组的哪一个点开始画起，一般为0，
        // 3. count 数组中顶点的数量
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        // 绘制方式 https://www.cnblogs.com/qingsunny/archive/2013/03/20/2971979.html

        // 1.绘制方式
        // 2.依次从索引数组中读取几个顶点来进行绘制（这里一个点有3个坐标，3个颜色，因此为6）
        // 3.索引数组中存放的元素的类型
        // 4.指向索引数组的指针，在前面填充部分已经设置了，也可以在此处设置 02_01_01_TEST6
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

#endif // TEST3
