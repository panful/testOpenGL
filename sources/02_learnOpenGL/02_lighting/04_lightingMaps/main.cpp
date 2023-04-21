/*
 * 1. 材质使用漫反射贴图
 * 2. 材质使用漫反射和镜面光照贴图
 * 3. 放射光贴图(Emission Map)，物体忽略光照条件进行发光
 */

#define TEST3

#ifdef TEST1

#include <array>
#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();

    ShaderProgram cubeProgram("resources/02_02_04_TEST1_map.vs", "resources/02_02_04_TEST1_map.fs");
    ShaderProgram lightProgram("resources/02_02_04_TEST1_light.vs", "resources/02_02_04_TEST1_light.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 光源位置
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

    // 漫反射贴图
    Texture texture("resources/container.png");

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 5.f);
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f); // 漫反射光照占光源颜色的50%
    auto ambientColor = lightColor * glm::vec3(0.2f); // 环境光照只占光源颜色的20%
    auto specularColor = lightColor * glm::vec3(1.f); // 镜面反射和光源颜色完全一样
    // 材质的反光度
    auto materialShininess = 32.f;
    // 材质的镜面反射颜色
    auto materialSpecular = glm::vec3(.5f);

    //----------------------------------------------------------------------------------
    // 材质的漫反射使用贴图，镜面反射手动设置
    glEnable(GL_DEPTH_TEST);
    glPointSize(10.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 被光照的立方体
        {
            cubeProgram.Use();
            // 漫反射贴图
            texture.Bind();

            auto rotateMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.f, glm::vec3(1, 1, 0));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);

            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.position", lightPos);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);
            cubeProgram.SetUniform3fv("material.specular", materialSpecular);

            cubeProgram.SetUniformMat4("model", rotateMat);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            texture.Release();
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

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <array>
#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();

    ShaderProgram cubeProgram("resources/02_02_04_TEST2_map.vs", "resources/02_02_04_TEST2_map.fs");
    ShaderProgram lightProgram("resources/02_02_04_TEST1_light.vs", "resources/02_02_04_TEST1_light.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
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

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 黑白的镜面光照贴图
    // Texture texture_specular("resources/container_specular.png", 1);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 5.f);
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f); // 漫反射光照占光源颜色的50%
    auto ambientColor = lightColor * glm::vec3(0.2f); // 环境光照只占光源颜色的20%
    auto specularColor = lightColor * glm::vec3(1.f); // 镜面反射和光源颜色完全一样
    // 材质的反光度
    auto materialShininess = 32.f;

    //----------------------------------------------------------------------------------
    // 材质的漫反射、镜面反射都是使用光照贴图
    glEnable(GL_DEPTH_TEST);
    glPointSize(10.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 被光照的立方体
        {
            cubeProgram.Use();
            texture_diffuse.Bind();
            texture_specular.Bind();

            cubeProgram.SetUniform1i("material.diffuse", 0);
            cubeProgram.SetUniform1i("material.specular", 1);

            auto rotateMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.f, glm::vec3(1, 1, 0));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);

            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.position", lightPos);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);

            cubeProgram.SetUniformMat4("model", rotateMat);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            texture_diffuse.Release();
            texture_specular.Release();
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

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();

    ShaderProgram cubeProgram("resources/02_02_04_TEST3_map.vs", "resources/02_02_04_TEST3_map.fs");
    ShaderProgram lightProgram("resources/02_02_04_TEST1_light.vs", "resources/02_02_04_TEST1_light.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
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

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 黑白的镜面光照贴图
    // Texture texture_specular("resources/container_specular.png", 1);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);
    // 放射光贴图
    Texture texture_emission("resources/emission.jpg", 2);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 5.f);
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f); // 漫反射光照占光源颜色的50%
    auto ambientColor = lightColor * glm::vec3(0.2f); // 环境光照只占光源颜色的20%
    auto specularColor = lightColor * glm::vec3(1.f); // 镜面反射和光源颜色完全一样
    // 材质的反光度
    auto materialShininess = 32.f;

    //----------------------------------------------------------------------------------
    // 材质的漫反射、镜面反射都使用贴图，再使用一个放射光贴图
    // 放射光贴图就是忽略光照条件进行发光，例如夜晚猫的眼睛
    glEnable(GL_DEPTH_TEST);
    glPointSize(10.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 被光照的立方体
        {
            cubeProgram.Use();

            texture_diffuse.Bind();
            texture_specular.Bind();
            texture_emission.Bind();

            cubeProgram.SetUniform1i("material.diffuse", 0);
            cubeProgram.SetUniform1i("material.specular", 1);
            cubeProgram.SetUniform1i("material.emission", 2);

            auto rotateMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.f, glm::vec3(1, 1, 0));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);

            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.position", lightPos);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);

            cubeProgram.SetUniformMat4("model", rotateMat);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            texture_diffuse.Release();
            texture_specular.Release();
            texture_emission.Release();
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

    cubeProgram.DeleteProgram();
    lightProgram.DeleteProgram();

    glfwTerminate();
    return 0;
}

#endif // TEST3