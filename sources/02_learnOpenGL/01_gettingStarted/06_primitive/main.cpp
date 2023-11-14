/*
 * 1. 设置图元填充模式，点的大小，线的粗细
 * 2. 对多个图元设置不同的填充方式
 * 3. glDrawArrays glDrawElements 绘制方式：点、线、三角形等
 * 4. 图元重启
 * 5. 自定义创建球体数据的函数
 * 6. 带有纹理坐标的球体
 */

#define TEST6

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
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
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

    // 设置图元填充模式
    // 第一个参数：
    // 设定多边形的两面，如果是顺时针绘制则是反面，逆时针绘制则是正面；
    // 第二个参数：
    // GL_FILL  默认填充方式
    // GL_LINE  呈现线框模型
    // GL_POINT 用点绘制图形
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 设置点的大小
    glPointSize(5.0f);

    // 设置线的粗细
    glLineWidth(5.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 3 * 6> vertices{
        // pos                  // color
        -0.9f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
        -0.1f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    std::array<GLfloat, 3 * 6> vertices2{
        // pos                  // color
         0.1f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
         0.9f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,
    };

    std::array<GLuint, 1 * 3> indices{
        0, 1, 2
    };
    // clang-format on

    unsigned int VBO, VAO, EBO;
    {
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
    }

    unsigned int VBO2, VAO2, EBO2;
    {
        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices2.size(), vertices2.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
    }

    //----------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        // 左边的三角形
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glLineWidth(5.0f);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        }

        // 右边的三角形
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBindVertexArray(VAO2);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.5f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.5f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.5f,     1.0f, 1.0f, 1.0f, // 左上
    };

    std::array<GLuint, 2 * 3> indices{
        0, 1, 3,
        1, 2, 3,
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

        // 绕（1,1,0）旋转
        auto resultMat = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()), glm::vec3(1, 1, 0));
        program.SetUniformMat4("transform", resultMat);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glDrawArrays:当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
        // glDrawArrays(GLenum mode, GLint first, GLsizei count) 参数：
        // 1. mode  绘制方式，GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN
        // 2. first 从数组缓存中的哪一位开始绘制，表示从指定数据数组的哪一个点开始画起，一般为0，
        // 3. count 数组中顶点的数量
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // 绘制方式见resources文件夹内的primitive.png

        // 1.绘制方式
        // 2.依次从索引数组中读取几个顶点来进行绘制（两个三角形为6）
        // 3.索引数组中存放的元素的类型
        // 4.指向索引数组的指针，在前面填充部分已经设置了（VAO绑定EBO），也可以在此处设置 02_01_01_TEST6
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // clang-format off
    std::array<GLfloat, 4 * 6> vertices{
        // pos                  // color
        -0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f, // 左下
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f, // 右下
         0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f, // 右上
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f, // 左上
    };

    // 拓扑数据，默认情况下：
    // 当数据类型时GLuint(32位)时0xFFFFFFFF是重启标志，
    // 当数据类型是GLshort(16位)时，标志用0xFFFF
    std::array<GLuint, 4 + 2> indices{
        0, 1, 2, 0xFFFFFFFF,
        3, 0
    };
    // clang-format on

    GLuint VAO { 0 };
    {
        GLuint VBO { 0 }, EBO { 0 };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * sizeof(indices), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    //---------------------------------------------------------------------------------

    // 开启图元重启，默认开启
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    // 设置图元重启标志，一般不需要设置，使用默认值即可
    glPrimitiveRestartIndex(0xFFFF);
    // 获取图元重启标志
    GLuint restart_index;
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)&restart_index);
    std::cout << "restart index: " << restart_index << '\n';

    // 当 glDrawElements() 绘制类型为：GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_LINE_STRIP, GL_LINE_LOOP时，
    // 使用图元重启才有用

    // 图元重启的意思就是遇到指定的重启标志时，重新开始计算索引
    // 例如当绘制类型为：GL_LINE_STRIP
    // 没有开启图元重启，索引数据为：{0,1,2,3,4,5} 时，会绘制5条线段(0,1)(1,2)(2,3)(3,4)(4,5)
    // 开启图元重启，索引数据为：{0,1,2,0xFFFFFFFF,3,4,5}时，只会绘制4条线段(0,1)(1,2)(3,4)(4,5)

    //---------------------------------------------------------------------------------

    glPrimitiveRestartIndex(0xFFFFFFFF);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", glm::mat4(1.f));

        glBindVertexArray(VAO);
        glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers

    glDeleteVertexArrays(1, &VAO);

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

#include <common.hpp>
#include <numbers>

