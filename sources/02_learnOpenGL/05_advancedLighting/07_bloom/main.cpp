/*
 * 1. Bloom 泛光 让光源有光晕效果
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };

int main()
{
    bool bloom { true };    // 是否开启泛光
    float exposure { 0.5 }; // 曝光值

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 10.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&bloom, &exposure](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                bloom = true;
                break;
            case 'n':
            case 'N':
                bloom = false;
                break;
            case 'a':
            case 'A':
                exposure += 0.01f;
                break;
            case 's':
            case 'S':
                exposure -= 0.01f;
                break;
            default:
                break;
            }
        });

    // 场景，输出两个颜色附件到帧缓冲
    ShaderProgram lightProgram("shaders/02_05_07_TEST1_light.vs", "shaders/02_05_07_TEST1_light.fs");
    // 光源
    ShaderProgram lightBoxProgram("shaders/02_05_07_TEST1_light.vs", "shaders/02_05_07_TEST1_light_box.fs");
    // 高斯模糊，光源的光晕效果
    ShaderProgram blurProgram("shaders/02_05_07_TEST1_blur.vs", "shaders/02_05_07_TEST1_blur.fs");
    // 纹理合并
    ShaderProgram finalProgram("shaders/02_05_07_TEST1_bloom_final.vs", "shaders/02_05_07_TEST1_bloom_final.fs");

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

    std::vector<GLfloat> vertices_quad {
        // pos                     // texCoords
        -1.f,   1.0f,  0.0f,     0.0f, 1.0f,
        -1.f,  -1.0f,  0.0f,     0.0f, 0.0f,
         1.f,  -1.0f,  0.0f,     1.0f, 0.0f,

        -1.f,   1.f,   0.0f,     0.0f, 1.0f,
         1.f,  -1.f,   0.0f,     1.0f, 0.0f,
         1.f,   1.f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(vertices_quad, { 3, 2 });

    Texture texture(std::string_view("shaders/wood.png"), 0, true);

    Texture fboTexture0(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
    Texture fboTexture1(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);

    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fbo;
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, fboTexture0);
    fbo.AddAttachment(GL_COLOR_ATTACHMENT1, fboTexture1);
    fbo.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fbo.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

    Texture pingpongTex[2] { Texture(windowWidth, windowHeight, GL_RGB16F, GL_RGBA, GL_FLOAT, 1),
        Texture(windowWidth, windowHeight, GL_RGB16F, GL_RGBA, GL_FLOAT) };

    FrameBufferObject pingpongFBO[2] { FrameBufferObject(), FrameBufferObject() };

    pingpongFBO[0].AddAttachment(GL_COLOR_ATTACHMENT0, pingpongTex[0]);
    pingpongFBO[1].AddAttachment(GL_COLOR_ATTACHMENT0, pingpongTex[1]);

    if (!fbo.Check() || !pingpongFBO[0].Check() || !pingpongFBO[1].Check())
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    //----------------------------------------------------------------------------------
    auto lightPos = glm::vec3(-2.f, 0.f, 0.f);
    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        fbo.Bind();
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 场景中的立方体
        lightProgram.Use();
        lightProgram.SetUniformMat4("model", glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, 0.f)));
        lightProgram.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        lightProgram.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        lightProgram.SetUniform3fv("lights.Position", lightPos);
        lightProgram.SetUniform3fv("lights.Color", glm::vec3(10.f, 10.f, 10.f));

        texture.Bind();
        cube.Draw(GL_TRIANGLES);
        texture.Release();

        // 光源
        lightBoxProgram.Use();
        lightBoxProgram.SetUniformMat4("model", glm::translate(glm::mat4(1.f), lightPos));
        lightBoxProgram.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        lightBoxProgram.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        cube.Draw(GL_TRIANGLES);

        glDisable(GL_DEPTH_TEST);
        fbo.Release();

        //----------------------------------------------------------------------------------
        bool horizontal { true }, first_iteration { true };
        unsigned int amount = 10;
        blurProgram.Use();
        for (unsigned int i = 0; i < amount; ++i)
        {
            pingpongFBO[horizontal].Bind();
            blurProgram.SetUniform1i("horizontal", horizontal);
            if (first_iteration)
            {
                fboTexture1.Bind();
            }
            else
            {
                pingpongTex[!horizontal].Bind();
            }

            quad.Draw(GL_TRIANGLES);

            horizontal = !horizontal;
            if (first_iteration)
            {
                first_iteration = false;
            }
            pingpongFBO[!horizontal].Release();
        }

        //----------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        finalProgram.Use();
        finalProgram.SetUniform1i("scene", 0);
        finalProgram.SetUniform1i("bloomBlur", 1);
        fboTexture0.Bind();
        pingpongTex[0].Bind();
        finalProgram.SetUniform1i("bloom", bloom);
        finalProgram.SetUniform1f("exposure", exposure);
        quad.Draw(GL_TRIANGLES);

        std::cout << "bloom: " << bloom << "\texposure: " << exposure << '\n';

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1