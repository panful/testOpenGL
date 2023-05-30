/*
 * 1. 点光源阴影
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

// 窗口的大小
uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };
// FBO纹理的大小
// 深度贴图（纹理）的大小影响阴影的质量，深度贴图太小就会出现走样
constexpr uint32_t textureWidth { 4096 };
constexpr uint32_t textureHeight { 4096 };
// 深度贴图FBO
GLuint depthMappingFBO { 0 };

// 相机和光源的位置
auto viewPos  = glm::vec3(0.f, 0.f, 0.f);
auto lightPos = glm::vec3(0.f, 0.f, 0.f);

// 光的视锥体的近远平面
auto nearPlane { 1.f };
auto farPlane { 25.f };

int main()
{
    InitOpenGL initOpenGL("Shadow Mapping", windowWidth, windowHeight, Camera({ 0.f, 0.f, 1.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram depthProgram(
        "resources/02_05_03_02_TEST1_depth.vs", "resources/02_05_03_02_TEST1_depth.fs", "resources/02_05_03_02_TEST1_depth.gs");
    ShaderProgram lightProgram("resources/02_05_03_02_TEST1_light.vs", "resources/02_05_03_02_TEST1_light.fs");

    // clang-format off
    std::vector<GLfloat> verticesCube{
        // back face
        -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,    0.0f,  0.0f, -1.0f,     0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f,   -1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,     0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,    0.0f, -1.0f,  0.0f,     0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,    0.0f,  1.0f,  0.0f,     0.0f, 0.0f  // bottom-left     
    };
    // clang-format on

    Renderer cube(verticesCube, { 3, 3, 2 });

    Texture woodTexture("resources/02_05_03_02_wood.png", 0);
    Texture rectpolyTexture("resources/02_05_03_02_rectpoly.jpg", 0);

    //----------------------------------------------------------------------------------
    GLuint depthTextureBuffer { 0 };
    glGenTextures(1, &depthTextureBuffer);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthTextureBuffer);
    for (GLuint i = 0; i < 6; i++)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    GLuint depthMappingFBO;
    glGenFramebuffers(1, &depthMappingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureBuffer, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------
    // 立方体贴图共6个vp矩阵
    auto projInLightSpace = glm::perspective(glm::radians(90.f), 1.f, nearPlane, farPlane);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
    shadowTransforms.emplace_back(projInLightSpace * glm::lookAt(lightPos, lightPos + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, -1.f, 0.f)));

    // 模型矩阵，深度贴图和实际场景是同一个模型矩阵
    auto modelMat = glm::mat4(1.f);

    //----------------------------------------------------------------------------------
    // 一个立方体盒子内部中心有一个点光源，盒子内部还有一个小的立方体用来测试点光源

    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        // 先在深度贴图FBO中将整个场景渲染一次
        glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
        glViewport(0, 0, textureWidth, textureHeight);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthProgram.Use();

        for (size_t i = 0; i < 6; ++i)
        {
            depthProgram.SetUniformMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms.at(i));
        }

        depthProgram.SetUniform3fv("lightPos", lightPos);
        depthProgram.SetUniform1f("far_plane", farPlane);

        //------------------
        // 外部大的立方体，相当于一个世界空间，相机在这个空间里边
        // 需要关闭背面剔除，因为是在立方体内部观察，所以看到的始终都是背面
        glDisable(GL_CULL_FACE);
        modelMat = glm::scale(glm::mat4(1.f), glm::vec3(10.f));
        depthProgram.SetUniformMat4("model", modelMat);
        cube.Draw(GL_TRIANGLES);
        glEnable(GL_CULL_FACE);

        //------------------
        // 大立方体内部的一个小立方体，用来测试阴影
        modelMat = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, -8.f)), glm::scale(glm::mat4(1.f), glm::vec3(.1f));
        depthProgram.SetUniformMat4("model", modelMat);
        cube.Draw(GL_TRIANGLES);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 正常渲染一次整个场景，并将深度贴图绑定到着色器
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightProgram.Use();

        lightProgram.SetUniform1i("diffuseTexture", 0);
        lightProgram.SetUniform1i("depthMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthTextureBuffer);

        lightProgram.SetUniform3fv("lightPos", lightPos);
        lightProgram.SetUniform3fv("viewPos", initOpenGL.GetViewPosition());
        lightProgram.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        lightProgram.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        lightProgram.SetUniform1f("far_plane", farPlane);

        //------------------
        modelMat = glm::scale(glm::mat4(1.f), glm::vec3(10.f));
        lightProgram.SetUniformMat4("model", modelMat);
        // 大立方体需要将法线方向翻转，关闭背面剔除
        glDisable(GL_CULL_FACE);
        lightProgram.SetUniform1i("reverse_normals", 1);
        woodTexture.Bind();
        cube.Draw(GL_TRIANGLES);
        woodTexture.Release();
        lightProgram.SetUniform1i("reverse_normals", 0);
        glEnable(GL_CULL_FACE);

        //------------------
        modelMat = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, -8.f)), glm::scale(glm::mat4(1.f), glm::vec3(.1f));
        lightProgram.SetUniformMat4("model", modelMat);

        rectpolyTexture.Bind();
        cube.Draw(GL_TRIANGLES);
        rectpolyTexture.Release();

        //----------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST1
