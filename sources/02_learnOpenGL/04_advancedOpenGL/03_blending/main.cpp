/*
 * 1. 混合的基础示例
 * 2. 深度测试和混合一起使用
 * 3. 纹理图片alpha小于0.1时跳过该片段，从而实现透明效果
 * 4. 交叉图像的混合
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
    ShaderProgram program("resources/02_04_03_TEST1.vs", "resources/02_04_03_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 3 * 7> verticesLeftTriangle{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     0.5f, 0.0f, 0.0f, 0.5f,  // 左下
         0.3f, -0.5f, 0.0f,     0.5f, 0.0f, 0.0f, 0.5f,  // 右下
        -0.1f,  0.5f, 0.0f,     0.5f, 0.0f, 0.0f, 0.5f,  // 中上
    };

    std::array<GLfloat, 3 * 7> verticesRightTriangle{
        // pos                  // color
        -0.3f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.5f,  // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.5f,  // 右下
         0.1f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.5f,  // 中上
    };
    // clang-format on

    unsigned int VAOleft;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAOleft);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAOleft);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesLeftTriangle.size(), verticesLeftTriangle.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int VAOright;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAOright);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAOright);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesRightTriangle.size(), verticesRightTriangle.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启混合
    glEnable(GL_BLEND);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        //---------------------------------------------------------------
        // 上层的颜色源颜色，下层的是目标颜色，混合时需要先和背景色混合
        // Result = Color_src * Factor_src + Color_des * Factor_des;

        // glBlendFunc设置源（Factor_src）和目标因子（Factor_des）
        // GL_ZERO	                    因子等于0
        // GL_ONE	                    因子等于1
        // GL_SRC_COLOR	                因子等于源颜色向量Color_src
        // GL_ONE_MINUS_SRC_COLOR	    因子等于1−Color_src
        // GL_DST_COLOR	                因子等于目标颜色向量Color_des
        // GL_ONE_MINUS_DST_COLOR	    因子等于1−Color_des
        // GL_SRC_ALPHA	                因子等于Color_src的alpha分量
        // GL_ONE_MINUS_SRC_ALPHA	    因子等于1−Color_src的alpha分量
        // GL_DST_ALPHA	                因子等于Color_des的alpha分量
        // GL_ONE_MINUS_DST_ALPHA	    因子等于1−Color_des的alpha分量
        // GL_CONSTANT_COLOR	        因子等于常数颜色向量Color_const
        // GL_ONE_MINUS_CONSTANT_COLOR	因子等于1−Color_const
        // GL_CONSTANT_ALPHA	        因子等于Color_const的alpha分量
        // GL_ONE_MINUS_CONSTANT_ALPHA	因子等于1−Color_const的alpha分量

        //---------------------------------------------------------------
        // result = src.a * src(r,g,b,a) + (1-src.a) * dst(r,g,b,a)
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 颜色向量和颜色向量相乘 r = r1 * r2
        // result = src(r,g,b,a) * src(r,g,b,a) + dst(r,g,b,a) * dst(r,g,b,a)
        // glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);

        // result = src(r,g,b,a) + dst(r,g,b,a)
        // glBlendFunc(GL_ONE, GL_ONE);

        // result = src(r,g,b,a)
        // glBlendFunc(GL_ONE, GL_ZERO);

        //---------------------------------------------------------------
        // Color_const可以通过glBlendColor()来设置
        // glBlendColor(0.4f, 0.3f, 0.2f, 0.1f);
        // glBlendFunc(GL_CONSTANT_ALPHA, GL_CONSTANT_COLOR);

        //---------------------------------------------------------------
        // RGB分量和ALPHA分量分开设置
        // 前两个参数用来设置rgb，和glBlendColor效果一样
        // 后两个参数用来设置alpha
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        //---------------------------------------------------------------
        // 设置源颜色和目标颜色之间的运算符，默认是加法
        glBlendEquation(GL_FUNC_ADD);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAOleft);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesLeftTriangle.size() / 7));

        glBindVertexArray(VAOright);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesRightTriangle.size() / 7));

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
    ShaderProgram program("resources/02_04_03_TEST1.vs", "resources/02_04_03_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 3 * 7> verticesLeftTriangle{
        // pos                  // color
        -0.5f, -0.5f, 0.1f,     1.0f, 0.0f, 0.0f, 0.1f,  // 左下
         0.3f, -0.5f, 0.1f,     1.0f, 0.0f, 0.0f, 0.1f,  // 右下
        -0.1f,  0.5f, 0.1f,     1.0f, 0.0f, 0.0f, 0.1f,  // 中上
    };

    std::array<GLfloat, 3 * 7> verticesRightTriangle{
        // pos                  // color
        -0.3f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.6f,  // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.6f,  // 右下
         0.1f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f, 0.6f,  // 中上
    };
    // clang-format on

    unsigned int VAOleft;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAOleft);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAOleft);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesLeftTriangle.size(), verticesLeftTriangle.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int VAOright;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAOright);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAOright);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesRightTriangle.size(), verticesRightTriangle.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 开启混合
    glEnable(GL_BLEND);
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置混合方式
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));
        // 旋转之后物体混合就会出错
        // program.SetUniformMat4("transform", glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / 2.), glm::vec3(0.f, 1.f, 0.f)));

        // OpenGL中深度测试在混合之前进行，所以距离眼睛更远的图元如果在后面绘制，
        // 就不会将颜色(R,G,B,A)缓存，所以就会导致距离眼睛更远的图元被丢弃
        // 正确的绘制顺序应当是：
        // 1. 先绘制所有不透明的物体。
        // 2. 对所有透明的物体排序。
        // 3. 按顺序绘制所有透明的物体。
        // 如果没有开启深度测试，则颜色结果按照drawcall顺序和混合方式计算

        // 先绘制距离眼睛更近的图元(NDC坐标系，z的范围为[-1, 1]，z为-1时距离眼睛最近)
        glBindVertexArray(VAOright);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesRightTriangle.size() / 7));

        // 再绘制距离眼睛更远的图元，由于开启了深度测试，和之前绘制的三角形重叠的区域会直接被丢弃
        glBindVertexArray(VAOleft);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesLeftTriangle.size() / 7));

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
    ShaderProgram program("resources/02_04_03_TEST3.vs", "resources/02_04_03_TEST3.fs");
    Texture textureGrass("resources/02_04_03_grass.png");
    Texture textureWall("resources/02_04_03_wall.jpg");

    // clang-format off
    std::array<GLfloat, 6 * 5> quadVertices{
        // pos                  // texture coord
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,   // 左下
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f,   // 右下
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,   // 右上

        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,   // 左下
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f,   // 右上
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f,   // 左上
    };
    // clang-format on

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * quadVertices.size(), quadVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        //------------------------------------------------
        // 左下角一堵墙前面一颗草
        auto transform = glm::translate(glm::mat4(1.f), glm::vec3(-.4f, -.4f, 0.f));

        program.Use();
        program.SetUniformMat4("transform", transform);

        textureWall.Bind();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quadVertices.size() / 5));

        textureGrass.Bind();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quadVertices.size() / 5));

        //------------------------------------------------
        // 右上角一堵墙前面一棵草
        transform = glm::translate(glm::mat4(1.f), glm::vec3(.4f, .4f, 0.f));
        program.SetUniformMat4("transform", transform);

        textureWall.Bind();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quadVertices.size() / 5));

        textureGrass.Bind();
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(quadVertices.size() / 5));

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

namespace {
// clang-format off
    std::array<GLfloat, 4 * 7> red{
         .9f,  .6f, -.5f,       1.f, 0.f, 0.f, .5f,
         .9f,  .5f, -.5f,       1.f, 0.f, 0.f, .5f,
        -.9f,  .6f,  .5f,       1.f, 0.f, 0.f, .5f,
        -.9f,  .5f,  .5f,       1.f, 0.f, 0.f, .5f,
    };

    std::array<GLfloat, 4 * 7> green{
        -.9f,  .9f, -.5f,       0.f, 1.f, 0.f, .5f,
        -.9f,  .7f, -.5f,       0.f, 1.f, 0.f, .5f,
         .5f, -.7f,  .5f,       0.f, 1.f, 0.f, .5f,
         .5f, -.9f,  .5f,       0.f, 1.f, 0.f, .5f,
    };

    std::array<GLfloat, 4 * 7> blue{
        -.5f, -.7f, -.5f,       0.f, 0.f, 1.f, .5f,
        -.5f, -.9f, -.5f,       0.f, 0.f, 1.f, .5f,
         .9f,  .9f,  .5f,       0.f, 0.f, 1.f, .5f,
         .9f,  .7f,  .5f,       0.f, 0.f, 1.f, .5f,

    };

    std::array<GLfloat, 4 * 7> red2{
         .9f,  .2f,  .5f,       1.f, 0.f, 0.f, 1.f,
         .9f,  .1f,  .5f,       1.f, 0.f, 0.f, 1.f,
        -.9f,  .2f,  .5f,       1.f, 0.f, 0.f, 1.f,
        -.9f,  .1f,  .5f,       1.f, 0.f, 0.f, 1.f,
    };

    std::array<GLfloat, 4 * 7> red3{
         .9f, -.5f, -.5f,       1.f, 0.f, 0.f, 1.f,
         .9f, -.6f, -.5f,       1.f, 0.f, 0.f, 1.f,
        -.9f, -.5f, -.5f,       1.f, 0.f, 0.f, 1.f,
        -.9f, -.6f, -.5f,       1.f, 0.f, 0.f, 1.f,
    };
// clang-format on

auto transformMat { glm::mat4(1.f) };
}

GLuint CreateVAO(const std::array<GLfloat, 4 * 7>& vertices);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetKeyCB(keyCB);
    ShaderProgram program("resources/02_04_03_TEST1.vs", "resources/02_04_03_TEST1.fs");

    std::array<GLuint, 5> VAOs {
        CreateVAO(red2),
        CreateVAO(red3),
        CreateVAO(red),
        CreateVAO(green),
        CreateVAO(blue),
    };

    //----------------------------------------------------------------------------------

    // 开启混合
    glEnable(GL_BLEND);
    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 设置混合方式
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        program.Use();
        program.SetUniformMat4("transform", transformMat);

        for (auto VAO : VAOs)
        {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

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

void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto offset { glm::radians(5.f) };

    switch (action)
    {
    case GLFW_PRESS:
    case GLFW_REPEAT:
    {
        switch (key)
        {
        case GLFW_KEY_DOWN:
            transformMat = glm::rotate(transformMat, offset, glm::vec3(1.f, 0.f, 0.f));
            break;
        case GLFW_KEY_UP:
            transformMat = glm::rotate(transformMat, offset, glm::vec3(-1.f, 0.f, 0.f));
            break;
        case GLFW_KEY_RIGHT:
            transformMat = glm::rotate(transformMat, offset, glm::vec3(0.f, 1.f, 0.f));
            break;
        case GLFW_KEY_LEFT:
            transformMat = glm::rotate(transformMat, offset, glm::vec3(0.f, -1.f, 0.f));
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    }
}

GLuint CreateVAO(const std::array<GLfloat, 4 * 7>& vertices)
{
    GLuint VAO { 0 }, VBO { 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

#endif // TEST4