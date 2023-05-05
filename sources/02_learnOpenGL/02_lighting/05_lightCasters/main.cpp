/*
 * 1. 平行光源，太阳光
 * 2. 使用键盘左右键切换点光源和平行光源
 * 3. 对点光源设置衰减
 * 4. 聚光灯，类似手电筒的光源
 * 5. 平滑聚光灯圆锥边缘的光照过渡
 */

#define TEST5

#ifdef TEST1

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("resources/02_02_05_TEST1_map.vs", "resources/02_02_05_TEST1_map.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 9个立方体的位置
    std::array<glm::vec3, 9> cubePosition {
        glm::vec3(-5.f),
        glm::vec3(-3.f),
        glm::vec3(-1.f),
        glm::vec3(1.f),
        glm::vec3(3.f),
        glm::vec3(-5.f, 5.f, -5.f),
        glm::vec3(-2.f, 2.f, -2.f),
        glm::vec3(2.f, -2.f, 2.f),
        glm::vec3(5.f, -5.f, 4.f),
    };

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 25.f);
    // 材质的反光度
    auto materialShininess = 32.f;
    // 光源方向(指向屏幕里面的平行光照)
    auto lightDirection = glm::vec3(0.f, 0.f, -1.f);
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f);
    auto ambientColor = lightColor * glm::vec3(0.2f);
    auto specularColor = lightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 9个随着时间旋转的立方体，可以将平行光源设置为点光源比较两者的区别
    // 使用点光源：物体法线、光源到物体的方向平行时，镜面反射颜色比较明显
    // 平行光源：物体法线、光源方向平行时，镜面反射颜色就会很明显
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < cubePosition.size(); i++)
        {
            cubeProgram.Use();
            texture_diffuse.Bind();
            texture_specular.Bind();

            cubeProgram.SetUniform1i("material.diffuse", 0);
            cubeProgram.SetUniform1i("material.specular", 1);

            auto model = glm::translate(glm::mat4(1.f), cubePosition.at(i))
                * glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / (i + 1)), glm::vec3(1.f, 1.f, 0.f));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);
            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.direction", lightDirection);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);

            cubeProgram.SetUniformMat4("model", model);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            texture_diffuse.Release();
            texture_specular.Release();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// 光源的方向（平行光）或位置（点光源）
glm::vec3 lightDirectionOrPosition = glm::vec3(0.f, 0.f, 25.f);
// 是否是点光源，0表示不是点光源
int pointLight { 1 };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("resources/02_02_05_TEST1_map.vs", "resources/02_02_05_TEST2_map.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 9个立方体的位置
    std::array<glm::vec3, 9> cubePosition {
        glm::vec3(-5.f),
        glm::vec3(-3.f),
        glm::vec3(-1.f),
        glm::vec3(1.f),
        glm::vec3(3.f),
        glm::vec3(-5.f, 5.f, -5.f),
        glm::vec3(-2.f, 2.f, -2.f),
        glm::vec3(2.f, -2.f, 2.f),
        glm::vec3(5.f, -5.f, 4.f),
    };

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 25.f);
    // 材质的反光度
    auto materialShininess = 32.f;
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f);
    auto ambientColor = lightColor * glm::vec3(0.2f);
    auto specularColor = lightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < cubePosition.size(); i++)
        {
            cubeProgram.Use();
            texture_diffuse.Bind();
            texture_specular.Bind();

            cubeProgram.SetUniform1i("material.diffuse", 0);
            cubeProgram.SetUniform1i("material.specular", 1);
            cubeProgram.SetUniform1i("pointLight", pointLight);

            auto model = glm::translate(glm::mat4(1.f), cubePosition.at(i))
                * glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / (i + 1)), glm::vec3(1.f, 1.f, 0.f));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);
            cubeProgram.SetUniform3fv("viewPos", viewPos);

            cubeProgram.SetUniform3fv("light.direction", lightDirectionOrPosition);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);

            cubeProgram.SetUniformMat4("model", model);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            texture_diffuse.Release();
            texture_specular.Release();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 键盘左键：点光源，光源位置和相机位置一样
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        pointLight = 1;
        lightDirectionOrPosition = glm::vec3(0.f, 0.f, 25.f);
        std::clog << "point light\n";
    }
    // 键盘右键：平行光源，光源指向屏幕里面
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        pointLight = 0;
        lightDirectionOrPosition = glm::vec3(0.f, 0.f, -1.f);
        std::clog << "direction light\n";
    }
}

#endif // TEST2

#ifdef TEST3

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("resources/02_02_05_TEST1_map.vs", "resources/02_02_05_TEST3_map.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 9个立方体的位置
    std::array<glm::vec3, 9> cubePosition {
        glm::vec3(-5.f),
        glm::vec3(-3.f),
        glm::vec3(-1.f),
        glm::vec3(1.f),
        glm::vec3(3.f),
        glm::vec3(-5.f, 5.f, -20.f),
        glm::vec3(-2.f, 2.f, -10.f),
        glm::vec3(2.f, -2.f, 5.f),
        glm::vec3(5.f, -5.f, 10.f),
    };

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 25.f);
    // 材质的反光度
    auto materialShininess = 32.f;
    // 光源位置(点光源)，和相机在同一位置
    auto lightPosition = glm::vec3(0.f, 0.f, 25.f);
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f);
    auto ambientColor = lightColor * glm::vec3(0.2f);
    auto specularColor = lightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    // 物体距离光源越远，物体的颜色越暗，即越黑越看不清颜色
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (size_t i = 0; i < cubePosition.size(); i++)
        {
            cubeProgram.Use();
            texture_diffuse.Bind();
            texture_specular.Bind();

            cubeProgram.SetUniform1i("material.diffuse", 0);
            cubeProgram.SetUniform1i("material.specular", 1);

            auto model = glm::translate(glm::mat4(1.f), cubePosition.at(i))
                * glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / (i + 1)), glm::vec3(1.f, 1.f, 0.f));
            auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
            auto projection = glm::perspective(glm::radians(45.f), 8.f / 6.f, 0.1f, 100.f);
            cubeProgram.SetUniform3fv("viewPos", viewPos);

            // 设置光源衰减参数（距离325）
            cubeProgram.SetUniform1f("light.constant", 1.0f);
            cubeProgram.SetUniform1f("light.linear", 0.014f);
            cubeProgram.SetUniform1f("light.quadratic", 0.0007f);

            cubeProgram.SetUniform3fv("light.position", lightPosition);
            cubeProgram.SetUniform3fv("light.ambient", ambientColor);
            cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
            cubeProgram.SetUniform3fv("light.specular", specularColor);

            cubeProgram.SetUniform1f("material.shininess", materialShininess);

            cubeProgram.SetUniformMat4("model", model);
            cubeProgram.SetUniformMat4("view", view);
            cubeProgram.SetUniformMat4("projection", projection);

            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            texture_diffuse.Release();
            texture_specular.Release();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("resources/02_02_05_TEST1_map.vs", "resources/02_02_05_TEST4_map.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 4.f);
    // 材质的反光度
    auto materialShininess = 32.f;
    // 光源位置(聚光灯)，和相机在同一位置
    auto lightPosition = viewPos;
    // 光源方向，指向屏幕里面
    auto lightDirection = glm::vec3(0.f, 0.f, -1.f);
    // 聚光灯圆锥张开的角度的余弦值
    auto lightCutOff = glm::cos(glm::radians(5.f));
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f);
    auto ambientColor = lightColor * glm::vec3(0.2f);
    auto specularColor = lightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeProgram.Use();
        texture_diffuse.Bind();
        texture_specular.Bind();

        cubeProgram.SetUniform1i("material.diffuse", 0);
        cubeProgram.SetUniform1i("material.specular", 1);

        auto model = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime()), glm::vec3(1.f, 1.f, 0.f));
        auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);
        cubeProgram.SetUniform3fv("viewPos", viewPos);

        // 设置聚光灯参数
        cubeProgram.SetUniform3fv("light.position", lightPosition);
        cubeProgram.SetUniform3fv("light.direction", lightDirection);
        cubeProgram.SetUniform1f("light.cutOff", lightCutOff);

        cubeProgram.SetUniform3fv("light.ambient", ambientColor);
        cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
        cubeProgram.SetUniform3fv("light.specular", specularColor);

        cubeProgram.SetUniform1f("material.shininess", materialShininess);

        cubeProgram.SetUniformMat4("model", model);
        cubeProgram.SetUniformMat4("view", view);
        cubeProgram.SetUniformMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        texture_diffuse.Release();
        texture_specular.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

