/*
 * 1. 直接辐照度光照（直接设置环境光颜色）
 * 2. 间接辐照度光照（将环境光照设置为一个立方体贴图，每个像素都是一个光源）IBL 基于图像的光照(Image based lighting)
 */

#define TEST2

#ifdef TEST1

#include <common.hpp>
#include <numbers>

Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = 1.f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
            // 法线
            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3, 3 });
}

Renderer CreateCube()
{
    std::vector<float> vertices {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    return Renderer(vertices, { 3, 3, 2 });
}

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 50.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    auto sphere = CreateSphere();
    auto cube   = CreateCube();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // 渲染被光照的物体
    ShaderProgram programPBR("shaders/02_06_02_TEST1_pbr.vs", "shaders/02_06_02_TEST1_pbr.fs");
    // 背景，通过将深度值设置为1，让立方体贴图始终在物体的后面
    ShaderProgram programBackground("shaders/02_06_02_TEST1_background.vs", "shaders/02_06_02_TEST1_background.fs");
    // 将HDR图片文件转换为立方体贴图
    ShaderProgram programCubeMap("shaders/02_06_02_TEST1_cubemap.vs", "shaders/02_06_02_TEST1_cubemap.fs");

    programPBR.Use();
    programPBR.SetUniform3f("albedo", 0.5f, 0.0f, 0.0f);
    programPBR.SetUniform1f("ao", 1.0f);

    programBackground.Use();
    programBackground.SetUniform1i("environmentMap", 0);

    // 光源的位置和颜色
    // clang-format off
    glm::vec3 lightPositions[] = {
        glm::vec3(-5.0f,  5.0f, 10.0f),
        glm::vec3( 5.0f,  5.0f, 10.0f),
        glm::vec3(-5.0f, -5.0f, 10.0f),
        glm::vec3( 5.0f, -5.0f, 10.0f),
    };

    glm::vec3 lightColors[] = { 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
    };
    // clang-format on

    int nrRows      = 5;
    int nrColumns   = 5;
    float spacing   = 3.0f;
    glm::mat4 model = glm::mat4(1.f);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[]    = { glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) };

    //---------------------------------------------------------------------------
    // 将背景（立方体贴图）绘制到FBO上
    GLuint fboWidth { 512 }, fboHeight { 512 };
    FrameBufferObject fboCapture;
    RenderBufferObject rboCapture(GL_DEPTH_COMPONENT24, fboWidth, fboHeight);
    Texture textureCubeMap(fboWidth, fboHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 0, true);
    fboCapture.AddAttachment(GL_DEPTH_ATTACHMENT, rboCapture);

    // 创建HDR图片纹理
    Texture textureHdr(std::string_view("textures/newport_loft.hdr"), 0, false, GL_RGB16F, GL_RGB, GL_FLOAT);

    glViewport(0, 0, fboWidth, fboHeight);
    fboCapture.Bind();
    programCubeMap.Use();
    programCubeMap.SetUniform1i("equirectangularMap", 0);
    programCubeMap.SetUniformMat4("projection", captureProjection);
    textureHdr.Use(0);

    for (uint32_t i = 0; i < 6; ++i)
    {
        programCubeMap.SetUniformMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureCubeMap.Get(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.Draw(GL_TRIANGLES);
    }

    //---------------------------------------------------------------------------
    // 设置默认帧缓冲的投影矩阵
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    programPBR.Use();
    programPBR.SetUniformMat4("projection", init.GetProjectionMatrix());
    programBackground.Use();
    programBackground.SetUniformMat4("projection", init.GetProjectionMatrix());

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programPBR.Use();
        programPBR.SetUniform3fv("camPos", init.GetViewPosition());
        programPBR.SetUniformMat4("view", init.GetViewMatrix());

        // 球体，nrRows*nrColumns个球体
        for (int row = 0; row < nrRows; ++row)
        {
            programPBR.SetUniform1f("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                programPBR.SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                auto x = (col - (nrColumns / 2)) * spacing;
                auto y = (row - (nrRows / 2)) * spacing;

                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(x, y, 0.0f));
                programPBR.SetUniformMat4("model", model);
                programPBR.SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

                sphere.Draw(GL_TRIANGLES);
            }
        }

        // 光源，设置四个光源的位置和颜色
        for (size_t i = 0; i < 4; ++i)
        {
            programPBR.SetUniform3fv("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            programPBR.SetUniform3fv("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        // 背景，使用FBO的附件cubeMap
        programBackground.Use();
        programBackground.SetUniformMat4("view", init.GetViewMatrix());
        textureCubeMap.Use(0);
        cube.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>
#include <numbers>

Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = 1.f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
            // 法线
            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3, 3 });
}

