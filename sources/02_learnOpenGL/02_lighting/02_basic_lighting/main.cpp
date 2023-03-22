/*
 * 1. 漫反射光照 diffuse lighting
 * 2. 镜面光照 specular lighting
 * 3. 光源随着时间旋转，对于TEST2渲染效果看起来更明显
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

    ShaderProgram cubeProgram("resources/02_02_02_TEST1.vs", "resources/02_02_02_TEST1.fs");
    ShaderProgram lightProgram("resources/02_02_02_TEST1_light.vs", "resources/02_02_02_TEST1_light.fs");

    // clang-format off
    std::array<GLfloat, 9 * 6 * 6>  vertices{
        // pos                // color           // normal
        // 后
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
        // 前 
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
        // 左
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        // 右
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
        // 下
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
        // 上
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
    };
    // clang-format on

    unsigned int VBO, VAO;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    unsigned int lightVBO, lightVAO;
    {
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);

        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        // 光源只需要pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    //----------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float lightColor[] { 1.f, 1.f, 1.f }; // 光的颜色
        float lightPos[] { .8f, .6f, 1.f };   // 光的位置

        // 光源和被光照的立方体使用相同的观察、投影矩阵，不同的模型矩阵
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 被光照的立方体
        {
            cubeProgram.Use();

            auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));

            cubeProgram.SetUniformMat4("model", modelMat);
            cubeProgram.SetUniformMat4("view", viewMat);
            cubeProgram.SetUniformMat4("projection", projectionMat);

            cubeProgram.SetUniform3f("lightPos", lightPos[0], lightPos[1], lightPos[2]);
            cubeProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 光源
        {
            lightProgram.Use();

            auto translate = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
            auto scale = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
            auto modelMat = translate * scale;

            lightProgram.SetUniformMat4("transform", projectionMat * viewMat * modelMat);
            lightProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

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

    // 物体
    ShaderProgram cubeProgram("resources/02_02_02_TEST2.vs", "resources/02_02_02_TEST2.fs");
    // 光源
    ShaderProgram lightProgram("resources/02_02_02_TEST2_light.vs", "resources/02_02_02_TEST2_light.fs");

    // clang-format off
    std::array<GLfloat, 9 * 6 * 6>  vertices{
        // pos                // color           // normal
        // 后
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        // 前
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        // 左
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        // 右
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
        // 下
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        // 上
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
    };
    // clang-format on

    unsigned int VBO, VAO;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    unsigned int lightVBO, lightVAO;
    {
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);

        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        // 光源只需要pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    //----------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float lightColor[] { 1.f, 1.f, 1.f }; // 光的颜色
        float lightPos[] { .8f, .6f, 1.f };   // 光的位置
        float viewPos[] { 0.f, 0.f, 5.f };    // 观察点

        // 光源和被光照的立方体使用相同的观察、投影矩阵，不同的模型矩阵
        auto viewMat = glm::lookAt(glm::vec3(viewPos[0], viewPos[1], viewPos[2]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 被光照的立方体
        {
            cubeProgram.Use();

            auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
            //auto modelMat = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(1.f, 1.f, 0.f));

            cubeProgram.SetUniformMat4("model", modelMat);
            cubeProgram.SetUniformMat4("view", viewMat);
            cubeProgram.SetUniformMat4("projection", projectionMat);

            cubeProgram.SetUniform3f("viewPos", viewPos[0], viewPos[1], viewPos[2]);
            cubeProgram.SetUniform3f("lightPos", lightPos[0], lightPos[1], lightPos[2]);
            cubeProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 光源
        {
            lightProgram.Use();

            auto translate = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
            auto scale = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
            auto modelMat = translate * scale;

            lightProgram.SetUniformMat4("transform", projectionMat * viewMat * modelMat);
            lightProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

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

    // 物体
    ShaderProgram cubeProgram("resources/02_02_02_TEST2.vs", "resources/02_02_02_TEST2.fs");
    // 光源
    ShaderProgram lightProgram("resources/02_02_02_TEST2_light.vs", "resources/02_02_02_TEST2_light.fs");

    // clang-format off
    std::array<GLfloat, 9 * 6 * 6>  vertices{
        // pos                // color           // normal
        // 后
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f, -1.0f,
        // 前
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  0.0f,  1.0f,
        // 左
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,  -1.0f,  0.0f,  0.0f,
        // 右
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   1.0f,  0.0f,  0.0f,
        // 下
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f, -1.0f,  0.0f,
        // 上
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.3f,   0.0f,  1.0f,  0.0f,
    };
    // clang-format on

    unsigned int VBO, VAO;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        // normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    unsigned int lightVBO, lightVAO;
    {
        glGenVertexArrays(1, &lightVAO);
        glGenBuffers(1, &lightVBO);

        glBindVertexArray(lightVAO);

        glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        // 光源只需要pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    //----------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float lightColor[] { 1.f, 1.f, 1.f }; // 光的颜色
        float lightPos[] { 1.f, 0.f, 0.f };   // 光的起始位置
        float viewPos[] { 0.f, 0.f, 5.f };    // 观察点

        // 光源和被光照的立方体使用相同的观察、投影矩阵，不同的模型矩阵
        auto viewMat = glm::lookAt(glm::vec3(viewPos[0], viewPos[1], viewPos[2]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 光源的模型矩阵
        auto lightRotate = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()) / 3.f, glm::vec3(0, 1, 0));
        auto lightTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        auto lightScale = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
        auto lightModelMat = lightRotate * lightTranslate * lightScale;

        // 随着时间绕y轴旋转的光源位置
        auto rotateLightPos = lightModelMat * glm::vec4(lightPos[0], lightPos[1], lightPos[2], 1.f);

        // 被光照的立方体
        {
            cubeProgram.Use();

            auto modelMat = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(0.f, 1.f, 0.f));

            cubeProgram.SetUniformMat4("model", modelMat);
            cubeProgram.SetUniformMat4("view", viewMat);
            cubeProgram.SetUniformMat4("projection", projectionMat);

            cubeProgram.SetUniform3f("viewPos", viewPos[0], viewPos[1], viewPos[2]);
            cubeProgram.SetUniform3f("lightPos", rotateLightPos[0], rotateLightPos[1], rotateLightPos[2]);
            cubeProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 光源
        {
            lightProgram.Use();

            lightProgram.SetUniformMat4("transform", projectionMat * viewMat * lightModelMat);
            lightProgram.SetUniform3f("lightColor", lightColor[0], lightColor[1], lightColor[2]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &lightVBO);

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

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
