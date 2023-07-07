/*
 * 1. 创建一个计算着色器，获取计算着色器支持的工作组数量
 * 2. 使用计算着色器生成一张纹理并绘制到屏幕
 */

#define TEST2

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

uint32_t CreateComputeShader(const std::string_view& path)
{
    auto comp                = ReadFile(path);
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
    }

    glDetachShader(shaderProgram, computeShader);
    glDeleteShader(computeShader);

    return shaderProgram;
}

int main()
{
    InitOpenGL opengl(4, 3);
    auto window = opengl.GetWindow();

    //--------------------------------------------------------------------------------------------
    auto computeShader = CreateComputeShader("resources/02_08_06_TEST2.comp");
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
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    //--------------------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glUseProgram(computeShader);
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

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST2