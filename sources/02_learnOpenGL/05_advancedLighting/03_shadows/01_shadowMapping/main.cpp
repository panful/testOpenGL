/*
 * 1. 绘制深度信息到帧缓冲并显示以及打印（深度贴图）
 * 2. 渲染物体的阴影并和不开启阴影进行比较
 */

#define TEST2

#ifdef TEST1

#include <common.hpp>

// 窗口的大小
constexpr uint32_t windowWidth { 800 };
constexpr uint32_t windowHeight { 600 };
double mouse_x { 0.0 };
double mouse_y { 0.0 };
GLuint depthMappingFBO { 0 };

void mouseCB(GLFWwindow* window, int button, int action, int mods);
void cursorPosCB(GLFWwindow* window, double xpos, double ypos);

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    initOpenGL.SetCursorPosCB(cursorPosCB);
    initOpenGL.SetMouseCB(mouseCB);
    auto window = initOpenGL.GetWindow();

    ShaderProgram program("resources/02_05_03_01_TEST1_depth.vs", "resources/02_05_03_01_TEST1_depth.fs");
    ShaderProgram rectProgram("resources/02_05_03_01_TEST1_FBO.vs", "resources/02_05_03_01_TEST1_FBO.fs");

    // clang-format off
    std::vector<GLfloat> verticesCube{
        -0.5f, -0.5f, 0.5f, 
         0.5f, -0.5f, 0.5f, 
         0.5f,  0.5f, 0.5f, 
        -0.5f,  0.5f, 0.5f, 

        -0.5f, -0.5f, -.5f, 
         0.5f, -0.5f, -.5f, 
         0.5f,  0.5f, -.5f, 
        -0.5f,  0.5f, -.5f, 
    };

    std::vector<GLuint> indicesCube{
        0, 1, 3,    1, 2, 3, // 前
        1, 5, 2,    5, 6, 2, // 右
        5, 4, 6,    4, 7, 6, // 后
        4, 0, 7,    0, 3, 7, // 左  
        3, 2, 7,    2, 6, 7, // 上
        0, 1, 4,    1, 5, 4, // 下
    };

    std::vector<GLfloat> verticesPlane{
        -1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
    };

    std::vector<GLfloat> verticesRect{
        -1.f, -1.f,     0.f, 0.f,
         1.f, -1.f,     1.f, 0.f,
        -1.f,  1.f,     0.f, 1.f,
         1.f,  1.f,     1.f, 1.f,
    };
    // clang-format on

    Renderer cube(verticesCube, indicesCube, { 3 });
    Renderer plane(verticesPlane, { 3 });
    Renderer rect(verticesRect, { 2, 2 });

    //----------------------------------------------------------------------------------
    GLuint depthTextureBuffer { 0 };
    glGenTextures(1, &depthTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint depthMappingFBO;
    glGenFramebuffers(1, &depthMappingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------

    auto viewPos = glm::vec3(0.f, 0.f, 5.f);

    auto scaleMat     = glm::mat4(1.f);
    auto rotateMat    = glm::mat4(1.f);
    auto translateMat = glm::mat4(1.f);
    auto viewMat      = glm::lookAt(viewPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
    // 为了便于观察将近远平面的间距设置足够小
    auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 4.f, 6.f);

    while (!glfwWindowShouldClose(window))
    {
        //----------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        program.Use();

        program.SetUniformMat4("transform", projectiongMat * viewMat * translateMat * rotateMat * scaleMat);
        cube.Draw(GL_TRIANGLES);

        program.SetUniformMat4("transform", projectiongMat * viewMat * translateMat * rotateMat * scaleMat);
        plane.Draw(GL_TRIANGLE_STRIP);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        rectProgram.Use();
        glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
        rect.Draw(GL_TRIANGLE_STRIP);

        //----------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
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

            // 按下鼠标左键打印像素值，红绿蓝分量都是一个值
            auto pixels = new GLubyte[4]();
            glReadPixels((GLint)mouse_x, (GLint)mouse_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
            std::cout << "pixels: " << (int)pixels[0] << '\n';
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

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

// 窗口的大小
uint32_t windowWidth { 800 };
uint32_t windowHeight { 600 };
// FBO纹理的大小
// 深度贴图（纹理）的大小影响阴影的质量，深度贴图太小就会出现走样
constexpr uint32_t textureWidth { 4096 };
constexpr uint32_t textureHeight { 4096 };
// 鼠标的位置
double mouse_x { 0.0 };
double mouse_y { 0.0 };
// 深度贴图FBO
GLuint depthMappingFBO { 0 };
// 是否开启阴影
bool isShadow { false };

void mouseCB(GLFWwindow* window, int button, int action, int mods);
void cursorPosCB(GLFWwindow* window, double xpos, double ypos);
void frameResize(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL("Frame Buffer", windowWidth, windowHeight);
    initOpenGL.SetCursorPosCB(cursorPosCB);
    initOpenGL.SetMouseCB(mouseCB);
    initOpenGL.SetFramebufferSizeCB(frameResize);
    auto window = initOpenGL.GetWindow();

    ShaderProgram depthProgram("resources/02_05_03_01_TEST2_depth.vs", "resources/02_05_03_01_TEST2_depth.fs");
    ShaderProgram lightProgram("resources/02_05_03_01_TEST2_light.vs", "resources/02_05_03_01_TEST2_light.fs");

    // clang-format off
    std::vector<GLfloat> verticesCube{
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left     
    };

    std::vector<GLfloat> verticesPlane{
        // positions            // normals         // texcoords
         25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, 0.0f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };

    // clang-format on

    Renderer cube(verticesCube, { 3, 3, 2 });
    Renderer plane(verticesPlane, { 3, 3, 2 });

    Texture woodTexture("resources/02_05_03_01_wood.png", 0);
    Texture rectpolyTexture("resources/02_05_03_01_rectpoly.jpg", 0);

    //----------------------------------------------------------------------------------
    GLuint depthTextureBuffer { 0 };
    glGenTextures(1, &depthTextureBuffer);
    glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, textureWidth, textureHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint depthMappingFBO;
    glGenFramebuffers(1, &depthMappingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::clog << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //----------------------------------------------------------------------------------
    auto viewPos  = glm::vec3(0.f, 20.f, 0.f);
    auto lightPos = glm::vec3(-5.f, 20.f, -5.f);

    // 模型变换矩阵，模型变换矩阵始终都是同一个，不论是在那个FBO
    auto modelMatPlane = glm::mat4(1.f);
    auto modelMatCube  = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 5.f, 0.f)) * glm::scale(glm::mat4(1.f), glm::vec3(.5f));

    // 实际显示窗口的VP矩阵
    auto viewMat        = glm::lookAt(viewPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));
    auto projectiongMat = glm::perspective(glm::radians(30.f), 8 / 6.f, 0.1f, 100.f);

    // 用来生成深度贴图的VP矩阵
    auto viewMatLightSpace       = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0.f, 0.f, 1.f));
    auto projectionMatLightSpace = glm::ortho(-50.f, 50.f, -50.f, 50.f, -50.f, 50.f);

    //----------------------------------------------------------------------------------
    // 按S开启阴影，按F关闭阴影
    // 阴影贴图只能实现硬阴影，不能实现软阴影
    // 只能处理点光源的阴影

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //----------------------------------------------------------------------------------
        // 先在深度贴图FBO中将整个场景渲染一次
        glBindFramebuffer(GL_FRAMEBUFFER, depthMappingFBO);
        glViewport(0, 0, textureWidth, textureHeight);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthProgram.Use();

        depthProgram.SetUniformMat4("transform", projectionMatLightSpace * viewMatLightSpace * modelMatCube);
        cube.Draw(GL_TRIANGLES);

        depthProgram.SetUniformMat4("transform", projectionMatLightSpace * viewMatLightSpace * modelMatPlane);
        plane.Draw(GL_TRIANGLES);

        glDisable(GL_DEPTH_TEST);

        //----------------------------------------------------------------------------------
        // 正常渲染一次整个场景，并将深度贴图绑定到着色器
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightProgram.Use();

        lightProgram.SetUniform1i("IsShadow", isShadow);

        lightProgram.SetUniform1i("diffuseTexture", 0);
        lightProgram.SetUniform1i("shadowMap", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTextureBuffer);

        lightProgram.SetUniform3fv("lightPos", lightPos);
        lightProgram.SetUniform3fv("viewPos", viewPos);
        lightProgram.SetUniformMat4("projection", projectiongMat);
        lightProgram.SetUniformMat4("view", viewMat);

        lightProgram.SetUniformMat4("lightSpaceMatrix", projectionMatLightSpace * viewMatLightSpace);
        lightProgram.SetUniformMat4("model", modelMatCube);
        rectpolyTexture.Bind();
        cube.Draw(GL_TRIANGLES);
        rectpolyTexture.Release();

        lightProgram.SetUniformMat4("lightSpaceMatrix", projectionMatLightSpace * viewMatLightSpace);
        lightProgram.SetUniformMat4("model", modelMatPlane);
        woodTexture.Bind();
        plane.Draw(GL_TRIANGLES);
        woodTexture.Release();

        //----------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer

    glfwTerminate();
    return 0;
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

            // 按下鼠标左键打印像素值
            auto pixels = new GLubyte[4]();
            glReadPixels((GLint)mouse_x, (GLint)mouse_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixels);
            std::cout << "pixels: " << (int)pixels[0] << '\t' << (int)pixels[1] << '\t' << (int)pixels[2] << '\t' << (int)pixels[3] << '\n';
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

void frameResize(GLFWwindow* window, int width, int height)
{
    windowWidth  = static_cast<uint32_t>(width);
    windowHeight = static_cast<uint32_t>(height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        isShadow = true;
        std::cout << "shadow: " << isShadow << '\n';
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        isShadow = false;
        std::cout << "shadow: " << isShadow << '\n';
    }
}

#endif // TEST2