#endif // TEST4

#ifdef TEST5

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);

    ShaderProgram cubeProgram("resources/02_02_05_TEST1_map.vs", "resources/02_02_05_TEST5_map.fs");

    // clang-format off
    std::array<GLfloat, 8 * 6 * 6>  vertices{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    unsigned int VAO;
    {
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    // 漫反射贴图
    Texture texture_diffuse("resources/container.png", 0);
    // 彩色的镜面光贴图
    Texture texture_specular("resources/container_specular_colored.png", 1);

    //----------------------------------------------------------------------------------
    // 相机位置
    auto viewPos = glm::vec3(0.f, 0.f, 4.f);
    // 材质的反光度
    auto materialShininess = 32.f;
    // 光源位置(聚光灯)，和相机在同一位置
    auto lightPosition = viewPos;
    // 光源方向，指向屏幕里面
    auto lightDirection = glm::vec3(0.f, 0.f, -1.f);
    // 聚光灯圆锥张开的角度的余弦值
    auto lightCutOff = glm::cos(glm::radians(5.f));
    // 聚光灯外圆锥夹角余弦值，用来平滑聚光灯边缘
    auto lightOuterCutOff = glm::cos(glm::radians(7.5f));
    // 光源颜色
    auto lightColor = glm::vec3(1.f);
    // 光源的漫反射、环境反射、镜面反射颜色
    auto diffuseColor = lightColor * glm::vec3(0.5f);
    auto ambientColor = lightColor * glm::vec3(0.2f);
    auto specularColor = lightColor * glm::vec3(1.f);

    //----------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeProgram.Use();
        texture_diffuse.Bind();
        texture_specular.Bind();

        cubeProgram.SetUniform1i("material.diffuse", 0);
        cubeProgram.SetUniform1i("material.specular", 1);

        auto model = glm::rotate(glm::mat4(1.f), static_cast<float>(glfwGetTime() / 2), glm::vec3(1.f, 1.f, 0.f));
        auto view = glm::lookAt(viewPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
        auto projection = glm::perspective(glm::radians(30.f), 8.f / 6.f, 0.1f, 100.f);
        cubeProgram.SetUniform3fv("viewPos", viewPos);

        // 设置聚光灯参数
        cubeProgram.SetUniform3fv("light.position", lightPosition);
        cubeProgram.SetUniform3fv("light.direction", lightDirection);
        cubeProgram.SetUniform1f("light.cutOff", lightCutOff);
        cubeProgram.SetUniform1f("light.outerCutOff", lightOuterCutOff);

        cubeProgram.SetUniform3fv("light.ambient", ambientColor);
        cubeProgram.SetUniform3fv("light.diffuse", diffuseColor);
        cubeProgram.SetUniform3fv("light.specular", specularColor);

        cubeProgram.SetUniform1f("material.shininess", materialShininess);

        cubeProgram.SetUniformMat4("model", model);
        cubeProgram.SetUniformMat4("view", view);
        cubeProgram.SetUniformMat4("projection", projection);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        texture_diffuse.Release();
        texture_specular.Release();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);

    

    glfwTerminate();
    return 0;
}

#endif // TEST5