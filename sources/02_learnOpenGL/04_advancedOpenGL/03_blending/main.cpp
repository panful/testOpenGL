/*
 * 1. 混合的基础示例
 */

#define TEST1

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
    // 8个顶点
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
