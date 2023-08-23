
/*
 * 1. 使用glm对三角形做最简单的缩放，平移，旋转，三个矩阵的使用顺序
 * 2. 一个绕(1,1,0)旋转30°的立方体
 * 3. MVP矩阵的作用，从模型坐标变换到屏幕坐标的过程
 * 4. 随时间旋转的立方体，开启深度测试
 * 5. 透视投影perspective和正交投影ortho
 * 6. NDC(Normalized Device Coordinates)标准化设备坐标系
 * 7. 透视除法
 * 8. z-fighting z冲突
 * 9. 解决z-fighting
 * 10 绘制多边形的边框、边线
 */

#define TEST10

#ifdef TEST1

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core                                     \n"
                 "layout (location = 0) in vec3 aPos;                   \n"
                 "uniform mat4 transform;                               \n"
                 "void main()                                           \n"
                 "{                                                     \n"
                 "   vec4 temp_pos = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                 "   gl_Position = transform * temp_pos;                \n"
                 "}                                                     \n\0";

const char* FS = "#version 330 core                                     \n"
                 "void main()                                           \n"
                 "{                                                     \n"
                 "   gl_FragColor = vec4(0.0f,0.8f,0.0f,1.0f);          \n"
                 "}                                                     \n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader & Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f, -0.5f, 0.0f,  // right
        0.0f, 0.5f, 0.0f,   // top
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // 一定注意平移，旋转，缩放三个矩阵的顺序
        // 先缩放再旋转再平移
        glm::mat4 trans = glm::mat4(1.0f);
        // +x平移0.5个单位，-y平移0.5个单位
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        // 绕z轴逆时针旋转
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        // x方向和y方向都缩放为原来的0.5
        trans = glm::scale(trans, glm::vec3(.5f, .5f, 0.f));

        //---------------------------------------------------------
        // 矩阵运算的顺序是相反的，需要从右向左读
        // 即模型矩阵应该是这样：
        // auto model = translate * rotate * scale;
        //---------------------------------------------------------

        // 设置着色器程序中的uniform
        // glUniformMatrix4fv的参数：
        // 1.uniform的位置值。
        // 2.告诉OpenGL我们将要发送多少个矩阵(
        // 3.是否对矩阵进行置换(Transpose)，也就是说交换矩阵的行和列。
        //     OpenGL开发者通常使用一种内部矩阵布局，叫做列主序(Column - major
        //     Ordering)布局。GLM的默认布局就是列主序，所以并不需要置换矩阵，我们填GL_FALSE。
        // 4.矩阵数据，但是GLM并不是把它们的矩阵储存为OpenGL所希望接受的那种，因此要先用GLM的自带的函数value_ptr来变换这些数据。
        auto transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST1

#ifdef TEST2

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绕（1,1,0）旋转30°
        auto resultMat = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        -0.5f, -0.5f,  0.0f,    1.0f, 0.0f, 0.0f,  // bottom left
         0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,  // bottom right
         0.5f,  0.5f,  0.0f,    0.0f, 0.0f, 1.0f,  // top right
        -0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 1.0f,  // top left
    };

    std::array<GLuint, 6> indices{
        0, 1, 3,
        1, 2, 3
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------
    // 打印MVP矩阵得到的顶点坐标
    auto print_vetices = [vertices](const char* name, const glm::mat4& mat)
    {
        std::cout << "---------------- " << name << '\n';
        for (size_t i = 0; i < 4; i++)
        {
            auto result_vertices = mat * glm::vec4(vertices[i * 6 + 0], vertices[i * 6 + 1], vertices[i * 6 + 2], 1.0f);
            auto w               = result_vertices.w;
            std::cout << "w: " << w << "\t\t" << result_vertices.x / w << "\t\t" << result_vertices.y / w << "\t\t" << result_vertices.z / w << '\n';
        }
    };

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 注意每个变换的输入坐标和输出坐标

        // 模型矩阵  模型坐标->世界坐标
        // 观察矩阵  世界坐标->观察坐标
        // 投影矩阵  观察坐标->裁剪坐标
        // 透视除法  裁剪坐标->标准化设备坐标（NDC）
        // 视口变换  标准化设备坐标->屏幕坐标

        // 模型矩阵，绕x轴顺时针旋转45°
        auto model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(-1, 0, 0));
        // 观察矩阵，将场景向后移动3个单位，相当于将观察点向前移动3个单位
        auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -3.f));
        // 投影矩阵，透视投影，第二个参数是窗口的宽高比，第三四是近远裁剪截面
        auto projection = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 1000.0f);
        // 正交投影
        // auto projection = glm::ortho(-1.f, 1.f, -1.f / (8 / 6.f), 1.f / (8 / 6.f), 2.f, 4.f);

        // 透视除法是将整个向量（顶点坐标）除以w分量，使向量可以从4维降到3维
        // OpenGL内部自动做透视除法，不需要写代码

        print_vetices("origin", glm::mat4(1.f));
        print_vetices("model", model);
        print_vetices("modle + view", view * model);
        print_vetices("model + view + projection", projection * view * model);

        auto resultMat = glm::mat4(1.0f);
        resultMat      = projection * view * model;
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST3

#ifdef TEST4

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    // 开启深度测试，默认关闭
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto m = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        auto v = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3));
        auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 100.f);
        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST4

#ifdef TEST5

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 前左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 前右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 前右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 前左上

        -0.5f, -0.5f, -.5f,     1.0f, 1.0f, 0.0f, // 后左下
         0.5f, -0.5f, -.5f,     0.0f, 1.0f, 1.0f, // 后右下
         0.5f,  0.5f, -.5f,     1.0f, 0.0f, 1.0f, // 后右上
        -0.5f,  0.5f, -.5f,     0.0f, 0.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indices{
        0, 1, 3, // 前
        1, 2, 3,

        1, 5, 2, // 右
        5, 6, 2,

        5, 4, 6, // 后
        4, 7, 6,

        4, 0, 7, // 左
        0, 3, 7,
              
        3, 2, 7, // 上
        2, 6, 7,

        0, 1, 4, // 下
        1, 5, 4,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    // 开启深度测试，默认关闭
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除深度缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 绕(1,1,0)旋转
        auto m = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        // auto m = glm::rotate(glm::mat4(1.0f), glm::radians(30.f), glm::vec3(1, 1, 0));

        // 观察矩阵，在(0,0,5)位置处观察，沿着(0,0,-1)观察
        // 就是将世界坐标的原点移动到(0,0,5)
        auto v = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // 透视投影
        // 近远平面需要为正值
        // 如果远平面小于近平面就相当于反方向观察
        // 以相机为原点，顶点距离相机的距离在[4,6]范围内的顶点可以显示
        auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 4.f, 6.f);

        // 正交投影
        // 将观察坐标在x:[-1,1] y:[-1,1] z:[4,6]范围内的显示
        // 注意z是[4,6]不是[-6,-4]，因为观察矩阵是将世界坐标系原点移动到相机位置
        // 注意和视口大小的影响，模型是正方体，绘制出来后看起来可能不是立方体
        // auto p = glm::ortho(-1.f, 1.f, -1.f, 1.f, 4.f, 6.f);
        // 避免视口的宽高比和正交平截头体宽高比不一致导致的拉伸，可以做如下变换：
        constexpr auto width  = 800.0f;
        constexpr auto height = 600.0f;
        // auto p = glm::ortho(-1.f, 1.f, -1.f / (width / height), 1.f / (width / height), 4.f, 6.f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST5

