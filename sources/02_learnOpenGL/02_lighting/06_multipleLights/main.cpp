/*
 * 1. 多光源
 */

#define TEST1

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("shaders/02_02_06_TEST1_map.vs", "shaders/02_02_06_TEST1_map.fs");

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

    // 漫反射贴图
    Texture texture_diffuse("shaders/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("shaders/container_specular.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 4.f);
    // 材质的镜面反射反光度
    auto materialShininess = 128.f;

    //----------------------------------------------------------------------------------
    // 聚光灯
    auto spotLightPosition = viewPos;
    auto spotLightDirection = glm::vec3(0.f, 0.f, -1.f);
    auto spotLightCutOff = glm::cos(glm::radians(3.f));
    auto spotLightOuterCutOff = glm::cos(glm::radians(5.f));
    auto spotLightColor = glm::vec3(0.f, 0.f, 1.f);
    auto spotLightAmbientColor = spotLightColor * glm::vec3(0.2f);
    auto spotLightDiffuseColor = spotLightColor * glm::vec3(0.5f);
    auto spotLightSpecularColor = spotLightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 平行光
    auto dirLightDirection = glm::vec3(0.f, -1.f, 0.f);
    auto dirLightColor = glm::vec3(1.f);
    auto dirLightAmbientColor = dirLightColor * glm::vec3(0.2f);
    auto dirLightDiffuseColor = dirLightColor * glm::vec3(0.5f);
    auto dirLightSpecularColor = dirLightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 点光源0
    auto pointLightPosition = glm::vec3(0.f, -4.f, 4.f);
    auto pointLightColor = glm::vec3(1.f, 0.f, 0.f);
    auto pointLightAmbientColor = pointLightColor * glm::vec3(0.2f);
    auto pointLightDiffuseColor = pointLightColor * glm::vec3(0.5f);
    auto pointLightSpecularColor = pointLightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 点光源1
    auto pointLightPosition1 = glm::vec3(0.f, 4.f, 4.f);
    auto pointLightColor1 = glm::vec3(0.f, 1.f, 0.f);
    auto pointLightAmbientColor1 = pointLightColor1 * glm::vec3(0.2f);
    auto pointLightDiffuseColor1 = pointLightColor1 * glm::vec3(0.5f);
    auto pointLightSpecularColor1 = pointLightColor1 * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 光源衰减（点光源、聚光灯）
    auto lightAttenuationConstant = 1.f;
    auto lightAttenuationLinear = .014f;
    auto lightAttenuationQuadratic = .0007f;

    //----------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeProgram.Use();
        texture_diffuse.Bind();
        texture_specular.Bind();

        //----------------------------------------------------------------------------------
        // 材质
        cubeProgram.SetUniform1i("material.diffuse", 0);
        cubeProgram.SetUniform1i("material.specular", 1);
        cubeProgram.SetUniform1f("material.shininess", materialShininess);

        //----------------------------------------------------------------------------------
        // MVP矩阵
        auto model = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / 2), glm::vec3(1.f, 1.f, 0.f));
        auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);
        cubeProgram.SetUniform3fv("viewPos", viewPos);
        cubeProgram.SetUniformMat4("model", model);
        cubeProgram.SetUniformMat4("view", view);
        cubeProgram.SetUniformMat4("projection", projection);

        //----------------------------------------------------------------------------------
        // 设置灯光
        cubeProgram.SetUniform3fv("dirLight.direction", dirLightDirection);
        cubeProgram.SetUniform3fv("dirLight.ambient", dirLightAmbientColor);
        cubeProgram.SetUniform3fv("dirLight.diffuse", dirLightDiffuseColor);
        cubeProgram.SetUniform3fv("dirLight.specular", dirLightSpecularColor);

        cubeProgram.SetUniform3fv("spotLight.position", spotLightPosition);
        cubeProgram.SetUniform3fv("spotLight.direction", spotLightDirection);
        cubeProgram.SetUniform1f("spotLight.cutOff", spotLightCutOff);
        cubeProgram.SetUniform1f("spotLight.outerCutOff", spotLightOuterCutOff);
        cubeProgram.SetUniform3fv("spotLight.ambient", spotLightAmbientColor);
        cubeProgram.SetUniform3fv("spotLight.diffuse", spotLightDiffuseColor);
        cubeProgram.SetUniform3fv("spotLight.specular", spotLightSpecularColor);
        cubeProgram.SetUniform1f("spotLight.constant", lightAttenuationConstant);
        cubeProgram.SetUniform1f("spotLight.linear", lightAttenuationLinear);
        cubeProgram.SetUniform1f("spotLight.quadratic", lightAttenuationQuadratic);

        cubeProgram.SetUniform3fv("pointLights[0].position", pointLightPosition);
        cubeProgram.SetUniform3fv("pointLights[0].ambient", pointLightAmbientColor);
        cubeProgram.SetUniform3fv("pointLights[0].diffuse", pointLightDiffuseColor);
        cubeProgram.SetUniform3fv("pointLights[0].specular", pointLightSpecularColor);
        cubeProgram.SetUniform1f("pointLights[0].constant", lightAttenuationConstant);
        cubeProgram.SetUniform1f("pointLights[0].linear", lightAttenuationLinear);
        cubeProgram.SetUniform1f("pointLights[0].quadratic", lightAttenuationQuadratic);

        cubeProgram.SetUniform3fv("pointLights[1].position", pointLightPosition1);
        cubeProgram.SetUniform3fv("pointLights[1].ambient", pointLightAmbientColor1);
        cubeProgram.SetUniform3fv("pointLights[1].diffuse", pointLightDiffuseColor1);
        cubeProgram.SetUniform3fv("pointLights[1].specular", pointLightSpecularColor1);
        cubeProgram.SetUniform1f("pointLights[1].constant", lightAttenuationConstant);
        cubeProgram.SetUniform1f("pointLights[1].linear", lightAttenuationLinear);
        cubeProgram.SetUniform1f("pointLights[1].quadratic", lightAttenuationQuadratic);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        texture_diffuse.Release();
        texture_specular.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

#endif // TEST1