/*
 * 01. 帧缓冲基础用法，附加颜色缓冲纹理到帧缓冲
 * 02. 修改附加的颜色缓冲纹理大小
 * 03. 将任意大小的颜色缓冲纹理附加到帧缓冲
 * 04. 附加深度缓冲纹理到帧缓冲，从纹理中读取深度缓冲数据并打印
 * 05. 使用渲染缓冲对象存储深度缓冲信息，绘制需要开启深度测试的图像到帧缓冲
 * 06. 使用纹理存储深度缓冲信息，绘制需要开启深度测试的图像到帧缓冲
 * 07. 附加附件为深度、模板缓冲的渲染缓冲对象到帧缓冲
 * 08. 对帧缓冲的纹理进行后期处理，核效果
 * 09. 获取FBO指定位置的像素数据
 * 10. 窗口的右上角绘制一个小窗口
 * 11. 测试自定义FrameBufferObject类
 * 12. MRT(Multiple Render Targets)多渲染目标，一个片段着色器输出到多个颜色附件
 */

#define TEST12

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 三角形，绘制到自定义的帧缓冲
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 中上
    };
    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    unsigned int triangleVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // FBO帧缓冲对象
    unsigned int FBO;
    // 创建一个帧缓冲
    glGenFramebuffers(1, &FBO);
    // 将创建的FBO绑定到激活的帧缓冲
    // 在绑定到 GL_FRAMEBUFFER 目标之后，所有的读取和写入帧缓冲的操作将会影响当前绑定的帧缓冲。
    // 第一个参数还可以是：GL_READ_FRAMEBUFFER 或 GL_DRAW_FRAMEBUFFER
    // GL_READ_FRAMEBUFFER 的帧缓冲将会使用在所有像是 glReadPixels() 的读取操作中
    // GL_DRAW_FRAMEBUFFER 的帧缓冲将会被用作渲染、清除等写入操作的目标。
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    // 最后一个参数为NULL，即在此处不给纹理添加图像数据，后面通过FBO将图像数据添加到纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // 将纹理附加到当前绑定的帧缓冲对象（可以理解为将帧缓冲对象(FBO)渲染到一个纹理上，将来可以像加载纹理一样使用这个纹理）
    // 1.帧缓冲的目标（绘制，读取或者两者皆可）
    // 2.指定帧缓冲的附着点（可以设置为附加到颜色、深度、模板纹理）
    // 3.附加的纹理类型
    // 4.要附加的纹理本身
    // 5.多级渐远纹理的级别
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // 检查帧缓冲是否完整，完整的帧缓冲需要满足以下的条件：
    // 1. 附加至少一个缓冲（颜色、深度或模板缓冲）。
    // 2. 至少有一个颜色附件(Attachment)。
    // 3. 所有的附件都必须是完整的（保留了内存）。
    // 4. 每个缓冲都应该有相同的样本数。
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    // 颜色缓冲、深度缓冲、模板缓冲结合起来叫作 帧缓冲(Frame Buffer Object)
    // 自定义的FBO可以理解为一个新创建但是隐藏的glfw窗口，默认的FBO可以理解为显示的glfw窗口。

    // 本示例的效果：
    // 将隐藏的glfw窗口渲染的图像内容保存到一个2D纹理中，然后在显示的glfw窗口中绘制一个矩形用来显示这个纹理

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 随时间绕y轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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

