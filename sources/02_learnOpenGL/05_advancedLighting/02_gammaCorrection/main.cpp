/*
 * 1. 使用OpenGL自带的gamma correction
 * 2. 在片段着色器中自定义gamma correction
 * 3. sRGB纹理
 * 4. 在开启gamma校正时使用光照衰减
 */

#define TEST4

#ifdef TEST1

#include <common.hpp>

int main()
{
    bool blinn { false };
    bool gamma { false };

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&blinn, &gamma](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                blinn = true;
                break;
            case 'n':
            case 'N':
                blinn = false;
                break;
            case 'g':
            case 'G':
                gamma = true;
                break;
            case 'h':
            case 'H':
                gamma = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram program("resources/02_05_02_TEST1.vs", "resources/02_05_02_TEST1.fs");

    // clang-format off
    std::vector<GLfloat>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });
    Texture texture("resources/wood.png");

    //----------------------------------------------------------------------------------
    auto lightPos      = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor    = glm::vec3(1.f);
    auto lightAmbient  = .2f * lightColor;
    auto lightDiffuse  = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

    while (!glfwWindowShouldClose(window))
    {
        if (gamma)
        {
            // 开启OpenGL自带的gamma校正
            glEnable(GL_FRAMEBUFFER_SRGB);
        }
        else
        {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }

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

        program.SetUniform1i("blinn", blinn);

        texture.Bind();
        renderer.Draw(GL_TRIANGLES);
        texture.Release();

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
    bool blinn { false };
    bool gamma { false };

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&blinn, &gamma](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                blinn = true;
                break;
            case 'n':
            case 'N':
                blinn = false;
                break;
            case 'g':
            case 'G':
                gamma = true;
                break;
            case 'h':
            case 'H':
                gamma = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram program("resources/02_05_02_TEST1.vs", "resources/02_05_02_TEST2.fs");

    // clang-format off
    std::vector<GLfloat>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });
    Texture texture("resources/wood.png");

    //----------------------------------------------------------------------------------
    auto lightPos      = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor    = glm::vec3(1.f);
    auto lightAmbient  = .2f * lightColor;
    auto lightDiffuse  = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

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

        program.SetUniform1i("blinn", blinn);
        program.SetUniform1i("gamma", gamma);

        texture.Bind();
        renderer.Draw(GL_TRIANGLES);
        texture.Release();

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
    bool blinn { false };

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&blinn](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                blinn = true;
                break;
            case 'n':
            case 'N':
                blinn = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram program("resources/02_05_02_TEST1.vs", "resources/02_05_02_TEST1.fs");

    // clang-format off
    std::vector<GLfloat>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });

    int width { 0 }, height { 0 }, channels { 0 };
    auto imageData = stbi_load("resources/wood.png", &width, &height, &channels, 0);
    std::clog << width << '\t' << height << '\t' << channels << '\n';

    GLuint texture { 0 };
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 将纹理的内部格式设置为 GL_SRGB OpenGL会自动把纹理颜色校正到线性空间
    // 纹理如果带有alpha，则需要将纹理的内部格式设置为 GL_SRGB_ALPHA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    stbi_image_free(imageData);

    //----------------------------------------------------------------------------------
    auto lightPos      = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor    = glm::vec3(1.f);
    auto lightAmbient  = .2f * lightColor;
    auto lightDiffuse  = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

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

        program.SetUniform1i("blinn", blinn);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        renderer.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

int main()
{
    bool blinn { false };
    bool gamma { false };
    bool attenuation { false };

    InitOpenGL initOpenGL(Camera({ 0.f, 0.f, 3.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetKeyCallback(
        [&blinn, &gamma, &attenuation](char c)
        {
            switch (c)
            {
            case 'b':
            case 'B':
                blinn = true;
                break;
            case 'n':
            case 'N':
                blinn = false;
                break;
            case 'g':
            case 'G':
                gamma = true;
                break;
            case 'h':
            case 'H':
                gamma = false;
                break;
            case 't':
            case 'T':
                attenuation = true;
                break;
            case 'y':
            case 'Y':
                attenuation = false;
                break;
            default:
                break;
            }
        });

    ShaderProgram program("resources/02_05_02_TEST1.vs", "resources/02_05_02_TEST4.fs");

    // clang-format off
    std::vector<GLfloat>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    Renderer renderer(vertices, { 3, 3, 2 });
    Texture texture("resources/wood.png");

    //----------------------------------------------------------------------------------
    auto lightPos      = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor    = glm::vec3(1.f);
    auto lightAmbient  = .2f * lightColor;
    auto lightDiffuse  = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

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

        program.SetUniform1i("blinn", blinn);
        program.SetUniform1i("gamma", gamma);
        program.SetUniform1i("attenuation", attenuation);

        std::cout << "blinn: " << blinn << "\tgamma: " << gamma << "\tattenuation: " << attenuation << '\n';

        texture.Bind();
        renderer.Draw(GL_TRIANGLES);
        texture.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4