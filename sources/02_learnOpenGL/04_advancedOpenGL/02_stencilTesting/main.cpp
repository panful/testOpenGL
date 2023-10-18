/*
 * 1. 模板测试简单示例
 * 2. 使用模板测试将矩形部分区域掏空
 * 3. X射线透视
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

        auto modelMat       = glm::mat4(1.f);
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        // 小矩形
        program.Use();
        program.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        // glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)函数的参数含义：
        // @note: 如果深度测试未开启，则深度测试的结果将始终为通过
        // sfail：模板测试失败时采取的行为。
        // dpfail：模板测试通过，但深度测试失败时采取的行为。
        // dppass：模板测试和深度测试都通过时采取的行为。

        // GL_KEEP	保持当前储存的模板值
        // GL_ZERO	将模板值设置为0
        // GL_REPLACE	将模板值设置为glStencilFunc函数设置的ref值
        // GL_INCR	如果模板值小于最大值则将模板值加1
        // GL_INCR_WRAP	与GL_INCR一样，但如果模板值超过了最大值则归零
        // GL_DECR	如果模板值大于最小值则将模板值减1
        // GL_DECR_WRAP	与GL_DECR一样，但如果模板值小于0则将其设置为最大值
        // GL_INVERT	按位翻转当前的模板缓冲值

        // 此处模板测试和深度测试都能通过，因此模板值将设置为glStencilFunc函数设置的ref值
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // 片段的模板值始终都可以通过，即小矩形所在的区域模板值都会被设置为1
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        // 比较模板值并将通过测试的模板值更新到模板缓冲中
        // 如果参数为0x00，则只做模板测试，并不会更新模板缓冲，和glDepthMask(GL_FALSE)效果类似
        // 掩码值还可以设置为0x00到0xFF之间的任意值
        // 当参数为0x00时也就没必要调用glStencilOp()
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
    ShaderProgram program("resources/02_04_02_TEST1.vs", "resources/02_04_02_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> verticesPlane1{
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

    std::array<GLfloat, 4 * 6> verticesPlane2{
        // pos                  // color
        -0.2f, -0.1f, 0.f,     0.0f, 1.0f, 0.0f, // 左下
         0.2f, -0.1f, 0.f,     0.0f, 1.0f, 0.0f, // 右下
         0.2f,  0.1f, 0.f,     0.0f, 1.0f, 0.0f, // 右上
        -0.2f,  0.1f, 0.f,     0.0f, 1.0f, 0.0f, // 左上
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane1.size(), verticesPlane1.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int plane2VAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &plane2VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(plane2VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesPlane2.size(), verticesPlane2.data(), GL_STATIC_DRAW);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        auto modelMat       = glm::mat4(1.f);
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.Use();
        program.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        // GL_NEVER设置模板测试始终都不通过
        // GL_REPLACE设置当模板测试不通过时，将模板值设置为99
        // 接下来绘制的小矩形区域的模板值都会被设置为99
        glStencilFunc(GL_NEVER, 99, 0xFF);
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
        glStencilMask(0xFF);

        glBindVertexArray(plane2VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        // 当模板值不等于99时，模板测试通过
        // 即除过小矩形以外的区域都通过
        glStencilFunc(GL_NOTEQUAL, 99, 0xFF);
        // 只做模板测试，不修改模板缓冲
        glStencilMask(0x00);

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

#endif // TEST2

#ifdef TEST3

#include <model.hpp>

int main()
{
    InitOpenGL init(Camera({ 5, 5, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    ShaderProgram program_model("resources/02_04_02_TEST3_model.vs", "resources/02_04_02_TEST3_model.fs");
    ShaderProgram program_plane("resources/02_04_02_TEST3_plane.vs", "resources/02_04_02_TEST3_plane.fs");

    ModelLoading::Model ourModel("resources/backpack/backpack.obj");

    // clang-format off
    std::vector<GLuint> indices{
        0, 1, 3,
        1, 2, 3,
    };

    std::vector<GLfloat> vertices{
        -0.3f, -0.3f, 0.f,
         0.3f, -0.3f, 0.f,
         0.3f,  0.3f, 0.f,
        -0.3f,  0.3f, 0.f,
    };
    // clang-format on

    Renderer plane(vertices, indices, { 3 }, GL_TRIANGLES);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //---------------------------------------------------------------------
        // 小矩形所在区域始终不通过模板测试，并将小矩形所在区域模板值设置为1
        glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_NEVER, 1, 0xFF);
        glStencilMask(0xFF);

        program_plane.Use();
        plane.Draw();

        //---------------------------------------------------------------------
        // 模型的MVP矩阵
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(.5f, .5f, .5f));
        program_model.Use();
        program_model.SetUniformMat4("projection", init.GetProjectionMatrix());
        program_model.SetUniformMat4("view", init.GetViewMatrix());
        program_model.SetUniformMat4("model", model);

        //---------------------------------------------------------------------
        // 非小矩形区域使用填充方式绘制三角形
        glStencilMask(0x00);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        ourModel.Draw(program_model);

        //---------------------------------------------------------------------
        // 小矩形所在区域始终线框方式绘制三角形
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        ourModel.Draw(program_model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3