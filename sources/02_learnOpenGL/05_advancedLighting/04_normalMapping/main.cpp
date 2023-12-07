/*
 * 1. 法线贴图(normal mapping)、凹凸贴图(bump mapping)
 * 2. 切线空间
 * 3. 不使用切线空间的错误演示，和TEST2对比
 */

#define TEST2

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

    ShaderProgram program("shaders/02_05_04_TEST1.vs", "shaders/02_05_04_TEST1.fs");

    // clang-format off
    std::vector<GLfloat> vertices{
        // positions        // normals      // texcoords
        -1.f, -1.f, 0.f,    0.f,0.f,1.f,    0.f,0.f,
         1.f, -1.f, 0.f,    0.f,0.f,1.f,    1.f,0.f,
        -1.f,  1.f, 0.f,    0.f,0.f,1.f,    0.f,1.f,
         1.f,  1.f, 0.f,    0.f,0.f,1.f,    1.f,1.f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });

    Texture diffuseMap("shaders/brickwall.jpg", 0);
    Texture normalMap("shaders/brickwall_normal.jpg", 1);

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

#ifdef TEST2

#include <common.hpp>

int main()
{
    auto lightPos = glm::vec3(0.f, 5.f, 0.f);
    InitOpenGL initOpenGL(Camera({ 0.f, 5.f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_05_04_TEST2.vs", "shaders/02_05_04_TEST2.fs");

    // clang-format off
    glm::vec3 pos1(-1.0f,  0.0f, -1.0f);
    glm::vec3 pos2(-1.0f,  0.0f,  1.0f);
    glm::vec3 pos3( 1.0f,  0.0f,  1.0f);
    glm::vec3 pos4( 1.0f,  0.0f, -1.0f);

    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);  
    glm::vec2 uv4(1.0f, 1.0f);

    glm::vec3 nm(0.f, 1.f, 0.f);

    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;

    // triangle 1
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    std::vector<GLfloat> vertices{
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2, 3, 3 });
    Texture diffuseMap("shaders/brickwall.jpg", 0);
    Texture normalMap("shaders/brickwall_normal.jpg", 1);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        program.SetUniform3fv("viewPos", initOpenGL.GetViewPosition());
        program.SetUniform3fv("lightPos", lightPos);

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);

        diffuseMap.Bind();
        normalMap.Bind();
        renderer.Draw(GL_TRIANGLES);
        diffuseMap.Release();
        normalMap.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

// 绘制的面朝向+z还是朝向+y
// 如果绘制的面不是朝向+z并且不使用切线空间，渲染结果就会出错
#define PLANE_FACING_Z

int main()
{
#ifdef PLANE_FACING_Z
    auto lightPos = glm::vec3(0.f, 0.f, 5.f);
    InitOpenGL initOpenGL(Camera({ lightPos.x, lightPos.y, lightPos.z }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
#else
    auto lightPos = glm::vec3(0.f, 5.f, 0.f);
    InitOpenGL initOpenGL(Camera({ lightPos.x, lightPos.y, lightPos.z }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, 0.f }));
#endif

    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_05_04_TEST3.vs", "shaders/02_05_04_TEST3.fs");

    // clang-format off
#ifdef PLANE_FACING_Z
    glm::vec3 pos1(-1.0f,  1.0f,  0.0f);
    glm::vec3 pos2(-1.0f, -1.0f,  0.0f);
    glm::vec3 pos3( 1.0f, -1.0f,  0.0f);
    glm::vec3 pos4( 1.0f,  1.0f,  0.0f);

    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);  
    glm::vec2 uv4(1.0f, 1.0f);

    glm::vec3 nm(0.f, 1.f, 0.f);
#else
    glm::vec3 pos1(-1.0f,  0.0f, -1.0f);
    glm::vec3 pos2(-1.0f,  0.0f,  1.0f);
    glm::vec3 pos3( 1.0f,  0.0f,  1.0f);
    glm::vec3 pos4( 1.0f,  0.0f, -1.0f);

    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);  
    glm::vec2 uv4(1.0f, 1.0f);

    glm::vec3 nm(0.f, 1.f, 0.f);
#endif

    std::vector<GLfloat> vertices{
        // positions                // normals              // texcoords
        pos1.x, pos1.y, pos1.z,     nm.x, nm.y, nm.z,       uv1.x, uv1.y,
        pos2.x, pos2.y, pos2.z,     nm.x, nm.y, nm.z,       uv2.x, uv2.y,
        pos3.x, pos3.y, pos3.z,     nm.x, nm.y, nm.z,       uv3.x, uv3.y,

        pos1.x, pos1.y, pos1.z,     nm.x, nm.y, nm.z,       uv1.x, uv1.y,
        pos3.x, pos3.y, pos3.z,     nm.x, nm.y, nm.z,       uv3.x, uv3.y,
        pos4.x, pos4.y, pos4.z,     nm.x, nm.y, nm.z,       uv4.x, uv4.y,
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });

    Texture diffuseMap("shaders/brickwall.jpg", 0);
    Texture normalMap("shaders/brickwall_normal.jpg", 1);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", initOpenGL.GetViewMatrix());
        program.SetUniformMat4("projection", initOpenGL.GetProjectionMatrix());
        program.SetUniform3fv("viewPos", initOpenGL.GetViewPosition());
        program.SetUniform3fv("lightPos", lightPos);

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);

        diffuseMap.Bind();
        normalMap.Bind();
        renderer.Draw(GL_TRIANGLES);
        diffuseMap.Release();
        normalMap.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3