// @brief 创建一个球心在(0,0,0)的球体
// @param [in] longitude 经线上的顶点个数
// @param [in] latitude 纬线上的顶点个数
// @param [in] radius 半径
Renderer CreateSphere(uint32_t longitude = 32, uint32_t latitude = 32, float radius = 1.f)
{
    longitude = longitude > 3 ? longitude : 3;
    latitude  = latitude > 3 ? latitude : 3;

    std::vector<float> vertices;
    vertices.reserve((latitude * (longitude - 2) + 2) * 3);
    std::vector<uint32_t> indices;
    indices.reserve((longitude - 2) * latitude * 2);

    auto deltaLatitude  = (2 * std::numbers::pi_v<float> / latitude);
    auto deltaLongitude = (std::numbers::pi_v<float> / (longitude - 1));

    // 最上面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(radius);
    vertices.emplace_back(0.f);

    // 每一层，即纬线所在的圈
    for (size_t i = 1; i < longitude - 1; ++i)
    {
        auto r = radius * std::sin(i * deltaLongitude);
        auto y = radius * std::cos(i * deltaLongitude);

        // 每一层上的每一个点（纬线上的每一个点）
        for (size_t j = 0; j < latitude; ++j)
        {
            auto x = r * std::sin(j * deltaLatitude);
            auto z = r * std::cos(j * deltaLatitude);

            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
            // 法线
            vertices.emplace_back(x);
            vertices.emplace_back(y);
            vertices.emplace_back(z);
        }
    }

    // 最下面单独的一个点
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);
    // 法线
    vertices.emplace_back(0.f);
    vertices.emplace_back(-radius);
    vertices.emplace_back(0.f);

    //---------------------------------------------------
    // 北极圈
    for (unsigned int j = 1; j < latitude; ++j)
    {
        indices.emplace_back(0);
        indices.emplace_back(j);
        indices.emplace_back(j + 1);
    }
    indices.emplace_back(0);
    indices.emplace_back(latitude);
    indices.emplace_back(1);

    // 中间
    for (unsigned int i = 1; i + 2 < longitude; ++i)
    {
        auto start = (1 + (i - 1) * latitude);

        for (unsigned int j = 0; j + 1 < latitude; ++j)
        {
            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude);
            indices.emplace_back(start + j + latitude + 1);

            indices.emplace_back(start + j);
            indices.emplace_back(start + j + latitude + 1);
            indices.emplace_back(start + j + 1);
        }

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude - 1 + latitude);
        indices.emplace_back(start + latitude);

        indices.emplace_back(start + latitude - 1);
        indices.emplace_back(start + latitude);
        indices.emplace_back(start);
    }

    // 南极圈
    auto south = (longitude - 2) * latitude + 1;
    assert(south > latitude);
    for (unsigned int i = 1; i < latitude; ++i)
    {
        indices.emplace_back(south);
        indices.emplace_back(south - i);
        indices.emplace_back(south - i - 1);
    }
    indices.emplace_back(south);
    indices.emplace_back(south - latitude);
    indices.emplace_back(south - 1);

    return Renderer(vertices, indices, { 3, 3 });
}

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    auto sphere = CreateSphere();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", init.GetProjectionMatrix() * init.GetViewMatrix() * glm::mat4(1.f));
        sphere.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST5

#ifdef TEST6

#include <cmath>
#include <common.hpp>
#include <numbers>
#include <utility>

// latitude  经线一圈的顶点个数的一半，连接南北极的是经线
// longitude 纬线一圈的顶点个数，赤道是纬线
Renderer CreateSphere(uint32_t longitude = 10, uint32_t latitude = 10, float radius = 1.0f)
{
    auto THE_PI = std::numbers::pi_v<float>;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    // Generate vertices
    for (uint32_t lat = 0; lat <= latitude; ++lat)
    {
        float theta    = lat * THE_PI / latitude;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (uint32_t lon = 0; lon <= longitude; ++lon)
        {
            float phi    = lon * 2 * THE_PI / longitude;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            // Vertex position
            float x = radius * cosPhi * sinTheta;
            float y = radius * cosTheta;
            float z = radius * sinPhi * sinTheta;

            // Vertex normal (same as position)
            float nx = x;
            float ny = y;
            float nz = z;

            // Vertex UV coordinates (spherical mapping)
            float u = 1.0f - static_cast<float>(lon) / longitude;
            float v = 1.0f - static_cast<float>(lat) / latitude;

            // Add vertex attributes to the vector
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generate indices
    for (uint32_t lat = 0; lat < latitude; ++lat)
    {
        for (uint32_t lon = 0; lon < longitude; ++lon)
        {
            uint32_t curr = lat * (longitude + 1) + lon;
            uint32_t next = curr + longitude + 1;

            // Add indices for the two triangles forming each quad
            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(curr + 1);
        }
    }

    std::cout << "points: " << vertices.size() / 8 << "\tindices: " << indices.size() / 3 << '\n';
    return Renderer(vertices, indices, { 3, 3, 2 });
}

int main()
{
    InitOpenGL init(Camera({ 0.f, 0.f, 5.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f, 0.f }));
    auto window = init.GetWindow();

    ShaderProgram program("resources/02_01_06_TEST1.vs", "resources/02_01_06_TEST1.fs");

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    auto sphere = CreateSphere();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.Use();
        program.SetUniformMat4("transform", init.GetProjectionMatrix() * init.GetViewMatrix() * glm::mat4(1.f));
        sphere.Draw(GL_TRIANGLES);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST6