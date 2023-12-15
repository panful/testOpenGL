/*
 * 1. 使用glfw的多重采样功能，实现MSAA抗锯齿
 * 2. FBO（离屏渲染）使用MSAA抗锯齿
 */

#define TEST2

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 设置glfw窗口使用一个包含8个子采样点的颜色缓冲
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    ShaderProgram program("shaders/02_04_11_TEST1.vs", "shaders/02_04_11_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.7f, -0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前左下
         0.7f, -0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前右下
         0.7f,  0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前右上
        -0.7f,  0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前左上

        -0.7f, -0.7f, -.7f,     0.0f, 1.0f, 0.0f, // 后左下
         0.7f, -0.7f, -.7f,     0.0f, 1.0f, 0.0f, // 后右下
         0.7f,  0.7f, -.7f,     0.0f, 1.0f, 0.0f, // 后右上
        -0.7f,  0.7f, -.7f,     0.0f, 1.0f, 0.0f, // 后左上
    };

    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCube.size(), indicesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //------------------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);

    // 启用多重采样（默认就是启用）
    glEnable(GL_MULTISAMPLE);

    // 查看是否启用多重采样
    GLint data { 0 };
    glGetIntegerv(GL_MULTISAMPLE, &data);
    std::cout << data << '\n';

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();

        auto modleMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1, 1, 0));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    glDeleteVertexArrays(1, &cubeVAO);

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

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

// 多重采样的样本个数
GLsizei samples { 9 };

int main()
{
    InitOpenGL initOpenGL("offscreen MSAA", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram FBOProgram("shaders/02_04_11_TEST1.vs", "shaders/02_04_11_TEST1.fs");
    ShaderProgram ScreenProgram("shaders/02_04_11_TEST2.vs", "shaders/02_04_11_TEST2.fs");

    // clang-format off
    // 立方体，绘制到帧缓冲
    std::array<GLfloat, 8 * 6> verticesCube{
        // pos                  // color
        -0.7f, -0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前左下
         0.7f, -0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前右下
         0.7f,  0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前右上
        -0.7f,  0.7f, 0.7f,     0.0f, 1.0f, 0.0f, // 前左上

        -0.7f, -0.7f, -.7f,     1.0f, 0.0f, 0.0f, // 后左下
         0.7f, -0.7f, -.7f,     1.0f, 0.0f, 0.0f, // 后右下
         0.7f,  0.7f, -.7f,     1.0f, 0.0f, 0.0f, // 后右上
        -0.7f,  0.7f, -.7f,     1.0f, 0.0f, 0.0f, // 后左上
    };
    // 6个面，12个三角形
    std::array<GLuint,6 * 2 * 3> indicesCube{
        0, 1, 3,    1, 2, 3,    // 前
        1, 5, 2,    5, 6, 2,    // 右
        5, 4, 6,    4, 7, 6,    // 后
        4, 0, 7,    0, 3, 7,    // 左
        3, 2, 7,    2, 6, 7,    // 上
        4, 1, 0,    4, 5, 1,    // 下
    };

    // 矩形，绘制到屏幕
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                   // texCoords
        -1.f,   1.f,   0.0f,     0.0f, 1.0f,
        -1.f,  -1.f,   0.0f,     0.0f, 0.0f,
         1.f,  -1.f,   0.0f,     1.0f, 0.0f,

        -1.f,   1.f,   0.0f,     0.0f, 1.0f,
         1.f,  -1.f,   0.0f,     1.0f, 0.0f,
         1.f,   1.f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesCube), indicesCube.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_quad), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------
    // FBO帧缓冲对象
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // 多重采样纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer);
    // 第二个参数表示纹理所拥有的样本个数，最后一个参数如果为true，图像将会对每个纹素使用相同的样本位置以及相同数量的子采样点个数
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, windowWidth, windowHeight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    // 将多重采样纹理附加到帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColorBuffer, 0);
    // RBO用来保存深度、模板缓冲
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    // 多重采样渲染缓冲对象
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------
    // 因为多重采样的图像包含比普通图像更多的信息，不能直接将该图像用于其他运算，比如着色器不能对它进行采样
    // 所以需要将多重采样的图像缩小或者还原(Resolve)
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0); // we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 立方体
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        FBOProgram.Use();
        FBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        // glBlitFramebuffer 能够将一个帧缓冲中的某个区域复制到另一个帧缓冲中，并且将多重采样缓冲还原。
        // 前四个参数是源帧缓冲大小，接下来四个参数是目标帧缓冲大小
        // 第九个参数表示需要复制那些区域：GL_COLOR_BUFFER_BIT、GL_DEPTH_BUFFER_BIT、GL_STENCIL_BUFFER_BIT
        // 最后一个参数表示拉伸图像时的插值方法：GL_NEAREST、GL_LINEAR
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        //----------------------------------------------------------------------------------
        // 屏幕
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.f, 1.f, 1.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ScreenProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

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