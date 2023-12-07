/*
 * 1. 光线步进
 * 2. 3D纹理，光线步进
 */
#define TEST2

#ifdef TEST1

#include "common.hpp"

float windowWidth  = 800.0f;
float windowHeight = 800.0f;

int main()
{
    InitOpenGL initOpenGL("Ray March", (uint32_t)windowWidth, (uint32_t)windowHeight);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_07_TEST1.vert", "shaders/02_08_07_TEST1.frag");

    // clang-format off
    std::vector<float> vertices {
        -1.0f, -1.0f, 
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    // clang-format on

    Renderer quad(vertices, { 2 }, GL_TRIANGLE_STRIP);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniform2f("iResolution", windowWidth, windowHeight);
        program.SetUniform1f("iTime", (GLfloat)glfwGetTime());

        quad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include "common.hpp"
#include <random>

float windowWidth  = 800.0f;
float windowHeight = 800.0f;

int main()
{
    InitOpenGL initOpenGL("Ray March", (uint32_t)windowWidth, (uint32_t)windowHeight);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_07_TEST1.vert", "shaders/02_08_07_TEST2.frag");

    // clang-format off
    std::vector<float> vertices {
        -1.0f, -1.0f, 
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };
    // clang-format on

    Renderer quad(vertices, { 2 }, GL_TRIANGLE_STRIP);

    // 定义3D纹理的尺寸
    const GLsizei textureWidth  = 10;
    const GLsizei textureHeight = 10;
    const GLsizei textureDepth  = 10;

    // 定义纹理数据类型
    uint8_t textureData[textureWidth * textureHeight * textureDepth];

    // 使用伪随机数生成纹理数据
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (int z = 0; z < textureDepth; ++z)
    {
        for (int y = 0; y < textureHeight; ++y)
        {
            for (int x = 0; x < textureWidth; ++x)
            {
                // 生成一个随机的纹素值
                uint8_t value = dis(gen);

                // 在纹理数据数组中设置纹素值
                int index          = z * textureWidth * textureHeight + y * textureWidth + x;
                textureData[index] = value;
            }
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_3D, textureID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, textureWidth, textureHeight, textureDepth, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    glBindTexture(GL_TEXTURE_3D, 0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniform2f("iResolution", windowWidth, windowHeight);
        program.SetUniform1f("iTime", (GLfloat)glfwGetTime());

        glBindTexture(GL_TEXTURE_3D, textureID);
        quad.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2