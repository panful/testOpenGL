/*
 * 1. 鼠标左键按下绘制一个四边形
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

static float mouse_x { 0.0 };
static float mouse_y { 0.0 };
static float mouse_x_last { 0.0 };
static float mouse_y_last { 0.0 };

constexpr float window_half_widht { 400.0f };
constexpr float window_half_height { 300.0f };

GLuint quad_vao { 0 };
bool button_down { false };

GLuint CreateGrid(float x0, float y0, float x1, float y1)
{
    auto v_x0 = (x0 - window_half_widht) / window_half_widht;
    auto v_y0 = (y0 - window_half_height) / window_half_height;
    auto v_x1 = (x1 - window_half_widht) / window_half_widht;
    auto v_y1 = (y1 - window_half_height) / window_half_height;

    // clang-format off
    float vertices[] {
        v_x0, v_y0,
        v_x1, v_y0,
        v_x1, v_y1,
        v_x0, v_y1,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    return VAO;
}

void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    switch (action)
    {
    case GLFW_PRESS:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            std::cout << "press\t" << mouse_x << '\t' << mouse_y << '\n';
            button_down  = true;
            mouse_x_last = mouse_x;
            mouse_y_last = mouse_y;
        }
        break;
        }
        break;
    case GLFW_RELEASE:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            std::cout << "release\t" << mouse_x << '\t' << mouse_y << '\n';
            button_down = false;
            if (0 != quad_vao && glIsVertexArray(quad_vao))
            {
                glDeleteVertexArrays(1, &quad_vao);
                quad_vao = 0;
            }
        }
        break;
        }
        break;
    }
}

void cursorPosCB(GLFWwindow* window, double xpos, double ypos)
{
    // 让左下角为(0,0) 右上角为(800,600)
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(window_half_height * 2 - ypos);

    if (0 != quad_vao && glIsVertexArray(quad_vao))
    {
        glDeleteVertexArrays(1, &quad_vao);
        quad_vao = 0;
    }

    // 此时左键处于按下状态，且光标位置和按下的那一刻不一样
    if (button_down && std::abs(mouse_x - mouse_x_last) > 0.0001 && std::abs(mouse_y - mouse_y_last) > 0.0001 && quad_vao == 0)
    {
        quad_vao = CreateGrid(mouse_x_last, mouse_y_last, mouse_x, mouse_y);
    }
}

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    init.SetCursorPosCB(cursorPosCB);
    init.SetMouseCB(mouseCB);
    auto window = init.GetWindow();

    ShaderProgram shader("resources/02_01_07_TEST1.vs", "resources/02_01_07_TEST1.fs");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (0 != quad_vao)
        {
            shader.Use();
            glBindVertexArray(quad_vao);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
#endif // TEST1
