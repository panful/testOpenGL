/*
 * 01. 纹理的简单使用
 * 02. 纹理环绕，纹理过滤，多级渐远纹理，纹理相关函数的参数解释
 * 03. 纹理单元，一个着色器使用多个纹理
 * 04. 对同一个着色器多次设置不同的纹理和VAO
 * 05. 采样器的使用，对纹理设置采样的方式
 * 06. 一个采样器应用到多个纹理
 * 07. 读取纹理的像素数据
 * 08. 自定义纹理数据，1D纹理的使用
 * 09. 使用纹理设置窗口背景，可以将背景设置为任意颜色：渐变色、图片等等
 * 10. TBO GL_TEXTURE_BUFFER的使用，通过纹理给每一个单元设置一个颜色
 * 11. 点精灵
 * 12. PBO GL_PIXEL_PACK_BUFFER GL_PIXEL_UNPACK_BUFFER 的简单使用
 */

#define TEST12

#ifdef TEST1

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST1.vs", "resources/02_01_03_TEST1.fs");

    float vertices[] = {
        // clang-format off
         // positions           // colors            // texture coords
         0.5f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,   // top right
         0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 创建并绑定纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // 绑定之后，接下来使用了GL_TEXTURE_2D作为参数的函数都对该纹理生效，直到绑定下一个纹理
    // 纹理环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 翻转图片，OpenGL要求图片的y轴从下到上，图片的y轴一般是从上到下
    unsigned char* data = stbi_load("resources/02_01_03_barce.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, texture);

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

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST1.vs", "resources/02_01_03_TEST1.fs");

    float vertices[] = {
        // clang-format off
         // positions           // colors            // texture coords
         0.5f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,    2.0f, 2.0f,   // top right
         0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 创建并绑定纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // 绑定之后，接下来使用了GL_TEXTURE_2D作为参数的函数都对该纹理生效，直到绑定下一个纹理

    // 纹理环绕
    // 第一个参数指定了纹理目标；我们使用的是2D纹理，因此纹理目标是GL_TEXTURE_2D。
    // 第二个参数需要我们指定设置的选项与应用的纹理轴。我们打算配置的是WRAP选项，并且指定S和T轴。（3d还有R，STR对应XYZ）
    // 最后一个参数需要我们传递一个环绕方式
    // GL_REPEAT 对纹理的默认行为。重复纹理图像。
    // GL_MIRRORED_REPEAT 和GL_REPEAT一样，但每次重复图片是镜像放置的。
    // GL_CLAMP_TO_EDGE 纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
    // GL_CLAMP_TO_BORDER 超出的坐标为用户指定的边缘颜色。
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); // 当环绕方式设置为GL_CLAMP_TO_BORDER时，设置边缘颜色
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // 纹理过滤
    // 实际的图元只有100*100个像素，但是纹理图片有1000*1000或者10*10个像素
    // 即纹理和图元的像素不相等
    // GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。
    // 当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。会产生颗粒状的图案，有种锯齿感
    // GL_LINEAR（也叫线性过滤，(Bi) linear Filtering）它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。
    // 一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大。GL_LINEAR能够产生更平滑的图案，但会让成像看起来模糊
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 纹理被缩小时
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               // 纹理被放大时

    // 多级渐远纹理，纹理被缩小的情况下使用，即很小的一个图元使用了一个分辨率非常大的纹理，放大时使用会报错
    // GL_NEAREST_MIPMAP_NEAREST 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
    // GL_LINEAR_MIPMAP_NEAREST 使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
    // GL_NEAREST_MIPMAP_LINEAR 在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
    // GL_LINEAR_MIPMAP_LINEAR 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 翻转图片，OpenGL要求图片的y轴从下到上，图片的y轴一般是从上到下
    unsigned char* data = stbi_load("resources/02_01_03_rectpoly.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        // 当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像
        // 第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
        // 第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。后面的纹理需要不断递增第二个参数
        // 第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
        // 第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
        // 第六个参数应该总是被设为0（历史遗留的问题）。
        // 第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte) 数组，我们将会传入对应值。
        // 最后一个参数是真正的图像数据。
        // @note 第3个参数可以和第7个参数不一样，第7个参数表示图片的数据格式，第3个表示纹理的数据格式
        // 如果7为GL_RGBA,3为GL_RGB则会使用4个通道的图像生成一个3个通道的纹理。
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // 调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理。不需要手动设置
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, texture);

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

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST3.vs", "resources/02_01_03_TEST3.fs");

    float vertices[] = {
        // clang-format off
         // positions           // colors            // texture coords
         0.5f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,    1.0f, 1.0f,   // top right
         0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f,  0.0f,    1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 创建两个纹理
    GLuint texture[2] { 0 };
    glGenTextures(2, texture);

    //--------------------------------------------------------------------------------
    // 激活纹理单元0
    glActiveTexture(GL_TEXTURE0); // GL_TEXURE0默认激活
    // 绑定当前操作的纹理，纹理单元0对应的纹理
    glBindTexture(GL_TEXTURE_2D, texture[0]); // 绑定之后，接下来使用了GL_TEXTURE_2D作为参数的函数都对该纹理生效，直到绑定下一个纹理
    // 纹理环绕，纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    auto data0 = stbi_load("resources/02_01_03_barce.jpg", &width, &height, &nrChannels, 0);
    if (data0)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data0);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data0);

    //--------------------------------------------------------------------------------
    // 激活纹理单元1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto data1 = stbi_load("resources/02_01_03_rectpoly.jpg", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture\n";
    }
    stbi_image_free(data1);

    // 取消纹理绑定
    glBindTexture(GL_TEXTURE_2D, 0);

    program.Use();
    // 纹理单元只需要设置一次，所以放在渲染循环外部
    program.SetUniform1i("texture_1", 0); // 将采样器 "texture_1"设置为纹理单元0即GL_TEXTURE0
    program.SetUniform1i("texture_2", 1);

    // OpenGL至少保证有16个纹理单元供你使用，也就是说你可以激活从GL_TEXTURE0到GL_TEXTRUE15。
    // 它们都是按顺序定义的，所以我们也可以通过GL_TEXTURE0 + 8的方式获得GL_TEXTURE8，这在当我们需要循环一些纹理单元的时候会很有用。

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture[1]);

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

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST4.vs", "resources/02_01_03_TEST4.fs");

    // 图1 -> 四边形
    //-------------------------------------------------------------------------
    float vertices1[] = {
        // clang-format off
         // positions           // texture coords
        -0.1f,  0.5f, 0.0f,     1.0f, 1.0f,  // top right
        -0.1f, -0.5f, 0.0f,     1.0f, 0.0f,  // bottom right
        -0.9f, -0.5f, 0.0f,     0.0f, 0.0f,  // bottom left
        -0.9f,  0.5f, 0.0f,     0.0f, 1.0f   // top left
        // clang-foramt on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 图2 -> 三角形
    //-------------------------------------------------------------------------
    float vertices2[] = {
        // clang-foramt off
        // positions            // texture coords
        0.1f, -0.5f, 0.0f,      0.0f, 0.0f,
        0.9f, -0.5f, 0.0f,      1.0f, 0.0f,
        0.5f, 0.5f, 0.0f,       0.5f, 1.0f
        // clang-format on
    };

    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //-------------------------------------------------------------------------
    // 纹理1
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 纹理2
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //-------------------------------------------------------------------------
    // 绑定texture1到当前要操作的纹理
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 加载纹理图片1
    int width, height, nrChannels;
    unsigned char* data1 = stbi_load("resources/02_01_03_rectpoly.jpg", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    //-------------------------------------------------------------------------
    // 绑定第二个纹理
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 加载纹理图片2
    unsigned char* data2 = stbi_load("resources/02_01_03_barce.jpg", &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data1);
    stbi_image_free(data2);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 同一个着色器使用了两次，两次分别设置不同的vao和texture
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

#endif // TEST4

#ifdef TEST5

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST4.vs", "resources/02_01_03_TEST4.fs");

    // 图1 左边
    float vertices1[] = {
        // clang-format off
         // positions                 // texture coords
        -0.1f,  0.5f,  0.0f,         2.0f,  2.0f,   // top right
        -0.1f, -0.5f,  0.0f,         2.0f, -2.0f,   // bottom right
        -0.9f, -0.5f,  0.0f,        -2.0f, -2.0f,   // bottom left
        -0.9f,  0.5f,  0.0f,        -2.0f,  2.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    GLuint texture1, sampler;
    glGenTextures(1, &texture1);

    // 下面的两段代码效果是一样的
    if (0)
    {
        glBindTexture(GL_TEXTURE_2D, texture1);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); // 当环绕方式设置为GL_CLAMP_TO_BORDER时，设置边缘颜色
    }
    else
    {
        // 采样器
        glGenSamplers(1, &sampler);

        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, borderColor);

        GLuint texture_uint = 0; // 纹理单元
        // 将采样器绑定到纹理单元0
        glBindSampler(texture_uint, sampler); // 注意第一个参数不是 GL_TEXTURE*，绑定第几个纹理单元就是数字几

        // 采样器可以绑定到多个纹理单元
        // 纹理可以理解为一张图片
        // 采样器可以理解为从图片上采样颜色的方式
        // 可以直接对纹理设置采样颜色的方式，也可以通过采样器设置
    }

    int width, height, nrChannels;

    // 加载纹理图片1
    glBindTexture(GL_TEXTURE_2D, texture1);
    auto data1 = stbi_load("resources/02_01_03_rectpoly.jpg", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        glBindTexture(GL_TEXTURE_2D, texture1);
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

#endif // TEST5

#ifdef TEST6

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST4.vs", "resources/02_01_03_TEST4.fs");

    // 图1 左边
    float vertices1[] = {
        // clang-format off
         // positions                 // texture coords
        -0.1f,  0.5f,  0.0f,         2.0f,  2.0f,   // top right
        -0.1f, -0.5f,  0.0f,         2.0f, -2.0f,   // bottom right
        -0.9f, -0.5f,  0.0f,        -2.0f, -2.0f,   // bottom left
        -0.9f,  0.5f,  0.0f,        -2.0f,  2.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 图2 右边
    float vertices2[] = {
        // clang-format off
        // 位置                       //纹理
        0.1f,  0.5f,  0.0f,         2.0f,  2.0f,   // top right
        0.1f, -0.5f,  0.0f,         2.0f, -2.0f,   // bottom right
        0.9f,  0.5f,  0.0f,        -2.0f,  2.0f,    // top left

        0.1f, -0.5f,  0.0f,         2.0f, -2.0f,   // bottom right
        0.9f, -0.5f,  0.0f,        -2.0f, -2.0f,   // bottom left
        0.9f,  0.5f,  0.0f,        -2.0f,  2.0f    // top left
        // clang-format on
    };

    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //----------------------------------------------------------------------------------

    GLuint texture1, texture2, sampler;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    if (0)
    {
        // 纹理1
        glBindTexture(GL_TEXTURE_2D, texture1);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // 纹理2
        glBindTexture(GL_TEXTURE_2D, texture2);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); // 当环绕方式设置为GL_CLAMP_TO_BORDER时，设置边缘颜色
    }
    else
    {
        // 采样器
        glGenSamplers(1, &sampler);

        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
        glSamplerParameterfv(sampler, GL_TEXTURE_BORDER_COLOR, borderColor);

        // 将一个采样器应用到两个纹理上，就不用对两个纹理分别设置纹理环绕，过滤等属性
        glBindSampler(0, sampler);
    }

    int width, height, nrChannels;

    // 加载纹理图片1
    glBindTexture(GL_TEXTURE_2D, texture1);
    auto data1 = stbi_load("resources/02_01_03_rectpoly.jpg", &width, &height, &nrChannels, 0);
    if (data1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // 加载纹理图片2
    glBindTexture(GL_TEXTURE_2D, texture2);
    auto data2 = stbi_load("resources/02_01_03_barce.jpg", &width, &height, &nrChannels, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data1);
    stbi_image_free(data2);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

#endif // TEST6

#ifdef TEST7

#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursorPosCB(GLFWwindow* w, double x, double y);
void mouseCB(GLFWwindow* window, int button, int action, int mods);

// 鼠标的位置
double mouse_x { 0 };
double mouse_y { 0 };

// 纹理
unsigned int texture { 0 };
// stb_image读上来的像素数据
unsigned char* data { nullptr };

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    initOpenGL.SetCursorPosCB(cursorPosCB);
    initOpenGL.SetMouseCB(mouseCB);

    ShaderProgram program("resources/02_01_03_TEST7.vs", "resources/02_01_03_TEST7.fs");

    float vertices[] = {
        // clang-format off
         // positions           // texture coords
         1.0f,  1.0f,  0.0f,    1.0f, 1.0f,   // top right
         1.0f, -1.0f,  0.0f,    1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f,  0.0f,    0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f,  0.0f,    0.0f, 1.0f    // top left
        // clang-format on
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 创建并绑定纹理
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 纹理环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 纹理过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // 翻转图片，OpenGL要求图片的y轴从下到上，图片的y轴一般是从上到下
    data = stbi_load("resources/02_01_03_alpha.png", &width, &height, &nrChannels, 0);
    std::cout << "image: " << width << '\t' << height << '\n';
    // 带alpha值的图像通道应该为4
    if (data && nrChannels == 4)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    stbi_image_free(data);

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

            GLint level { 0 };        // 细节级别。0是基本图像级别
            GLint format { GL_RGBA }; // 像素数据的格式

            // 获取纹理的宽度和高度
            GLint width { 0 }, height { 0 };
            glBindTexture(GL_TEXTURE_2D, texture);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);

            std::cout << width << '\t' << height << '\n';

            // 为像素数据分配内存
            auto pixels = new GLubyte[width * height * 4];

            // 获取像素数据
            glGetTexImage(GL_TEXTURE_2D, level, format, GL_UNSIGNED_BYTE, pixels);

            // 像素的索引，注意最后的'*4'以及y轴需要翻转
            int index = ((int)(height - mouse_y) * width + (int)mouse_x) * 4;

            // 纹理中的数据
            auto r = (int)pixels[index + 0];
            auto g = (int)pixels[index + 1];
            auto b = (int)pixels[index + 2];
            auto a = (int)pixels[index + 3];

            // 使用stb_image读上来的图片原始数据
            auto r2 = (int)data[index + 0];
            auto g2 = (int)data[index + 1];
            auto b2 = (int)data[index + 2];
            auto a2 = (int)data[index + 3];

            std::cout << r << '\t' << g << '\t' << b << '\t' << a << '\n';
            std::cout << r2 << '\t' << g2 << '\t' << b2 << '\t' << a2 << '\n';

            // 删除像素数据
            delete[] pixels;

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        break;
        default:
            break;
        }
    }
}

