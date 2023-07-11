
#define TEST1

#ifdef TEST1

#include "common.hpp"

float windowWidth  = 800.0f;
float windowHeight = 800.0f;

int main()
{
    InitOpenGL initOpenGL("Ray March", (uint32_t)windowWidth, (uint32_t)windowHeight);
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_08_07_TEST1.vert", "resources/02_08_07_TEST1.frag");

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
