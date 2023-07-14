/*
 * 1. 创建一个计算着色器，获取计算着色器支持的工作组数量
 * 2. 使用计算着色器生成一张纹理并绘制到屏幕
 * 3. glsl中imageBuffer的使用，将顶点经过计算着色器修改后输出到Buffer
 * 4. GL_UNIFORM_BUFFER 的使用，粒子模拟器
 * 5. GL_SHADER_STORAGE_BUFFER 的使用
 */

#define TEST5

#ifdef TEST1

#include <common.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Compute Shader", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    int max_compute_work_group_count[3] { 0 };
    int max_compute_work_group_size[3] { 0 };
    int max_compute_work_group_invocations { 0 };

    for (int idx = 0; idx < 3; idx++)
    {
        // 本地工作组的数量
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, idx, &max_compute_work_group_count[idx]);
        // 本地工作组的大小
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, idx, &max_compute_work_group_size[idx]);
    }
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);

    std::cout << "OpenGL Limitations: " << std::endl;
    std::cout << "maximum number of work groups in X dimension\t" << max_compute_work_group_count[0] << std::endl;
    std::cout << "maximum number of work groups in Y dimension\t" << max_compute_work_group_count[1] << std::endl;
    std::cout << "maximum number of work groups in Z dimension\t" << max_compute_work_group_count[2] << std::endl;

    std::cout << "maximum size of a work group in X dimension\t" << max_compute_work_group_size[0] << std::endl;
    std::cout << "maximum size of a work group in Y dimension\t" << max_compute_work_group_size[1] << std::endl;
    std::cout << "maximum size of a work group in Z dimension\t" << max_compute_work_group_size[2] << std::endl;

    std::cout << "Number of invocations in a single local work group that may be dispatched to a compute shader "
              << max_compute_work_group_invocations << std::endl;

    //--------------------------------------------------------------------------------------------
    auto comp                = ReadFile("resources/02_08_06_TEST1.comp");
    auto computeShaderSource = comp.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // compute shader
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
    glCompileShader(computeShader);
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
        std::cerr << "Compute shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, computeShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, computeShader);
    glDeleteShader(computeShader);

    //--------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

