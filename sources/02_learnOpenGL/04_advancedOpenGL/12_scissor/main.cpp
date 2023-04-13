/*
 * 1. 使用glViewport设置视口，在窗口的不同位置绘制指定图元
 */

#define TEST1

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

GLint windowWidth = 800;
GLint windowHeight = 600;

int main()
{
    InitOpenGL initOpenGL("viewport", windowWidth, windowHeight);
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_04_12_TEST1.vs", "resources/02_04_12_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 6 * 3> vertices {
        -0.5f, -0.5f,  0.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,     0.0f, 1.0f, 0.0f,
         0.0f,  0.5f,  0.0f,     0.0f, 0.0f, 1.0f,
    };

    std::array<GLfloat, 6 * 4> vertices2 {
         0.0f,  1.0f,  0.0f,     1.0f, 0.0f, 0.0f,
         0.0f, -1.0f,  0.0f,     1.0f, 0.0f, 0.0f,
        -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, 0.0f,
         1.0f,  0.0f,  0.0f,     0.0f, 1.0f, 0.0f,
    };
    // clang-format on

    GLuint VAO_triangle { 0 };
    {
        GLuint VBO { 0 };
        glGenVertexArrays(1, &VAO_triangle);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    GLuint VAO_line { 0 };
    {
        GLuint VBO { 0 };
        glGenVertexArrays(1, &VAO_line);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO_line);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    //----------------------------------------------------------------------------------------------
    // 绘制两条在原点相交的线将整个窗口分为4个小窗口
    // 使用glViewport在4个窗口分别绘制不同的图元

    glm::mat4 model(1.f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.f, 100.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //-------------------------------------------------------
        // 整个窗口，横竖两根分割线
        glViewport(0, 0, windowWidth, windowHeight);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAO_line);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices2.size() / 6));
        glBindVertexArray(0);

        //-------------------------------------------------------
        // 左下1/4窗口
        glViewport(0, 0, windowWidth / 2, windowHeight / 2);

        model = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.f, 0.f, 0.f));
        program.SetUniformMat4("transform", projection * view * model);

        glBindVertexArray(VAO_triangle);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);

        //-------------------------------------------------------
        // 右上1/4窗口
        glViewport(windowWidth / 2, windowHeight / 2, windowWidth / 2, windowHeight / 2);

        model = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        program.SetUniformMat4("transform", projection * view * model);

        glBindVertexArray(VAO_triangle);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);

        //-------------------------------------------------------
        // 左上1/4窗口
        glViewport(0, windowHeight / 2, windowWidth / 2, windowHeight / 2);

        model = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        program.SetUniformMat4("transform", projection * view * model);

        glBindVertexArray(VAO_triangle);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);

        //-------------------------------------------------------
        // 右下1/4窗口
        glViewport(windowWidth / 2, 0, windowWidth / 2, windowHeight / 2);

        model = glm::rotate(glm::mat4(1.f), static_cast<GLfloat>(glfwGetTime()), glm::vec3(0.f, 0.f, 1.f));
        program.SetUniformMat4("transform", projection * view * model);

        glBindVertexArray(VAO_triangle);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 6));
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
}

#endif // TEST1
