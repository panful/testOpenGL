/*
 * 1. 加权混合实现OIT(order-independent transparency 顺序无关透明度)，加权函数可以根据具体情况修改
 * 2. 更多物体透明设置
 * 3. 更多物体加入相机，不同视角观察透明效果
 * 4. 互相交互的物体的透明度
 * 
 * 21.Per-Pixel Linked List实现顺序无关透明
 */

#define TEST21

#ifdef TEST1

#include <common.hpp>

constexpr int windowWidth { 800 };
constexpr int windowHeight { 600 };

int main()
{
    // clang-format off
    // 透明红色
    std::vector<GLfloat> red{
        -0.8f, -0.8f, 0.3f,
         0.0f, -0.8f, 0.3f,
        -0.8f,  0.8f, 0.3f,
         0.0f,  0.8f, 0.3f,
    };

    // 不透明绿色
    std::vector<GLfloat> green{
        -0.5f, -0.8f, 0.4f,
         0.3f, -0.8f, 0.4f,
        -0.5f,  0.8f, 0.4f,
         0.3f,  0.8f, 0.4f,
    };

    // 透明蓝色
    std::vector<GLfloat> blue{
        -0.3f, -0.8f, 0.5f,
         0.5f, -0.8f, 0.5f,
        -0.3f,  0.8f, 0.5f,
         0.5f,  0.8f, 0.5f,
    };

    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 2);
    auto window = initOpenGL.GetWindow();

    Renderer rendererRed(red, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererGreen(green, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererBlue(blue, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programTransparent("resources/02_08_02_TEST1_transparent.vs", "resources/02_08_02_TEST1_transparent.fs");
    ShaderProgram programOpaque("resources/02_08_02_TEST1_opaque.vs", "resources/02_08_02_TEST1_opaque.fs");
    ShaderProgram programComposite("resources/02_08_02_TEST1_composite.vs", "resources/02_08_02_TEST1_composite.fs");
    ShaderProgram programScreen("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST1_screen.fs");

    //----------------------------------------------------------------------------------
    FrameBufferObject opaqueFBO;
    Texture opaqueTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture depthTex(windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    opaqueFBO.AddAttachment(GL_COLOR_ATTACHMENT0, opaqueTex);
    opaqueFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);

    FrameBufferObject transparentFBO;
    Texture accumTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture revealTex(windowWidth, windowHeight, GL_R8, GL_RED, GL_FLOAT);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT0, accumTex);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT1, revealTex);
    transparentFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);
    transparentFBO.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

    glm::vec4 zeroFillerVec(0.0f);
    glm::vec4 oneFillerVec(1.0f);

    //----------------------------------------------------------------------------------
    // 在标准化设备坐标系(NDC)中，OpenGL实际使用的是左手坐标系（投影矩阵交换了左右手），左手坐标系z轴指向屏幕里边
    // 透明和不透明帧缓冲并不是必须的，例如不透明的帧缓冲可以放到默认帧缓冲中，
    // 甚至透明帧缓冲也可以省略，默认帧缓冲可以包含四个附件（上述代码的四个纹理）

    while (!glfwWindowShouldClose(window))
    {
        //-------------------------------------------------------------------------
        // 先绘制不透明的物体，关闭混合，开启深度测试，深度测试函数使用默认值
        opaqueFBO.Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programOpaque.Use();
        programOpaque.SetUniformMat4("mvp", glm::mat4(1.f));
        programOpaque.SetUniform3f("color", 0.f, 1.f, 0.f);
        rendererGreen.Draw();

        //-------------------------------------------------------------------------
        // 将深度缓冲区设置为只读(掩码设置为false)，开启混合
        // 此处的深度缓冲使用的是不透明物体的深度缓冲，所以如果不透明物体在透明物体的前面（距离相近更近），透明物体的片段会被丢弃
        transparentFBO.Bind();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        // 设置像素混合方式，第一个参数表示FBO颜色附件的ID
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        // glClearBufferfv用来将指定的缓冲区设置为特定的颜色或浮点值
        // 参数1 可以是 GL_COLOR GL_DEPTH GL_STENCIL
        // 参数2 仅在清除颜色缓冲时有效，数值对应FBO颜色纹理附件（片段着色器可以输出多个颜色附件）
        // 参数3 颜色为 float[4] 深度和模板为 float[1]
        glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
        glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

        programTransparent.Use();
        programTransparent.SetUniformMat4("mvp", glm::mat4(1.f));

        programTransparent.SetUniform4f("color", 0.f, 0.f, 1.f, .5f);
        rendererBlue.Draw();

        programTransparent.SetUniform4f("color", 1.f, 0.f, 0.f, .5f);
        rendererRed.Draw();

        //-------------------------------------------------------------------------
        // 绑定不透明的帧缓冲，这个时候不透明帧缓冲中保存着不透明物体的颜色缓冲和深度缓冲
        // 透明帧缓冲的两个颜色附件中，accumTex是所有在不透明物体之上的，透明物体的颜色累加（因为透明缓冲使用了不透明缓冲的深度缓冲）
        // 将透明的帧缓冲合并到不透明的帧缓冲上
        // 深度测试函数设置为GL_ALWAYS，为了让quad的所有像素都能通过
        opaqueFBO.Bind();
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        programComposite.Use();
        accumTex.Use(0);
        revealTex.Use(1);
        rendererQuad.Draw();

        //-------------------------------------------------------------------------
        // 将结果呈现到屏幕
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        programScreen.Use();
        opaqueTex.Use(0);
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

constexpr int windowWidth { 800 };
constexpr int windowHeight { 600 };

int main()
{
    // clang-format off
    // 透明红色
    std::vector<GLfloat> red{
        -0.8f, -0.8f, 0.1f,
         0.0f, -0.8f, 0.1f,
        -0.8f,  0.8f, 0.1f,
         0.0f,  0.8f, 0.1f,
    };

    // 透明绿色
    std::vector<GLfloat> green{
        -0.5f, -0.8f, 0.2f,
         0.3f, -0.8f, 0.2f,
        -0.5f,  0.8f, 0.2f,
         0.3f,  0.8f, 0.2f,
    };

    // 透明蓝色
    std::vector<GLfloat> blue{
        -0.3f, -0.8f, 0.3f,
         0.5f, -0.8f, 0.3f,
        -0.3f,  0.8f, 0.3f,
         0.5f,  0.8f, 0.3f,
    };

    // 不透明黄色
    std::vector<GLfloat> yellow{
         0.0f, -0.8f, 0.4f,
         0.8f, -0.8f, 0.4f,
         0.0f,  0.8f, 0.4f,
         0.8f,  0.8f, 0.4f,
    };

    // 不透明青色
    std::vector<GLfloat> cyan{
        -0.1f, -0.8f, 0.5f,
         0.1f, -0.8f, 0.5f,
        -0.1f,  0.8f, 0.5f,
         0.1f,  0.8f, 0.5f,
    };

    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 2);
    auto window = initOpenGL.GetWindow();

    Renderer rendererRed(red, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererGreen(green, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererBlue(blue, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererYellow(yellow, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererCyan(cyan, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programTransparent("resources/02_08_02_TEST1_transparent.vs", "resources/02_08_02_TEST1_transparent.fs");
    ShaderProgram programOpaque("resources/02_08_02_TEST1_opaque.vs", "resources/02_08_02_TEST1_opaque.fs");
    ShaderProgram programComposite("resources/02_08_02_TEST1_composite.vs", "resources/02_08_02_TEST1_composite.fs");
    ShaderProgram programScreen("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST1_screen.fs");

    //----------------------------------------------------------------------------------
    FrameBufferObject opaqueFBO;
    Texture opaqueTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture depthTex(windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    opaqueFBO.AddAttachment(GL_COLOR_ATTACHMENT0, opaqueTex);
    opaqueFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);

    FrameBufferObject transparentFBO;
    Texture accumTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture revealTex(windowWidth, windowHeight, GL_R8, GL_RED, GL_FLOAT);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT0, accumTex);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT1, revealTex);
    transparentFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);
    transparentFBO.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

    glm::vec4 zeroFillerVec(0.0f);
    glm::vec4 oneFillerVec(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        opaqueFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programOpaque.Use();
        programOpaque.SetUniformMat4("mvp", glm::mat4(1.f));
        programOpaque.SetUniform3f("color", 1.f, 1.f, 0.f);
        rendererYellow.Draw();
        programOpaque.SetUniform3f("color", 0.f, 1.f, 1.f);
        rendererCyan.Draw();

        //-------------------------------------------------------------------------
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        transparentFBO.Bind();
        glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
        glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

        programTransparent.Use();
        programTransparent.SetUniformMat4("mvp", glm::mat4(1.f));

        programTransparent.SetUniform4f("color", 1.f, 0.f, 0.f, .5f);
        rendererRed.Draw();

        programTransparent.SetUniform4f("color", 0.f, 1.f, 0.f, .5f);
        rendererGreen.Draw();

        programTransparent.SetUniform4f("color", 0.f, 0.f, 1.f, .5f);
        rendererBlue.Draw();

        //-------------------------------------------------------------------------
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        opaqueFBO.Bind();
        programComposite.Use();
        accumTex.Use(0);
        revealTex.Use(1);
        rendererQuad.Draw();

        //-------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        programScreen.Use();
        opaqueTex.Use(0);
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

constexpr int windowWidth { 800 };
constexpr int windowHeight { 600 };

int main()
{
    // clang-format off
    // 透明红色
    std::vector<GLfloat> red{
        -0.8f, -0.8f, 0.1f,
         0.0f, -0.8f, 0.1f,
        -0.8f,  0.8f, 0.1f,
         0.0f,  0.8f, 0.1f,
    };

    // 透明绿色
    std::vector<GLfloat> green{
        -0.5f, -0.8f, 0.2f,
         0.3f, -0.8f, 0.2f,
        -0.5f,  0.8f, 0.2f,
         0.3f,  0.8f, 0.2f,
    };

    // 透明蓝色
    std::vector<GLfloat> blue{
        -0.3f, -0.8f, 0.3f,
         0.5f, -0.8f, 0.3f,
        -0.3f,  0.8f, 0.3f,
         0.5f,  0.8f, 0.3f,
    };

    // 不透明黄色
    std::vector<GLfloat> yellow{
         0.0f, -0.8f, 0.4f,
         0.8f, -0.8f, 0.4f,
         0.0f,  0.8f, 0.4f,
         0.8f,  0.8f, 0.4f,
    };

    // 不透明青色
    std::vector<GLfloat> cyan{
        -0.1f, -0.8f, 0.5f,
         0.1f, -0.8f, 0.5f,
        -0.1f,  0.8f, 0.5f,
         0.1f,  0.8f, 0.5f,
    };

    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 2, Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    Renderer rendererRed(red, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererGreen(green, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererBlue(blue, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererYellow(yellow, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererCyan(cyan, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programTransparent("resources/02_08_02_TEST1_transparent.vs", "resources/02_08_02_TEST1_transparent.fs");
    ShaderProgram programOpaque("resources/02_08_02_TEST1_opaque.vs", "resources/02_08_02_TEST1_opaque.fs");
    ShaderProgram programComposite("resources/02_08_02_TEST1_composite.vs", "resources/02_08_02_TEST1_composite.fs");
    ShaderProgram programScreen("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST1_screen.fs");

    //----------------------------------------------------------------------------------
    FrameBufferObject opaqueFBO;
    Texture opaqueTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture depthTex(windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    opaqueFBO.AddAttachment(GL_COLOR_ATTACHMENT0, opaqueTex);
    opaqueFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);

    FrameBufferObject transparentFBO;
    Texture accumTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture revealTex(windowWidth, windowHeight, GL_R8, GL_RED, GL_FLOAT);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT0, accumTex);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT1, revealTex);
    transparentFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);
    transparentFBO.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

    glm::vec4 zeroFillerVec(0.0f);
    glm::vec4 oneFillerVec(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        auto v = initOpenGL.GetViewMatrix();
        auto p = initOpenGL.GetProjectionMatrix();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        opaqueFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programOpaque.Use();
        programOpaque.SetUniformMat4("mvp", p * v);
        programOpaque.SetUniform3f("color", 1.f, 1.f, 0.f);
        rendererYellow.Draw();
        programOpaque.SetUniform3f("color", 0.f, 1.f, 1.f);
        rendererCyan.Draw();

        //-------------------------------------------------------------------------
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        transparentFBO.Bind();
        glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
        glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

        programTransparent.Use();
        programTransparent.SetUniformMat4("mvp", p * v);

        programTransparent.SetUniform4f("color", 1.f, 0.f, 0.f, .5f);
        rendererRed.Draw();

        programTransparent.SetUniform4f("color", 0.f, 1.f, 0.f, .5f);
        rendererGreen.Draw();

        programTransparent.SetUniform4f("color", 0.f, 0.f, 1.f, .5f);
        rendererBlue.Draw();

        //-------------------------------------------------------------------------
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        opaqueFBO.Bind();
        programComposite.Use();
        accumTex.Use(0);
        revealTex.Use(1);
        rendererQuad.Draw();

        //-------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        programScreen.Use();
        opaqueTex.Use(0);
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

constexpr int windowWidth { 800 };
constexpr int windowHeight { 600 };

int main()
{
    // clang-format off
    // 透明红色
    std::vector<GLfloat> red{
         .9f,  .6f, -.1f,
         .9f,  .5f, -.1f,
        -.9f,  .6f,  .1f,
        -.9f,  .5f,  .1f,
    };

    // 透明绿色
    std::vector<GLfloat> green{
        -.9f,  .9f, -.1f,
        -.9f,  .7f, -.1f,
         .5f, -.7f,  .1f,
         .5f, -.9f,  .1f,
    };

    // 透明蓝色
    std::vector<GLfloat> blue{
        -.5f, -.7f, -.1f,
        -.5f, -.9f, -.1f,
         .9f,  .9f,  .1f,
         .9f,  .7f,  .1f,

    };

    // 不透明黄色
    std::vector<GLfloat> yellow{
         .9f,  .2f,  .1f,
         .9f,  .1f,  .1f,
        -.9f,  .2f,  .1f,
        -.9f,  .1f,  .1f,
    };

    // 不透明青色
    std::vector<GLfloat> cyan{
         .9f, -.5f, -.1f,
         .9f, -.6f, -.1f,
        -.9f, -.5f, -.1f,
        -.9f, -.6f, -.1f,
    };

    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 2, Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    Renderer rendererRed(red, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererGreen(green, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererBlue(blue, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererYellow(yellow, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererCyan(cyan, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programTransparent("resources/02_08_02_TEST1_transparent.vs", "resources/02_08_02_TEST1_transparent.fs");
    ShaderProgram programOpaque("resources/02_08_02_TEST1_opaque.vs", "resources/02_08_02_TEST1_opaque.fs");
    ShaderProgram programComposite("resources/02_08_02_TEST1_composite.vs", "resources/02_08_02_TEST1_composite.fs");
    ShaderProgram programScreen("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST1_screen.fs");

    //----------------------------------------------------------------------------------
    FrameBufferObject opaqueFBO;
    Texture opaqueTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture depthTex(windowWidth, windowHeight, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
    opaqueFBO.AddAttachment(GL_COLOR_ATTACHMENT0, opaqueTex);
    opaqueFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);

    FrameBufferObject transparentFBO;
    Texture accumTex(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT);
    Texture revealTex(windowWidth, windowHeight, GL_R8, GL_RED, GL_FLOAT);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT0, accumTex);
    transparentFBO.AddAttachment(GL_COLOR_ATTACHMENT1, revealTex);
    transparentFBO.AddAttachment(GL_DEPTH_ATTACHMENT, depthTex);
    transparentFBO.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });

    glm::vec4 zeroFillerVec(0.0f);
    glm::vec4 oneFillerVec(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        auto v = initOpenGL.GetViewMatrix();
        auto p = initOpenGL.GetProjectionMatrix();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        opaqueFBO.Bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programOpaque.Use();
        programOpaque.SetUniformMat4("mvp", p * v);
        programOpaque.SetUniform3f("color", 1.f, 1.f, 0.f);
        rendererYellow.Draw();
        programOpaque.SetUniform3f("color", 0.f, 1.f, 1.f);
        rendererCyan.Draw();

        //-------------------------------------------------------------------------
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        transparentFBO.Bind();
        glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
        glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

        programTransparent.Use();
        programTransparent.SetUniformMat4("mvp", p * v);

        programTransparent.SetUniform4f("color", 1.f, 0.f, 0.f, .5f);
        rendererRed.Draw();

        programTransparent.SetUniform4f("color", 0.f, 1.f, 0.f, .5f);
        rendererGreen.Draw();

        programTransparent.SetUniform4f("color", 0.f, 0.f, 1.f, .5f);
        rendererBlue.Draw();

        //-------------------------------------------------------------------------
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        opaqueFBO.Bind();
        programComposite.Use();
        accumTex.Use(0);
        revealTex.Use(1);
        rendererQuad.Draw();

        //-------------------------------------------------------------------------
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        programScreen.Use();
        opaqueTex.Use(0);
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST4

#ifdef TEST21

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };

int main()
{
    // clang-format off
    std::vector<GLfloat> vertices0{
        -0.5f,  0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
         0.5f,  0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
    };

    std::vector<GLfloat> vertices1{
        -0.2f,  0.5f, -0.5f,
        -0.2f, -0.5f, -0.5f,
        -0.2f,  0.5f,  0.5f,
        -0.2f, -0.5f,  0.5f,
    };

    std::vector<GLfloat> vertices2{
         0.2f,  0.5f, -0.5f,
         0.2f, -0.5f, -0.5f,
         0.2f,  0.5f,  0.5f,
         0.2f, -0.5f,  0.5f,
    };

    std::vector<GLfloat> quad{
        -1.f,  1.f,
        -1.f, -1.f,
         1.f,  1.f,
         1.f, -1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(vertices2, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programGeometry("resources/02_08_02_TEST5_geometry.vs", "resources/02_08_02_TEST5_geometry.fs");
    ShaderProgram programColor("resources/02_08_02_TEST5_color.vs", "resources/02_08_02_TEST5_color.fs");

    //-----------------------------------------------------------------------------------
    // 保存每个像素的队头索引
    GLuint headIndexImageTexture { 0 };
    glGenTextures(1, &headIndexImageTexture);
    glBindTexture(GL_TEXTURE_2D, headIndexImageTexture);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, width, height);
    glBindTexture(GL_TEXTURE_2D, 0);

    //-----------------------------------------------------------------------------------
    // 链表当前的索引，链表最大长度
    struct geometrySBO
    {
        GLuint count;
        GLuint maxNodeCount;
    };

    GLuint NODE_COUNT = 20;
    geometrySBO geometry_sbo { 0, NODE_COUNT * width * height };
    GLuint buffer1 { 0 };
    glGenBuffers(1, &buffer1);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer1);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(geometrySBO), &geometry_sbo, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //-----------------------------------------------------------------------------------
    // 保存每个像素的所有片段信息，一个像素可能会由多个片段构成（多个片段就需要混合）
    struct Node
    {
        glm::vec4 color; // 片段的颜色
        float depth;     // 片段的深度值
        GLuint next;     // 下一个片段的索引
    };

    std::vector<Node> nodes(NODE_COUNT * width * height); // 链表的长度需要提前指定，太大会造成内存浪费，太小结果会错误
    GLuint buffer3 { 0 };
    glGenBuffers(1, &buffer3);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer3);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Node) * nodes.size(), nodes.data(), GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //-----------------------------------------------------------------------------------
    // 不需要开启深度测试，也不需要开启混合
    // 两个Buffer，一个Buffer和窗口（帧缓冲）大小一致，保存窗口每个像素的最后一个片段索引
    // 另一个Buffer保存所有的片段信息，片段信息包含颜色、深度和当前像素下一个片段的索引
    // 通过找到构成当前像素的所有片段信息，然后根据深度值排序，再把这些片段按顺序混合就是当前像素的实际颜色
    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------------------
        // 每一帧开始时，将链表当前索引count置为0
        GLuint data { 0 };
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer1);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        // 将每个像素的队头链表索引置为0xFFFFFFFF
        GLuint clearColor = 0xFFFFFFFF;
        glBindTexture(GL_TEXTURE_2D, headIndexImageTexture);
        glClearTexImage(headIndexImageTexture, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearColor);
        glBindTexture(GL_TEXTURE_2D, 0);

        //-----------------------------------------------------------------------------------
        // 绘制所有图元，不需要排序
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        programGeometry.Use();
        programGeometry.SetUniformMat4("model", glm::mat4(1.f));
        programGeometry.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programGeometry.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, buffer3);
        glBindImageTexture(2, headIndexImageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

        programGeometry.SetUniform4fv("uColor", glm::vec4(0.f, 1.f, 0.f, 0.5f));
        rendererRect1.Draw();
        programGeometry.SetUniform4fv("uColor", glm::vec4(1.f, 0.f, 0.f, 1.0f));
        rendererRect0.Draw();
        programGeometry.SetUniform4fv("uColor", glm::vec4(0.f, 0.f, 1.f, 0.5f));
        rendererRect2.Draw();

        //-----------------------------------------------------------------------------------
        // 对每个像素的所有片段进行深度排序，然后颜色混合
        programColor.Use();
        glBindImageTexture(0, headIndexImageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer3);
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST21