Texture* texColorBuffer { nullptr };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 三角形，绘制到自定义的帧缓冲
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 中上
    };
    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    unsigned int triangleVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    texColorBuffer = new Texture(windowWidth, windowHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer->Get(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 随时间绕y轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();
        texColorBuffer->Bind();
        glBindVertexArray(quadVAO);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    delete texColorBuffer;
    texColorBuffer = nullptr;

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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
    // 第一次显示窗口并不会调用该函数
    // 只有显示完窗口之后修改窗口大小才会调用
    // glViewport更应该在绑定完FBO后调用，可以看示例3的用法
    std::cout << "width: " << width << "\theigth: " << height << '\n';
    glViewport(0, 0, width, height);

    if (texColorBuffer)
    {
        texColorBuffer->ResetSize(width, height);
    }
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };

constexpr uint32_t textureWidth { 8000 };
constexpr uint32_t textureHeight { 6000 };

Texture* texColorBuffer { nullptr };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 三角形，绘制到自定义的帧缓冲
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 中上
    };
    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    unsigned int triangleVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 可以生成任意大小的纹理，但前提是自定义的FBO视口大小和纹理大小一致
    // 不然会出现显示不全或显示过小的问题
    // 纹理比视口大时，纹理的左下角只有一部分图像
    // 纹理比视口小时，纹理只能显示一部分图像
    texColorBuffer = new Texture(textureWidth, textureHeight);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer->Get(), 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // 设置自定义的FBO的视口大小（保证和附加的颜色缓冲纹理大小一致）
        glViewport(0, 0, textureWidth, textureHeight);

        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 随时间绕y轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // 设置显示窗口上的FBO的视口大小（需要随着窗口大小改变而改变）
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();
        texColorBuffer->Bind();
        glBindVertexArray(quadVAO);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    delete texColorBuffer;
    texColorBuffer = nullptr;

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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
    // 在这里不需要再调用glViewport，因为glViewport只对当前绑定的FBO生效
    // 应该在默认的FBO绑定到当前窗口后调用glViewport

    windowWidth  = width;
    windowHeight = height;
}

#endif // TEST3

#ifdef TEST4

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCB(GLFWwindow* window, int button, int action, int mods);
void cursorPosCB(GLFWwindow* window, double xpos, double ypos);

// 鼠标的位置
double mouse_x { 0. };
double mouse_y { 0. };
// 窗口的大小
constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

// 附加到帧缓冲的深度缓冲纹理
uint32_t depthTextureBuffer { 0 };
// 深度缓冲数据的类型
GLenum dataType { GL_FLOAT };
// 纹理数据的格式
GLenum format { GL_DEPTH_COMPONENT };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetCursorPosCB(cursorPosCB);
    initOpenGL.SetMouseCB(mouseCB);

    ShaderProgram program("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.0f,  0.5f, -.5f,     0.0f, 0.0f, 1.0f, // 中上
    };
    // clang-format on

    unsigned int triangleVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // FBO帧缓冲对象
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &depthTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
    // 纹理数据的格式和内部格式为 GL_DEPTH_COMPONENT
    // 纹理数据的类型为GL_FLOAT
    glTexImage2D(GL_TEXTURE_2D, 0, format, windowWidth, windowHeight, 0, format, dataType, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 附加深度缓冲纹理到帧缓冲 GL_DEPTH_ATTACHMENT
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureBuffer, 0);

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
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // 开启深度测试，不然深度缓冲中的数据全部为1
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // 自定义的FBO使用完深度测试后关闭深度测试
        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO，在默认的帧缓冲上也绘制一个和自定义帧缓冲上一样的三角形
        // 为了使用鼠标获取深度值时确定三角形范围
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        // 直接使用NDC坐标，不做任何变换
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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

void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    switch (action)
    {
    case GLFW_PRESS:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
        {
            std::cout << "-------------------------------------\n" << mouse_x << '\t' << mouse_y << '\n';

            GLint level = 0; // 细节级别，0是基本图像级别

            // 获取纹理的宽度和高度
            GLint width { 0 }, height { 0 };
            glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);

            // 为像素数据分配内存
            auto pixels = new GLfloat[width * height];

            // 获取像素数据
            glGetTexImage(GL_TEXTURE_2D, level, format, dataType, pixels);
            // 像素数据的索引
            int index = (int)(windowHeight - mouse_y) * width + (int)mouse_x;

            // 源数据（NDC坐标）的z值范围为[-1, 1] 深度缓冲的值范围为[0, 1]
            std::cout << pixels[index] << '\n';
            delete[] pixels;

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        break;
        default:
            break;
        }
    }
}

void cursorPosCB(GLFWwindow* window, double xpos, double ypos)
{
    mouse_x = xpos;
    mouse_y = ypos;
}

#endif // TEST4

#ifdef TEST5

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

unsigned int texColorBuffer { 0 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
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
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
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

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

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

    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    // 纹理数据格式为GL_RGBA（GL_RGB也可以） 纹理数据类型为GL_UNSIGNED_BYTE
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 附加颜色缓冲纹理到帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //----------------------------------------------------------------------------------
    // 渲染缓冲对象 Render Buffer Object
    // 渲染缓冲对象直接将所有的渲染数据储存到它的缓冲中，不会做任何针对纹理格式的转换，让它变为一个更快的可写储存介质。
    // 然而，渲染缓冲对象通常都是只写的，所以你不能读取它们（比如使用纹理访问）。
    // 当然你仍然能够使用glReadPixels来读取它，这会从当前绑定的帧缓冲，而不是附件本身中返回特定区域的像素。
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    // 存储深度缓冲到渲染缓冲对象中
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);

    // 附加渲染缓冲对象到帧缓冲
    // 因为只需要对深度值进行写操作，所以可以使用 GL_DRAW_FRAMEBUFFER
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // 为自定义的帧缓冲开启深度测试
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 随时间绕xy轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 自定义的帧缓冲使用完毕后关闭深度测试
        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO，只是简单地绘制一张2D纹理
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);
    // 删除渲染缓冲
    glDeleteRenderbuffers(1, &RBO);

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

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

unsigned int texColorBuffer { 0 };
unsigned int texDepthBuffer { 0 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
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
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
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

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

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

    //----------------------------------------------------------------------------------
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 附加颜色缓冲纹理到帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //----------------------------------------------------------------------------------
    glGenTextures(1, &texDepthBuffer);
    glBindTexture(GL_TEXTURE_2D, texDepthBuffer);
    // 纹理的数据格式和内部格式为 GL_DEPTH_COMPONENT
    // 纹理的数据类型为 GL_FLOAT 也可以为其他类型，例如 GL_UNSIGNED_BYTE、GL_UNSIGNED_INT、GL_INT 等
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 附加深度缓冲纹理到帧缓冲
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texDepthBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------
    // 渲染缓冲对象和纹理的区别：
    // 渲染缓冲对象能为你的帧缓冲对象提供一些优化，但知道什么时候使用渲染缓冲对象，什么时候使用纹理是很重要的。
    // 通常的规则是，如果你不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象会是明智的选择。
    // 如果你需要从缓冲中采样颜色或深度值等数据，那么你应该选择纹理附件。性能方面它不会产生非常大的影响的。

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // 为自定义的帧缓冲开启深度测试
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 随时间绕xy轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 自定义的帧缓冲使用完毕后关闭深度测试
        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO，只是简单地绘制一张2D纹理
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffers

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

unsigned int texColorBuffer { 0 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
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
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
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

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

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

    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //----------------------------------------------------------------------------------
    // 渲染缓冲对象 Render Buffer Object
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    // GL_DEPTH24_STENCIL8 表示一个 uint32 类型的数据前24位为深度信息，后8位为模板信息
    //   depth = 0xFFFFFF00 & pixelsData
    // stencil = 0x000000FF & pixelsData
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    // 附加附件为深度、模板缓冲的渲染缓冲对象到帧缓冲
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // 为自定义的帧缓冲开启深度测试
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 随时间绕xy轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 自定义的帧缓冲使用完毕后关闭深度测试
        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO，只是简单地绘制一张2D纹理
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);
    // 删除渲染缓冲
    glDeleteRenderbuffers(1, &RBO);

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

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

unsigned int texColorBuffer { 0 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST7_Default_FBO.vs", "resources/02_04_05_TEST7_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST7_Custom_FBO.vs", "resources/02_04_05_TEST7_Custom_FBO.fs");

    // clang-format off
    // 立方体
    std::array<GLfloat, 36 * 5> verticesCube{
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    unsigned int cubeVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCube.size(), verticesCube.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    Texture myTexture("resources/02_04_05_wall.jpg");
    // Texture myTexture("resources/02_04_05_container.jpg");

    // 注意，核在对屏幕纹理的边缘进行采样的时候，由于还会对中心像素周围的8个像素进行采样，其实会取到纹理之外的像素。
    // 由于环绕方式默认是GL_REPEAT，所以在没有设置的情况下取到的是屏幕另一边的像素，
    // 而另一边的像素本不应该对中心像素产生影响，这就可能会在屏幕边缘产生很奇怪的条纹。
    // 为了消除这一问题，我们可以将屏幕纹理的环绕方式都设置为GL_CLAMP_TO_EDGE。
    // 这样子在取到纹理外的像素时，就能够重复边缘的像素来更精确地估计最终的值了。
    myTexture.SetWarpParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    //----------------------------------------------------------------------------------
    // FBO帧缓冲对象
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //----------------------------------------------------------------------------------
    // 纹理
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    //----------------------------------------------------------------------------------
    // RBO渲染缓冲对象
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // 为自定义的帧缓冲开启深度测试
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 随时间绕xy轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        myTexture.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verticesCube.size()) / 5);
        glBindVertexArray(0);

        // 自定义的帧缓冲使用完毕后关闭深度测试
        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO，只是简单地绘制一张2D纹理
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);
    // 删除渲染缓冲
    glDeleteRenderbuffers(1, &RBO);

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

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursorPosCB(GLFWwindow* w, double x, double y);
void mouseCB(GLFWwindow* window, int button, int action, int mods);

