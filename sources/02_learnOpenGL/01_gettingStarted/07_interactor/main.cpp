/*
 * 1. 鼠标中键按下并拖动，绘制一个四边形
 * 2. 3D图元的拾取
 * 3. 拾取多边形的顶点
 */

#define TEST3

#ifdef TEST1

#include <common.hpp>

static float mouse_x { 0.0 };
static float mouse_y { 0.0 };
static float mouse_x_last { 0.0 };
static float mouse_y_last { 0.0 };

constexpr float window_half_widht { 400.0f };
constexpr float window_half_height { 300.0f };

GLuint quad_vbo { 0 };
bool button_down { false };
bool draw { false };

void UpdateRect(float x0, float y0, float x1, float y1)
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

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    init.SetMouseMoveCallback([](int x, int y) { // 让左下角为(0,0) 右上角为(800,600)
        mouse_x = static_cast<float>(x);
        mouse_y = static_cast<float>(window_half_height * 2 - y);

        // 此时处于按下状态，且光标位置和按下的那一刻不一样
        if (button_down && std::abs(mouse_x - mouse_x_last) > 0.0001 && std::abs(mouse_y - mouse_y_last) > 0.0001)
        {
            UpdateRect(mouse_x_last, mouse_y_last, mouse_x, mouse_y);
            draw = true;
        }
    });
    init.SetMiddleButtonCallback(
        [](int x, int y)
        {
            button_down  = true;
            mouse_x_last = mouse_x;
            mouse_y_last = mouse_y;
        });
    init.SetMiddleButtonUpCallback(
        [](int x, int y)
        {
            button_down = false;
            draw        = false;
        });

    ShaderProgram shader("resources/02_01_07_TEST1.vs", "resources/02_01_07_TEST1.fs");

    GLuint VAO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &quad_vbo);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

    // 只需要创建一次VBO，当矩形数据更新时，同步更新VBO即可，创建时使用 GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (draw)
        {
            shader.Use();
            glBindVertexArray(VAO);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete buffer

    glfwTerminate();
    return 0;
}
#endif // TEST1

#ifdef TEST2

#include <chrono>
#include <common.hpp>
#include <memory>
#include <numbers>
#include <random>

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
    auto THE_PI        = std::numbers::pi_v<float>;

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

    return std::make_unique<Renderer>(vertices, indices, std::initializer_list<unsigned int> { 3 });
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

#ifdef TEST3

#include <common.hpp>
#include <set>

static float mouse_x { 0.0 };
static float mouse_y { 0.0 };
static float mouse_x_last { 0.0 };
static float mouse_y_last { 0.0 };

constexpr float window_half_widht { 400.0f };
constexpr float window_half_height { 300.0f };
constexpr int window_width { 800 };
constexpr int window_height { 600 };

GLuint quad_vbo { 0 }, quad_vao { 0 }, pick_fbo { 0 };
bool button_down { false };
bool draw { false };

Renderer* triangle { nullptr };
Renderer* point { nullptr };
ShaderProgram* program_point { nullptr };
ShaderProgram* program_triangle { nullptr };
InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));

void UpdateRect(float x0, float y0, float x1, float y1)
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

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CreateRenderer()
{
    // clang-format off
    std::vector<GLfloat> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左上

        -0.5f, -0.5f, -.5f,     0.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 0.0f, // 后右下
         0.5f,  0.5f, -.5f,     0.0f, 1.0f, 0.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 1.0f, 0.0f, // 后左上
    };

    std::vector<GLuint> indices{
        0, 1, 3,    1, 2, 3, // 前
        1, 5, 2,    5, 6, 2, // 右
        5, 4, 6,    4, 7, 6, // 后
        4, 0, 7,    0, 3, 7, // 左
        3, 2, 7,    2, 6, 7, // 上
        0, 1, 4,    1, 5, 4, // 下
    };
    // clang-format on

    triangle = new Renderer(vertices, indices, { 3, 3 }, GL_TRIANGLES);
    point    = new Renderer(vertices, { 3, 3 }, GL_POINTS);
}

void CreateQuad()
{
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);

    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

    // 只需要创建一次VBO，当矩形数据更新时，同步更新VBO即可，创建时使用 GL_DYNAMIC_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);
}

