/*
 * 1. 屏幕空间环境光遮蔽(Screen-Space Ambient Occlusion, SSAO) 法向半球体
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>
#include <random>

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

void DrawCube(const Renderer& cube, const ShaderProgram& shader)
{
    glm::mat4 scale, rotate, translate;

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 1.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(-3.f, 3.f, 0.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(1.f, 1.f, 1.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(-3.f, -3.f, 0.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(2.f, 2.f, 2.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(30.f), glm::vec3(1.f, 1.f, 0.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(0.f), glm::vec3(1.f, 1.f, 0.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 1.f, 3.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(1.f, -1.f, 5.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);

    scale     = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
    rotate    = glm::rotate(glm::mat4(1.f), glm::radians(45.f), glm::vec3(1.f, 0.f, 0.f));
    translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -5.f));

    shader.SetUniformMat4("model", translate * rotate * scale);
    cube.Draw(GL_TRIANGLES);
}

int main()
{
    InitOpenGL init("Deferred Rendering", windowWidth, windowHeight, Camera({ 0.f, 0.f, 20.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    // clang-format off
    std::vector<GLfloat> verticesCube {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
    };

    std::vector<GLfloat> verticesQuad {
        // pos                 // texCoords
        -1.f,   1.f,  0.f,     0.f, 1.f,
        -1.f,  -1.f,  0.f,     0.f, 0.f,
         1.f,   1.f,  0.f,     1.f, 1.f,
         1.f,  -1.f,  0.f,     1.f, 0.f,
    };
    // clang-format on

    ShaderProgram programGBuffer("resources/02_05_08_TEST1_gbuffer.vs", "resources/02_05_08_TEST1_gbuffer.fs");
    ShaderProgram programDeferred("resources/02_05_08_TEST1_deferred.vs", "resources/02_05_08_TEST1_deferred.fs");
    ShaderProgram programSsao("resources/02_05_08_TEST1_deferred.vs", "resources/02_05_08_TEST1_ssao.fs");
    ShaderProgram programSsaoBlur("resources/02_05_08_TEST1_deferred.vs", "resources/02_05_08_TEST1_blur.fs");

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(verticesQuad, { 3, 2 });

    //---------------------------------------------------------------------------------------
    // 三个纹理分别保存位置向量、法向量、颜色和镜面反光度
    Texture gPosition(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 0);
    gPosition.SetWarpParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    gPosition.SetFilterParameter(GL_NEAREST, GL_NEAREST);
    Texture gNormal(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 1);
    gNormal.SetFilterParameter(GL_NEAREST, GL_NEAREST);
    Texture gAlbedo(windowWidth, windowHeight, GL_RGB, GL_RGB, GL_FLOAT, 2);
    gAlbedo.SetFilterParameter(GL_NEAREST, GL_NEAREST);

    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fboQuad;
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT0, gPosition);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT1, gNormal);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT2, gAlbedo);
    fboQuad.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fboQuad.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });

    Texture ssaoTex(windowWidth, windowHeight, GL_RED, GL_RED, GL_FLOAT);
    ssaoTex.SetFilterParameter(GL_NEAREST, GL_NEAREST);
    Texture ssaoBlurTex(windowWidth, windowHeight, GL_RED, GL_RED, GL_FLOAT);
    ssaoBlurTex.SetFilterParameter(GL_NEAREST, GL_NEAREST);

    FrameBufferObject fboSsao;
    fboSsao.AddAttachment(GL_COLOR_ATTACHMENT0, ssaoTex);
    FrameBufferObject fboSsaoBlur;
    fboSsaoBlur.AddAttachment(GL_COLOR_ATTACHMENT0, ssaoBlurTex);

    if (!fboQuad.Check() || !fboSsao.Check() || !fboSsaoBlur.Check())
    {
        std::cerr << "Frame buffer object error\n";
        return -1;
    }

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    auto ourLerp = [](float a, float b, float f) { return a + f * (b - a); };
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = ourLerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    Texture noiseTex(&ssaoNoise[0], 4, 4, GL_RGBA32F, GL_RGB, GL_FLOAT);
    noiseTex.SetFilterParameter(GL_NEAREST, GL_NEAREST);

    // 光源
    std::pair<glm::vec3, glm::vec3> light { glm::vec3(0.f, 0.f, 10.f), glm::vec3(1.f, 1.f, 1.f) };

    while (!glfwWindowShouldClose(window))
    {
        fboQuad.Bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        programGBuffer.Use();
        programGBuffer.SetUniformMat4("view", init.GetViewMatrix());
        programGBuffer.SetUniformMat4("projection", init.GetProjectionMatrix());
        DrawCube(cube, programGBuffer);
        glDisable(GL_DEPTH_TEST);

        fboSsao.Bind();
        glClear(GL_COLOR_BUFFER_BIT);
        programSsao.Use();
        programSsao.SetUniform1i("gPosition", 0);
        programSsao.SetUniform1i("gNormal", 1);
        programSsao.SetUniform1i("texNoise", 2);
        for (size_t i = 0; i < ssaoKernel.size(); ++i)
        {
            programSsao.SetUniform3fv("samples[" + std::to_string(i) + "]", ssaoKernel.at(i));
        }
        programSsao.SetUniformMat4("projection", init.GetProjectionMatrix());
        gPosition.Use(0);
        gNormal.Use(1);
        noiseTex.Use(2);
        quad.Draw(GL_TRIANGLE_STRIP);

        fboSsaoBlur.Bind();
        glClear(GL_COLOR_BUFFER_BIT);
        programSsaoBlur.Use();
        ssaoTex.Use(0);
        quad.Draw(GL_TRIANGLE_STRIP);
        fboSsaoBlur.Release();

        glClearColor(1.0, 1.0, 1.0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        programDeferred.Use();
        programDeferred.SetUniform1i("gPosition", 0);
        programDeferred.SetUniform1i("gNormal", 1);
        programDeferred.SetUniform1i("gAlbedo", 2);
        programDeferred.SetUniform1i("ssao", 3);
        programDeferred.SetUniform3fv("viewPos", init.GetViewPosition());

        programDeferred.SetUniform3fv("light.Position", glm::vec3(init.GetViewMatrix() * glm::vec4(light.first, 1.f)));
        programDeferred.SetUniform3fv("light.Color", light.second);
        programDeferred.SetUniform1f("light.Linear", 0.09f);
        programDeferred.SetUniform1f("light.Quadratic", 0.032f);

        gPosition.Use(0);
        gNormal.Use(1);
        gAlbedo.Use(2);
        ssaoBlurTex.Use(3);
        quad.Draw(GL_TRIANGLE_STRIP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1
