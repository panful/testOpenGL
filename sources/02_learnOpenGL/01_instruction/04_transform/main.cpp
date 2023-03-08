
/*
 * 1. 使用glm对三角形做最简单的缩放，平移，旋转
 * 2. 一个绕(1,1,0)旋转30°的立方体
 * 3. MVP矩阵的作用，从模型坐标变换到屏幕坐标的过程
 * 4. MVP矩阵，随时间旋转的立方体，开启深度测试
 * 5. 透视投影perspective和正交投影ortho
 */

#define TEST5

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

const unsigned int SCR_WIDTH = 800;
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
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

        // 设置着色器程序中的uniform
        // glUniformMatrix4fv的参数：
        // 1.uniform的位置值。
        // 2.告诉OpenGL我们将要发送多少个矩阵(
        // 3.是否对矩阵进行置换(Transpose)，也就是说交换矩阵的行和列。
        //     OpenGL开发者通常使用一种内部矩阵布局，叫做列主序(Column - major Ordering)布局。GLM的默认布局就是列主序，所以并不需要置换矩阵，我们填GL_FALSE。
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
#include <stb_image.h>

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
    program.DeleteProgram();

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
#include <stb_image.h>

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
        // 透视除法  裁剪坐标->标准化设备坐标
        // 视口变换  标准化设备坐标->屏幕坐标

        // 模型矩阵，绕x轴顺时针旋转45°
        auto model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(-1, 0, 0));
        // 观察矩阵，将场景向后移动3个单位，相当于将观察点向前移动3个单位
        auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, -3.f));
        // 投影矩阵，透视投影，第二个参数是窗口的宽高比，第三四是近远裁剪截面
        auto project = glm::perspective(glm::radians(45.0f), 8 / 6.f, 0.1f, 1000.0f);

        auto resultMat = glm::mat4(1.0f);
        resultMat = project * view * model;
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    program.DeleteProgram();

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
#include <stb_image.h>

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
    program.DeleteProgram();

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
#include <stb_image.h>

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
        //auto m = glm::rotate(glm::mat4(1.0f), glm::radians(30.f), glm::vec3(1, 1, 0));

        // 观察矩阵，在(0,0,5)位置处观察，相当于将物体向屏幕里面平移5个单位
        auto v = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // 透视投影
        // 近远平面需要为正值
        // 如果远平面小于近平面就相当于反方向观察
        // z坐标在[4,6]范围内的显示
        //auto p = glm::perspective(glm::radians(45.0f), 8 / 6.f, 4.f, 6.f);

        // 正交投影
        // 将观察坐标在x:[-1,1] y:[-1,1] z:[4,6]范围内的显示
        // 注意z是[4,6]不是[-6,-4]
        // 注意和视口大小的影响，模型是正方体，绘制出来后看起来可能不是立方体
        //auto p = glm::ortho(-1.f, 1.f, -1.f, 1.f, 4.f, 6.f);
        // 避免视口的宽高比和正交平截头体宽高比不一致导致的拉伸
        constexpr auto width = 800.0f;
        constexpr auto height = 600.0f;
        auto p = glm::ortho(-1.f, 1.f, -1.f / (width / height), 1.f / (width / height), 4.f, 6.f);

        program.SetUniformMat4("transform", p * v * m);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    program.DeleteProgram();

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