void DrawPick(float x, float y, float x2, float y2)
{
    //------------------------------------------------
    // 以GL_POINTS的方式绘制
    glBindFramebuffer(GL_FRAMEBUFFER, pick_fbo);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program_triangle->Use();
    program_triangle->SetUniformMat4("view", init.GetViewMatrix());
    program_triangle->SetUniformMat4("projection", init.GetProjectionMatrix());
    program_triangle->SetUniformMat4("model", glm::mat4(1.f));
    triangle->Draw();

    // 清除颜色缓冲
    glClear(GL_COLOR_BUFFER_BIT);

    glDepthMask(GL_FALSE);
    program_point->Use();
    program_point->SetUniformMat4("view", init.GetViewMatrix());
    program_point->SetUniformMat4("projection", init.GetProjectionMatrix());
    program_point->SetUniformMat4("model", glm::mat4(1.f));
    program_point->SetUniform1f("cOffset", -2.f); // 深度偏移
    point->Draw();

    glDepthMask(GL_TRUE); // 不要将DepthMask在glClear之后才设置为true

    //---------------------------------------------
    // 读取颜色缓冲
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    auto xx  = static_cast<int>(x);
    auto yy  = static_cast<int>(y);
    auto xx2 = static_cast<int>(x2);
    auto yy2 = static_cast<int>(y2);

    auto x_s = std::min(xx, xx2);
    auto y_s = std::min(yy, yy2);
    auto w   = std::abs(xx2 - xx);
    auto h   = std::abs(yy2 - yy);

    std::vector<unsigned char> pixel(static_cast<size_t>(w * h * 4));
    glReadPixels(x_s, y_s, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixel.data());

    std::set<unsigned int> ids;
    for (size_t i = 0; i < w * h * 4; i += 4)
    {
        unsigned char tmpPixel[4] { pixel[i], pixel[i + 1], pixel[i + 2], pixel[i + 3] };
        unsigned int index = 0;
        index |= tmpPixel[0];
        index |= tmpPixel[1] << 8;
        index |= tmpPixel[2] << 16;

        if (index > 0)
        {
            ids.emplace(index - 1);
        }
    }
    std::cout << "pick " << ids.size() << " ids:\n";
    for (auto id : ids)
    {
        std::cout << id << ' ';
    }
    std::cout << std::endl;
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main()
{
    auto window = init.GetWindow();
    init.SetMiddleButtonCallback(
        [](int x, int y)
        {
            button_down  = true;
            mouse_x_last = mouse_x;
            mouse_y_last = mouse_y;
        });
    init.SetMiddleButtonUpCallback(
        [](int x, int y)
        {
            button_down = false;
            draw        = false;
            DrawPick(mouse_x_last, mouse_y_last, mouse_x, mouse_y);
        });
    init.SetMouseMoveCallback(
        [](int x, int y)
        {
            mouse_x = static_cast<float>(x);
            mouse_y = static_cast<float>(window_half_height * 2 - y);

            // 此时处于按下状态，且光标位置和按下的那一刻不一样
            if (button_down && std::abs(mouse_x - mouse_x_last) > 0.1 && std::abs(mouse_y - mouse_y_last) > 0.1)
            {
                UpdateRect(mouse_x_last, mouse_y_last, mouse_x, mouse_y);
                draw = true;
            }
        });

    ShaderProgram program_quad("resources/02_01_07_TEST1.vs", "resources/02_01_07_TEST1.fs");

    program_triangle = new ShaderProgram("resources/02_01_07_TEST3.vs", "resources/02_01_07_TEST3.fs");
    program_point    = new ShaderProgram("resources/02_01_07_TEST3_point.vs", "resources/02_01_07_TEST3_point.fs");

    FrameBufferObject fbo;
    Texture tex(window_width, window_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    RenderBufferObject rbo(GL_DEPTH_COMPONENT24, window_width, window_height);
    fbo.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, tex);
    pick_fbo = fbo.GetHandle();

    CreateQuad();
    CreateRenderer();

    glEnable(GL_DEPTH_TEST);
    // glPointSize(5.f);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (draw)
        {
            program_quad.Use();
            glBindVertexArray(quad_vao);
            glDrawArrays(GL_LINE_LOOP, 0, 4);
            glBindVertexArray(0);
        }

        program_triangle->Use();
        program_triangle->SetUniformMat4("view", init.GetViewMatrix());
        program_triangle->SetUniformMat4("projection", init.GetProjectionMatrix());
        program_triangle->SetUniformMat4("model", glm::mat4(1.f));
        triangle->Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffer

    glfwTerminate();
    return 0;
}
#endif // TEST3
