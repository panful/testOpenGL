/*
 * 1. 分批填充数据（顶点、颜色、法线、纹理坐标等分开填充）glBufferSubData
 * 2. 在指定时刻分批填充数据
 * 3. 从内存拷贝数据到指定缓冲 glMapBuffer
 * 4. 复制缓冲 glCopyBufferSubData
 * 5. 将多个VBO绑定到一个VAO上，顶点位置、颜色等分开使用glBufferData提交，类似glBufferSubData
 * 6. 顶点着色器的顶点属性设置为mat4
 * 7. 动态更新顶点的颜色属性，但是不更新位置属性
 * 8. 顶点位置、颜色数据分开提交，且位置和颜色的数据类型不一样
 */

#define TEST8

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLuint vertexArrayObject { 0 };
GLuint vertexBufferObject { 0 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    // 顶点数据
    std::array<GLfloat, 3 * 3> vertices {
        -0.5f,  -0.5f,  0.0f,   // left
         0.5f,  -0.5f,  0.0f,   // right
         0.0f,   0.5f,  0.0f    // top
    };

    // 颜色数据
    std::array<GLfloat, 3 * 3> colors {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    // clang-format on

    //---------------------------------------------------------------------------
    // 先将VBO绑定到VAO，然后给VBO申请内存，不填充数据
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() + sizeof(GLfloat) * colors.size(), NULL, GL_STATIC_DRAW);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------
    // 绑定VAO，将数据分批填充到VBO
    glBindVertexArray(vertexArrayObject);
    // glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
    // 1.类型
    // 2.开始位置
    // 3.传入数据的大小
    // 4.数据指针
    // @note 缓冲需要有足够的已分配内存，所以对一个缓冲调用glBufferSubData之前必须要先调用glBufferData。
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), sizeof(GLfloat) * colors.size(), colors.data());

    // 位置数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色数据
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * vertices.size()));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

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
#include <mutex>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLuint vertexArrayObject { 0 };
GLuint vertexBufferObject { 0 };

namespace {
// clang-format off
// 顶点数据
std::array<GLfloat, 3 * 3> vertices {
    -0.5f,  -0.5f,  0.0f,   // left
        0.5f,  -0.5f,  0.0f,   // right
        0.0f,   0.5f,  0.0f    // top
};

// 颜色数据
std::array<GLfloat, 3 * 3> colors {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};
// clang-format on
} // namespace

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size() + sizeof(GLfloat) * colors.size(), NULL, GL_STATIC_DRAW);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------
    // 按下F1之后才填充数据

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        // 保证只被填充一次，当然填充多次也不会出错。
        static std::once_flag once_flag;

        std::call_once(once_flag,
            []()
            {
                // 一定要先绑定VAO
                glBindVertexArray(vertexArrayObject);

                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
                glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors.data());

                // 位置
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
                glEnableVertexAttribArray(0);
                // 颜色
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * vertices.size()));
                glEnableVertexAttribArray(1);

                glBindVertexArray(0);

                std::cout << "Fill data successfully\n";
            });
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>
#include <mutex>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLuint vertexArrayObject { 0 };
GLuint vertexBufferObject { 0 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    // 顶点数据
    std::array<GLfloat, 3 * 6> vertices {
        -0.5f,  -0.5f,  0.0f,    1.0f, 0.0f, 0.0f,
         0.5f,  -0.5f,  0.0f,    1.0f, 0.0f, 0.0f,
         0.0f,   0.5f,  0.0f,    1.0f, 0.0f, 0.0f,
    };
    // clang-format on

    //---------------------------------------------------------------------------

    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // 必须提前预分配好内存
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), nullptr, GL_STATIC_DRAW);
    // 获取指针
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    // 拷贝数据到内存
    std::copy(vertices.begin(), vertices.end(), static_cast<GLfloat*>(ptr));
    // 停止映射，返回true表示数据映射成功，false失败
    auto success = glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 按下F1之后，将原来的顶点数据颜色修改
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        // clang-format off
        // 顶点数据
        std::array<GLfloat, 3 * 6> vertices {
            -0.5f,  -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,
             0.5f,  -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,
             0.0f,   0.5f,  0.0f,    0.0f, 1.0f, 0.0f,
        };
        // clang-format on

        static std::once_flag once_flag;

        std::call_once(once_flag,
            [vertices]()
            {
                glBindVertexArray(vertexArrayObject);

                // 获取指针
                void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
                // 拷贝数据到内存
                std::copy(vertices.begin(), vertices.end(), static_cast<GLfloat*>(ptr));
                // 停止映射，返回true表示数据映射成功，false失败
                auto success = glUnmapBuffer(GL_ARRAY_BUFFER);

                glBindVertexArray(0);

                std::cout << "Replace data successfully.\n";
            });
    }
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

