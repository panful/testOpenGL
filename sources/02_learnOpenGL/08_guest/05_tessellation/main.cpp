/*
 * 1. 基于CPU的曲面细分
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 0.f, 30.f, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }));
    auto window = opengl.GetWindow();
    opengl.SetKeyCallback(
        [](char key)
        {
            // 使用'L'和'F'切换显示模式
            if ('l' == key || 'L' == key)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else if ('f' == key || 'F' == key)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        });

    ShaderProgram shader("resources/02_08_05_TEST1.vs", "resources/02_08_05_TEST1.fs");

    stbi_set_flip_vertically_on_load(true);
    int width { 0 }, height { 0 }, nrChannels { 0 };
    unsigned char* data = stbi_load("resources/iceland_heightmap.png", &width, &height, &nrChannels, 0);
    // unsigned char* data = stbi_load("resources/river_heightmap.png", &width, &height, &nrChannels, 0);
    // unsigned char* data = stbi_load("resources/river2_heightmap.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        std::cout << "Image size: " << height << ", " << width << ", " << nrChannels << '\n';
    }
    else
    {
        std::cout << "Failed to load texture\n";
        return -1;
    }

    // 将图片的颜色值（灰度图）设置为网格坐标的y分量
    // 网格的长宽(xz)分别为图像的长宽
    float yScale { 64.f / 256.f }, yShift { 16.f };
    uint32_t bytePerPixel = static_cast<uint32_t>(nrChannels);
    std::vector<float> vertices;
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            // 灰度图 rgb三个分量都是相同的值
            uint8_t y = data[(w + width * h) * bytePerPixel];

            vertices.emplace_back(-height / 2.f + (float)h);
            vertices.emplace_back((int)y * yScale - yShift);
            vertices.emplace_back(-width / 2.f + (float)w);
        }
    }

    stbi_image_free(data);

    // C   D   F   H
    // -------------
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // -------------
    // A   B   E   G
    // 从AB到CD为一个三角形带，BE到DF为一个三角形带，总共有numStrips个三角形带
    // 每个三角形带共有numTrisPerStrip个顶点，索引顺序为ABCD，使用GL_TRIANGLE_STRIP模式绘制
    int rez { 1 };
    std::vector<uint32_t> indices;
    for (int i = 0; i < height - 1; i += rez)
    {
        for (int j = 0; j < width; j += rez)
        {
            for (int k = 0; k < 2; k++)
            {
                indices.emplace_back(static_cast<uint32_t>(j + width * (i + k * rez)));
            }
        }
    }

    std::cout << "vertices size:\t" << vertices.size() << "\nindices size:\t" << indices.size() << '\n';

    const int numStrips       = (height - 1) / rez;
    const int numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Number of strips:\t" << numStrips << '\n';
    std::cout << "Number of triangles on each strip:\t" << numTrisPerStrip << '\n';
    std::cout << "Number of total triangles:\t" << numStrips * numTrisPerStrip << '\n';

    //------------------------------------------------------------------------------
    unsigned int terrainVAO, terrainVBO, terrainIBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    //------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        shader.SetUniformMat4("projection", opengl.GetProjectionMatrix());
        shader.SetUniformMat4("view", opengl.GetViewMatrix());
        shader.SetUniformMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.01f)));

        glBindVertexArray(terrainVAO);

        for (int strip = 0; strip < numStrips; strip++)
        {
            // 第一个参数为模式，第一个参数此次绘制多少个顶点
            // 第三个参数为数据类型，第四个参数为此次数据的起始位置
            glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * (numTrisPerStrip + 2) * strip));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1