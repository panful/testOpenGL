/*
 * 1. 环境和漫反射光照 diffuse lighting
 * 2. 环境和漫反射以及镜面光照 specular lighting 冯氏着色(Phong Shading)
 * 3. 光源随着时间旋转，对于TEST2渲染效果看起来更明显
 * 4. 高洛德着色(Gouraud Shading)，在顶点着色器进行光照计算，片段的颜色值是由插值光照颜色所得来的
 * 5. 在没有法线的情况下，利用GLSL的dFdx dFdy计算法线，并设置与相机方向平行的平行光
 */

#define TEST5

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

    ShaderProgram cubeProgram("shaders/02_02_02_TEST1.vs", "shaders/02_02_02_TEST1.fs");
    ShaderProgram lightProgram("shaders/02_02_02_TEST1_light.vs", "shaders/02_02_02_TEST1_light.fs");

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
        auto viewMat       = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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
            auto scale     = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
            auto modelMat  = translate * scale;

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
    ShaderProgram cubeProgram("shaders/02_02_02_TEST2.vs", "shaders/02_02_02_TEST2.fs");
    // 光源
    ShaderProgram lightProgram("shaders/02_02_02_TEST2_light.vs", "shaders/02_02_02_TEST2_light.fs");

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
        auto viewMat       = glm::lookAt(glm::vec3(viewPos[0], viewPos[1], viewPos[2]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 被光照的立方体
        {
            cubeProgram.Use();

            auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
            // auto modelMat = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(1.f, 1.f, 0.f));

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
            auto scale     = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
            auto modelMat  = translate * scale;

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
    ShaderProgram cubeProgram("shaders/02_02_02_TEST2.vs", "shaders/02_02_02_TEST2.fs");
    // 光源
    ShaderProgram lightProgram("shaders/02_02_02_TEST2_light.vs", "shaders/02_02_02_TEST2_light.fs");

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
        float lightPos[] { -1.f, 0.f, 0.f };  // 光的起始位置
        float viewPos[] { 0.f, 0.f, 5.f };    // 观察点

        // 光源和被光照的立方体使用相同的观察、投影矩阵，不同的模型矩阵
        auto viewMat       = glm::lookAt(glm::vec3(viewPos[0], viewPos[1], viewPos[2]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 光源的模型矩阵
        auto lightRotate    = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()) / 3.f, glm::vec3(0, 1, 0));
        auto lightTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        auto lightScale     = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
        auto lightModelMat  = lightRotate * lightTranslate * lightScale;

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

    // 物体
    ShaderProgram cubeProgram("shaders/02_02_02_TEST4.vs", "shaders/02_02_02_TEST4.fs");
    // 光源
    ShaderProgram lightProgram("shaders/02_02_02_TEST4_light.vs", "shaders/02_02_02_TEST4_light.fs");

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
        float lightPos[] { -1.f, 0.f, 0.f };  // 光的起始位置
        float viewPos[] { 0.f, 0.f, 5.f };    // 观察点

        // 光源和被光照的立方体使用相同的观察、投影矩阵，不同的模型矩阵
        auto viewMat       = glm::lookAt(glm::vec3(viewPos[0], viewPos[1], viewPos[2]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.0f);

        // 光源的模型矩阵
        auto lightRotate    = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()) / 3.f, glm::vec3(0, 1, 0));
        auto lightTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
        auto lightScale     = glm::scale(glm::mat4(1.f), glm::vec3(.2f, .2f, .2f));
        auto lightModelMat  = lightRotate * lightTranslate * lightScale;

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

#include <common.hpp>
#include <numbers>

// @brief 创建一个球心在(0,0,0)的球体
// @param [in] longitude 经线上的顶点个数
// @param [in] latitude 纬线上的顶点个数
// @param [in] radius 半径
Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = .5f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);
    // 法线
    // vertices.emplace_back(0.f);
    // vertices.emplace_back(radius);
    // vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
            // 法线
            // vertices.emplace_back(x);
            // vertices.emplace_back(y);
            // vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);
    // 法线
    // vertices.emplace_back(0.f);
    // vertices.emplace_back(-radius);
    // vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3 }, GL_TRIANGLES);
}

Renderer CreateSphere2()
{
    std::ifstream ifs_vertex("models/sphere/sphere_vertex.txt");
    std::ifstream ifs_index("models/sphere/sphere_index.txt");
    std::string line;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // 每一行是一个坐标：XYZ
    while (std::getline(ifs_vertex, line))
    {
        std::stringstream ss(line);
        std::string pos;
        while (std::getline(ss, pos, ','))
        {
            vertices.emplace_back(std::stof(pos));
        }
    }

    // 每一行是一个三角形的三个索引
    while (std::getline(ifs_index, line))
    {
        std::stringstream ss(line);
        std::string index;
        while (std::getline(ss, index, ','))
        {
            indices.emplace_back(std::stoul(index));
        }
    }

    return Renderer(vertices, indices, { 3 }, GL_TRIANGLES);
}

int main()
{
    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_02_02_TEST5.vert", "shaders/02_02_02_TEST5.frag");

    auto sphere = CreateSphere();
    // auto sphere = CreateSphere2();

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        sphere.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST5