GLuint VAO { 0 };
GLuint VBO { 0 };
GLuint EBO { 0 };
GLuint EBO2 { 0 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 5 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f, // 左上
         0.0f,  0.0f, 0.0f,     1.0f, 1.0f, 1.0f, // 原点
    };

    // 左右两个三角形
    std::array<GLuint, 3 * 2> indices{
        0, 4, 3,
        1, 2, 4,
    };

    // 上下两个三角形
    std::array<GLuint, 3 * 2> indices2{
        0, 1, 4,
        2, 3, 4,
    };
    // clang-format on

    //---------------------------------------------------------------------------

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &EBO2);

    //---------------------------------------------------------------------------
    // 将数据填充到VBO、EBO、EBO2
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices2.size(), indices2.data(), GL_STATIC_DRAW);

    //-----------------------------------------------------------------------
    // 将EBO2的数据拷贝到EBO
    // 方式1：
    glBindBuffer(GL_COPY_READ_BUFFER, EBO2);
    glBindBuffer(GL_COPY_WRITE_BUFFER, EBO);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(GLuint) * indices.size());
    // 方式2：
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    // glBindBuffer(GL_COPY_WRITE_BUFFER, EBO);
    // glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(GLuint) * indices.size());

    //-----------------------------------------------------------------------
    // 将VBO和EBO绑定到VAO
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &EBO2);

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

void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    // 顶点数据
    std::array<GLfloat, 3 * 3> vertices {
        -0.5f,  -0.5f,  0.0f,   // left
         0.5f,  -0.5f,  0.0f,   // right
         0.0f,   0.5f,  0.0f    // top
    };

    // 颜色数据
    std::array<GLfloat, 3 * 3> colors {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };
    // clang-format on

    //---------------------------------------------------------------------------
    // 创建多个VBO和一个VAO
    GLuint VAO { 0 }, VBO_Pos { 0 }, VBO_Color { 0 };

    glGenBuffers(1, &VBO_Pos);
    glGenBuffers(1, &VBO_Color);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //---------------------------------------------------------------------------
    // 将多个VBO绑定到一个VAO上
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 绑定完GL_ARRAY_BUFFER之后需要立即启用顶点属性，且绑定操作和属性设置操作顺序不可打乱
    // 顶点属性的设置顺序可以打乱：对颜色和位置的设置顺序没有要求
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pos);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
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

#endif // TEST5