#ifdef TEST6

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    // NDC可以看作一个XYZ三个方向都属于[-1,1]范围的立方体
    // 只有坐标xyz分量都属于这个范围才能显示，其他顶点都会被丢弃
    // 透视除法的作用就是将裁剪坐标转换为NDC
    // 裁剪坐标由投影矩阵变换得来

    // 不做【任何变换】的顶点坐标就是【标准化设备坐标系】的坐标

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f,  0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f,  1.5f,     0.0f, 1.0f, 0.0f, // 右下 z大于 1.0，因此会被丢弃
         0.5f,  0.5f,  0.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, -1.5f,     1.0f, 1.0f, 1.0f, // 左上 z小于-1.0，因此会被丢弃
    };

    std::array<GLuint,2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        auto resultMat = glm::mat4(1.0f); // 不做任何变换
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST6

#ifdef TEST7

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST7.vs", "resources/02_01_04_TEST7.fs");

    // clang-format off
    // NDC范围内的坐标
    std::array<GLfloat, 3 * 7> vertices1{
        -0.5f, -0.5f,  0.0f,  1.f,    1.0f, 0.0f, 0.0f,  // left
         0.5f, -0.5f,  0.0f,  1.f,    0.0f, 1.0f, 0.0f,  // right
         0.0f,  0.5f,  0.0f,  1.f,    0.0f, 0.0f, 1.0f,  // top
    };

    // 超出NDC的坐标
    std::array<GLfloat, 3 * 7> vertices2{
        -5.0f, -0.5f,  0.0f,  1.f,    1.0f, 0.0f, 0.0f,  // left
         0.5f, -0.5f,  0.0f,  1.f,    0.0f, 1.0f, 0.0f,  // right
         0.0f,  0.5f,  0.0f,  1.f,    0.0f, 0.0f, 1.0f,  // top
    };

    // 使用w做透视除法
    // 渲染出来的三角形颜色和预期会不一致
    // 透视除法必须在顶点着色器之后再做，具体应该是在裁剪(裁剪在几何着色器之后)之后，视口变换之前
    // 透视矫正插值
    // https://zhuanlan.zhihu.com/p/383793695
    std::array<GLfloat, 3 * 7> vertices3{
        -5.0f, -5.0f,  0.0f, 10.f,    1.0f, 0.0f, 0.0f,  // left
         0.5f, -0.5f,  0.0f,  1.f,    0.0f, 1.0f, 0.0f,  // right
         0.0f,  0.5f,  0.0f,  1.f,    0.0f, 0.0f, 1.0f,  // top
    };

    // 所有顶点的w分量都为10
    std::array<GLfloat, 3 * 7> vertices4{
        -5.0f, -5.0f,  0.0f, 10.f,    1.0f, 0.0f, 0.0f,  // left
         5.0f, -5.0f,  0.0f, 10.f,    0.0f, 1.0f, 0.0f,  // right
         0.0f,  5.0f,  0.0f, 10.f,    0.0f, 0.0f, 1.0f,  // top
    };

    // w等于0，表示一个无穷远的点或无限长的向量
    std::array<GLfloat, 3 * 7> vertices5{
        -0.5f, -0.5f,  0.0f,  0.f,    1.0f, 0.0f, 0.0f,  // left
         0.5f, -0.5f,  0.0f,  0.f,    0.0f, 1.0f, 0.0f,  // right
         0.0f,  0.5f,  0.0f,  0.f,    0.0f, 0.0f, 1.0f,  // top
    };

    std::array<GLuint, 6> indices{
        0, 1, 2
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices3.size(), vertices3.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST7

#ifdef TEST8

#include <common.hpp>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    constexpr GLfloat offset { 0.01f };

    std::vector<GLfloat> vertices;
    constexpr GLfloat startPoint { -.7f };
    for (size_t i = 0; i < 100; i++)
    {
        for (size_t j = 0; j < 100; j++)
        {
            vertices.emplace_back(startPoint + i * offset);
            vertices.emplace_back(startPoint + j * offset);
            vertices.emplace_back(0.0f);

            vertices.emplace_back(1.0f);
            vertices.emplace_back(0.0f);
            vertices.emplace_back(0.0f);
        }
    }

    std::vector<GLfloat> vertices2;
    constexpr GLfloat startPoint2 { -.3f };
    for (size_t i = 0; i < 100; i++)
    {
        for (size_t j = 0; j < 100; j++)
        {
            // pos
            vertices2.emplace_back(startPoint2 + i * offset);
            vertices2.emplace_back(startPoint2 + j * offset);
            vertices2.emplace_back(0.0f);

            // color
            vertices2.emplace_back(0.0f);
            vertices2.emplace_back(1.0f);
            vertices2.emplace_back(0.0f);
        }
    }

    std::vector<GLuint> indices;
    for (size_t i = 0; i < 99; i++)
    {
        for (size_t j = 0; j < 99; j++)
        {
            indices.emplace_back(static_cast<GLuint>(j + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 100 + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 1 + i * 100));

            indices.emplace_back(static_cast<GLuint>(100 + j + i * 100));
            indices.emplace_back(static_cast<GLuint>(100 + 1 + j + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 1 + i * 100));
        }
    }

    unsigned int VBO, VAO, EBO;
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int VBO2, VAO2, EBO2;
    {
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 要想出现z冲突现象，必须开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 绕(1,1,0)旋转
        auto m = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));

        // 观察矩阵
        auto v = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // 透视投影
        // auto p = glm::perspective(glm::radians(30.0f), 8 / 6.f, 4.f, 6.f);

        // 正交投影
        auto p = glm::ortho(-1.f, 1.f, -1.f / (8 / 6.f), 1.f / (8 / 6.f), 4.f, 6.f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST8

#ifdef TEST9

// 解决Z-fighting：
// 多边形偏移、混合 https://www.jianshu.com/p/8f8f420a3702
// 反向深度 https://blog.51cto.com/u_15057807/2621354

// z-fighting问题的预防：
// 避免两个物体靠的太近：在绘制时，插入一个小偏移
// 将近裁剪面（设置透视投影时设置）设置的离观察者远一些：提高裁剪范围内的精确度
// 使用更高位数的深度缓冲区：提高深度缓冲区的精确度

#include <common.hpp>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_04_TEST2.vs", "resources/02_01_04_TEST2.fs");

    constexpr GLfloat offset { 0.01f };

    std::vector<GLfloat> vertices;
    constexpr GLfloat startPoint { -.7f };
    for (size_t i = 0; i < 100; i++)
    {
        for (size_t j = 0; j < 100; j++)
        {
            vertices.emplace_back(startPoint + i * offset);
            vertices.emplace_back(startPoint + j * offset);
            vertices.emplace_back(0.0f);

            vertices.emplace_back(1.0f);
            vertices.emplace_back(0.0f);
            vertices.emplace_back(0.0f);
        }
    }

    std::vector<GLfloat> vertices2;
    constexpr GLfloat startPoint2 { -.3f };
    for (size_t i = 0; i < 100; i++)
    {
        for (size_t j = 0; j < 100; j++)
        {
            // pos
            vertices2.emplace_back(startPoint2 + i * offset);
            vertices2.emplace_back(startPoint2 + j * offset);
            vertices2.emplace_back(0.0f);

            // color
            vertices2.emplace_back(0.0f);
            vertices2.emplace_back(1.0f);
            vertices2.emplace_back(0.0f);
        }
    }

    std::vector<GLuint> indices;
    for (size_t i = 0; i < 99; i++)
    {
        for (size_t j = 0; j < 99; j++)
        {
            indices.emplace_back(static_cast<GLuint>(j + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 100 + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 1 + i * 100));

            indices.emplace_back(static_cast<GLuint>(100 + j + i * 100));
            indices.emplace_back(static_cast<GLuint>(100 + 1 + j + i * 100));
            indices.emplace_back(static_cast<GLuint>(j + 1 + i * 100));
        }
    }

    unsigned int VBO, VAO, EBO;
    {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int VBO2, VAO2, EBO2;
    {
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // 要想出现z冲突现象，必须开启深度测试
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        // 绕(1,1,0)旋转
        auto m = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));

        // 观察矩阵
        auto v = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // 透视投影
        // auto p = glm::perspective(glm::radians(30.0f), 8 / 6.f, 4.f, 6.f);

        // 正交投影
        auto p = glm::ortho(-1.f, 1.f, -1.f / (8 / 6.f), 1.f / (8 / 6.f), 4.f, 6.f);

        program.SetUniformMat4("transform", p * v * m);

        //-----------------------------------------------------

        // 开启多边形偏移
        // GL_POLYGON_OFFSET_POINT 对应光栅化模式: GL_POINT
        // GL_POLYGON_OFFSET_LINE  对应光栅化模式: GL_LINE
        // GL_POLYGON_OFFSET_FILL  对应光栅化模式: GL_FILL
        glEnable(GL_POLYGON_OFFSET_FILL);
        // glPolygonOffset (GLfloat factor, GLfloat units);
        // 每个Fragment的深度值都会增加偏移量: Offset = (m * factor) + (r * units);
        // m: 多边形的深度的斜率的最⼤值,理解⼀个多边形越是与近裁剪⾯平行,m 就越接近于0.
        // r: 能产⽣于窗⼝坐标系的深度值中可分辨的差异最小值.r 是是由具体OpenGL 平台指定的 ⼀个常量.
        // ⼀个大于0的Offset 会把模型推到离你(摄像机) 更远的位置, 相应的⼀个⼩于0的Offset 会把模型拉近，
        // 一般⽽⾔, 只需要将 -1.f 和 1.f 这样简单赋值给glPolygonOffset 基本可以满⾜足需求.
        // glPolygonOffset(1.f, 1.f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        glDisable(GL_POLYGON_OFFSET_FILL);

        //-----------------------------------------------------

        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

        //-----------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST9

#ifdef TEST10

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 2.f, 2.f, 8.f }));
    auto window = opengl.GetWindow();

    ShaderProgram program("resources/02_01_04_TEST10.vs", "resources/02_01_04_TEST10.fs");

    // clang-format off
    std::vector<float> vertices {
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    std::vector<uint32_t> indices_poly { 
        0, 1, 2, 0, 2, 3,
        1, 5, 6, 1, 6, 2,
        5, 4, 7, 5, 7, 6,
        4, 0, 3, 4, 3, 7,
        3, 2, 6, 3, 6, 7,
        4, 5, 1, 4, 1, 0,
    };

    std::vector<uint32_t> indices_line { 
        0, 1, 1, 2, 2, 3, 3, 0,
        1, 5, 5, 6, 6, 2, 2, 1,
        5, 4, 4, 7, 7, 6, 6, 5,
        4, 0, 0, 3, 3, 7, 7, 4,
        3, 2, 2, 6, 6, 7, 7, 3,
        4, 5, 5, 1, 1, 0, 0, 4,
    };
    // clang-format on

    Renderer poly(vertices, indices_poly, { 3 }, GL_TRIANGLES);
    Renderer line(vertices, indices_line, { 3 }, GL_LINES);

    // clang-format off
    std::vector<glm::mat4> models { 
        glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 0.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 0.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(1.f, 1.f, 0.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 1.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(1.f, 0.f, 1.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(0.f, 1.f, 1.f)),
        glm::translate(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f)),
    };
    // clang-format on

    glEnable(GL_DEPTH_TEST);
    glLineWidth(2.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto vp = opengl.GetProjectionMatrix() * opengl.GetViewMatrix();

        program.Use();

        //--------------------------------------------------------------------------
        // 多边形面
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.f, 1.f);

        program.SetUniform3f("uColor", 1.f, 0.f, 0.f);
        for (auto&& m : models)
        {
            program.SetUniformMat4("transform", vp * m);
            poly.Draw();
        }

        glDisable(GL_POLYGON_OFFSET_FILL);

        //--------------------------------------------------------------------------
        // 多边形边框
        program.SetUniform3f("uColor", 0.f, 1.f, 0.f);
        for (auto&& m : models)
        {
            program.SetUniformMat4("transform", vp * m);
            line.Draw();
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif // TEST10
