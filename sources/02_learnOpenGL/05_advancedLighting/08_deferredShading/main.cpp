/*
 * 1. 可视化GBuffer中的位置向量、法线向量、漫反射向量
 * 2. 延迟着色
 * 3. 正向着色
 * 4. 延迟渲染的背景色光照问题
 */

#define TEST4

#ifdef TEST1

#include <common.hpp>

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
    char visualType { 'p' };
    InitOpenGL init("GBuffer", windowWidth, windowHeight, Camera({ 0.f, 0.f, 20.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();
    init.SetKeyCallback(
        [&visualType](char c)
        {
            switch (c)
            {
            case 'p':
            case 'P':
            case 'n':
            case 'N':
            case 'a':
            case 'A':
                visualType = c;
                break;
            default:
                break;
            }
        });

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

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(verticesQuad, { 3, 2 });

    Texture woodTex("resources/wood.png");

    //---------------------------------------------------------------------------------------
    // 三个纹理分别保存位置向量、法向量、颜色和镜面反光度
    Texture gPosition(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT);
    Texture gNormal(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT);
    Texture gAlbedoSpec(windowWidth, windowHeight, GL_RGBA, GL_RGBA, GL_FLOAT);

    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fboQuad;
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT0, gPosition);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT1, gNormal);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT2, gAlbedoSpec);
    fboQuad.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fboQuad.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
    if (!fboQuad.Check())
    {
        std::cerr << "Frame buffer object error\n";
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        fboQuad.Bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        programGBuffer.Use();
        programGBuffer.SetUniformMat4("view", init.GetViewMatrix());
        programGBuffer.SetUniformMat4("projection", init.GetProjectionMatrix());
        programGBuffer.SetUniform1f("specular", 32.f);
        woodTex.Bind();
        DrawCube(cube, programGBuffer);
        woodTex.Release();
        glDisable(GL_DEPTH_TEST);
        fboQuad.Release();

        glClearColor(1.0, 1.0, 1.0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        programDeferred.Use();

        switch (visualType)
        {
        case 'p':
        case 'P':
            gPosition.Bind();
            break;
        case 'n':
        case 'N':
            gNormal.Bind();
            break;
        case 'a':
        case 'A':
            gAlbedoSpec.Bind();
            break;
        default:
            break;
        }

        quad.Draw(GL_TRIANGLE_STRIP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

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
    ShaderProgram programDeferred("resources/02_05_08_TEST1_deferred.vs", "resources/02_05_08_TEST2_deferred.fs");

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(verticesQuad, { 3, 2 });

    Texture woodTex("resources/wood.png");

    //---------------------------------------------------------------------------------------
    // 三个纹理分别保存位置向量、法向量、颜色和镜面反光度
    Texture gPosition(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 0);
    Texture gNormal(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 1);
    Texture gAlbedoSpec(windowWidth, windowHeight, GL_RGBA, GL_RGBA, GL_FLOAT, 2);

    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fboQuad;
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT0, gPosition);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT1, gNormal);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT2, gAlbedoSpec);
    fboQuad.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fboQuad.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
    if (!fboQuad.Check())
    {
        std::cerr << "Frame buffer object error\n";
        return -1;
    }

    // 八个光源
    std::vector<std::pair<glm::vec3, glm::vec3>> lights {
        { glm::vec3(-5.f, -5.f, -5.f), glm::vec3(1.f, 0.f, 0.f) },
        { glm::vec3(5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 0.f) },
        { glm::vec3(-5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 1.f, 0.f) },
        { glm::vec3(-5.f, -5.f, 5.f), glm::vec3(0.f, 0.f, 1.f) },
        { glm::vec3(-5.f, 5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
    };

    //---------------------------------------------------------------------------------------
    // 背景也会被光照

    while (!glfwWindowShouldClose(window))
    {
        fboQuad.Bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        programGBuffer.Use();
        programGBuffer.SetUniformMat4("view", init.GetViewMatrix());
        programGBuffer.SetUniformMat4("projection", init.GetProjectionMatrix());
        programGBuffer.SetUniform1f("specular", 2.f);
        woodTex.Bind();
        DrawCube(cube, programGBuffer);
        woodTex.Release();
        glDisable(GL_DEPTH_TEST);
        fboQuad.Release();

        glClearColor(1.0, 1.0, 1.0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        programDeferred.Use();
        programDeferred.SetUniform1i("gPosition", 0);
        programDeferred.SetUniform1i("gNormal", 1);
        programDeferred.SetUniform1i("gAlbedoSpec", 2);
        programDeferred.SetUniform3fv("viewPos", init.GetViewPosition());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            programDeferred.SetUniform3fv("lights[" + std::to_string(i) + "].Position", lights.at(i).first);
            programDeferred.SetUniform3fv("lights[" + std::to_string(i) + "].Color", lights.at(i).second);
            programDeferred.SetUniform1f("lights[" + std::to_string(i) + "].Linear", 0.01f);
            programDeferred.SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", 0.1f);
        }
        gPosition.Bind();
        gNormal.Bind();
        gAlbedoSpec.Bind();
        quad.Draw(GL_TRIANGLE_STRIP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

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
    InitOpenGL init("Forward Rendering", windowWidth, windowHeight, Camera({ 0.f, 0.f, 20.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
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

    ShaderProgram programGBuffer("resources/02_05_08_TEST3_forward.vs", "resources/02_05_08_TEST3_forward.fs");

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(verticesQuad, { 3, 2 });

    Texture woodTex("resources/wood.png");

    // 八个光源
    std::vector<std::pair<glm::vec3, glm::vec3>> lights {
        { glm::vec3(-5.f, -5.f, -5.f), glm::vec3(1.f, 0.f, 0.f) },
        { glm::vec3(5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 0.f) },
        { glm::vec3(-5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 1.f, 0.f) },
        { glm::vec3(-5.f, -5.f, 5.f), glm::vec3(0.f, 0.f, 1.f) },
        { glm::vec3(-5.f, 5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
    };

    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        programGBuffer.Use();
        programGBuffer.SetUniformMat4("view", init.GetViewMatrix());
        programGBuffer.SetUniformMat4("projection", init.GetProjectionMatrix());
        programGBuffer.SetUniform3fv("viewPos", init.GetViewPosition());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            programGBuffer.SetUniform3fv("lights[" + std::to_string(i) + "].Position", lights.at(i).first);
            programGBuffer.SetUniform3fv("lights[" + std::to_string(i) + "].Color", lights.at(i).second);
            programGBuffer.SetUniform1f("lights[" + std::to_string(i) + "].Linear", 0.01f);
            programGBuffer.SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", 0.1f);
        }
        programGBuffer.SetUniform1f("specular", 2.f);

        woodTex.Bind();
        DrawCube(cube, programGBuffer);
        woodTex.Release();
        glDisable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

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

    ShaderProgram programGBuffer("resources/02_05_08_TEST1_gbuffer.vs", "resources/02_05_08_TEST4_gbuffer.fs");
    ShaderProgram programDeferred("resources/02_05_08_TEST1_deferred.vs", "resources/02_05_08_TEST4_deferred.fs");

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer quad(verticesQuad, { 3, 2 });

    Texture woodTex("resources/wood.png");

    //---------------------------------------------------------------------------------------
    // 三个纹理分别保存位置向量、法向量、颜色和镜面反光度
    Texture gPosition(windowWidth, windowHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, 0);
    Texture gNormal(windowWidth, windowHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 1);
    Texture gAlbedoSpec(windowWidth, windowHeight, GL_RGBA, GL_RGBA, GL_FLOAT, 2);

    RenderBufferObject rbo(GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    FrameBufferObject fboQuad;
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT0, gPosition);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT1, gNormal);
    fboQuad.AddAttachment(GL_COLOR_ATTACHMENT2, gAlbedoSpec);
    fboQuad.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fboQuad.SetDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 });
    if (!fboQuad.Check())
    {
        std::cerr << "Frame buffer object error\n";
        return -1;
    }

    // 八个光源
    std::vector<std::pair<glm::vec3, glm::vec3>> lights {
        { glm::vec3(-5.f, -5.f, -5.f), glm::vec3(1.f, 0.f, 0.f) },
        { glm::vec3(5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 0.f) },
        { glm::vec3(-5.f, 5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, -5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 1.f, 0.f) },
        { glm::vec3(-5.f, -5.f, 5.f), glm::vec3(0.f, 0.f, 1.f) },
        { glm::vec3(-5.f, 5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
        { glm::vec3(5.f, -5.f, 5.f), glm::vec3(1.f, 1.f, 1.f) },
    };

    //---------------------------------------------------------------------------------------
    // 在gbuffer的片段着色器中将当前片段gPosition的w分量设置为0.0，因为背景色是1.0
    // 在延迟渲染时，如果获取到纹理的a分量为1.0，则该片段是背景不需要被光照

    while (!glfwWindowShouldClose(window))
    {
        fboQuad.Bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        programGBuffer.Use();
        programGBuffer.SetUniformMat4("view", init.GetViewMatrix());
        programGBuffer.SetUniformMat4("projection", init.GetProjectionMatrix());
        programGBuffer.SetUniform1f("specular", 2.f);
        woodTex.Bind();
        DrawCube(cube, programGBuffer);
        woodTex.Release();
        glDisable(GL_DEPTH_TEST);
        fboQuad.Release();

        glClearColor(1.0, 1.0, 1.0, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        programDeferred.Use();
        programDeferred.SetUniform1i("gPosition", 0);
        programDeferred.SetUniform1i("gNormal", 1);
        programDeferred.SetUniform1i("gAlbedoSpec", 2);
        programDeferred.SetUniform3fv("viewPos", init.GetViewPosition());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            programDeferred.SetUniform3fv("lights[" + std::to_string(i) + "].Position", lights.at(i).first);
            programDeferred.SetUniform3fv("lights[" + std::to_string(i) + "].Color", lights.at(i).second);
            programDeferred.SetUniform1f("lights[" + std::to_string(i) + "].Linear", 0.01f);
            programDeferred.SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", 0.1f);
        }
        gPosition.Bind();
        gNormal.Bind();
        gAlbedoSpec.Bind();
        quad.Draw(GL_TRIANGLE_STRIP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4