// 窗口大小
constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

// 鼠标的位置
double mouse_x { 0 };
double mouse_y { 0 };

GLuint FBO { 0 };
GLuint texColorBuffer;

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetCursorPosCB(cursorPosCB);
    initOpenGL.SetMouseCB(mouseCB);

    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
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
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    // 矩形，绘制到默认帧缓冲
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
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

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    // FBO帧缓冲对象
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 颜色纹理附件
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    // 如果这里的数据格式为GL_RGB，后面读取时alpha始终都为1
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

    // RBO 渲染缓冲对象
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 随时间绕y轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);

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

void cursorPosCB(GLFWwindow* w, double x, double y)
{
    mouse_x = x;
    mouse_y = y;
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
            std::cout << "-------------------------------------\n" << mouse_x << '\t' << mouse_y << '\t';

            // 指定后续的操作对自定义的帧缓冲生效
            glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

            // glReadBuffer指定一个颜色缓冲区作为后续glReadPixels、glCopyTexImage1D、glCopyTexImage2D、glCopyTexSubImage1D，
            // glCopyTexSubImage2D和glCopyTexSubImage3D命令的源。模式接受十二个或多个预定义值中的一个。
            // 在完全配置的系统中，GL_FRONT、GL_LEFT和GL_FRONT_LEFT都命名为左前缓冲器，
            // GL_FRONT_RIGHT和GL_RIGHT命名为右前缓冲器，而GL_BACK_LEFT和GL_BACK命名为左后缓冲器。
            // 此外，常数GL_COLOR_ATTACHMENTi可用于指示第i个颜色附件，其中i的范围从零到GL_MAX_COLOR_TTACHMENTS的值减去1。
            // 这里不调用glReadBuffer，只要绑定了自定义FBO，也可以读取像素
            glReadBuffer(GL_COLOR_ATTACHMENT0);

            auto pixel = new GLubyte[4]();
            glReadPixels(int(mouse_x), (int)(windowHeight - mouse_y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

            // 获取alpha值需要在附加帧缓冲纹理时数据格式设置为GL_RGBA
            std::cout << "r: " << (int)pixel[0] << '\t' << "g: " << (int)pixel[1] << '\t' << "b: " << (int)pixel[2] << '\t' << "a: " << (int)pixel[3]
                      << '\n';

            glReadBuffer(GL_NONE);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

            delete[] pixel;
            pixel = nullptr;
        }
        break;
        case GLFW_MOUSE_BUTTON_RIGHT:
        {
            GLint level = 0; // 细节级别，0是基本图像级别

            // 获取纹理的宽度和高度
            GLint width { 0 }, height { 0 };
            glBindTexture(GL_TEXTURE_2D, texColorBuffer);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);

            // 为像素数据分配内存
            auto pixels = new GLubyte[width * height * 4];

            // 获取像素数据
            glGetTexImage(GL_TEXTURE_2D, level, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            // 像素数据的索引
            int index = (int)(windowHeight - mouse_y) * width + (int)mouse_x;

            std::cout << "r: " << (int)pixels[index * 4 + 0] << '\t' << "g: " << (int)pixels[index * 4 + 1] << '\t'
                      << "b: " << (int)pixels[index * 4 + 2] << '\t' << "a: " << (int)pixels[index * 4 + 3] << '\n';

            delete[] pixels;
            pixels = nullptr;

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        break;
        default:
            break;
        }
    }
}

