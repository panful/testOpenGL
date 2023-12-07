/*
 * 1. 材质对环境、漫反射、镜面反射的颜色
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

    ShaderProgram cubeProgram("shaders/02_02_03_TEST1.vs", "shaders/02_02_03_TEST1.fs");
    ShaderProgram lightProgram("shaders/02_02_03_TEST1_light.vs", "shaders/02_02_03_TEST1_light.fs");

    // clang-format off
    std::array<GLfloat, 6 * 6 * 6>  vertices{
        // pos                       // normal
        -0.5f, -0.5f, -0.5f,         0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,         0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,         0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,         0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,         0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,         0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,         0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,         0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,         0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,         0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,         0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,         0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,        -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,        -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,         1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,         1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,         1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,         1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,         1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,         1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,         0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,         0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,         0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,         0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,         0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,         0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,         0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,         0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,         0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,         0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,         0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,         0.0f,  1.0f,  0.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    auto lightPos = glm::vec3(.8f, .8f, .0f);

    unsigned int VAO_light;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO_light);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO_light);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, glm::value_ptr(lightPos), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    //----------------------------------------------------------------------------------

    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 5.f);

    // 光源颜色
    auto r { 1.f }, g { 1.f }, b { 1.f };
    auto lightColor = glm::vec3(r, b, b);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f); // 漫反射光照占光源颜色的50%
    auto ambientColor = lightColor * glm::vec3(0.2f); // 环境光照只占光源颜色的20%
    auto specularColor = lightColor * glm::vec3(1.f); // 镜面反射和光源颜色完全一样

    // 材质的反光度
    auto materialShininess = 32.f;
    // 材质的漫反射、环境反射、镜面反射颜色
    auto materialDiffuse = glm::vec3(1.f, .5f, .3f);
    auto materialAmbient = glm::vec3(1.f, .5f, .3f);
    auto materialSpecular = glm::vec3(.5f);

    //----------------------------------------------------------------------------------
    // 光源设置为一个点
    glEnable(GL_DEPTH_TEST);
    glPointSize(10.f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 被光照的立方体
        {
            cubeProgram.Use();
            auto rotateMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.f, glm::vec3(1, 1, 0));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);

            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.position", lightPos);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);
            cubeProgram.SetUniform3fv("material.ambient", materialAmbient);
            cubeProgram.SetUniform3fv("material.diffuse", materialDiffuse);
            cubeProgram.SetUniform3fv("material.specular", materialSpecular);

            cubeProgram.SetUniformMat4("model", rotateMat);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
        }

        // 光源
        {
            lightProgram.Use();
            lightProgram.SetUniform3fv("lightColor", lightColor);

            glBindVertexArray(VAO_light);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VAO_light);

    
    

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