#endif // TEST7

#ifdef TEST8

#include <array>
#include <common.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    initOpenGL.SetFramebufferSizeCB(framebuffer_size_callback);
    ShaderProgram program("resources/02_01_03_TEST8.vs", "resources/02_01_03_TEST8.fs");

    // clang-format off
    std::array<GLfloat, 4 * 4> vertices {
         // positions             // texture coords
         0.5f,  0.5f,  0.0f,      1.0f,
         0.5f, -0.5f,  0.0f,      1.0f,
        -0.5f,  0.5f,  0.0f,      0.0f,
        -0.5f, -0.5f,  0.0f,      0.0f,
    };
    // clang-format on

    unsigned int VBO, VAO;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    // 自定义一个包含10个像素的1D纹理，红 -> 绿 -> 蓝
    // 1D纹理就是一个高度为1，宽度不小于1的1维图片
    constexpr size_t numOfPixels { 10 };
    std::array<GLubyte, numOfPixels * 4> imageData;
    for (size_t i = 0; i < numOfPixels; i++)
    {
        GLubyte r, g, b;
        // clang-format off
        switch (i)
        {
        case 0:
        case 1:
            { r = 255; g = 0; b = 0; } break;
        case 2:
        case 3:
            { r = 255; g = 255; b = 0; } break;
        case 4:
        case 5:
            { r = 0; g = 255; b = 0; } break;
        case 6:
        case 7:
            { r = 0; g = 255; b = 255; } break;
        case 8:
        case 9:
            { r = 0; g = 0; b = 255; } break;
        default:
            break;
        }
        // clang-format on

        imageData[i * 4 + 0] = r;
        imageData[i * 4 + 1] = g;
        imageData[i * 4 + 2] = b;
        imageData[i * 4 + 3] = 255;

        std::cout << (int)r << '\t' << (int)g << '\t' << (int)b << '\n';
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);

    // 1D纹理只需要设置S方向的环绕方式
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // 可以尝试不同的过滤方式，绘制的图案会不一样
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 1D纹理不需要设置纹理的height（始终为1），其他参数和2D一样
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, numOfPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
    glGenerateMipmap(GL_TEXTURE_1D);

    glBindTexture(GL_TEXTURE_1D, 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();

        glBindTexture(GL_TEXTURE_1D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size() / 4));
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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

