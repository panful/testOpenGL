/**
 * 1. glDrawElements 设置索引起始地址
 * 2. glDrawElementsBaseVertex 设置顶点偏移
 * 3. glDrawRangeElements 指定顶点的范围，可能提高性能
 * 4. glMultiDrawElements 一次提交多个索引数组
 * 5. glDrawArraysIndirect
 */

#define TEST4

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_13_TEST1.vs", "resources/02_04_13_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices {
        -0.8f, -0.8f, 0.0f,
        -0.1f, -0.8f, 0.0f,
        -0.1f, -0.1f, 0.0f,
        -0.8f, -0.1f, 0.0f,

         0.1f,  0.1f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.8f,  0.8f, 0.0f,
         0.1f,  0.8f, 0.0f,
    };

    std::vector<GLuint> indices{
        0, 1, 2, 0, 2, 3, // 左下角四边形
        4, 5, 6, 4, 6, 7, // 右上角四边形
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        // 从第4个索引开始，绘制6个顶点（两个三角形）
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * 3));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_13_TEST1.vs", "resources/02_04_13_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices {
        -0.8f, -0.8f, 0.0f,
        -0.1f, -0.8f, 0.0f,
        -0.1f, -0.1f, 0.0f,
        -0.8f, -0.1f, 0.0f,

         0.1f,  0.8f, 0.0f,
         0.8f,  0.8f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
    };

    std::vector<GLuint> indices{
        0, 1, 2, 0, 2, 3,
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);
        // 右上角的四边形，将顶点数组的第4个顶点作为基准顶点（相当于第四个顶点的索引为0）
        glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 4);
        // 左下角的四边形，使用原始的顶点索引
        glDrawElementsBaseVertex(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 0);
        // 左下角的四边形，和glDrawElementsBaseVertex最后一个参数设置为0效果一样
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_13_TEST1.vs", "resources/02_04_13_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices {
        -0.8f, -0.8f, 0.0f,
        -0.1f, -0.8f, 0.0f,
        -0.1f, -0.1f, 0.0f,
        -0.8f, -0.1f, 0.0f,

         0.1f,  0.1f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.8f,  0.8f, 0.0f,
         0.1f,  0.8f, 0.0f,
    };

    std::vector<GLuint> indices{
        0, 1, 2, 0, 2, 3, // 左下角四边形
        4, 5, 6, 4, 6, 7, // 右上角四边形
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindVertexArray(VAO);

        // http://www.songho.ca/opengl/gl_vertexarray.html
        // https://blog.csdn.net/u012501459/article/details/12945153
        // glDrawRangeElements
        // 第2和第3个参数用来指定顶点的范围，通过设置范围，OpenGL可以只获得访问有限范围的顶点来优先渲染，从而提高性能
        // 即使设置一个错的范围，也可以正常绘制？？？

        // 渲染索引数组中的前6个顶点，前6个顶点的索引值在[0,3]的范围
        glDrawRangeElements(GL_TRIANGLES, 0, 3, 6, GL_UNSIGNED_INT, 0);
        // 渲染索引数组中的后6个顶点，后6个顶点的索引值在[4,7]的范围中
        glDrawRangeElements(GL_TRIANGLES, 4, 7, 6, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * 6));

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_13_TEST1.vs", "resources/02_04_13_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices {
        -0.8f, -0.8f, 0.0f,
        -0.1f, -0.8f, 0.0f,
        -0.1f, -0.1f, 0.0f,
        -0.8f, -0.1f, 0.0f,

         0.1f,  0.1f, 0.0f,
         0.8f,  0.1f, 0.0f,
         0.8f,  0.8f, 0.0f,
         0.1f,  0.8f, 0.0f,
    };
    // clang-format on

    GLuint VAO { 0 };
    {
        GLuint VBO { 0 }, EBO { 0 };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    //------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GLuint ind0[] { 0, 1, 2 };
        GLuint ind1[] { 0, 2, 3 };
        GLuint ind2[] { 4, 5, 6 };
        GLuint ind3[] { 4, 6, 7 };
        std::vector<void*> indices { ind0, ind1, ind2, ind3 };

        program.Use();
        glBindVertexArray(VAO);

        //---------------------------------------------------------------
        // 方式1和方式2的绘制结果完全一样

        // 方式1：多次drawcall
        {
            for (auto index : indices)
            {
                glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, index);
            }
        }
        // 方式2：一次drawcall
        {
            std::vector<GLsizei> count { 3, 3, 3, 3 }; // 每个索引数组的大小
            glMultiDrawElements(GL_TRIANGLES, count.data(), GL_UNSIGNED_INT, indices.data(), 4);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

#endif // TEST4