#endif // TEST9

#ifdef TEST10

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

GLuint windowWidth { 800 };
GLuint windowHeight { 600 };

Texture* myTexture { nullptr };
unsigned int RBO;

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST7_Default_FBO.vs", "resources/02_04_05_TEST7_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 6> verticesCube{
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
    std::array<GLuint,6 * 2 * 3> indicesCube{
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

    // 小窗口坐标（NDC）
    std::array<GLfloat, 5 * 6> vertices_quad {
        // pos                    // texCoords
        0.8f,   1.0f,   0.0f,     0.0f, 1.0f,
        0.8f,   0.8f,   0.0f,     0.0f, 0.0f,
        1.0f,   0.8f,   0.0f,     1.0f, 0.0f,

        0.8f,   1.0f,   0.0f,     0.0f, 1.0f,
        1.0f,   0.8f,   0.0f,     1.0f, 0.0f,
        1.0f,   1.0f,   0.0f,     1.0f, 1.0f
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

    unsigned int quadVAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices_quad.size(), vertices_quad.data(), GL_STATIC_DRAW);

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

    myTexture = new Texture(windowWidth, windowHeight, GL_RGBA);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myTexture->Get(), 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    // 将当前激活的帧缓冲绑定为默认帧缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 随时间绕xy轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 先将旋转的立方体绘制一次
        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 再在窗口的右上角绘制一个小矩形，并将自定义FBO的纹理绘制上去
        // 这样就有了一个小窗口的效果，为了显示效果明显，对纹理进行了后处理
        defaultFBOProgram.Use();
        glBindVertexArray(quadVAO);
        myTexture->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    // 删除帧缓冲
    glDeleteFramebuffers(1, &FBO);
    // 删除渲染缓冲
    glDeleteRenderbuffers(1, &RBO);

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
    windowWidth  = width;
    windowHeight = height;

    // 当窗口大小改变时，纹理大小和渲染缓冲大小都需要更改

    glViewport(0, 0, windowWidth, windowHeight);
    myTexture->ResetSize(windowWidth, windowHeight);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

#endif // TEST10

#ifdef TEST11

#include <common.hpp>

uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };

constexpr uint32_t texWidth { 1920 };
constexpr uint32_t texHeight { 1080 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST1_Custom_FBO.fs");

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
        0, 1, 3,        1, 2, 3,
        1, 5, 2,        5, 6, 2,
        5, 4, 6,        4, 7, 6,
        4, 0, 7,        0, 3, 7,
        3, 2, 7,        2, 6, 7,
        0, 1, 4,        1, 5, 4,
    };

    // 矩形，绘制到默认帧缓冲
    std::vector<GLfloat> vertices_quad {
        // pos                     // texCoords
        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
        -0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

        -0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
         0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
         0.8f,   0.8f,   0.0f,     1.0f, 1.0f
    };
    // clang-format on

    Renderer cube(verticesCube, indicesCube, { 3, 3 });
    Renderer quad(vertices_quad, { 3, 2 });

    Texture texture(texWidth, texHeight, GL_RGBA);
    RenderBufferObject rbo(GL_DEPTH_COMPONENT, texWidth, texHeight);

    FrameBufferObject fbo;
    fbo.AddAttachment(GL_COLOR_ATTACHMENT0, texture, 0);
    fbo.AddAttachment(GL_DEPTH_ATTACHMENT, rbo);
    if (!fbo.Check())
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        // 立方体
        fbo.Bind();
        glViewport(0, 0, texWidth, texHeight);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);
        cube.Draw(GL_TRIANGLES);

        glDisable(GL_DEPTH_TEST);
        fbo.Release();

        //----------------------------------------------------------------------------------
        // 四边形
        glViewport(0, 0, windowWidth, windowHeight);
        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();
        texture.Bind();
        quad.Draw(GL_TRIANGLES);
        texture.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth  = width;
    windowHeight = height;
}