Renderer CreateCube()
{
    std::vector<float> vertices {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // top-left

        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-left

        // left face
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // top-right
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,   // top-right

        // right face
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,  // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,   // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  // bottom-left

        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-right

        // top face
        -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  // bottom-left
    };

    return Renderer(vertices, { 3, 3 });
}

Renderer CreateQuad()
{
    // clang-format off
    std::vector<float> vertices {
        -1.f,   1.0f,   0.0f,     0.0f, 1.0f,
        -1.f,  -1.0f,   0.0f,     0.0f, 0.0f,
         1.f,   1.0f,   0.0f,     1.0f, 1.0f,
         1.f,  -1.0f,   0.0f,     1.0f, 0.0f,
    };
    // clang-format on

    return Renderer(vertices, { 3, 2 });
}

int main()
{
    InitOpenGL init(Camera({ 0, 0, 5 }, { 0, 1, 0 }, { 0, 0, 0 }));
    auto window = init.GetWindow();
    auto cube   = CreateCube();
    auto quad   = CreateQuad();

    constexpr size_t numOfPixels { 10 };
    std::array<GLubyte, numOfPixels * 4> imageData;
    for (size_t i = 0; i < numOfPixels; i++)
    {
        GLubyte r, g, b;
        // clang-format off
        switch (i)
        {
        case 0:
        case 1:
            { r = 255; g = 0; b = 0; } break;
        case 2:
        case 3:
            { r = 255; g = 255; b = 0; } break;
        case 4:
        case 5:
            { r = 0; g = 255; b = 0; } break;
        case 6:
        case 7:
            { r = 0; g = 255; b = 255; } break;
        case 8:
        case 9:
            { r = 0; g = 0; b = 255; } break;
        default:
            break;
        }
        // clang-format on

        imageData[i * 4 + 0] = r;
        imageData[i * 4 + 1] = g;
        imageData[i * 4 + 2] = b;
        imageData[i * 4 + 3] = 255;

        std::cout << (int)r << '\t' << (int)g << '\t' << (int)b << '\n';
    }

    Texture background(imageData.data(), 10, 1, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    background.Bind();
    background.SetWarpParameter(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    ShaderProgram shaderCube("resources/02_01_03_TEST9.vs", "resources/02_01_03_TEST9.fs");
    ShaderProgram shaderBackground("resources/02_01_03_TEST9_back.vs", "resources/02_01_03_TEST9_back.fs");

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto view       = init.GetViewMatrix();
        auto projection = init.GetProjectionMatrix();

        //---------------------------------------------------------------------
        // 将深度测试函数设置为默认值，根据具体情况设置
        glDepthFunc(GL_LESS);

        shaderCube.Use();
        shaderCube.SetUniformMat4("transform", projection * view);
        cube.Draw(GL_TRIANGLES);

        //---------------------------------------------------------------------
        // 背景，深度值小于等于深度缓存中的都通过
        glDepthFunc(GL_LEQUAL);

        shaderBackground.Use();
        background.Use();
        quad.Draw(GL_TRIANGLE_STRIP);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST9

#ifdef TEST10

#include <common.hpp>

Renderer CreateTriangles()
{
    // clang-format off
    std::vector<float> vertices {
        -0.5f, -0.5f,  -0.5f, 0.5f,
        -0.2f, -0.5f,  -0.2f, 0.5f,
         0.0f, -0.5f,   0.0f, 0.5f,
         0.2f, -0.5f,   0.2f, 0.5f,
         0.5f, -0.5f,   0.5f, 0.5f,
    };

    std::vector<uint32_t> indices {
        0,3,1,  0,2,3,
        2,5,3,  2,4,5,
        4,7,5,  4,6,7,
        6,9,7,  6,8,9,
    };
    // clang-format on

    return Renderer(vertices, indices, { 2 });
}

int main()
{
    InitOpenGL init;
    auto window    = init.GetWindow();
    auto triangles = CreateTriangles();

    ShaderProgram shader("resources/02_01_03_TEST10.vs", "resources/02_01_03_TEST10.fs");

    // clang-format off

    // GL_RGB32F
    // std::vector<float> data {
    //     1.f, 0.f, 0.f, 
    //     0.f, 1.f, 0.f, 
    //     0.f, 0.f, 1.f, 
    //     1.f, 0.f, 0.f, 
    //     0.f, 1.f, 0.f, 
    //     0.f, 0.f, 1.f, 
    //     1.f, 0.f, 0.f, 
    //     0.f, 1.f, 0.f,
    //     0.f, 0.f, 1.f,
    // };

    // GL_RGBA8 不支持 GL_RGB8
    std::vector<uint8_t> data { 
        255,    0,      0,      255,
        0,      255,    0,      255,
        0,      0,      255,    255,
        255,    0,      0,      255,
        0,      255,    0,      255,
        0,      0,      255,    255,
        255,    0,      0,      255,
        0,      255,    0,      255,
        0,      0,      255,    255,
    };

    // clang-format on

    // 创建texture buffer object
    unsigned int TBO;
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_TEXTURE_BUFFER, TBO);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(data), data.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    // 创建并绑定纹理对象
    // 只有将TBO绑定到一个纹理上才可以使用
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_BUFFER, texture);
    // 绑定TBO到纹理，第二个参数是TBO的数据类型
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA8, TBO);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    CheckError();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();

        // 默认激活纹理单元GL_TEXTURE0
        // shader.SetUniform1i("colorTexture", 0);
        // glActiveTexture(GL_TEXTURE0);

        // 绑定纹理
        // 注意第二个参数是纹理ID，不是TBO
        glBindTexture(GL_TEXTURE_BUFFER, texture);

        // 绘制三角面
        triangles.Draw(GL_TRIANGLES);

        // 解绑
        glBindTexture(GL_TEXTURE_BUFFER, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST10

#ifdef TEST11

#include <common.hpp>

int main()
{
    InitOpenGL initOpenGL;
    auto window = initOpenGL.GetWindow();
    ShaderProgram program("resources/02_01_03_TEST11.vs", "resources/02_01_03_TEST11.fs");

    // clang-format off
    std::vector<float> vertices {
         0.5f,  0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
        -0.5f,  0.5f,  0.0f,
    };
    // clang-format on

    Renderer points(vertices, { 3 }, GL_POINTS);
    Texture texture(std::string_view("resources/02_01_03_snow.png"));

    // 设置点的大小
    // glPointSize(10.f);
    // 开启在着色器中设置点的大小，否则只能使用glPointSize设置大小
    glEnable(GL_PROGRAM_POINT_SIZE);
    // 设置纹理坐标gl_PointCoord.y的方向
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    // 设置点精灵渐隐效果
    glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.f);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.Use();
        texture.Use();
        points.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST11

#ifdef TEST12

#include <common.hpp>

GLuint pbo { 0 };
GLuint texture { 0 };

int main()
{
    InitOpenGL opengl;
    auto window = opengl.GetWindow();
    opengl.SetMiddleButtonCallback(
        [](int x, int y)
        {
            auto index = (800 * (600 - y) + x) * 4;

            //------------------------------------------------------------------------
            // 首先我们要把缓冲区绑定到 GL_PIXEL_PACK_BUFFER 这个地方
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
            // 这个函数会判断 GL_PIXEL_PACK_BUFFER 这个地方有没有绑定一个缓冲区，如果有，那就把数据写入到这个缓冲区里
            // 前4个参数就是要读取的屏幕区域，格式是RGBA，类型是BYTE，每个像素4字节
            // 如果GL_PIXEL_PACK_BUFFER有绑定缓冲区，最后一个参数就作为偏移值来使用
            glReadPixels(0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // 已经成功把屏幕的像素数据复制到了缓冲区里
            // 这时候，可以用 glMapBuffer 得到缓冲区的内存指针，来读取里面的像素数据，保存到图片文件，完成截图
            // 注意glMapBuffer的第1个参数不一定要是GL_PIXEL_PACK_BUFFER，可以把缓冲区绑定到比如创建PBO时的GL_ARRAY_BUFFER
            // 然后这里也传GL_ARRAY_BUFFER，由于懒得再绑定一次，就接着用上面绑定的GL_PIXEL_PACK_BUFFER
            void* data = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_WRITE);
            if (data)
            {
                auto pixels = static_cast<uint8_t*>(data);
                std::cout << "GL_PIXEL_PACK_BUFFER\t" << (int)pixels[index] << '\t' << (int)pixels[index + 1] << '\t' << (int)pixels[index + 2]
                          << '\n';
                glUnmapBuffer(GL_PIXEL_PACK_BUFFER); // 不要忘了解除Map
            }

            // 完事了把GL_PIXEL_PACK_BUFFER这个地方的缓冲区解绑掉，以免别的函数误操作
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

            //------------------------------------------------------------------------
            // 把缓冲区中的像素数据传给纹理，把缓冲区绑定到 GL_PIXEL_UNPACK_BUFFER ，注意是GL_PIXEL_UNPACK_BUFFER
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
            // 绑定纹理
            glBindTexture(GL_TEXTURE_2D, texture);
            // 这个函数会判断 GL_PIXEL_UNPACK_BUFFER 这个地方有没有绑定一个缓冲区
            // 如果有，就从这个缓冲区读取数据，而不是data参数指定的那个内存
            // 前面参数很简单就不解释了，最后一个参数和上面glReadPixels同理，传NULL就行
            // 这样glTexSubImage2D就会从我们的缓冲区中读取数据了
            // 这里为什么要用glTexSubImage2D呢，因为如果用glTexImage2D，glTexImage2D会销毁纹理内存重新申请，
            // glTexSubImage2D就仅仅只是更新纹理中的数据，这就提高了速度，并且优化了显存的利用率
            // 因为是从缓存获取数据，这里是并行发生的，非常快
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            // 完事了把GL_PIXEL_UNPACK_BUFFER这个地方的缓冲区解绑掉，以免别的函数误操作
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            // 获取纹理的像素值，这里只是演示，实际应用中可以把纹理传给着色器使用等等
            // 从纹理读取像素数据是一个比较耗时的操作，最好不要使用，如果非要使用最好使用PBO
            std::vector<uint8_t> pixels(800 * 600 * 4);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
            std::cout << "GL_PIXEL_UNPACK_BUFFER\t" << (int)pixels[index] << '\t' << (int)pixels[index + 1] << '\t' << (int)pixels[index + 2] << '\n';
            // 解绑纹理
            glBindTexture(GL_TEXTURE_2D, 0);
        });

    ShaderProgram shader("resources/02_01_03_TEST9.vs", "resources/02_01_03_TEST9.fs");

    // clang-format off
    std::vector<float> vertices{
        -.5f, -.5f, 0.f,    1.f, 0.f, 0.f,
         .5f, -.5f, 0.f,    0.f, 1.f, 0.f,
         0.f,  .5f, 0.f,    0.f, 0.f, 1.f,
    };
    // clang-format on

    Renderer triangle(vertices, { 3, 3 }, GL_TRIANGLES);

    // 创建一个缓冲区，并分配内存（显存），注意这里的格式是：GL_ARRAY_BUFFER
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_ARRAY_BUFFER, pbo);
    glBufferData(GL_ARRAY_BUFFER, 800 * 600 * 4, nullptr, GL_STREAM_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 创建一个2D纹理
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Use();
        shader.SetUniformMat4("transform", glm::mat4(1.f));
        triangle.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST12
