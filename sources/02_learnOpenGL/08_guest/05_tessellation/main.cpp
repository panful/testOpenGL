/*
 * 1. 基于CPU的曲面细分
 * 2. 对一个三角形进行简单的细分
 * 3. 对两个四边形进行细分，并解释细分着色器的内部变量的含义
 * 4. 使用glPatchParameterfv替代细分控制着色器
 */

#define TEST4

#ifdef TEST1

#include <common.hpp>

int main()
{
    InitOpenGL opengl(Camera({ 0.f, 30.f, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }));
    auto window = opengl.GetWindow();
    opengl.SetKeyCallback(
        [](char key)
        {
            // 使用'L'和'F'切换显示模式
            if ('l' == key || 'L' == key)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else if ('f' == key || 'F' == key)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        });

    ShaderProgram shader("resources/02_08_05_TEST1.vs", "resources/02_08_05_TEST1.fs");

    stbi_set_flip_vertically_on_load(true);
    int width { 0 }, height { 0 }, nrChannels { 0 };
    unsigned char* data = stbi_load("resources/iceland_heightmap.png", &width, &height, &nrChannels, 0);
    // unsigned char* data = stbi_load("resources/river_heightmap.png", &width, &height, &nrChannels, 0);
    // unsigned char* data = stbi_load("resources/river2_heightmap.png", &width, &height, &nrChannels, 0);

    if (data)
    {
        std::cout << "Image size: " << height << ", " << width << ", " << nrChannels << '\n';
    }
    else
    {
        std::cout << "Failed to load texture\n";
        return -1;
    }

    // 将图片的颜色值（灰度图）设置为网格坐标的y分量
    // 网格的长宽(xz)分别为图像的长宽
    float yScale { 64.f / 256.f }, yShift { 16.f };
    uint32_t bytePerPixel = static_cast<uint32_t>(nrChannels);
    std::vector<float> vertices;
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            // 灰度图 rgb三个分量都是相同的值
            uint8_t y = data[(w + width * h) * bytePerPixel];

            vertices.emplace_back(-height / 2.f + (float)h);
            vertices.emplace_back((int)y * yScale - yShift);
            vertices.emplace_back(-width / 2.f + (float)w);
        }
    }

    stbi_image_free(data);

    // C   D   F   H
    // -------------
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // |   |   |   |
    // -------------
    // A   B   E   G
    // 从AB到CD为一个三角形带，BE到DF为一个三角形带，总共有numStrips个三角形带
    // 每个三角形带共有numTrisPerStrip个顶点，索引顺序为ABCD，使用GL_TRIANGLE_STRIP模式绘制
    int rez { 1 };
    std::vector<uint32_t> indices;
    for (int i = 0; i < height - 1; i += rez)
    {
        for (int j = 0; j < width; j += rez)
        {
            for (int k = 0; k < 2; k++)
            {
                indices.emplace_back(static_cast<uint32_t>(j + width * (i + k * rez)));
            }
        }
    }

    std::cout << "vertices size:\t" << vertices.size() << "\nindices size:\t" << indices.size() << '\n';

    const int numStrips       = (height - 1) / rez;
    const int numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Number of strips:\t" << numStrips << '\n';
    std::cout << "Number of triangles on each strip:\t" << numTrisPerStrip << '\n';
    std::cout << "Number of total triangles:\t" << numStrips * numTrisPerStrip << '\n';

    //------------------------------------------------------------------------------
    unsigned int terrainVAO, terrainVBO, terrainIBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    //------------------------------------------------------------------------------
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();
        shader.SetUniformMat4("projection", opengl.GetProjectionMatrix());
        shader.SetUniformMat4("view", opengl.GetViewMatrix());
        shader.SetUniformMat4("model", glm::scale(glm::mat4(1.f), glm::vec3(0.01f)));

        glBindVertexArray(terrainVAO);

        for (int strip = 0; strip < numStrips; strip++)
        {
            // 第一个参数为模式，第一个参数此次绘制多少个顶点
            // 第三个参数为数据类型，第四个参数为此次数据的起始位置
            glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * (numTrisPerStrip + 2) * strip));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <common.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tessellation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    auto vert                 = ReadFile("resources/02_08_05_TEST2.vert");
    auto frag                 = ReadFile("resources/02_08_05_TEST2.frag");
    auto tesc                 = ReadFile("resources/02_08_05_TEST2.tesc"); // tessellation control
    auto tese                 = ReadFile("resources/02_08_05_TEST2.tese"); // tessellation evaluation
    auto vertexShaderSource   = vert.c_str();
    auto fragmentShaderSource = frag.c_str();
    auto tescShaderSource     = tesc.c_str();
    auto teseShaderSource     = tese.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // tessellation control shader
    GLuint tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tescShader, 1, &tescShaderSource, nullptr);
    glCompileShader(tescShader);
    glGetShaderiv(tescShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tescShader, 512, nullptr, infoLog);
        std::cerr << "Tesc shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // tessellation evaluation shader
    GLuint teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(teseShader, 1, &teseShaderSource, nullptr);
    glCompileShader(teseShader);
    glGetShaderiv(teseShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(teseShader, 512, nullptr, infoLog);
        std::cerr << "Tese shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, tescShader);
    glAttachShader(shaderProgram, teseShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgram, tescShader);
    glDetachShader(shaderProgram, teseShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(tescShader);
    glDeleteShader(teseShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         -.5f,  -.5f,   1.0f, 0.0f, 0.0f,
          .5f,  -.5f,   0.0f, 1.0f, 0.0f,
          .0f,   .5f,   0.0f, 0.0f, 1.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 2));

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 设置构成一个面片的顶点总数，第一个参数必须为GL_PATCH_VERTICES
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    //--------------------------------------------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // 使用了细分着色器之后，不能再使用GL_TRIANGLES之类的绘制模式
        // 需要使用一种新的模式：GL_PATCHES（面片）
        // 设置GL_PATCH_VERTICES就相当于指定绘制图元的类型
        glDrawArrays(GL_PATCHES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST2

#ifdef TEST3

#include <common.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tessellation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    auto vert                 = ReadFile("resources/02_08_05_TEST2.vert");
    auto frag                 = ReadFile("resources/02_08_05_TEST2.frag");
    auto tesc                 = ReadFile("resources/02_08_05_TEST3.tesc"); // tessellation control
    auto tese                 = ReadFile("resources/02_08_05_TEST3.tese"); // tessellation evaluation
    auto vertexShaderSource   = vert.c_str();
    auto fragmentShaderSource = frag.c_str();
    auto tescShaderSource     = tesc.c_str();
    auto teseShaderSource     = tese.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // tessellation control shader
    GLuint tescShader = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tescShader, 1, &tescShaderSource, nullptr);
    glCompileShader(tescShader);
    glGetShaderiv(tescShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tescShader, 512, nullptr, infoLog);
        std::cerr << "Tesc shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // tessellation evaluation shader
    GLuint teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(teseShader, 1, &teseShaderSource, nullptr);
    glCompileShader(teseShader);
    glGetShaderiv(teseShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(teseShader, 512, nullptr, infoLog);
        std::cerr << "Tese shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, tescShader);
    glAttachShader(shaderProgram, teseShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgram, tescShader);
    glDetachShader(shaderProgram, teseShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(tescShader);
    glDeleteShader(teseShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         -.5f,  -.5f,   1.0f, 0.0f, 0.0f,
          .0f,  -.5f,   0.0f, 1.0f, 0.0f,
          .0f,   .5f,   0.0f, 1.0f, 0.0f,
         -.5f,   .5f,   1.0f, 0.0f, 0.0f,

          .0f,  -.5f,   0.0f, 1.0f, 0.0f,
          .5f,  -.5f,   0.0f, 0.0f, 1.0f,
          .5f,   .5f,   0.0f, 0.0f, 1.0f,
          .0f,   .5f,   0.0f, 1.0f, 0.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 2));

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 设置构成一个面片的顶点总数，第一个参数必须为GL_PATCH_VERTICES
    // 此处共有8个顶点，一个面片有4个顶点，所有共有两个面片
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    //--------------------------------------------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 8);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST3

#ifdef TEST4

#include <common.hpp>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Tessellation", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    auto vert                 = ReadFile("resources/02_08_05_TEST2.vert");
    auto frag                 = ReadFile("resources/02_08_05_TEST2.frag");
    auto tese                 = ReadFile("resources/02_08_05_TEST4.tese"); // tessellation evaluation
    auto vertexShaderSource   = vert.c_str();
    auto fragmentShaderSource = frag.c_str();
    auto teseShaderSource     = tese.c_str();

    GLint success { 0 };
    char infoLog[512](0);

    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    // tessellation evaluation shader
    GLuint teseShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(teseShader, 1, &teseShaderSource, nullptr);
    glCompileShader(teseShader);
    glGetShaderiv(teseShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(teseShader, 512, nullptr, infoLog);
        std::cerr << "Tese shader compilation failed: " << infoLog << '\n';
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, teseShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";
        return -1;
    }

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDetachShader(shaderProgram, teseShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(teseShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         -.5f,  -.5f,   1.0f, 0.0f, 0.0f,
          .0f,  -.5f,   0.0f, 1.0f, 0.0f,
          .0f,   .5f,   0.0f, 1.0f, 0.0f,
         -.5f,   .5f,   1.0f, 0.0f, 0.0f,

          .0f,  -.5f,   0.0f, 1.0f, 0.0f,
          .5f,  -.5f,   0.0f, 0.0f, 1.0f,
          .5f,   .5f,   0.0f, 0.0f, 1.0f,
          .0f,   .5f,   0.0f, 1.0f, 0.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 2));

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 设置构成一个面片的顶点总数，第一个参数必须为GL_PATCH_VERTICES
    // 此处共有8个顶点，一个面片有4个顶点，所有共有两个面片
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    const float outer[] { 4.f, 4.f, 4.f, 4.f };
    const float inter[] { 3.f, 3.f };
    glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outer);
    glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, inter);

    //--------------------------------------------------------------------------------------------
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_PATCHES, 0, 8);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST4
