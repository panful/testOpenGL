/*
 * 1. 法线贴图(normal mapping)、凹凸贴图(bump mapping)
 * 2. 
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

int main()
{
    bool blinn { false };
    bool normal { false };

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&blinn, &normal](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                blinn = true;
                break;
            case 'g':
            case 'G':
                blinn = false;
                break;
            case 'n':
            case 'N':
                normal = true;
                break;
            case 'h':
            case 'H':
                normal = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram program("resources/02_05_04_TEST1.vs", "resources/02_05_04_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices{
        // positions        // normals      // texcoords
        -1.f, -1.f, 0.f,    0.f,0.f,1.f,    0.f,0.f,
         1.f, -1.f, 0.f,    0.f,0.f,0.f,    1.f,0.f,
        -1.f,  1.f, 0.f,    0.f,0.f,1.f,    0.f,1.f,
         1.f,  1.f, 0.f,    0.f,0.f,1.f,    1.f,1.f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });

    Texture diffuseMap("resources/brickwall.jpg", 0);
    Texture normalMap("resources/brickwall_normal.jpg", 1);

    //----------------------------------------------------------------------------------
    auto lightPos      = glm::vec3(0.f, 0.f, 5.f);
    auto lightColor    = glm::vec3(1.f);
    auto lightAmbient  = .2f * lightColor;
    auto lightDiffuse  = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

    //----------------------------------------------------------------------------------
    // 贴图的rgb对应法线的xyz，贴图蓝色表示法线为(0,0,1),贴图绿色法线为(0,1,0),贴图红色法线为(1,0,0)
    // 使用法线贴图时需要注意图元在世界空间的朝向以及贴图的朝向是否一致
    // 图元的正面和背面光照效果是不一样的，因为背面的法线是相反方向的

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        program.SetUniform3fv("viewPos", initOpenGL.GetViewPosition());
        program.SetUniform3fv("light.ambient", lightAmbient);
        program.SetUniform3fv("light.diffuse", lightDiffuse);
        program.SetUniform3fv("light.specular", lightSpecular);
        program.SetUniform3fv("light.position", lightPos);

        program.SetUniform1i("blinnSwitch", blinn);
        program.SetUniform1i("normalSwitch", normal);

        std::cout << "blinn: " << blinn << "\tnormal: " << normal << '\n';

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);

        diffuseMap.Bind();
        normalMap.Bind();
        renderer.Draw(GL_TRIANGLE_STRIP);
        diffuseMap.Release();
        normalMap.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1
