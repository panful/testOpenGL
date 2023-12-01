/**
 * 11. 加权混合实现OIT 加权函数可以根据具体情况修改
 * 12. 加权混合对于互相交叉的图元会有误差
 *
 * 21. Per-Pixel Linked List 实现OIT A-Buffer
 * 22. 互相交叉图元 OIT
 *
 * 31. 绘制距离相机第二近的片段
 * 32. 深度剥离(Depth peeling)实现 OIT
 * 33. 互相交叉图元 OIT （从前往后剥离）
 * 34. 互相交叉图元 OIT （从后往前剥离）
 * 35. 获取指定位置深度值最大和最小的片段
 * 36. 双向深度剥离实现OIT DualDepthPeeling
 * 37. 互相交叉图元 OIT (Dual Depth Peeling)
 *
 *
 * 透明度混合 https://zhuanlan.zhihu.com/p/368065919
 * OIT https://blog.csdn.net/qq_35312463/article/details/115827894
 */

#define TEST36

#ifdef TEST11

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
    // https://learnopengl.com/Guest-Articles/2020/OIT/Weighted-Blended
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

#endif // TEST11

#ifdef TEST12

#include <common.hpp>

constexpr int windowWidth { 800 };
constexpr int windowHeight { 600 };

int main()
{
    // clang-format off
    std::vector<GLfloat> rect1{
        -.5f,  .5f,  0.f,
        -.5f, -.5f,  0.f,
         .5f,  .5f,  0.f,
         .5f, -.5f,  0.f,
    };

    std::vector<GLfloat> rect2{
         0.f,  .5f, -.5f,
         0.f, -.5f, -.5f,
         0.f,  .5f,  .5f,
         0.f, -.5f,  .5f,
    };

    std::vector<GLfloat> rect3{
        -.5f,  .5f,  .2f,
        -.5f, -.5f,  .2f,
         .5f,  .5f,  .2f,
         .5f, -.5f,  .2f,
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

    Renderer rendererRect1(rect1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(rect2, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect3(rect3, { 3 }, GL_TRIANGLE_STRIP);
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

        // 不透明
        // programOpaque.Use();
        // programOpaque.SetUniformMat4("mvp", p * v);
        // programOpaque.SetUniform3f("color", 0.f, 0.f, 1.f);
        // rendererRect3.Draw();

        //-------------------------------------------------------------------------
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunci(0, GL_ONE, GL_ONE);
        glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);

        transparentFBO.Bind();
        glClearBufferfv(GL_COLOR, 0, &zeroFillerVec[0]);
        glClearBufferfv(GL_COLOR, 1, &oneFillerVec[0]);

        // 半透明
        programTransparent.Use();
        programTransparent.SetUniformMat4("mvp", p * v);
        programTransparent.SetUniform4f("color", 1.f, 0.f, 0.f, .5f);
        rendererRect1.Draw();
        programTransparent.SetUniform4f("color", 0.f, 1.f, 0.f, .5f);
        rendererRect2.Draw();

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

#endif // TEST12

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

    ShaderProgram programGeometry("resources/02_08_02_TEST2_geometry.vs", "resources/02_08_02_TEST2_geometry.fs");
    ShaderProgram programColor("resources/02_08_02_TEST2_color.vs", "resources/02_08_02_TEST2_color.fs");

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
    // 无法在渲染前知道所需内存的大小，一般用于离线渲染
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

#ifdef TEST22

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

    std::vector<GLfloat> vertices3{
        -0.5f,  0.5f,  0.2f,
        -0.5f, -0.5f,  0.2f,
         0.5f,  0.5f,  0.2f,
         0.5f, -0.5f,  0.2f,

        -0.5f,  0.5f, -0.2f,
        -0.5f, -0.5f, -0.2f,
         0.5f,  0.5f, -0.2f,
         0.5f, -0.5f, -0.2f,
    };

    std::vector<GLfloat> quad{
        -1.f,  1.f,
        -1.f, -1.f,
         1.f,  1.f,
         1.f, -1.f,
    };

    std::vector<GLuint> indices{0,1,2,1,3,2, 4,5,6,5,7,6};
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(vertices2, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect3(vertices3, indices, { 3 }, GL_TRIANGLES);
    Renderer rendererQuad(quad, { 2 }, GL_TRIANGLE_STRIP);

    ShaderProgram programGeometry("resources/02_08_02_TEST2_geometry.vs", "resources/02_08_02_TEST2_geometry.fs");
    ShaderProgram programColor("resources/02_08_02_TEST2_color.vs", "resources/02_08_02_TEST2_color.fs");

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
        programGeometry.SetUniform4fv("uColor", glm::vec4(1.f, 1.f, 1.f, 0.5f));
        rendererRect3.Draw();

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

#endif // TEST22

#ifdef TEST31

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
        -0.5f,  0.5f,  0.1f,
        -0.5f, -0.5f,  0.1f,
         0.5f,  0.5f,  0.1f,
         0.5f, -0.5f,  0.1f,
    };

    std::vector<GLfloat> vertices2{
        -0.5f,  0.5f,  0.2f,
        -0.5f, -0.5f,  0.2f,
         0.5f,  0.5f,  0.2f,
         0.5f, -0.5f,  0.2f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programPass1("resources/02_08_02_TEST31_pass1.vs", "resources/02_08_02_TEST31_pass1.fs");
    ShaderProgram programPass2("resources/02_08_02_TEST31_pass2.vs", "resources/02_08_02_TEST31_pass2.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(vertices2, { 3 }, GL_TRIANGLE_STRIP);

    // 用来保存距离相机最近的片段的深度值
    Texture texDepth(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

    FrameBufferObject pass1;
    pass1.AddAttachment(GL_DEPTH_ATTACHMENT, texDepth);

    //-----------------------------------------------------------------------------------
    // 首先绘制整个场景，将距离相机最近的所有片段深度值写入texDepth
    // 然后再绘制整个场景，当片段的深度值不大于texDepth中的深度值时，则丢弃该片段（通过将深度值设置为1.0，让该片段无法通过深度测试来丢弃该片段）
    // 最后显示到屏幕上的就是距离相机第二近的片段颜色
    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------------------
        // 将距离相机最近的片段的深度值保存到texDepth中
        pass1.Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programPass1.Use();
        programPass1.SetUniformMat4("model", glm::mat4(1.f));
        programPass1.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programPass1.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        rendererRect0.Draw();
        rendererRect1.Draw();
        rendererRect2.Draw();
        pass1.Release();

        //-----------------------------------------------------------------------------------
        // 绘制距离相机第二近的片段
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programPass2.Use();
        programPass2.SetUniformMat4("model", glm::mat4(1.f));
        programPass2.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programPass2.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        texDepth.Use();

        programPass2.SetUniform4f("uColor", 1.f, 0.f, 0.f, 1.f);
        rendererRect0.Draw();
        programPass2.SetUniform4f("uColor", 0.f, 1.f, 0.f, 1.f);
        rendererRect1.Draw();
        programPass2.SetUniform4f("uColor", 0.f, 0.f, 1.f, 1.f);
        rendererRect2.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST31

#ifdef TEST32

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };
constexpr size_t numPeels { 8 }; // 深度剥离的层数，可以根据需要增加，层数改变后着色器需要修改

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
        -0.5f,  0.5f,  0.1f,
        -0.5f, -0.5f,  0.1f,
         0.5f,  0.5f,  0.1f,
         0.5f, -0.5f,  0.1f,
    };

    std::vector<GLfloat> vertices2{
        -0.5f,  0.5f,  0.2f,
        -0.5f, -0.5f,  0.2f,
         0.5f,  0.5f,  0.2f,
         0.5f, -0.5f,  0.2f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programFirst("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST32_dp_first.fs");
    ShaderProgram programPeeling("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST32_dp_peeling.fs");
    ShaderProgram programBlend("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST32_dp_blend.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(vertices2, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    std::vector<Texture*> depthTextures;
    std::vector<Texture*> colorTextures;
    std::vector<FrameBufferObject*> pass;
    for (size_t i = 0; i < numPeels; ++i)
    {
        colorTextures.emplace_back(new Texture(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT));
        depthTextures.emplace_back(new Texture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
        pass.emplace_back(new FrameBufferObject());

        pass[i]->AddAttachment(GL_COLOR_ATTACHMENT0, *colorTextures[i]);
        pass[i]->AddAttachment(GL_DEPTH_ATTACHMENT, *depthTextures[i]);
    }

    //-----------------------------------------------------------------------------------
    // 将片段按照距离相机的远近分为8份（片段如果超过8个会有误差）
    // 然后按照从距离相机最远到最近混合片段
    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------------------
        // 距离相机最近的片段，直接将通过深度测试的片段信息（颜色、深度）保存即可
        pass[0]->Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programFirst.Use();
        programFirst.SetUniformMat4("model", glm::mat4(1.f));
        programFirst.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programFirst.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        programFirst.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
        rendererRect0.Draw();
        programFirst.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
        rendererRect1.Draw();
        programFirst.SetUniform4f("uColor", 0.f, 0.f, 1.f, .5f);
        rendererRect2.Draw();
        pass[0]->Release();

        //-----------------------------------------------------------------------------------
        // 根据上一次的片段信息，将距离相机比上一次远的片段信息保存
        for (size_t i = 1; i < numPeels; ++i)
        {
            pass[i]->Bind();
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            programPeeling.Use();
            programPeeling.SetUniformMat4("model", glm::mat4(1.f));
            programPeeling.SetUniformMat4("view", initOpenGL.GetViewMatrix());
            programPeeling.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());
            depthTextures[i - 1]->Use();
            programPeeling.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
            rendererRect0.Draw();
            programPeeling.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
            rendererRect1.Draw();
            programPeeling.SetUniform4f("uColor", 0.f, 0.f, 1.f, .5f);
            rendererRect2.Draw();
            pass[i]->Release();
        }

        //-----------------------------------------------------------------------------------
        // 将按照距离相机远近排序的片段混合
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        programBlend.Use();
        for (size_t i = 0; i < numPeels; ++i)
        {
            programBlend.SetUniform1i("pass" + std::to_string(i), static_cast<GLint>(i));
            colorTextures[i]->Use(static_cast<GLuint>(i));
        }
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST32

#ifdef TEST33

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };
constexpr size_t numPeels { 8 };

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
         0.0f,  0.5f, -0.5f,
         0.0f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.5f,
         0.0f, -0.5f,  0.5f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programFirst("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST32_dp_first.fs");
    ShaderProgram programPeeling("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST32_dp_peeling.fs");
    ShaderProgram programBlend("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST32_dp_blend.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    std::vector<Texture*> depthTextures;
    std::vector<Texture*> colorTextures;
    std::vector<FrameBufferObject*> pass;
    for (size_t i = 0; i < numPeels; ++i)
    {
        colorTextures.emplace_back(new Texture(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT));
        depthTextures.emplace_back(new Texture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
        pass.emplace_back(new FrameBufferObject());

        pass[i]->AddAttachment(GL_COLOR_ATTACHMENT0, *colorTextures[i]);
        pass[i]->AddAttachment(GL_DEPTH_ATTACHMENT, *depthTextures[i]);
    }

    //-----------------------------------------------------------------------------------
    // 将片段按照距离相机的远近分为8份（片段如果超过8个会有误差）
    // 然后按照从距离相机最远到最近混合片段
    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------------------
        // 距离相机最近的片段，直接将通过深度测试的片段信息（颜色、深度）保存即可
        pass[0]->Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programFirst.Use();
        programFirst.SetUniformMat4("model", glm::mat4(1.f));
        programFirst.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programFirst.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        programFirst.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
        rendererRect0.Draw();
        programFirst.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
        rendererRect1.Draw();
        pass[0]->Release();

        //-----------------------------------------------------------------------------------
        // 根据上一次的片段信息，将距离相机比上一次远的片段信息保存
        for (size_t i = 1; i < numPeels; ++i)
        {
            pass[i]->Bind();
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            programPeeling.Use();
            programPeeling.SetUniformMat4("model", glm::mat4(1.f));
            programPeeling.SetUniformMat4("view", initOpenGL.GetViewMatrix());
            programPeeling.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());
            depthTextures[i - 1]->Use();
            programPeeling.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
            rendererRect0.Draw();
            programPeeling.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
            rendererRect1.Draw();
            pass[i]->Release();
        }

        //-----------------------------------------------------------------------------------
        // 将按照距离相机远近排序的片段混合
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        programBlend.Use();
        for (size_t i = 0; i < numPeels; ++i)
        {
            programBlend.SetUniform1i("pass" + std::to_string(i), static_cast<GLint>(i));
            colorTextures[i]->Use(static_cast<GLuint>(i));
        }
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST33

#ifdef TEST34

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };
constexpr size_t numPeels { 8 };

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
         0.0f,  0.5f, -0.5f,
         0.0f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.5f,
         0.0f, -0.5f,  0.5f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programFirst("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST32_dp_first.fs");
    ShaderProgram programPeeling("resources/02_08_02_TEST32_depthpeeling.vs", "resources/02_08_02_TEST34_dp_peeling.fs");
    ShaderProgram programBlend("resources/02_08_02_TEST1_screen.vs", "resources/02_08_02_TEST32_dp_blend.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    std::vector<Texture*> depthTextures;
    std::vector<Texture*> colorTextures;
    std::vector<FrameBufferObject*> pass;
    for (size_t i = 0; i < numPeels; ++i)
    {
        colorTextures.emplace_back(new Texture(width, height, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT));
        depthTextures.emplace_back(new Texture(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
        pass.emplace_back(new FrameBufferObject());

        pass[i]->AddAttachment(GL_COLOR_ATTACHMENT0, *colorTextures[i]);
        pass[i]->AddAttachment(GL_DEPTH_ATTACHMENT, *depthTextures[i]);
    }

    //-----------------------------------------------------------------------------------
    // 从距离相机最远的片段依次从后往前剥离
    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------------------
        pass[0]->Bind();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER); // 比深度缓冲中的值大的片段通过深度测试
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClearDepth(0.); // 将深度缓冲的值初始化为0.0
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programFirst.Use();
        programFirst.SetUniformMat4("model", glm::mat4(1.f));
        programFirst.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programFirst.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        programFirst.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
        rendererRect0.Draw();
        programFirst.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
        rendererRect1.Draw();
        pass[0]->Release();

        //-----------------------------------------------------------------------------------
        for (size_t i = 1; i < numPeels; ++i)
        {
            pass[i]->Bind();
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_GREATER);
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClearDepth(0.);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            programPeeling.Use();
            programPeeling.SetUniformMat4("model", glm::mat4(1.f));
            programPeeling.SetUniformMat4("view", initOpenGL.GetViewMatrix());
            programPeeling.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());
            depthTextures[i - 1]->Use();
            programPeeling.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
            rendererRect0.Draw();
            programPeeling.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
            rendererRect1.Draw();
            pass[i]->Release();
        }

        //-----------------------------------------------------------------------------------
        // 将按照距离相机远近排序的片段混合，注意colorTextures是从远到近
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        programBlend.Use();
        for (int i = numPeels - 1; i >= 0; --i)
        {
            programBlend.SetUniform1i("pass" + std::to_string(i), static_cast<GLint>(i));
            colorTextures[i]->Use(static_cast<GLuint>(i));
        }
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST34

#ifdef TEST35

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
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram quadProgram("resources/02_08_02_TEST35_quad.vs", "resources/02_08_02_TEST35_quad.fs");
    ShaderProgram ddpeelProgram("resources/02_08_02_TEST35_minmax_depth.vs", "resources/02_08_02_TEST35_minmax_depth.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    Texture texColor(width, height, GL_RG32F, GL_RG, GL_FLOAT);
    FrameBufferObject fbo;
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, texColor);

    const float depth_clear[4] = { -1.0, -1.0, -1.0, -1.0 };

    while (!glfwWindowShouldClose(window))
    {
        // 不开启深度测试，或者深度测试设置为GL_ALWAYS(深度测试如果失败，就不会混合)
        // 将帧缓冲的颜色附件初始化为{-1,-1}
        // 开启混合，且混合方式设置为：取两个颜色中的较大者GL_MAX
        // 着色器输出设置为：vec2(-gl_FragCoord.z, gl_FragCoord.z);
        // 这样就能获取指定像素位置所有片段的最大和最小深度值
        fbo.Bind();
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
        glBlendEquation(GL_MAX);
        glClearBufferfv(GL_COLOR, 0, depth_clear);

        ddpeelProgram.Use();
        ddpeelProgram.SetUniformMat4("model", glm::mat4(1.f));
        ddpeelProgram.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        ddpeelProgram.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        rendererRect0.Draw();
        rendererRect1.Draw();
        fbo.Release();

        //--------------------------------------------------------------------------------
        // 可视化最大最小深度值: {Min,Max,0.f,1.f}，红色通道是最小深度值，绿色是最大
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        quadProgram.Use();
        texColor.Use();
        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST35

#ifdef TEST36

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };
constexpr size_t numPeels { 5 }; // 实际剥离层数 = (5-1)*2

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
        -0.5f,  0.5f,  0.1f,
        -0.5f, -0.5f,  0.1f,
         0.5f,  0.5f,  0.1f,
         0.5f, -0.5f,  0.1f,
    };

    std::vector<GLfloat> vertices2{
        -0.5f,  0.5f,  0.2f,
        -0.5f, -0.5f,  0.2f,
         0.5f,  0.5f,  0.2f,
         0.5f, -0.5f,  0.2f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programMinMax("resources/02_08_02_TEST36_minmax_depth.vs", "resources/02_08_02_TEST36_minmax_depth.fs");
    ShaderProgram programDualDp("resources/02_08_02_TEST36_minmax_depth.vs", "resources/02_08_02_TEST36_dualdp.fs");
    ShaderProgram programBlend("resources/02_08_02_TEST35_quad.vs", "resources/02_08_02_TEST36_blend.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect2(vertices2, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    std::vector<Texture*> frontTexs;
    std::vector<Texture*> backTexs;
    std::vector<Texture*> depthTexs;
    std::vector<FrameBufferObject*> ddpFBOs;

    for (size_t i = 0; i < numPeels; ++i)
    {
        frontTexs.emplace_back(new Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT));
        backTexs.emplace_back(new Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT));
        depthTexs.emplace_back(new Texture(width, height, GL_RG32F, GL_RG, GL_FLOAT));
        ddpFBOs.emplace_back(new FrameBufferObject());
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT0, *depthTexs[i]);
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT1, *frontTexs[i]);
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT2, *backTexs[i]);
        ddpFBOs[i]->SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
    }

    const float front_clear[4] = { 0.f, 0.f, 0.f, 0.f };
    const float back_clear[4]  = { 0.f, 0.f, 0.f, 0.f };
    const float depth_clear[4] = { -1.0, -1.0, -1.0, -1.0 };

    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------
        // 首先将距离相机最近和最远片段的深度值获取到
        ddpFBOs[0]->Bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
        glBlendEquation(GL_MAX);
        glClearBufferfv(GL_COLOR, 0, depth_clear);
        glClearBufferfv(GL_COLOR, 1, front_clear);
        glClearBufferfv(GL_COLOR, 2, back_clear);

        programMinMax.Use();
        programMinMax.SetUniformMat4("model", glm::mat4(1.f));
        programMinMax.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programMinMax.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        rendererRect0.Draw();
        rendererRect1.Draw();
        rendererRect2.Draw();
        ddpFBOs[0]->Release();

        //--------------------------------------------------------------------------------
        // 使用上一步获取到的最近和最远深度值，依次向中间夹逼
        for (size_t i = 1; i < numPeels; ++i)
        {
            ddpFBOs[i]->Bind();
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquation(GL_MAX);
            glClearBufferfv(GL_COLOR, 0, depth_clear);
            glClearBufferfv(GL_COLOR, 1, front_clear);
            glClearBufferfv(GL_COLOR, 2, back_clear);

            programDualDp.Use();
            programDualDp.SetUniformMat4("model", glm::mat4(1.f));
            programDualDp.SetUniformMat4("view", initOpenGL.GetViewMatrix());
            programDualDp.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

            depthTexs[i - 1]->Use();

            programDualDp.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
            rendererRect0.Draw();
            programDualDp.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
            rendererRect1.Draw();
            programDualDp.SetUniform4f("uColor", 0.f, 0.f, 1.f, .5f);
            rendererRect2.Draw();
        }

        //--------------------------------------------------------------------------------
        // 使用第二步生成的所有剥离层进行颜色混合
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        programBlend.Use();
        for (size_t i = 1; i < numPeels; ++i)
        {
            programBlend.SetUniform1i("pass" + std::to_string(i - 1), static_cast<GLint>(i - 1));
            backTexs[i]->Use(static_cast<GLuint>(i - 1));

            programBlend.SetUniform1i("pass" + std::to_string(i - 1 + numPeels - 1), static_cast<GLint>(i - 1 + numPeels - 1));
            frontTexs[numPeels - i]->Use(static_cast<GLuint>(i - 1 + numPeels - 1));
        }

        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST36

#ifdef TEST37

#include <common.hpp>

constexpr int width { 800 };
constexpr int height { 600 };
constexpr size_t numPeels { 5 }; // 实际剥离层数 = (5-1)*2

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
         0.0f,  0.5f, -0.5f,
         0.0f, -0.5f, -0.5f,
         0.0f,  0.5f,  0.5f,
         0.0f, -0.5f,  0.5f,
    };
        
    std::vector<GLfloat> quad{
        -1.f, -1.f, 0.f,    0.f, 0.f,
         1.f, -1.f, 0.f,    1.f, 0.f,
        -1.f,  1.f, 0.f,    0.f, 1.f,
         1.f,  1.f, 0.f,    1.f, 1.f,
    };
    // clang-format on

    InitOpenGL initOpenGL(4, 5, Camera({ 0, 0, 3 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = initOpenGL.GetWindow();

    ShaderProgram programMinMax("resources/02_08_02_TEST36_minmax_depth.vs", "resources/02_08_02_TEST36_minmax_depth.fs");
    ShaderProgram programDualDp("resources/02_08_02_TEST36_minmax_depth.vs", "resources/02_08_02_TEST36_dualdp.fs");
    ShaderProgram programBlend("resources/02_08_02_TEST35_quad.vs", "resources/02_08_02_TEST36_blend.fs");

    Renderer rendererRect0(vertices0, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererRect1(vertices1, { 3 }, GL_TRIANGLE_STRIP);
    Renderer rendererQuad(quad, { 3, 2 }, GL_TRIANGLE_STRIP);

    std::vector<Texture*> frontTexs;
    std::vector<Texture*> backTexs;
    std::vector<Texture*> depthTexs;
    std::vector<FrameBufferObject*> ddpFBOs;

    for (size_t i = 0; i < numPeels; ++i)
    {
        frontTexs.emplace_back(new Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT));
        backTexs.emplace_back(new Texture(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT));
        depthTexs.emplace_back(new Texture(width, height, GL_RG32F, GL_RG, GL_FLOAT));
        ddpFBOs.emplace_back(new FrameBufferObject());
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT0, *depthTexs[i]);
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT1, *frontTexs[i]);
        ddpFBOs[i]->AddAttachment(GL_COLOR_ATTACHMENT2, *backTexs[i]);
        ddpFBOs[i]->SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
    }

    const float front_clear[4] = { 0.f, 0.f, 0.f, 0.f };
    const float back_clear[4]  = { 0.f, 0.f, 0.f, 0.f };
    const float depth_clear[4] = { -1.0, -1.0, -1.0, -1.0 };

    while (!glfwWindowShouldClose(window))
    {
        //-----------------------------------------------------------------------
        // 首先将距离相机最近和最远片段的深度值获取到
        ddpFBOs[0]->Bind();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
        glBlendEquation(GL_MAX);
        glClearBufferfv(GL_COLOR, 0, depth_clear);
        glClearBufferfv(GL_COLOR, 1, front_clear);
        glClearBufferfv(GL_COLOR, 2, back_clear);

        programMinMax.Use();
        programMinMax.SetUniformMat4("model", glm::mat4(1.f));
        programMinMax.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        programMinMax.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

        rendererRect0.Draw();
        rendererRect1.Draw();
        ddpFBOs[0]->Release();

        //--------------------------------------------------------------------------------
        // 使用上一步获取到的最近和最远深度值，依次向中间夹逼
        for (size_t i = 1; i < numPeels; ++i)
        {
            ddpFBOs[i]->Bind();
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);
            glBlendEquation(GL_MAX);
            glClearBufferfv(GL_COLOR, 0, depth_clear);
            glClearBufferfv(GL_COLOR, 1, front_clear);
            glClearBufferfv(GL_COLOR, 2, back_clear);

            programDualDp.Use();
            programDualDp.SetUniformMat4("model", glm::mat4(1.f));
            programDualDp.SetUniformMat4("view", initOpenGL.GetViewMatrix());
            programDualDp.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());

            depthTexs[i - 1]->Use();

            programDualDp.SetUniform4f("uColor", 1.f, 0.f, 0.f, .5f);
            rendererRect0.Draw();
            programDualDp.SetUniform4f("uColor", 0.f, 1.f, 0.f, .5f);
            rendererRect1.Draw();
        }

        //--------------------------------------------------------------------------------
        // 使用第二步生成的所有剥离层进行颜色混合
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        programBlend.Use();
        for (size_t i = 1; i < numPeels; ++i)
        {
            programBlend.SetUniform1i("pass" + std::to_string(i - 1), static_cast<GLint>(i - 1));
            backTexs[i]->Use(static_cast<GLuint>(i - 1));

            programBlend.SetUniform1i("pass" + std::to_string(i - 1 + numPeels - 1), static_cast<GLint>(i - 1 + numPeels - 1));
            frontTexs[numPeels - i]->Use(static_cast<GLuint>(i - 1 + numPeels - 1));
        }

        rendererQuad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST37