#endif // TEST11

#ifdef TEST12

#include <array>
#include <common.hpp>

constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();
    ShaderProgram defaultFBOProgram("resources/02_04_05_TEST1_Default_FBO.vs", "resources/02_04_05_TEST1_Default_FBO.fs");
    ShaderProgram customFBOProgram("resources/02_04_05_TEST1_Custom_FBO.vs", "resources/02_04_05_TEST12_Custom_FBO.fs");

    // clang-format off
    std::vector<GLfloat> vertices{
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };

    std::vector<GLfloat> vertices_quad1 {
        -0.9f, -0.9f, 0.f,  0.f, 0.f,
        -0.1f, -0.9f, 0.f,  1.f, 0.f,
        -0.9f, -0.1f, 0.f,  0.f, 1.f,
        -0.1f, -0.1f, 0.f,  1.f, 1.f
    };
    std::vector<GLfloat> vertices_quad2 {
         0.1f,  0.1f, 0.f,  0.f, 0.f,
         0.9f,  0.1f, 0.f,  1.f, 0.f,
         0.1f,  0.9f, 0.f,  0.f, 1.f,
         0.9f,  0.9f, 0.f,  1.f, 1.f
    };
    // clang-format on

    Renderer triangle(vertices, { 3 });
    Renderer quad1(vertices_quad1, { 3, 2 });
    Renderer quad2(vertices_quad2, { 3, 2 });

    //----------------------------------------------------------------------------------
    // 纹理
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int texColorBuffer1;
    glGenTextures(1, &texColorBuffer1);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // FBO帧缓冲对象
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texColorBuffer1, 0);

    // 设置OpenGL渲染到多个（此处为2）颜色缓存，否则只会渲染到帧缓冲的第一个颜色附件
    // 需要在绑定FBO之后使用，否则渲染到的是默认的帧缓冲
    // GL_COLOR_ATTACHMENT0 对应于片段着色器中的 layout (location = 0) 其他附件类似
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        // 绑定自定义FBO
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 随时间绕y轴旋转
        auto modelMat       = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat        = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        customFBOProgram.Use();
        customFBOProgram.SetUniformMat4("transform", projectiongMat * viewMat * modelMat);

        triangle.Draw(GL_TRIANGLES);

        //----------------------------------------------------------------------------------
        // 绑定默认的FBO
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        defaultFBOProgram.Use();

        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        quad1.Draw(GL_TRIANGLE_STRIP);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer1);
        quad2.Draw(GL_TRIANGLE_STRIP);

        //----------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &texColorBuffer);
    glDeleteTextures(1, &texColorBuffer1);

    glfwTerminate();
    return 0;
}

#endif // TEST12