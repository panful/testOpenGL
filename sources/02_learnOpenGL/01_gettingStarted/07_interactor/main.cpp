/*
 * 1. 鼠标左键按下绘制一个四边形
 * 2. 3D图元的拾取
 */

#define TEST2

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

#ifdef TEST2

#include <chrono>
#include <common.hpp>
#include <numbers>
#include <random>
#include <memory>

InitOpenGL init(Camera({ 0, 0, 25 }, { 0, 1, 0 }, { 0, 0, 0 }));

ShaderProgram shader("resources/02_01_07_TEST2.vs", "resources/02_01_07_TEST2.fs");

// 用来拾取的FBO
GLuint pick_fbo { 0 };

// 窗口的大小
constexpr int window_width { 800 };
constexpr int window_height { 600 };

// 绘制5*5=25个球
std::vector<std::tuple<std::unique_ptr<Renderer>, glm::mat4, glm::vec3>> primitives;
constexpr int nRow      = 5;
constexpr int nCol      = 5;
constexpr float spacing = 3.f;

// 用来访问primitives的std::tuple
enum
{
    N_Primitive = 0, // Renderer
    N_Model     = 1, // 模型矩阵
    N_Color     = 2  // 颜色RGB
};

void Draw(bool pick = false)
{
    if (pick)
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }
    else
    {
        glClearColor(.1f, .2f, .3f, 1.f);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    shader.SetUniformMat4("view", init.GetViewMatrix());
    shader.SetUniformMat4("projection", init.GetProjectionMatrix());

    for (uint32_t i = 0; i < primitives.size(); ++i)
    {
        shader.SetUniformMat4("model", std::get<N_Model>(primitives.at(i)));
        if (pick)
        {
            // 0xAABBGGRR
            // 背景色是(0,0,0,1)，所以图元id从1开始
            auto r = (i + 1) & 0xFF;
            auto g = (i + 1) >> 8 & 0xFF;
            auto b = (i + 1) >> 16 & 0xFF;
            auto a = (i + 1) >> 24 & 0xFF;

            // 在独立的帧缓冲中将所有的图元再绘制一次，每个图元的颜色设置为图元的ID
            shader.SetUniform3f("uColor", r / 255.f, g / 255.f, b / 255.f);
        }
        else
        {
            shader.SetUniform3fv("uColor", std::get<N_Color>(primitives.at(i)));
        }
        std::get<N_Primitive>(primitives.at(i))->Draw(GL_TRIANGLES);
    }
}

std::unique_ptr<Renderer> CreateSphere()
{
    uint32_t longitude = 32;
    uint32_t latitude  = 32;
    float radius       = 1.0f;
    auto THE_PI          = std::numbers::pi_v<float>;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (uint32_t lat = 0; lat <= latitude; ++lat)
    {
        float theta    = lat * THE_PI / latitude;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (uint32_t lon = 0; lon <= longitude; ++lon)
        {
            float phi    = lon * 2 * THE_PI / longitude;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Vertex position
            float x = radius * cosPhi * sinTheta;
            float y = radius * cosTheta;
            float z = radius * sinPhi * sinTheta;

            // Add vertex attributes to the vector
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Generate indices
    for (uint32_t lat = 0; lat < latitude; ++lat)
    {
        for (uint32_t lon = 0; lon < longitude; ++lon)
        {
            uint32_t curr = lat * (longitude + 1) + lon;
            uint32_t next = curr + longitude + 1;

            // Add indices for the two triangles forming each quad
            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(curr + 1);
        }
    }

    return std::make_unique<Renderer>(vertices, indices, std::initializer_list<unsigned int>{ 3 });
}

int main()
{
    auto window = init.GetWindow();
    init.SetMiddleButtonCallback(
        [](int x, int y)
        {
            if (0 != pick_fbo)
            {
                // 将左下角设置为(0, 0)
                int p_x = x;
                int p_y = window_height - y;

                std::cout << "Pick: (" << p_x << ",\t" << p_y << ")\t";

                // 使用glReadPixels读取光标所在位置的像素，并将它转换为ID
                // ID=0时，拾取的是背景色
                glBindFramebuffer(GL_FRAMEBUFFER, pick_fbo);
                Draw(true);
                glReadBuffer(GL_COLOR_ATTACHMENT0);

                unsigned char pixel[4] { 0 };
                glReadPixels(p_x, p_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

                // 将RGB转换为unsigned int类型的ID，此处不使用alpha
                unsigned int index = 0;
                index |= pixel[0];
                index |= pixel[1] << 8;
                index |= pixel[2] << 16;

                std::cout << "ID: " << index << '\n';

                glReadBuffer(GL_NONE);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        });

    // 使用随机数生成颜色
    std::default_random_engine randomEngine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<float> randomColor(0.f, 1.f);

    for (int row = 0; row < nRow; ++row)
    {
        for (int col = 0; col < nCol; ++col)
        {
            auto x = row - (nRow / 2);
            auto y = col - (nCol / 2);

            auto prim  = CreateSphere();
            auto model = glm::translate(glm::mat4(1.f), glm::vec3(spacing * x, spacing * y, 0.f));
            auto color = glm::vec3(randomColor(randomEngine), randomColor(randomEngine), randomColor(randomEngine));

            primitives.emplace_back(std::forward_as_tuple(std::move(prim), model, color));
        }
    }

    FrameBufferObject fbo;
    Texture tex(window_width, window_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    RenderBufferObject rbo(GL_DEPTH_COMPONENT24, window_width, window_height);
    fbo.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, tex);
    pick_fbo = fbo.GetHandle();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        Draw(false);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2