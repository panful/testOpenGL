/**
 * 1. 光线步进
 * 2. 3D纹理，光线步进
 * 3. 光线步进绘制体积云（立方体的云）
 * 4. 通过perlin噪声生成云朵
 */

#define TEST4

#ifdef TEST1

#include <common.hpp>

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

#include <common.hpp>
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

#ifdef TEST3

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL({ { 0, 0, 10 }, { 0, 1, 0 }, { 0, 0, 0 } });
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_08_07_TEST3.vert", "shaders/02_08_07_TEST3.frag");

    // clang-format off
    std::vector<GLfloat> vertices{
        -1.f, -1.f,  1.f,    // 前左下
         1.f, -1.f,  1.f,    // 前右下
         1.f,  1.f,  1.f,    // 前右上
        -1.f,  1.f,  1.f,    // 前左上

        -1.f, -1.f, -1.f,    // 后左下
         1.f, -1.f, -1.f,    // 后右下
         1.f,  1.f, -1.f,    // 后右上
        -1.f,  1.f, -1.f,    // 后左上
    };

    std::vector<GLuint> indices{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    // 体积云所在的立方体（该立方体将体积云全部包裹在里边）
    Renderer cloud(vertices, indices, { 3 }, GL_TRIANGLES);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());
        program.SetUniform3fv("uCameraPos", initOpenGL.GetViewPosition());

        // 体积云的范围，就是立方体的范围
        program.SetUniform2f("uCloudX", -1.f, 1.f);
        program.SetUniform2f("uCloudY", -1.f, 1.f);
        program.SetUniform2f("uCloudZ", -1.f, 1.f);

        cloud.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

int main()
{
    InitOpenGL initOpenGL({ { 0, 0, 10 }, { 0, 1, 0 }, { 0, 0, 0 } });
    auto window = initOpenGL.GetWindow();

    //---------------------------------------------------------------------------------
    // 初始化Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    ShaderProgram program("shaders/02_08_07_TEST3.vert", "shaders/02_08_07_TEST4.frag");

    // clang-format off
    std::vector<GLfloat> vertices{
        -1.f, -1.f,  1.f,    // 前左下
         1.f, -1.f,  1.f,    // 前右下
         1.f,  1.f,  1.f,    // 前右上
        -1.f,  1.f,  1.f,    // 前左上

        -1.f, -1.f, -1.f,    // 后左下
         1.f, -1.f, -1.f,    // 后右下
         1.f,  1.f, -1.f,    // 后右上
        -1.f,  1.f, -1.f,    // 后左上
    };

    std::vector<GLuint> indices{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    // 体积云所在的立方体（该立方体将体积云全部包裹在里边）
    Renderer cloud(vertices, indices, { 3 }, GL_TRIANGLES);

    // 纹理最好使用边缘镜像的，这样边缘的云朵就不会很突兀
    Texture perlinNosize("textures/perlinNosize.jpg");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    float uCoordWeight { 0.0025f };
    float uThreshold { 0.4f };
    float uSampling { 1.4472f };
    float uSampling1 { 3.5f };
    float uSampling2 { 12.25f };
    float uSampling3 { 42.87f };
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("proj", initOpenGL.GetProjectionMatrix());
        program.SetUniform3fv("uCameraPos", initOpenGL.GetViewPosition());

        // 体积云的范围，就是立方体的范围
        program.SetUniform2f("uCloudX", -1.f, 1.f);
        program.SetUniform2f("uCloudY", -1.f, 1.f);
        program.SetUniform2f("uCloudZ", -1.f, 1.f);

        program.SetUniform1f("uCoordWeight", uCoordWeight);
        program.SetUniform1f("uThreshold", uThreshold);
        program.SetUniform1f("uSampling", uSampling);
        program.SetUniform1f("uSampling1", uSampling1);
        program.SetUniform1f("uSampling2", uSampling2);
        program.SetUniform1f("uSampling3", uSampling3);

        perlinNosize.Use();
        cloud.Draw();

        //---------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("FlowMap");
            ImGui::SliderFloat("coord weight", &uCoordWeight, 0.f, .01f);
            ImGui::SliderFloat("threshold", &uThreshold, 0.f, 1.f);
            ImGui::SliderFloat("sampling", &uSampling, 0.f, 5.f);
            ImGui::SliderFloat("sampling 1", &uSampling1, 0.f, 10.f);
            ImGui::SliderFloat("sampling 2", &uSampling2, 0.f, 50.f);
            ImGui::SliderFloat("sampling 3", &uSampling3, 0.f, 100.f);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4