#ifdef TEST6

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
    ShaderProgram program("resources/02_04_07_TEST6.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 5 * 4> vertices {
        -0.5f, -0.5f,      1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,      0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,      0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,      1.0f,  1.0f,  1.0f,
    };

    std::array<GLuint, 3 * 2> indices {
        0, 1, 2,
        0, 2, 3,
    };

    // 四个顶点共需要四个相同的模型矩阵
    std::array<GLfloat, 4 * 4 *4> model {
        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        .5f,    .5f,    0.f,    1.f,

        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        .5f,    .5f,    0.f,    1.f,

        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        .5f,    .5f,    0.f,    1.f,

        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        .5f,    .5f,    0.f,    1.f,
    };
    // clang-format on

    //------------------------------------------------------------------------
    // 生成VBO EBO
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

    GLuint VBO2 { 0 };
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model), model.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //------------------------------------------------------------------------
    // 绑定VBO到VAO
    GLuint VAO { 0 };
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    // 顶点位置和颜色
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    // 顶点模型变换矩阵
    // 顶点属性最大允许的数据大小等于一个vec4。如果将glVertexAttribPointer第二个参数设置为4*4程序会崩溃
    // 因为一个mat4本质上是4个vec4，我们需要为这个矩阵预留4个顶点属性。
    // 因为我们将它的位置值设置为2，矩阵每一列的顶点属性位置值就是2、3、4、5
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * sizeof(glm::vec4)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBindVertexArray(0);

    //------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        program.Use();
        // 通过uniform设置顶点的模型变换矩阵，和使用顶点属性有相同的效果
        // auto model = glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.0f));
        // program.SetUniformMat4("model", glm::translate(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.0f)));
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
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

#endif // TEST6

#ifdef TEST7

#include <array>
#include <common.hpp>
#include <mutex>
#include <random>

void processInput(GLFWwindow* window);

GLuint VAO { 0 };
GLuint VBO { 0 };

constexpr size_t vertex_size { 3 * 3 }; // 3个vec3

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_size * 2, nullptr, GL_STATIC_DRAW);

    // 位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    // 颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(sizeof(GLfloat) * vertex_size));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------
    // 按下F1之后更新一次颜色数据
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

float genColor()
{
    static std::default_random_engine randomEngine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<float> randomColor(0.f, 1.f);

    return randomColor(randomEngine);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
        // clang-format off
        static std::array<GLfloat, vertex_size> pos {
            -0.5f,  -0.5f,  0.0f,
             0.5f,  -0.5f,  0.0f,
             0.0f,   0.5f,  0.0f,
        };
        // clang-format on

        std::vector<GLfloat> color;
        for (size_t i = 0; i < vertex_size; ++i)
        {
            color.emplace_back(genColor());
        }

        //--------------------------------------------------------------------------------------
        glBindVertexArray(VAO);

        // 只提交一次顶点位置数据
        static std::once_flag flag {};
        std::call_once(flag, []() { glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertex_size, pos.data()); });

        // 每次按下F1都更新一次颜色数据
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_size, sizeof(GLfloat) * vertex_size, color.data());

        glBindVertexArray(0);
    }
}

#endif // TEST7

#ifdef TEST8

#include <array>
#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_07_TEST1.vs", "resources/02_04_07_TEST1.fs");

    // clang-format off
    // 顶点数据
    std::array<GLfloat, 3 * 3> vertices {
        -0.5f,  -0.5f,  0.0f,   // left
         0.5f,  -0.5f,  0.0f,   // right
         0.0f,   0.5f,  0.0f    // top
    };

    // 颜色数据
    std::array<GLubyte, 3 * 3> colors {
        255,   0,   0,
          0, 255,   0,
          0,   0, 255,
    };
    // clang-format on

    //---------------------------------------------------------------------------
    // 创建多个VBO和一个VAO
    GLuint VAO { 0 }, VBO_Pos { 0 }, VBO_Color { 0 };

    glGenBuffers(1, &VBO_Pos);
    glGenBuffers(1, &VBO_Color);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLubyte) * colors.size(), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //---------------------------------------------------------------------------
    // 将多个VBO绑定到一个VAO上
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Pos);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // 注意：使用unsigned char类型的数据时，着色器接受类型仍然为vec3
    // glVertexAttribPointer的第四个参数：数据是否标准化应该设置为true
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Color);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 3 * sizeof(GLubyte), (void*)0);

    glBindVertexArray(0);

    CheckError();

    //---------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glfwTerminate();
    return 0;
}

#endif // TEST8