/*
 * 1. 视差贴图(Parallax Mapping)属于位移贴图(Displacement Mapping)的一种
 * 2. 陡峭视差映射(Steep Parallax Mapping)
 * 3. 视差遮蔽映射(Parallax Occlusion Mapping)
 */

#define TEST3

#ifdef TEST1

#include <common.hpp>

int main()
{
    auto lightPos = glm::vec3(0.f, 5.f, 0.f);
    InitOpenGL initOpenGL(Camera({ 0.f, 5.f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_05_05_TEST1.vs", "shaders/02_05_05_TEST1.fs");

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
    Texture diffuseMap("shaders/bricks.jpg", 0);       // 纹理贴图
    Texture normalMap("shaders/bricks_normal.jpg", 1); // 法线贴图
    Texture heightMap("shaders/bricks_disp.jpg", 2);   // 高度贴图

    //----------------------------------------------------------------------------------
    // 视差贴图：对顶点进行位移或偏移，就能将平坦的平面变换为凹凸不平的表面
    // 但是顶点太多计算量太大，所以采用高度纹理让一个片段的表面看起来比实际更高或更低

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

        program.SetUniform1f("height_scale", 0.1f);

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);
        program.SetUniform1i("depthMap", 2);

        diffuseMap.Bind();
        normalMap.Bind();
        heightMap.Bind();
        renderer.Draw(GL_TRIANGLES);
        diffuseMap.Release();
        normalMap.Release();
        heightMap.Release();

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
    ShaderProgram program("shaders/02_05_05_TEST1.vs", "shaders/02_05_05_TEST2.fs");

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
    Texture diffuseMap("shaders/bricks.jpg", 0);       // 纹理贴图
    Texture normalMap("shaders/bricks_normal.jpg", 1); // 法线贴图
    Texture heightMap("shaders/bricks_disp.jpg", 2);   // 高度贴图

    //----------------------------------------------------------------------------------
    // 将深度范围划分为多个层，从每个层中沿着viewDir(眼睛看向片段的方向)移动采样纹理
    // 直到找到一个采样低于当前层的深度值

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

        program.SetUniform1f("height_scale", 0.1f);

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);
        program.SetUniform1i("depthMap", 2);

        diffuseMap.Bind();
        normalMap.Bind();
        heightMap.Bind();
        renderer.Draw(GL_TRIANGLES);
        diffuseMap.Release();
        normalMap.Release();
        heightMap.Release();

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
    auto lightPos = glm::vec3(0.f, 5.f, 0.f);
    InitOpenGL initOpenGL(Camera({ 0.f, 5.f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("shaders/02_05_05_TEST1.vs", "shaders/02_05_05_TEST3.fs");

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
    Texture diffuseMap("shaders/bricks.jpg", 0);       // 纹理贴图
    Texture normalMap("shaders/bricks_normal.jpg", 1); // 法线贴图
    Texture heightMap("shaders/bricks_disp.jpg", 2);   // 高度贴图

    //----------------------------------------------------------------------------------
    // 视差遮蔽映射(Parallax Occlusion Mapping)和陡峭视差映射的原理相同，
    // 但不是用触碰的第一个深度层的纹理坐标，而是在触碰之前和之后，在深度层之间进行线性插值。

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

        program.SetUniform1f("height_scale", 0.1f);

        program.SetUniform1i("diffuseMap", 0);
        program.SetUniform1i("normalMap", 1);
        program.SetUniform1i("depthMap", 2);

        diffuseMap.Bind();
        normalMap.Bind();
        heightMap.Bind();
        renderer.Draw(GL_TRIANGLES);
        diffuseMap.Release();
        normalMap.Release();
        heightMap.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3