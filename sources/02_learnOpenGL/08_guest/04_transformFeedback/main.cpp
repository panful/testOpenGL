/*
 * 1. 利用transform feedback将顶点着色器的输出获取到cpu
 * 2. 将经过顶点着色器变换后的顶点信息，通过TF捕获并利用变换后的顶点绘制新的图元
 * 3. 两个TF来回切换
 * 4. 两个TF来回切换，绘制一个点绕屏幕中心运动
 * 5. 向上移动的点阵
 * 6. 一堆顶点落地弹起
 */

#define TEST6

#ifdef TEST1

#include <common.hpp>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 outPos;
out vec2 outTex;

void main()                                
{
   outPos = vec3(aPos) + vec3(1.0);
   outTex = vec2(aPos.xy) + vec2(2.0);
   gl_Position = vec4(aPos, 1.0);
}
)";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    // 必须在link之前设置反馈变量，如果顶点着色器没有对应的输出，着色器程序会报错
    GLchar const* varyings[] = { "outPos", "outTex" };
    glTransformFeedbackVaryings(shaderProgram, 2, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         2.0f,  0.0f,  0.0f,
         3.0f,  0.0f,  0.0f,
         4.0f,  0.0f,  0.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 创建 Transform Feedback 缓冲区，并申请内存
    // sizeof(float) * 3 * (3 + 2) 表示顶点着色器输出3个顶点，每个顶点一个outPos(vec3)和outTex(vec2)
    GLuint tfBuffer { 0 };
    glGenBuffers(1, &tfBuffer);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3 * (3 + 2), NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 创建 Transform Feedback 对象，并绑定缓冲区
    GLuint tfObject { 0 };
    glGenTransformFeedbacks(1, &tfObject);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 启动变换反馈，在绘制结束后停止变换反馈
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);

    // 关闭光栅化，不关闭也不影响，因为不使用片段着色器，所以最好关闭
    glEnable(GL_RASTERIZER_DISCARD);

    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEndTransformFeedback();

    // 开启光栅化
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 方式1：读取 Transform Feedback 缓冲区数据
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    void* rawData = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(float) * 3 * (3 + 2), GL_MAP_READ_BIT);
    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    float* p = reinterpret_cast<float*>(rawData);
    for (int i = 0; i < 3; i++)
    {
        std::cout << "outPos " << i << " : " << p[i * 5 + 0] << ", " << p[i * 5 + 1] << ", " << p[i * 5 + 2] << '\t';
        std::cout << "outTex " << i << " : " << p[i * 5 + 3] << ", " << p[i * 5 + 4] << '\n';
    }

    //--------------------------------------------------------------------------------------------
    // 方式2：读取 Transform Feedback 缓冲区数据
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    GLfloat p2[15](0.0f);
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(p2), p2);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    std::cout << "-----------------------------------------\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "outPos " << i << " : " << p2[i * 5 + 0] << ", " << p2[i * 5 + 1] << ", " << p2[i * 5 + 2] << '\t';
        std::cout << "outTex " << i << " : " << p2[i * 5 + 3] << ", " << p2[i * 5 + 4] << '\n';
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>
#include <thread>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    auto vs                   = ReadFile("resources/02_08_04_TEST2.vs");
    auto fs                   = ReadFile("resources/02_08_04_TEST2.fs");
    auto vertexShaderSource   = vs.c_str();
    auto fragmentShaderSource = fs.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    GLchar const* varyings[] = { "nextPos" };
    glTransformFeedbackVaryings(shaderProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         -.5f,  -.5f,  0.0f,
         -.1f,   .0f,  0.0f,
         -.5f,   .5f,  0.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    GLuint tfBuffer { 0 };
    glGenBuffers(1, &tfBuffer);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3 * 3, NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    GLuint tfObject { 0 };
    glGenTransformFeedbacks(1, &tfObject);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 将原始顶点通过顶点着色器变换后，保存到tfBuffer
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 创建一个新的VBO，并将刚才经过变换后的数据绑定到该VBO
    GLuint VAO2 { 0 }, VBO2 { 0 };
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, tfBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制变换后的顶点信息
        // 原始顶点是一个屏幕左边的三角形，变换后是一个屏幕右边的三角形
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>
#include <thread>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    auto vs                   = ReadFile("resources/02_08_04_TEST2.vs");
    auto fs                   = ReadFile("resources/02_08_04_TEST2.fs");
    auto vertexShaderSource   = vs.c_str();
    auto fragmentShaderSource = fs.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    GLchar const* varyings[] = { "nextPos" };
    glTransformFeedbackVaryings(shaderProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         -.5f,  -.5f,  0.0f,
         -.1f,   .0f,  0.0f,
         -.5f,   .5f,  0.0f,
    };
    // clang-format on

    GLuint VAO[2] { 0 }, VBO[2] { 0 };
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 创建TF Buffer，并申请内存，不填充数据
    GLuint tfBuffer[2] { 0 };
    glGenBuffers(2, tfBuffer);

    // 申请内存
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3 * 3, NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3 * 3, NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 将TF Buffer 绑定到 TF Object
    GLuint tfObject[2] { 0 };
    glGenTransformFeedbacks(2, tfObject);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 不开启光栅化执行一次顶点着色器，使用顶点着色器对顶点做一次变换并使用TF获取变换后的结果
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------

    bool flag { true };
    while (!glfwWindowShouldClose(window))
    {
        // 睡眠一秒，便于观察渲染结果
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        if (flag)
        {
            // tfBuffer[0]保存的是VAO[0]执行顶点着色器之后的结果
            // 将它绑定到VAO[1]，执行顶点着色器将变换后的结果通过tranform feedback保存到tfBuffer[1]
            // 再执行片段着色器将tfBuffer[0]的数据显示到屏幕
            glBindVertexArray(VAO[1]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[1]);

            glBeginTransformFeedback(GL_TRIANGLES);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[1]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);

            glBeginTransformFeedback(GL_TRIANGLES);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        flag = !flag;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>
#include <thread>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    auto vs                   = ReadFile("resources/02_08_04_TEST4.vs");
    auto fs                   = ReadFile("resources/02_08_04_TEST2.fs");
    auto vertexShaderSource   = vs.c_str();
    auto fragmentShaderSource = fs.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    GLchar const* varyings[] = { "nextPos" };
    glTransformFeedbackVaryings(shaderProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //--------------------------------------------------------------------------------------------
    float vertices[] = { 0.0f, 0.5f, 0.0f };

    GLuint VAO[2] { 0 }, VBO[2] { 0 };
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    GLuint tfBuffer[2] { 0 };
    glGenBuffers(2, tfBuffer);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    GLuint tfObject[2] { 0 };
    glGenTransformFeedbacks(2, tfObject);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 使用vertices绘制一个起始点用来填充tfBuffer[0]，
    // 因为如果不绘制，tfBuffer[0]还没有数据，第一次渲染循环时使用tfBuffer[0]绑定到VBO，点将出现在屏幕中心(0,0,0)
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 1);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    bool flag { true };
    glPointSize(5.f);
    while (!glfwWindowShouldClose(window))
    {
        // 可以添加一个睡眠用来观察渲染结果
        // std::this_thread::sleep_for(std::chrono::seconds(1));

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), static_cast<float>(glfwGetTime()));

        if (flag)
        {
            glBindVertexArray(VAO[1]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[1]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, 1);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[1]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, 1);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        flag = !flag;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <common.hpp>
#include <thread>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    auto vs                   = ReadFile("resources/02_08_04_TEST5.vs");
    auto fs                   = ReadFile("resources/02_08_04_TEST2.fs");
    auto vertexShaderSource   = vs.c_str();
    auto fragmentShaderSource = fs.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    GLchar const* varyings[] = { "nextPos" };
    glTransformFeedbackVaryings(shaderProgram, 1, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = { 
        -0.2f, -0.8f, 0.0f,
        -0.1f, -0.8f, 0.0f,
         0.0f, -0.8f, 0.0f,
         0.1f, -0.8f, 0.0f,
         0.2f, -0.8f, 0.0f,

        -0.1f, -0.7f, 0.0f,
         0.0f, -0.7f, 0.0f,
         0.1f, -0.7f, 0.0f,
         
         0.0f, -0.6f, 0.0f,
    };
    // clang-format on

    int numPoints = 9;
    int dataSize  = numPoints * 3 * sizeof(float);

    GLuint VAO[2] { 0 }, VBO[2] { 0 };
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, dataSize, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    GLuint tfBuffer[2] { 0 };
    glGenBuffers(2, tfBuffer);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, dataSize, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, dataSize, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    GLuint tfObject[2] { 0 };
    glGenTransformFeedbacks(2, tfObject);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 使用vertices绘制一个起始点用来填充tfBuffer[0]，
    // 因为如果不绘制，tfBuffer[0]还没有数据，第一次渲染循环时使用tfBuffer[0]绑定到VBO，点将出现在屏幕中心(0,0,0)
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    bool flag         = true;
    float lastTime    = 0.0f;
    float deltaTime   = 0.0f;
    float currentTime = 0.0f;

    glPointSize(5.f);

    while (!glfwWindowShouldClose(window))
    {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime   = currentTime - lastTime;
        lastTime    = currentTime;

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), deltaTime);
        glUniform1f(glGetUniformLocation(shaderProgram, "uSpeed"), 0.3f);

        if (flag)
        {
            glBindVertexArray(VAO[1]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[1]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, numPoints);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[1]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, numPoints);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        flag = !flag;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <common.hpp>
#include <random>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
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
    auto vs                   = ReadFile("resources/02_08_04_TEST6.vs");
    auto fs                   = ReadFile("resources/02_08_04_TEST2.fs");
    auto vertexShaderSource   = vs.c_str();
    auto fragmentShaderSource = fs.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    GLchar const* varyings[] = { "nextPos", "nextVelocity", "nextElasticity" };
    glTransformFeedbackVaryings(shaderProgram, 3, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //--------------------------------------------------------------------------------------------
    std::vector<float> vertices;
    int numPoints = 1000;
    int dataSize  = numPoints * 5 * sizeof(float);
    std::default_random_engine engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<float> velocity(0.1f, 1.f);
    std::uniform_real_distribution<float> elasticity(0.1f, 0.8f);

    for (int i = 0; i < numPoints; ++i)
    {
        vertices.emplace_back(1.0f / static_cast<float>(numPoints) * static_cast<float>(i) - 0.5f);
        vertices.emplace_back(0.8f);
        vertices.emplace_back(0.0f);
        vertices.emplace_back(velocity(engine));
        vertices.emplace_back(elasticity(engine));
    }

    GLuint VAO[2] { 0 }, VBO[2] { 0 };
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, dataSize, vertices.data(), GL_STATIC_DRAW);
    // 位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // 初速度
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    // 弹性系数
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 4));

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    GLuint tfBuffer[2] { 0 };
    glGenBuffers(2, tfBuffer);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, dataSize, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, dataSize, nullptr, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    GLuint tfObject[2] { 0 };
    glGenTransformFeedbacks(2, tfObject);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 使用vertices绘制一个起始点用来填充tfBuffer[0]，
    // 因为如果不绘制，tfBuffer[0]还没有数据，第一次渲染循环时使用tfBuffer[0]绑定到VBO，点将出现在屏幕中心(0,0,0)
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO[0]);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);

    glEnable(GL_RASTERIZER_DISCARD);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, numPoints);
    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    bool flag         = true;
    float lastTime    = 0.0f;
    float deltaTime   = 0.0f;
    float currentTime = 0.0f;

    glPointSize(3.f);

    while (!glfwWindowShouldClose(window))
    {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime   = currentTime - lastTime;
        lastTime    = currentTime;

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), deltaTime);
        // glUniform1f(glGetUniformLocation(shaderProgram, "uSpeed"), 0.3f);

        if (flag)
        {
            glBindVertexArray(VAO[1]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[0]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 4));

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[1]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[1]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[1]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, numPoints);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }
        else
        {
            glBindVertexArray(VAO[0]);
            glBindBuffer(GL_ARRAY_BUFFER, tfBuffer[1]);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 4));

            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer[0]);
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject[0]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer[0]);

            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, numPoints);
            glEndTransformFeedback();

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
            glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        flag = !flag;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST6