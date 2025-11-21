/*
 * 1. 鼠标中键按下并拖动，绘制一个四边形
 * 2. 3D图元的拾取
 * 3. 拾取多边形的顶点
 * 4. 屏幕坐标 世界坐标 互相转换，绘制拾取的顶点，使用 Camera
 * 5. 将屏幕坐标转换为世界坐标，绘制拾取的顶点，使用 Camera2
 * 6. 将屏幕坐标转换为世界坐标，绘制鼠标中键选择的框
 */

#define TEST6

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

    ShaderProgram shader("shaders/02_01_07_TEST1.vs", "shaders/02_01_07_TEST1.fs");

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

ShaderProgram shader("shaders/02_01_07_TEST2.vs", "shaders/02_01_07_TEST2.fs");

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
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
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

    ShaderProgram program_quad("shaders/02_01_07_TEST1.vs", "shaders/02_01_07_TEST1.fs");

    program_triangle = new ShaderProgram("shaders/02_01_07_TEST3.vs", "shaders/02_01_07_TEST3.fs");
    program_point    = new ShaderProgram("shaders/02_01_07_TEST3_point.vs", "shaders/02_01_07_TEST3_point.fs");

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

#ifdef TEST4

#include <common.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

constexpr float ww { 800.f };
constexpr float wh { 600.f };

int main()
{
    static InitOpenGL initOpenGL(Camera({ 0, 0, 3.f }, { 0, 1, 0 }, { 0, 0, 0 }));
    initOpenGL.SetClipRange({ 2.f, 4.f });
    initOpenGL.SetParallel(true);
    auto window = initOpenGL.GetWindow();

    static std::vector<float> pickPos;

    initOpenGL.SetMiddleButtonCallback(
        [](int x, int y)
        {
            // z使用-1，就是投影矩阵的近裁剪平面
            auto NDCPos = glm::vec4((x * 2 - ww) / ww, (y * 2 - wh) / wh, -1.f, 1.f);
            auto inView = glm::inverse(initOpenGL.GetViewMatrix());
            auto inProj = glm::inverse(initOpenGL.GetProjectionMatrix());

            auto projPos  = inProj * NDCPos;
            auto worldPos = inView * projPos;

            auto wx = worldPos.x / worldPos.w;
            auto wy = worldPos.y / worldPos.w;
            auto wz = worldPos.z / worldPos.w;

            std::cout << worldPos.w << '\t' << wz << '\n';
            pickPos.insert(pickPos.end(), { wx, wy, wz, 0.f, 0.f, 1.f });
        });

    ShaderProgram program("shaders/02_01_07_TEST3.vs", "shaders/02_01_07_TEST3.fs");

    //---------------------------------------------------------------------------------
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    // clang-format off
    // 8个顶点
    std::vector<GLfloat> verticesCube{
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
    // 6个面，12个三角形
    std::vector<GLuint> indicesCube{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };

    std::vector<float> verticesPoint{ -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, }; // 前左下
    // clang-format on

    Renderer cube(verticesCube, indicesCube, { 3, 3 }, GL_TRIANGLES);
    Renderer* line { nullptr };

    // 程序启动后，按下鼠标中键拾取几个点，然后点击IMGUI的按钮，将拾取的点连成一条线，鼠标右键旋转即可看到拾取的点
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto model = glm::mat4(1.f);
        auto view  = initOpenGL.GetViewMatrix();
        auto proj  = initOpenGL.GetProjectionMatrix();

        program.Use();
        program.SetUniformMat4("model", model);
        program.SetUniformMat4("view", view);
        program.SetUniformMat4("projection", proj);
        cube.Draw();

        // 世界坐标转换为屏幕坐标
        auto ndcPoint      = proj * view * model * glm::vec4(verticesPoint[0], verticesPoint[1], verticesPoint[2], 1.f);
        auto viewportPoint = glm::vec2((ndcPoint.x / ndcPoint.w + 1.f) / 2.f, (ndcPoint.y / ndcPoint.w + 1.f) / 2.f);
        auto displayPoint  = glm::vec2(ww, wh) * viewportPoint;

        //---------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            ImGui::Text(("display point: " + std::to_string(displayPoint.x) + ", " + std::to_string(displayPoint.y)).c_str());
            if (ImGui::Button("Test Button"))
            {
                if (line == nullptr)
                {
                    line = new Renderer(pickPos, { 3, 3 }, GL_LINE_LOOP);
                }
                else
                {
                    pickPos.clear();
                    delete line;
                    line = nullptr;
                }
            }
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (line != nullptr)
        {
            program.Use();
            line->Draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <common2.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

static constexpr int win_w { 800 };
static constexpr int win_h { 600 };

static std::vector<float> picked_pos {};

int main()
{
    Window window("Test Window", win_w, win_h);
    window.interactor.camera.type = Camera2::Type::Perspective;

    window.interactor.middleButtonPressCallback = [&window](const glm::vec2& pos)
    {
        float x = pos.x;
        float y = pos.y + win_h;                                    // 转换为窗口左下角为原点

        auto w = window.interactor.DisplayToWorld(glm::vec2(x, y)); // NDC 的 z 值是 0

        std::cout << "Mouse pos: " << x << ", " << y << std::endl;
        std::cout << "World pos: " << w.x << ", " << w.y << ", " << w.z << std::endl;
        picked_pos.insert(picked_pos.cend(), { w.x, w.y, w.z, 0.f, 0.f, 1.f });
    };

    ShaderProgram program("shaders/02_01_07_TEST3.vs", "shaders/02_01_07_TEST3.fs");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    // clang-format off
    std::vector<GLfloat> verticesCube{
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

    std::vector<GLuint> indicesCube{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    Renderer drawable(vertices, indices, { 3, 3 }, GL_TRIANGLES);
    Renderer* picked_line_drawable {};

    auto aabb = AABBTool::ComputeAABB(vertices, 6);
    window.interactor.camera.Reset(aabb);

    bool draw_cube = true;

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (picked_pos.size() > 1 && picked_line_drawable != nullptr)
        {
            auto line_aabb = AABBTool::ComputeAABB(picked_pos, 6);
            aabb           = AABBTool::MergeAABB({ line_aabb, aabb });
        }

        window.interactor.camera.ResetClipRange(aabb);

        auto proj = window.interactor.camera.projMat;
        auto view = window.interactor.camera.viewMat;

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", view);
        program.SetUniformMat4("projection", proj);

        if (draw_cube)
            drawable.Draw();
        if (picked_line_drawable)
            picked_line_drawable->Draw();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            if (ImGui::Button("Generator line"))
            {
                if (picked_pos.size() > 1 && picked_line_drawable == nullptr)
                {
                    picked_line_drawable = new Renderer(picked_pos, { 3, 3 }, GL_LINE_LOOP);
                }
            }

            if (ImGui::Button("Change cube visible"))
            {
                draw_cube = !draw_cube;
            }
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
}

#endif // TEST5

#ifdef TEST6

#include <common2.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

static constexpr int win_w { 800 };
static constexpr int win_h { 600 };

static glm::vec3 press_pos {};
static glm::vec3 release_pos {};
static bool middle_release { false };

std::vector<float> GeneratorCube()
{
    glm::vec3 min { std::min(press_pos.x, release_pos.x), std::min(press_pos.y, release_pos.y), std::min(press_pos.z, release_pos.z) };
    glm::vec3 max { std::max(press_pos.x, release_pos.x), std::max(press_pos.y, release_pos.y), std::max(press_pos.z, release_pos.z) };

    std::vector<float> cube {};
    cube.insert(cube.cend(), { min.x, min.y, min.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { max.x, min.y, min.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { max.x, max.y, min.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { min.x, max.y, min.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { min.x, min.y, max.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { max.x, min.y, max.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { max.x, max.y, max.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { min.x, max.y, max.z, 0.f, 1.f, 0.f });
    return cube;
}

std::vector<float> GeneratorCube2(const Camera2& camera)
{
    glm::vec3 dir   = glm::normalize((camera.focalPos - camera.eyePos));
    glm::vec3 up    = glm::normalize(camera.viewUp);
    glm::vec3 right = glm::cross(dir, up);

    float vDis = glm::dot((release_pos - press_pos), -up);
    float hDis = glm::dot((release_pos - press_pos), right);
    float zN   = camera.clipRange[0];
    float zF   = camera.clipRange[1];

    auto p1 = press_pos - up * vDis;
    auto p2 = release_pos;
    auto p3 = release_pos + up * vDis;
    auto p4 = press_pos;

    auto p5 = press_pos - up * vDis + dir * zF;
    auto p6 = release_pos + dir * zF;
    auto p7 = release_pos + up * vDis + dir * zF;
    auto p8 = press_pos + dir * zF;

    std::vector<float> cube {};
    cube.insert(cube.cend(), { p1.x, p1.y, p1.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { p2.x, p2.y, p2.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { p3.x, p3.y, p3.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { p4.x, p4.y, p4.z, 1.f, 0.f, 0.f });
    cube.insert(cube.cend(), { p5.x, p5.y, p5.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { p6.x, p6.y, p6.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { p7.x, p7.y, p7.z, 0.f, 1.f, 0.f });
    cube.insert(cube.cend(), { p8.x, p8.y, p8.z, 0.f, 1.f, 0.f });
    return cube;
}

int main()
{
    Window window("Test Window", win_w, win_h);
    window.interactor.camera.type = Camera2::Type::Perspective;

    window.interactor.middleButtonPressCallback = [&window](const glm::vec2& pos)
    {
        float x = pos.x;
        float y = pos.y + win_h;                                    // 转换为窗口左下角为原点

        auto w = window.interactor.DisplayToWorld(glm::vec2(x, y)); // NDC 的 z 值是 0

        std::cout << "Mouse pos: " << x << ", " << y << std::endl;
        std::cout << "World pos: " << w.x << ", " << w.y << ", " << w.z << std::endl;
        press_pos = w;
    };

    window.interactor.middleButtonReleaseCallback = [&window](const glm::vec2& pos)
    {
        float x = pos.x;
        float y = pos.y + win_h;

        auto w = window.interactor.DisplayToWorld(glm::vec2(x, y));

        std::cout << "Mouse pos: " << x << ", " << y << std::endl;
        std::cout << "World pos: " << w.x << ", " << w.y << ", " << w.z << std::endl;
        release_pos = w;

        middle_release = true;
    };

    ShaderProgram program("shaders/02_01_07_TEST3.vs", "shaders/02_01_07_TEST3.fs");

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    // clang-format off
    std::vector<GLfloat> verticesCube{
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

    std::vector<GLuint> indicesCube{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    Renderer drawable(vertices, indices, { 3, 3 }, GL_TRIANGLES);
    Renderer* picked_cube_drawable {};

    auto aabb = AABBTool::ComputeAABB(vertices, 6);
    window.interactor.camera.Reset(aabb);

    bool draw_cube     = true;
    bool draw_pick_box = true;

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window.window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.interactor.camera.ResetClipRange(aabb);

        auto proj = window.interactor.camera.projMat;
        auto view = window.interactor.camera.viewMat;

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", view);
        program.SetUniformMat4("projection", proj);

        if (draw_cube)
            drawable.Draw();
        if (picked_cube_drawable)
            picked_cube_drawable->Draw();

        if (middle_release)
        {
            if (picked_cube_drawable)
                delete picked_cube_drawable;

            std::vector<float> cube_vertices = draw_pick_box ? GeneratorCube() : GeneratorCub2(window.interactor.camera);

            picked_cube_drawable = new Renderer(cube_vertices, indices, { 3, 3 }, GL_TRIANGLES);

            auto cube_aabb = AABBTool::ComputeAABB(cube_vertices, 6);
            aabb           = AABBTool::MergeAABB({ cube_aabb, aabb });

            middle_release = false;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin("Camera");
            if (ImGui::Button("Change cube visible"))
            {
                draw_cube = !draw_cube;
            }
            if (ImGui::Button("Switch draw type"))
            {
                draw_pick_box = !draw_pick_box;
            }
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.window);
    }
}

#endif // TEST6
