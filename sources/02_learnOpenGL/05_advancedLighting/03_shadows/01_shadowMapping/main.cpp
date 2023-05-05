/*
 * 1. 绘制深度信息到帧缓冲并显示（深度贴图）
 */

#define TEST1

#ifdef TEST1

#include <array>
#include <common.hpp>

// 窗口的大小
constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    auto window = initOpenGL.GetWindow();

    ShaderProgram program("resources/02_05_03_01_TEST1.vs", "resources/02_05_03_01_TEST1.fs");
    ShaderProgram rectProgram("resources/02_05_03_01_FBO_TEST1.vs", "resources/02_05_03_01_FBO_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 3 * 3> verticesTriangle{
        // pos                  
        -0.5f, -0.5f, 0.5f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, -.5f,
    };
    std::array<GLfloat, 4 * 4> verticesRect{
        -1.f, -1.f,     0.f, 0.f,
         1.f, -1.f,     1.f, 0.f,
        -1.f,  1.f,     0.f, 1.f,
         1.f,  1.f,     1.f, 1.f,
    };
    // clang-format on

    unsigned int triangleVAO { 0 };
    {
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &triangleVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(triangleVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesTriangle.size(), verticesTriangle.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    }
    unsigned int rectVAO { 0 };
    {
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesRect.size(), verticesRect.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    }

    //----------------------------------------------------------------------------------
    GLuint depthTextureBuffer { 0 };
    glGenTextures(1, &depthTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
    // 数据格式 GL_DEPTH_COMPONENT
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // 深度附件 GL_DEPTH_ATTACHMENT
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
        //----------------------------------------------------------------------------------
        // 自定义帧缓冲，只需要深度信息，因此只需要清除深度缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::rotate(glm::mat4(1.f), (float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f)));
        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 默认帧缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        rectProgram.Use();
        glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
        glBindVertexArray(rectVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        //----------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
}

#endif // TEST1
