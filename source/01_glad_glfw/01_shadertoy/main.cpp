/*
*      *** 将shadertoy的例子用到OpenGL中 ***
* 顶点着色器只需要绘制全屏的一个矩形（两个三角形）
* 片段着色器中使用shadertoy的代码，需要在主程序中设置uniform，其中iResolution（分辨率）一般是必须的
* 1. 仅设置分辨率，即窗口大小
* 2. 纹理
* 3. 鼠标
* 4. 时间
*/

#define TEST4

#ifdef TEST1

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float windowWidth = 800;
float windowHeight = 600;

Shader* myShader = nullptr;
uint32_t myShaderProgram = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "ShaderToy", NULL, NULL);
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

    myShader = new Shader("resources/01vs.glsl", "resources/01fs.glsl");
    myShaderProgram = myShader->GetProgram();

    std::vector<float> vertices{
        -1.0f, -1.0f,  // 左下
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,  // 右上
         1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(myShaderProgram);
        glUniform2f(glGetUniformLocation(myShaderProgram, "iResolution"), windowWidth, windowHeight);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myShaderProgram);

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

    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
}

#endif // TEST1

#ifdef TEST2

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "shader.hpp"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float windowWidth = 800.0f;
float windowHeight = 600.0f;

Shader* myShader = nullptr;
uint32_t myShaderProgram = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "ShaderToy", NULL, NULL);
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

    myShader = new Shader("resources/01vs.glsl", "resources/01fs.glsl");
    myShaderProgram = myShader->GetProgram();

    std::vector<float> vertices{
        -1.0f, -1.0f,  // 左下
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,  // 右上
         1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    //注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
    unsigned char* imageData = stbi_load("barce.jpg", &width, &height, &nrChannels, 0);
    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    stbi_image_free(imageData);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(myShaderProgram);
        glUniform2f(glGetUniformLocation(myShaderProgram, "iResolution"), windowWidth, windowHeight);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myShaderProgram);

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

    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
}

#endif // TEST2

#ifdef TEST3

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "shader.hpp"
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);

float windowWidth = 800.0f;
float windowHeight = 600.0f;

Shader* myShader = nullptr;
uint32_t myShaderProgram = 0;

float mousePosX = 0.0f;
float mousePosY = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "ShaderToy", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    myShader = new Shader("resources/01vs.glsl", "resources/01fs.glsl");
    myShaderProgram = myShader->GetProgram();

    std::vector<float> vertices{
        -1.0f, -1.0f,  // 左下
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,  // 右上
         1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    //注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
    unsigned char* imageData = stbi_load("barce.jpg", &width, &height, &nrChannels, 0);
    if (imageData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }

    //绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    stbi_image_free(imageData);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(myShaderProgram);
        glUniform2f(glGetUniformLocation(myShaderProgram, "iResolution"), windowWidth, windowHeight);
        glUniform2f(glGetUniformLocation(myShaderProgram, "iMouse"), mousePosX, mousePosY);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myShaderProgram);

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

    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
    std::cout << x << "," << y << '\n';
    mousePosX = static_cast<float>(x);
    mousePosY = static_cast<float>(y);
}

#endif // TEST3

#ifdef TEST4

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float windowWidth = 800.0f;
float windowHeight = 600.0f;

Shader* myShader = nullptr;
uint32_t myShaderProgram = 0;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow((int)windowWidth, (int)windowHeight, "ShaderToy", NULL, NULL);
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

    myShader = new Shader("resources/01vs.glsl", "resources/01fs.glsl");
    myShaderProgram = myShader->GetProgram();

    std::vector<float> vertices{
        -1.0f, -1.0f,  // 左下
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,  // 右上
         1.0f, -1.0f,
        -1.0f, -1.0f,
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(myShaderProgram);
        glUniform2f(glGetUniformLocation(myShaderProgram, "iResolution"), windowWidth, windowHeight);
        //std::cout << (float)(int)glfwGetTime() << '\n';
        glUniform1f(glGetUniformLocation(myShaderProgram, "iTime"), (float)/*(int)*/glfwGetTime());

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(myShaderProgram);

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

    windowWidth = static_cast<float>(width);
    windowHeight = static_cast<float>(height);
}

#endif // TEST4
