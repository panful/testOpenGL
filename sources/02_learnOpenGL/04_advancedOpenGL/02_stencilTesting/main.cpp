/*
 * 1. 模板测试简单示例
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
    ShaderProgram program("resources/02_04_02_TEST1.vs", "resources/02_04_02_TEST1.fs");
    ShaderProgram border("resources/02_04_02_TEST1.vs", "resources/02_04_02_TEST1_border.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.f,     1.0f, 0.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.f,     1.0f, 0.0f, 0.0f, // 右上
        -0.5f,  0.5f, 0.f,     1.0f, 0.0f, 0.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
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

    // 开启模板测试
    glEnable(GL_STENCIL_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 将模板缓冲清空为0
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto modelMat = glm::mat4(1.f);
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        // 小矩形
        program.Use();
        program.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        // glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)函数的参数含义：
        // sfail：模板测试失败时采取的行为。
        // dpfail：模板测试通过，但深度测试失败时采取的行为。
        // dppass：模板测试和深度测试都通过时采取的行为。
        // @note: 如果深度测试未开启，则深度测试的结果将始终为通过

        // 此处模板测试和深度测试都能通过，因此模板值将设置为glStencilFunc函数设置的ref值
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // 片段的模板值始终都可以通过，即小矩形所在的区域模板值都会被设置为1
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // 比较模板值并将通过测试的模板值更新到模板缓冲中
        // 如果参数为0x00，则只做模板测试，并不会更新模板缓冲，和glDepthMask(GL_FALSE)效果类似
        // 掩码值还可以设置为0x00到0xFF之间的任意值
        glStencilMask(0xFF);

        glBindVertexArray(plane1VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        // 大矩形
        border.Use();
        // 对矩形的xy进行放大
        modelMat = glm::scale(glm::mat4(1.f), glm::vec3(1.1f, 1.1f, 1.f));
        border.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        // 如果片段的模板值不等于1则通过测试并被绘制
        // 因为之前的小矩形所在区域所有的模板值都被设置为1
        // 所以大矩形的边框部分会通过模板测试。大矩形和小矩形重合的部分因为模板值相等所以不能通过
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

        glBindVertexArray(plane1VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

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