int main()
{
    InitOpenGL opengl(4, 3);
    auto window = opengl.GetWindow();

    //--------------------------------------------------------------------------------------------
    ComputeShader computeShader("resources/02_08_06_TEST2.comp");
    ShaderProgram program("resources/02_08_06_TEST2.vert", "resources/02_08_06_TEST2.frag");

    //--------------------------------------------------------------------------------------------
    // clang-format off
    std::vector<float> vertices{
        -1.f, -1.f,     0.f, 0.f,
         1.f, -1.f,     1.f, 0.f,
        -1.f,  1.f,     0.f, 1.f,
         1.f,  1.f,     1.f, 1.f,
    };
    // clang-format on

    Renderer quad(vertices, { 2, 2 }, GL_TRIANGLE_STRIP);

    //--------------------------------------------------------------------------------------------
    int texWidth { 1000 }, texHeight { 1000 };
    uint32_t texture { 0 };
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, nullptr);

    // 将纹理绑定到计算着色器，用来保存计算着色器的输出
    // 这个函数和着色器中image2D或image1D、image3D对应，不能是sampler2D等
    // 第一个参数和着色器中的layout(rgba32f, binding = 0)的第二个参数binding对应
    glBindImageTexture(2, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    //--------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        computeShader.Use();
        computeShader.SetUniform1f("t", (float)glfwGetTime());
        glDispatchCompute((unsigned int)texWidth / 10, (unsigned int)texHeight / 10, 1);
        // 保证计算着色器已经将纹理的每一个像素填充完成
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //--------------------------------------------------------------------------------------------
        // 使用计算着色器的输出纹理
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        glBindTexture(GL_TEXTURE_2D, texture);
        quad.Draw();
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>
#include <thread>

int main()
{
    InitOpenGL opengl(4, 3);
    auto window = opengl.GetWindow();

    ShaderProgram shader("resources/02_08_06_TEST3.vert", "resources/02_08_06_TEST3.frag");
    ComputeShader compute("resources/02_08_06_TEST3.comp");

    // clang-format off
    float data[] = {
       -0.5f, -0.5f, 0.f, 1.f,
        0.5f, -0.5f, 0.f, 1.f,
        0.0f,  0.5f, 0.f, 1.f,
    };
    // clang-format on

    //---------------------------------------------------------------------
    GLuint vao { 0 }, buffer { 0 };
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 4, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    //---------------------------------------------------------------------
    // 关于TBO的使用可以查看 02_01_03_TEST10
    GLuint texture { 0 };
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    float offset = 0.3f;

    while (!glfwWindowShouldClose(window))
    {
        offset = -offset;

        compute.Use();
        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        compute.SetUniform1f("offset", offset);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //---------------------------------------------------------------------
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

#endif // TEST3

#ifdef TEST4

#include <chrono>
#include <common.hpp>
#include <random>

float MyRandom(float min, float max)
{
    static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(engine);
}

constexpr int PARTICLE_COUNT = 1'000'000;

int main()
{
    InitOpenGL opengl(4, 3);
    auto window = opengl.GetWindow();

    ShaderProgram shader("resources/02_08_06_TEST3.vert", "resources/02_08_06_TEST3.frag");
    ComputeShader compute("resources/02_08_06_TEST4.comp");

    GLuint pos_vel[2] { 0 };
    glGenBuffers(2, pos_vel);

    //---------------------------------------------------------------------
    // 初始化位置缓存，分配内存，如果没有绑定target或者没有分配内存，glMapBufferRange都会报错
    glBindBuffer(GL_ARRAY_BUFFER, pos_vel[0]);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(float) * 4, nullptr, GL_DYNAMIC_COPY);
    // 映射位置缓存并使用随机向量填充 xyz是速度，w是生命值
    auto pos = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(float) * 4, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    for (int i = 0; i < PARTICLE_COUNT; ++i)
    {
        pos[i] = glm::vec4(MyRandom(-1.f, 1.f), MyRandom(-1.f, 1.f), MyRandom(-1.f, 1.f), MyRandom(0.5f, 1.f));
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    //---------------------------------------------------------------------
    // 初始化速度缓存xyz是速度，w没用
    glBindBuffer(GL_ARRAY_BUFFER, pos_vel[1]);
    glBufferData(GL_ARRAY_BUFFER, PARTICLE_COUNT * sizeof(float) * 4, nullptr, GL_DYNAMIC_COPY);
    // 映射速度缓存并使用随机向量填充
    auto vel = (glm::vec4*)glMapBufferRange(GL_ARRAY_BUFFER, 0, PARTICLE_COUNT * sizeof(float) * 4, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    for (int i = 0; i < PARTICLE_COUNT; ++i)
    {
        vel[i] = glm::vec4(MyRandom(-.1f, .1f), MyRandom(-.1f, .1f), MyRandom(-.1f, .1f), 0.f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    //---------------------------------------------------------------------
    GLuint tex_pos_vel[2] { 0 };
    glGenTextures(2, tex_pos_vel);

    glBindTexture(GL_TEXTURE_BUFFER, tex_pos_vel[0]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pos_vel[0]);

    glBindTexture(GL_TEXTURE_BUFFER, tex_pos_vel[1]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pos_vel[1]);

    //---------------------------------------------------------------------
    GLuint VAO { 0 };
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pos_vel[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    //---------------------------------------------------------------------
    GLint ubSize { 4 * sizeof(float) * 4 };
    GLuint uniform_buffer { 0 };
    glGenBuffers(1, &uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER, ubSize, nullptr, GL_DYNAMIC_COPY);
    auto attr = (glm::vec4*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, ubSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    attr[0]   = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    attr[1]   = glm::vec4(0.5f, -0.5f, 0.0f, 2.0f);
    attr[2]   = glm::vec4(0.5f, 0.5f, 0.0f, 3.0f);
    attr[3]   = glm::vec4(-0.5f, 0.5f, 0.0f, 4.0f);
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    float currentTime { 0.f }, lastTime { 0.f }, deltaTime { 0.f };

    while (!glfwWindowShouldClose(window))
    {
        compute.Use();
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniform_buffer, 0, ubSize);
        glBindImageTexture(0, tex_pos_vel[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindImageTexture(1, tex_pos_vel[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

        currentTime = (float)glfwGetTime();
        deltaTime   = currentTime - lastTime;
        lastTime    = currentTime;

        std::cout << "FPS:\t" << 1.f / deltaTime << '\n';

        compute.SetUniform1f("dt", deltaTime);
        glDispatchCompute(PARTICLE_COUNT, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //---------------------------------------------------------------------
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, PARTICLE_COUNT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

#endif // TEST4

#ifdef TEST5

#include <common.hpp>
#include <thread>

int main()
{
    InitOpenGL opengl(4, 3);
    auto window = opengl.GetWindow();

    ShaderProgram shader("resources/02_08_06_TEST3.vert", "resources/02_08_06_TEST3.frag");
    ComputeShader compute("resources/02_08_06_TEST5.comp");

    // clang-format off
    float data[] = {
       -1.0f, -0.5f, 0.f, 1.f,
        0.0f, -0.5f, 0.f, 1.f,
       -0.5f,  0.5f, 0.f, 1.f,
    };
    // clang-format on

    //---------------------------------------------------------------------
    GLuint vao { 0 }, buffer { 0 };
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(data) * 4, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    //---------------------------------------------------------------------
    GLuint texture { 0 };
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    //---------------------------------------------------------------------
    struct test_struct
    {
        int test;
        float offset;
    };

    test_struct buffer_data(1, 0.1f);

    GLuint storage_buffer { 0 };
    glGenBuffers(1, &storage_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, storage_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(test_struct), &buffer_data, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        compute.Use();
        glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        // 第二个参数对应着色器中的"layout(std430, binding = 0) buffer" => "binding = 0"
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        //---------------------------------------------------------------------
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

#endif // TEST5