Renderer CreateCube()
{
    std::vector<float> vertices {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };

    return Renderer(vertices, { 3, 3, 2 });
}

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 50.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    auto sphere = CreateSphere();
    auto cube   = CreateCube();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    ShaderProgram programPBR("shaders/02_06_02_TEST1_pbr.vs", "shaders/02_06_02_TEST2_pbr.fs");
    ShaderProgram programBackground("shaders/02_06_02_TEST1_background.vs", "shaders/02_06_02_TEST1_background.fs");
    ShaderProgram programCubeMap("shaders/02_06_02_TEST1_cubemap.vs", "shaders/02_06_02_TEST1_cubemap.fs");
    ShaderProgram programIrradiance("shaders/02_06_02_TEST1_cubemap.vs", "shaders/02_06_02_TEST2_IBL.fs");

    programPBR.Use();
    programPBR.SetUniform1i("irradianceMap", 0);
    programPBR.SetUniform3f("albedo", 0.5f, 0.0f, 0.0f);
    programPBR.SetUniform1f("ao", 1.0f);

    programBackground.Use();
    programBackground.SetUniform1i("environmentMap", 0);

    // 光源的位置和颜色
    // clang-format off
    glm::vec3 lightPositions[] = {
        glm::vec3(-5.0f,  5.0f, 10.0f),
        glm::vec3( 5.0f,  5.0f, 10.0f),
        glm::vec3(-5.0f, -5.0f, 10.0f),
        glm::vec3( 5.0f, -5.0f, 10.0f),
    };

    glm::vec3 lightColors[] = { 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f), 
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
    };
    // clang-format on

    int nrRows      = 5;
    int nrColumns   = 5;
    float spacing   = 3.0f;
    glm::mat4 model = glm::mat4(1.f);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[]    = { glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
           glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)) };

    //---------------------------------------------------------------------------
    // 将背景（立方体贴图）绘制到FBO上
    GLuint fboWidth { 512 }, fboHeight { 512 };
    FrameBufferObject fboCapture;
    RenderBufferObject rboCapture(GL_DEPTH_COMPONENT24, fboWidth, fboHeight);
    Texture textureCubeMap(fboWidth, fboHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 0, true);
    fboCapture.AddAttachment(GL_DEPTH_ATTACHMENT, rboCapture);

    Texture textureHdr(std::string_view("textures/newport_loft.hdr"), 0, false, GL_RGB16F, GL_RGB, GL_FLOAT);

    glViewport(0, 0, fboWidth, fboHeight);
    fboCapture.Bind();
    programCubeMap.Use();
    programCubeMap.SetUniform1i("equirectangularMap", 0);
    programCubeMap.SetUniformMat4("projection", captureProjection);
    textureHdr.Use(0);

    for (uint32_t i = 0; i < 6; ++i)
    {
        programCubeMap.SetUniformMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureCubeMap.Get(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.Draw(GL_TRIANGLES);
    }

    //---------------------------------------------------------------------------
    // 对上一步的立方体贴图做一次IBL生成一个新的立方体贴图用于环境光照，还是在fboCapture中绘制
    GLuint irrWidth { 32 }, irrHeight { 32 };
    Texture textureIrradianceMap(irrWidth, irrHeight, GL_RGB16F, GL_RGB, GL_FLOAT, 0, true);

    programIrradiance.Use();
    programIrradiance.SetUniform1i("environmentMap", 0);
    programIrradiance.SetUniformMat4("projection", captureProjection);
    textureCubeMap.Use(0);

    glViewport(0, 0, irrWidth, irrHeight);
    for (unsigned int i = 0; i < 6; ++i)
    {
        programIrradiance.SetUniformMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, textureIrradianceMap.Get(), 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cube.Draw(GL_TRIANGLES);
    }

    //---------------------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int scrWidth, scrHeight;
    glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
    glViewport(0, 0, scrWidth, scrHeight);

    programPBR.Use();
    programPBR.SetUniformMat4("projection", init.GetProjectionMatrix());
    programBackground.Use();
    programBackground.SetUniformMat4("projection", init.GetProjectionMatrix());

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programPBR.Use();
        programPBR.SetUniform3fv("camPos", init.GetViewPosition());
        programPBR.SetUniformMat4("view", init.GetViewMatrix());
        textureIrradianceMap.Use(0);

        // 球体
        for (int row = 0; row < nrRows; ++row)
        {
            programPBR.SetUniform1f("metallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col)
            {
                programPBR.SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                auto x = (col - (nrColumns / 2)) * spacing;
                auto y = (row - (nrRows / 2)) * spacing;

                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(x, y, 0.0f));
                programPBR.SetUniformMat4("model", model);
                programPBR.SetUniformMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

                sphere.Draw(GL_TRIANGLES);
            }
        }

        // 光源
        for (size_t i = 0; i < 4; ++i)
        {
            programPBR.SetUniform3fv("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            programPBR.SetUniform3fv("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        // 背景，立方体贴图
        programBackground.Use();
        programBackground.SetUniformMat4("view", init.GetViewMatrix());
        textureCubeMap.Use(0);
        // textureIrradianceMap.Use(0);
        cube.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2
