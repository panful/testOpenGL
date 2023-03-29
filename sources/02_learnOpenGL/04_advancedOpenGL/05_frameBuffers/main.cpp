/*
 * 1. 帧缓冲基础用法，附加颜色缓冲纹理到帧缓冲
 * 2. 修改附加的颜色缓冲纹理大小
 */

#define TEST2

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
        auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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
    defaultFBOProgram.DeleteProgram();
    customFBOProgram.DeleteProgram();

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
        auto modelMat = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(0.f, 1.f, 0.f));
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
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
    defaultFBOProgram.DeleteProgram();
    customFBOProgram.DeleteProgram();

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
    std::cout << "width: " << width << "\theigth: " << height << '\n';
    glViewport(0, 0, width, height);

    if (texColorBuffer)
    {
        texColorBuffer->ResetSize(width, height);
    }
}

#endif // TEST2
