/*
 * 1. phong模型的缺陷，blinn-phong与phong模型对比
 * 2. 设置反光度让blinn-phong和phong模型光照效果差别更小一些
 */

#define TEST2

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

bool blinn { false };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram program("resources/02_05_01_TEST1.vs", "resources/02_05_01_TEST1.fs");

    // clang-format off
    // 纹理坐标设置为10可以重复更多纹理
    std::array<GLfloat, 6 * 8>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    unsigned int VAO { 0 };
    {
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // texture coord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    }

    Texture texture("resources/wood.png");

    //----------------------------------------------------------------------------------
    // 使用'a'切换PHONG模型 使用'b'切换BLINN-PHONG
    // 视线与反射方向的夹角大于90°时，镜面光分量会变为0
    // 因为小于90°时反光度越小，镜面光照就越明显，突然大于等于90°，镜面光照一下子变为0，这时光照变化就会很突兀

    auto viewPos = glm::vec3(0.f, 0.f, 3.f);
    auto lightPos = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor = glm::vec3(1.f);
    auto lightAmbient = .2f * lightColor;
    auto lightDiffuse = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto viewMat = glm::lookAt(viewPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(60.0f), 8 / 6.f, 0.1f, 100.0f);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", viewMat);
        program.SetUniformMat4("projection", projectionMat);
        program.SetUniform3fv("viewPos", viewPos);
        program.SetUniform3fv("light.ambient", lightAmbient);
        program.SetUniform3fv("light.diffuse", lightDiffuse);
        program.SetUniform3fv("light.specular", lightSpecular);
        program.SetUniform3fv("light.position", lightPos);

        program.SetUniform1i("blinn", blinn);

        texture.Bind();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        blinn = true;
        std::cout << "blinn: " << blinn << '\n';
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        blinn = false;
        std::cout << "blinn: " << blinn << '\n';
    }
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

bool blinn { false };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram program("resources/02_05_01_TEST1.vs", "resources/02_05_01_TEST2.fs");

    // clang-format off
    // 纹理坐标设置为10可以重复更多纹理
    std::array<GLfloat, 6 * 8>  vertices{
        // positions            // normals         // texcoords
         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

         10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
        -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
         10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // clang-format on

    unsigned int VAO { 0 };
    {
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

        // normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

        // texture coord
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    }

    Texture texture("resources/wood.png");

    //----------------------------------------------------------------------------------
    // 使用'a'切换PHONG模型 使用'b'切换BLINN-PHONG
    // 想获得和冯氏着色类似的效果，就必须在使用Blinn-Phong模型时将镜面反光度设置更高一点

    auto viewPos = glm::vec3(0.f, 0.f, 3.f);
    auto lightPos = glm::vec3(0.f, 0.f, 0.f);
    auto lightColor = glm::vec3(1.f);
    auto lightAmbient = .2f * lightColor;
    auto lightDiffuse = .5f * lightColor;
    auto lightSpecular = 1.f * lightColor;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto viewMat = glm::lookAt(viewPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projectionMat = glm::perspective(glm::radians(60.0f), 8 / 6.f, 0.1f, 100.0f);

        program.Use();
        program.SetUniformMat4("model", glm::mat4(1.f));
        program.SetUniformMat4("view", viewMat);
        program.SetUniformMat4("projection", projectionMat);
        program.SetUniform3fv("viewPos", viewPos);
        program.SetUniform3fv("light.ambient", lightAmbient);
        program.SetUniform3fv("light.diffuse", lightDiffuse);
        program.SetUniform3fv("light.specular", lightSpecular);
        program.SetUniform3fv("light.position", lightPos);

        program.SetUniform1i("blinn", blinn);

        texture.Bind();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        blinn = true;
        std::cout << "blinn: " << blinn << '\n';
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        blinn = false;
        std::cout << "blinn: " << blinn << '\n';
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST2