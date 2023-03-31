/*
 * 1. 立方体贴图
 */

#define TEST1

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_04_06_TEST1.vs", "resources/02_04_06_TEST1.fs");

    // clang-format off
    // 8个顶点
    std::array<GLfloat, 8 * 3> verticesCube{
        // pos
        -1.f, -1.f,  1.f,   // 前左下
         1.f, -1.f,  1.f,   // 前右下
         1.f,  1.f,  1.f,   // 前右上
        -1.f,  1.f,  1.f,   // 前左上

        -1.f, -1.f, -1.f,   // 后左下
         1.f, -1.f, -1.f,   // 后右下
         1.f,  1.f, -1.f,   // 后右上
        -1.f,  1.f, -1.f,   // 后左上
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

    std::array<std::string_view, 6> cubeFaces{
        "resources/02_04_06_skybox_right.jpg",
        "resources/02_04_06_skybox_left.jpg",
        "resources/02_04_06_skybox_top.jpg",
        "resources/02_04_06_skybox_bottom.jpg",
        "resources/02_04_06_skybox_front.jpg",
        "resources/02_04_06_skybox_back.jpg"
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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
    }

    GLuint cubeTexture { 0 };
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

    int width { 0 }, height { 0 }, comp { 0 };
    for (GLenum i = 0; i < 6; i++)
    {
        if (auto imageData = stbi_load(cubeFaces.at(i).data(), &width, &height, &comp, 0); imageData && comp == 3)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        }
        else
        {
            std::cerr << "load image failed!\n";
            return -1;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // GL_TEXTURE_WRAP_R为纹理的R坐标设置环绕方式，相当于z坐标
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //----------------------------------------------------------------------------------
    // 立方体纹理通过一个方向向量来进行采样，方向向量的原点位于立方体的中心
    // 使用立方体的位置坐标值作为纹理的坐标值，对立方体做模型变换并不会影响纹理采样

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 不做缩放变换
        auto scaleMat = glm::mat4(1.f);
        // 随时间绕y轴旋转
        auto rotateMat = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() / 2.f, glm::vec3(0, 1, 0));
        // 不做位移变换，即立方体的中心仍为(0,0,0)
        auto translateMat = glm::mat4(1.f);
        auto modleMat = translateMat * rotateMat * scaleMat;
        auto viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        // 将观察矩阵的平移部分移除，这样就相当于相机位置始终在原点(0,0,0)，立方体不会被拉近或者拉远
        viewMat = glm::mat4(glm::mat3(viewMat));
        auto projectiongMat = glm::perspective(glm::radians(30.0f), 8 / 6.f, 0.1f, 100.f);

        program.Use();
        program.SetUniformMat4("transform", projectiongMat * viewMat * modleMat);

        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indicesCube.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete the buffer
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

#endif // TEST1
