/*
 * 1. HDR(High Dynamic Range)高动态范围，亮的东西变得非常亮，暗的东西变得非常暗
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };

int main()
{
    bool hdr { false };            // hdr是否开启
    float exposure { 0.5 };        // 设置曝光参数
    bool exposureSwitch { false }; // 使用使用曝光参数，否则使用Reinhard色调映射

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&hdr, &exposure, &exposureSwitch](char c)
        {
            switch (c)
            {
            case 'h':
            case 'H':
                hdr = true;
                break;
            case 'n':
            case 'N':
                hdr = false;
                break;
            case 'a':
            case 'A':
                exposure += 0.01f;
                break;
            case 's':
            case 'S':
                exposure -= 0.01f;
                break;
            case 'e':
            case 'E':
                exposureSwitch = true;
                break;
            case 'r':
            case 'R':
                exposureSwitch = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram lightProgram("resources/02_05_06_TEST1_light.vs", "resources/02_05_06_TEST1_light.fs");
    ShaderProgram hdrProgram("resources/02_05_06_TEST1_hdr.vs", "resources/02_05_06_TEST1_hdr.fs");

    // clang-format off
    // 8个顶点
    std::vector<GLfloat> verticesCube {
            -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,    0.0f, 0.0f, 1.0f,     0.0f, 1.0f, // top-left 
        };

    // 矩形，绘制到默认帧缓冲
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

    Texture texture("resources/wood.png", 0, true);
    Texture fboTexture(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fbo;
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, fboTexture, 0);
    fbo.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    if (!fbo.Check())
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    //----------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        // 立方体
        fbo.Bind();
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightProgram.Use();
        lightProgram.SetUniformMat4("model", glm::mat4(1.f));
        lightProgram.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        lightProgram.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        lightProgram.SetUniform3fv("lights.Position", glm::vec3(0.f));
        lightProgram.SetUniform3fv("lights.Color", glm::vec3(100.f));

        texture.Bind();
        cube.Draw(GL_TRIANGLES);
        texture.Release();

        glDisable(GL_DEPTH_TEST);
        fbo.Release();

        //----------------------------------------------------------------------------------
        // 四边形
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        std::cout << "hdr: " << hdr << "\texposure switch: " << exposureSwitch << "\texposure: " << exposure << '\n';

        hdrProgram.Use();
        hdrProgram.SetUniform1i("hdr", hdr);
        hdrProgram.SetUniform1i("exposureSwitch", exposureSwitch);
        hdrProgram.SetUniform1f("exposure", exposure);
        fboTexture.Bind();
        quad.Draw(GL_TRIANGLES);
        fboTexture.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1