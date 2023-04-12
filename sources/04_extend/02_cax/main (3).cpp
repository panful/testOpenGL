/*
1.

8.每一个像素赋值一个序号用来拾取(glReadPixels的使用示例）



15.局部坐标系坐标映射到世界坐标系，使用几何着色器将线段生成箭头，矢量图
16.将例15的变换矩阵放在着色器中
17.对用例4设置glViewPort,glfw滚轮事件
18.多个线程中都使用glfw，即创建多个窗口
19.材质
20.
21.
22.3维坐标转2维屏幕坐标，给旋转的物体添加一个2d标签(坐标轴的文本xyz随着轴变换位置）
23.
*/

#define TEST8


#ifdef TEST8

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <array>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform vec2 offsets[100];"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   fColor = vec3(0.0f,0.0f,float(gl_InstanceID/255.0f));\n"
"   vec2 offset = offsets[gl_InstanceID];"
"   gl_Position = vec4(aPos + offset, 0.0f, 1.0f);\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
    //初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //设置当前上下文，回调函数（键盘，鼠标等）
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //着色器 -> 创建 加载 编译 链接 绑定
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    float vertices[] = {
        -0.05f,  0.05f,
         0.05f, -0.05f,
        -0.05f, -0.05f
    };

    //缓冲对象：创建 绑定类型 链接数据 配置数据
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2)
    {
        for (int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    glUseProgram(shaderProgram);

    for (unsigned int i = 0; i < 100; i++)
    {
        std::stringstream ss;
        std::string index;
        ss << i;
        index = ss.str();
        glUniform2f(glGetUniformLocation(shaderProgram, ("offsets[" + index + "]").c_str()), translations[i][0], translations[i][1]);
    }


    //循环绘制
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
        glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 100);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    double x, y;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwGetCursorPos(window, &x, &y);

        //std::vector< unsigned char > pixels(1 * 1 * 4);

        //unsigned char* pixels = new unsigned char[4];
        std::array<unsigned char, 4> arrPixels;
        //glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);  //y坐标需要转换
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, arrPixels.data());  //y坐标需要转换

        std::cout << "r:" << (int)arrPixels[0] << std::endl;  //一定要在此处将pixels转为int
        std::cout << "g:" << (int)arrPixels[1] << std::endl;
        std::cout << "b:" << (int)arrPixels[2] << std::endl;
        std::cout << "a:" << (int)arrPixels[3] << std::endl;

        std::cout << std::endl;

        //delete[] pixels;
        //pixels = nullptr;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif //TEST8


#ifdef TEST15
//添加了三个uniform，模型矩阵，观察矩阵，投影矩阵，将这三个矩阵同gl_Position相乘（注意顺序），再开启深度缓冲就可以实现立体效果
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <list>

struct Vertex
{
    float x;
    float y;
    float z;

    Vertex(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) :
        x(_x),
        y(_y),
        z(_z)
    {}

    Vertex operator/(int n)
    {
        if (n <= 0)
        {
            return *this;
        }

        return Vertex(this->x / n, this->y / n, this->z / n);
    }
};

glm::mat4 genMatrix(Vertex v0, Vertex v1)
{
    glm::vec3 r(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    glm::vec3 s(1.0f);

    if (fabs(r.x) <= fabs(r.y) && fabs(r.x) <= fabs(r.z))
    {
        s.x = 0.0f;
        s.y = -r.z;
        s.z = r.y;
    }
    else if (fabs(r.y) <= fabs(r.x) && fabs(r.y) <= fabs(r.z))
    {
        s.x = -r.z;
        s.y = 0.0f;
        s.z = r.x;
    }
    else if (fabs(r.z) <= fabs(r.x) && fabs(r.z) <= fabs(r.y))
    {
        s.x = -r.y;
        s.y = r.x;
        s.z = 0.0f;
    }

    s /= std::hypot(s.x, s.y, s.z);

    //glm::vec3 t = r * s;
    glm::vec3 t(fabs(r.y * s.z - r.z * s.y), fabs(r.z * s.x - r.x * s.z), fabs(r.x * s.y - r.y * s.x));

    return glm::mat4(
        r.x, s.x, t.x, 0.0,
        r.y, s.y, t.y, 0.0,
        r.z, s.z, t.z, 0.0,
        v0.x, v0.y, v0.z, 1.0f);
}

struct Color
{
    float r, g, b, a; // 每个分量的取值范围为[0.0, 1.0]
};

/// 颜色映射表
class ColorMap
{
public:
    bool IsEmpty() const;

    // 根据param，将节点插入mColorNodes中
    void AddNode(float param, Color color) { mColorNodes.emplace_back(ColorNode{ param,color }); }
    void Pop() {}

    Color Lookup(float param) const {}

    //const size_t size() const { return mColorNodes.size(); }

    void Clear() {}
private:
    struct ColorNode
    {
        float param;
        Color color;
    };
public:
    std::list<ColorNode> mColorNodes;
};

unsigned char* GenTexture(ColorMap myCM)
{
    //归一化
    ColorMap cm = myCM;
    auto start = cm.mColorNodes.front().param;
    auto end = cm.mColorNodes.back().param;

    for (auto& elem : cm.mColorNodes)
    {
        elem.param = elem.param / (end - start);
    }


    ///< 生成纹理的结果
    unsigned char* result = new unsigned char[1024 * 4];
    ///< 用来保存前一个特征点的序号
    int preIndex = 0;
    ///< 用来保存前一个特征点的颜色
    Color preColor = cm.mColorNodes.begin()->color;
    for (auto it = cm.mColorNodes.begin(); it != cm.mColorNodes.end(); it++)
    {
        ///< 输出颜色的序号
        const int index = static_cast<const int>(it->param * 1023);
        ///< 给定的特征点直接使用，不需要计算
        result[index * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
        result[index * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
        result[index * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
        result[index * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);

        if (it == cm.mColorNodes.begin())
            continue;

        ///< 两个特征点之间序号的间隔
        int distance = index - preIndex - 1;
        ///< 两个特征点之间颜色的间隔
        float rStep = it->color.r - preColor.r;
        float gStep = it->color.g - preColor.g;
        float bStep = it->color.b - preColor.b;
        float aStep = it->color.a - preColor.a;
        ///< 两个特征点之间的所有插值
        for (int i = preIndex + 1; i < index; i++)
        {
            if (false)
            {
                result[i * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);
            }
            else
            {
                result[i * 4 + 0] = static_cast<unsigned char>((preColor.r + (i - preIndex) * rStep / distance) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((preColor.g + (i - preIndex) * gStep / distance) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((preColor.b + (i - preIndex) * bStep / distance) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((preColor.a + (i - preIndex) * aStep / distance) * 255);
            }

        }
        preIndex = index;
        preColor = it->color;
    }
    return result;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec4 aPosParam;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"out VS_OUT{vec2 TexCoord;} vs_out;\n"
"void main()\n"
"{\n"
"   vs_out.TexCoord = vec2(aPosParam.w,0.0f);\n"
"   gl_Position = projection * view * model * vec4(aPosParam.xyz, 1.0f);\n"
"}\0";
const char* FS = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D myTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(myTexture,fTexCoord);\n"
"}\n\0";

const char* GS = "#version 330 core\n"
"layout(lines) in;\n"
"layout(line_strip, max_vertices = 2) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数

"in VS_OUT{\n"
"    vec2 TexCoord;\n"
"} gs_in[];\n"  //注意此处是gs_in，是自定义数据，不是gl_in[]


"out vec2 fTexCoord;\n"
"void main()\n"
"{\n"
"   fTexCoord = gs_in[0].TexCoord;\n"  //纹理坐标

"    gl_Position = gl_in[0].gl_Position;\n"
"    EmitVertex();\n"

"    gl_Position = gl_in[1].gl_Position;\n"

"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  //开启深度测试，后面还需要清除GL_DEPTH_BUFFER_BIT

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // geometry shader
    unsigned int geomotryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geomotryShader, 1, &GS, NULL);
    glCompileShader(geomotryShader);
    // check for shader compile errors
    glGetShaderiv(geomotryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geomotryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geomotryShader);

    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geomotryShader);

    //纹理
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    ColorMap cm;

    cm.AddNode(0.0, Color{ 1.0, 0.0, 0.0, 1.0 });
    cm.AddNode(0.3, Color{ 1.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.5, Color{ 0.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.8, Color{ 0.0, 1.0, 1.0, 1.0 });
    cm.AddNode(1.0, Color{ 0.0, 0.0, 1.0, 1.0 });

    unsigned char* retx = GenTexture(cm);

    if (retx)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, retx);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float sqrt3_20 = sqrt(3) / 20;

    float vertices[] = {
         0.0f,      0.0f,  0.0f,
         0.0f,      1.0f,  0.0f,

         0.0f,      1.0f,  0.0f,
         0.0f,      0.9f, -0.1f,

         0.0f,      1.0f,  0.0f,
        -sqrt3_20,  0.9f,  0.05f,

         0.0f,      1.0f,  0.0f,
         sqrt3_20,  0.9f,  0.05f,
    };
    std::vector<glm::mat4> vecMat;
    vecMat.emplace_back(genMatrix(Vertex(1.0, 0.0, 0.0), Vertex(2.0, 0.0, 0.0)));
    vecMat.emplace_back(genMatrix(Vertex(0.0, 1.0, 0.0), Vertex(0.0, 2.0, 0.0)));
    vecMat.emplace_back(genMatrix(Vertex(0.0, 0.0, 1.0), Vertex(0.0, 0.0, 2.0)));

    std::vector<float> newVertices;
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            auto ret = vecMat[i] * glm::vec4(vertices[j * 3 + 0], vertices[j * 3 + 1], vertices[j * 3 + 2], 1.0f);

            newVertices.emplace_back(ret.x);
            newVertices.emplace_back(ret.y);
            newVertices.emplace_back(ret.z);

            //newVertices.emplace_back(1.0f);  //颜色参数
        }
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * newVertices.size(), newVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //3d模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()/*glm::radians(-45.0f)*/, glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //3d观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //投影

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_2D, texture1);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_LINES, 0, 24);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

#endif // TEST15

#ifdef TEST16
//添加了三个uniform，模型矩阵，观察矩阵，投影矩阵，将这三个矩阵同gl_Position相乘（注意顺序），再开启深度缓冲就可以实现立体效果
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <list>

struct Vertex
{

    //云图添加变换矩阵
    //start end越界
    float x;
    float y;
    float z;

    Vertex(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) :
        x(_x),
        y(_y),
        z(_z)
    {}

    Vertex operator-(const Vertex& v)
    {
        return Vertex(this->x - v.x, this->y - v.y, this->z - v.z);
    }

    Vertex operator/(int n)
    {
        if (n <= 0)
        {
            return *this;
        }

        return Vertex(this->x / n, this->y / n, this->z / n);
    }
};

struct Color
{
    float r, g, b, a; // 每个分量的取值范围为[0.0, 1.0]
};

/// 颜色映射表
class ColorMap
{
public:
    bool IsEmpty() const;

    // 根据param，将节点插入mColorNodes中
    void AddNode(float param, Color color) { mColorNodes.emplace_back(ColorNode{ param,color }); }
    void Pop() {}

    Color Lookup(float param) const {}

    //const size_t size() const { return mColorNodes.size(); }

    void Clear() {}
private:
    struct ColorNode
    {
        float param;
        Color color;
    };
public:
    std::list<ColorNode> mColorNodes;
};

unsigned char* GenTexture(ColorMap myCM)
{
    //归一化
    ColorMap cm = myCM;
    auto start = cm.mColorNodes.front().param;
    auto end = cm.mColorNodes.back().param;

    for (auto& elem : cm.mColorNodes)
    {
        elem.param = elem.param / (end - start);
    }


    ///< 生成纹理的结果
    unsigned char* result = new unsigned char[1024 * 4];
    ///< 用来保存前一个特征点的序号
    int preIndex = 0;
    ///< 用来保存前一个特征点的颜色
    Color preColor = cm.mColorNodes.begin()->color;
    for (auto it = cm.mColorNodes.begin(); it != cm.mColorNodes.end(); it++)
    {
        ///< 输出颜色的序号
        const int index = static_cast<const int>(it->param * 1023);
        ///< 给定的特征点直接使用，不需要计算
        result[index * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
        result[index * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
        result[index * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
        result[index * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);

        if (it == cm.mColorNodes.begin())
            continue;

        ///< 两个特征点之间序号的间隔
        int distance = index - preIndex - 1;
        ///< 两个特征点之间颜色的间隔
        float rStep = it->color.r - preColor.r;
        float gStep = it->color.g - preColor.g;
        float bStep = it->color.b - preColor.b;
        float aStep = it->color.a - preColor.a;
        ///< 两个特征点之间的所有插值
        for (int i = preIndex + 1; i < index; i++)
        {
            if (false)
            {
                result[i * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);
            }
            else
            {
                result[i * 4 + 0] = static_cast<unsigned char>((preColor.r + (i - preIndex) * rStep / distance) * 255);
                result[i * 4 + 1] = static_cast<unsigned char>((preColor.g + (i - preIndex) * gStep / distance) * 255);
                result[i * 4 + 2] = static_cast<unsigned char>((preColor.b + (i - preIndex) * bStep / distance) * 255);
                result[i * 4 + 3] = static_cast<unsigned char>((preColor.a + (i - preIndex) * aStep / distance) * 255);
            }

        }
        preIndex = index;
        preColor = it->color;
    }
    return result;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec4 aPosParam;\n"
//"uniform mat4 view;\n"
//"uniform mat4 projection;\n"
//"uniform mat4 model;\n"
"uniform float maxParam;\n"
"uniform float minParam;\n"
"out VS_OUT{vec2 TexCoord;} vs_out;\n"
"void main()\n"
"{\n"
"float paramDistance = maxParam - minParam;\n"
"   vs_out.TexCoord = vec2((aPosParam.w - minParam)/paramDistance,0.0f);\n"
//"   gl_Position = projection * view * model * vec4(aPosParam.xyz, 1.0f);\n"
"   gl_Position = vec4(aPosParam.xyz,1.0f);\n"
"}\0";
const char* FS = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D myTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(myTexture,fTexCoord);\n"
"}\n\0";

const char* GS = "#version 330 core\n"
"layout(lines) in;\n"
"layout(line_strip, max_vertices = 8) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数
"in VS_OUT{\n"
"    vec2 TexCoord;\n"
"} gs_in[];\n"  //注意此处是gs_in，是自定义数据，不是gl_in[]
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"mat4 genMatrix(vec3 v0, vec3 v1)                                                                        \n"
"{                                                                                                            \n"
"    vec3 r = vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);                                                        \n"
"    vec3 s = vec3(1.0f,0.0f,0.0f);                                                                                        \n"
"                                                                                                            \n"
"    if (abs(r.x) <= abs(r.y) && abs(r.x) <= abs(r.z))                                                    \n"
"    {                                                                                                        \n"
"        s.x = 0.0f;                                                                                            \n"
"        s.y = -r.z;                                                                                            \n"
"        s.z = r.y;                                                                                            \n"
"    }                                                                                                        \n"
"    else if (abs(r.y) <= abs(r.x) && abs(r.y) <= abs(r.z))                                                \n"
"    {                                                                                                        \n"
"        s.x = -r.z;                                                                                            \n"
"        s.y = 0.0f;                                                                                            \n"
"        s.z = r.x;                                                                                            \n"
"    }                                                                                                        \n"
"    else if (abs(r.z) <= abs(r.x) && abs(r.z) <= abs(r.y))                                                \n"
"    {                                                                                                        \n"
"        s.x = -r.y;                                                                                            \n"
"        s.y = r.x;                                                                                            \n"
"        s.z = 0.0f;                                                                                            \n"
"    }                                                                                                        \n"
//" s = normalize(s)\n;"
//"    vec3 t = r * s;                                                                                    \n"
"    vec3 t = vec3(abs(r.y * s.z - r.z * s.y), abs(r.z * s.x - r.x * s.z), abs(r.x * s.y - r.y * s.x));        \n"
"     return mat4(\n"
"        r.x, s.x, t.x, 0.0, \n"
"        r.y, s.y, t.y, 0.0,    \n"
"        r.z, s.z, t.z, 0.0,    \n"
"        v0.x, v0.y, v0.z, 1.0f);\n"
"}    \n"
"out vec2 fTexCoord;\n"
"void main()\n"
"{\n"
//"float sqrt3_20 = sqrt(3) / 20;          \n"
//"vec4 vertices[8] = vec4[]\n"
//"(\n"
//"(0.0f, 0.0f, 0.0f, 1.0f),\n"
//"(0.0f, 1.0f, 0.0f, 1.0f),\n"
//"(0.0f, 1.0f, 0.0f, 1.0f),\n"
//"(0.0f, 0.9f, -0.1f,1.0f),\n"
//"(0.0f,1.0f,0.0f,1.0f),\n"
//"(-0.09,0.9f,0.05f,1.0f),\n"
//"(0.0f,1.0f,0.0f, 1.0f),\n"
//"(0.09,0.9f,0.05f,1.0f)\n"
//");\n"
" float vertices2[32] = float[]\n"
"(0.0f, 0.0f, 0.0f, 1.0f, \n"
"0.0f, 0.5f, 0.0f, 1.0f,\n"

"0.0f, 0.5f, 0.0f, 1.0f,\n"
"0.0f, 0.4f, -0.1f,1.0f,\n"

"0.0f,0.5f,0.0f,1.0f,\n"
"-0.0866f,0.4f,0.05f,1.0f,\n"

"0.0f,0.5f,0.0f, 1.0f,\n"
"0.0866f,0.4f,0.05f,1.0f);\n"
//"vec4 vertices3[2] = vec4[]((1.0f,0.0f,0.0f,1.0f),(0.0f,0.0f,0.0f,1.0f));\n"
"   fTexCoord = gs_in[0].TexCoord;\n"  //纹理坐标
//"   fTexCoord = vec2(0.5f,0.0f);\n"
"float len = length(gl_in[1].gl_Position - gl_in[0].gl_Position);\n"
"vec4 v = gl_in[0].gl_Position + (gl_in[1].gl_Position - gl_in[0].gl_Position)/len;\n"
"   mat4 mat = genMatrix(vec3(gl_in[0].gl_Position.xyz),v.xyz);\n"
"for (int i = 0; i < 8; i++)                                                                        \n"
"{                                                                                                    \n"
//"  vec4 pos = mat * vertices3[i];\n"

"vec4 pos = mat * vec4(vertices2[i*4],vertices2[i*4+1],vertices2[i*4+2],vertices2[i*4+3]);\n"
" gl_Position = projection * view * model * pos;\n"
//"   gl_Position = vertices[i];\n"
"    EmitVertex();                                                                                    \n"
"}                                                                                                    \n"
//"    gl_Position = gl_in[0].gl_Position;\n"
//"    EmitVertex();\n"
//"    gl_Position = gl_in[1].gl_Position;\n"
//"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  //开启深度测试，后面还需要清除GL_DEPTH_BUFFER_BIT

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // geometry shader
    unsigned int geomotryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geomotryShader, 1, &GS, NULL);
    glCompileShader(geomotryShader);
    // check for shader compile errors
    glGetShaderiv(geomotryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geomotryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geomotryShader);

    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geomotryShader);

    //纹理
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    ColorMap cm;

    cm.AddNode(0.0, Color{ 1.0, 0.0, 0.0, 1.0 });
    cm.AddNode(0.3, Color{ 1.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.5, Color{ 0.0, 1.0, 0.0, 1.0 });
    cm.AddNode(0.8, Color{ 0.0, 1.0, 1.0, 1.0 });
    cm.AddNode(1.0, Color{ 1.0, 0.0, 1.0, 1.0 });

    unsigned char* retx = GenTexture(cm);

    if (retx)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, retx);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    //传入的箭头主轴顶点，不需要归一化，前提是箭头颜色参数已经归一化
    //

    float vertices[] = {
        1.0, 0.0, 0.0, 0.0f,
        3.0, 0.0, 0.0, 0.0f,

        0.0, 1.0, 0.0, 2.0f,
        0.0, 2.0, 0.0, 2.0f,

        0.0, 0.0, 1.0, 5.0f,
        0.0, 0.0, 2.0, 5.0f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //3d模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()/*glm::radians(-45.0f)*/, glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //3d观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //投影

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform1f(glGetUniformLocation(shaderProgram, "maxParam"), 5.0f);
        glUniform1f(glGetUniformLocation(shaderProgram, "minParam"), 0.0f);

        glBindTexture(GL_TEXTURE_2D, texture1);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_LINES, 0, 6);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

#endif // TEST16

#ifdef TEST17
//添加了三个uniform，模型矩阵，观察矩阵，投影矩阵，将这三个矩阵同gl_Position相乘（注意顺序），再开启深度缓冲就可以实现立体效果
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);
void mousePress_callback(GLFWwindow* window, int btn, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool firstMove = true;  //鼠标是否第一次移动
double lastXPos = 0.0f;
double lastYPos = 0.0f;  //鼠标上一次的位置
float xOffset;
float yOffset;
float rad = 0.0f;
float fScal = 0.0f;
float fov = 45.0f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor =  vec4(fColor.rgb, 1.0f);\n"
"}\n\0";

//
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);        //鼠标位置回调函数
    glfwSetScrollCallback(window, scroll_callback);          //鼠标滚轮回调函数
    glfwSetMouseButtonCallback(window, mousePress_callback); //鼠标按下回调函数

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
    };

    float vertices1[] = {
         0.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,
         0.0f, -1.0f,  0.0f,    1.0f, 0.0f, 0.0f,
        -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 0.0f,
         1.0f,  0.0f,  0.0f,    1.0f, 0.0f, 0.0f,
    };

    //! [0]
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    //! [0]

    //! [1]
    unsigned int VBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    //! [1]

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //std::cout << (float)glfwGetTime() << std::endl;

        //模型矩阵可以实现实体旋转
        //观察矩阵可以实现实体平移，缩放，旋转  glm::lookAt()：设置观察位置 glm::translate()：通过向某一方向平移设置观察位置
        //投影矩阵可以实现实体缩放

        //3d模型矩阵
        //glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(1.0f, 1.0f, 0.0f));

        //3d观察矩阵
        //glm::mat4 view = glm::mat4(1.0f);
        //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        //glm::mat4 projection = glm::mat4(1.0f);
        //glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);  //正射投影
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //透视投影

        //! [0] 两根分割线
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        //! [0]

        //! [1] 左下
        glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0);
        //! [1]

        //! [2] 右上
        glViewport(SCR_WIDTH / 2, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);

        rad += xOffset;
        glm::mat4 model2 = glm::rotate(glm::mat4(1.0f), rad / 10.0f, glm::vec3(1.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model2));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0);
        //! [2]

        //! [3] 左上
        glViewport(0, SCR_HEIGHT / 2, SCR_WIDTH / 2, SCR_HEIGHT / 2);

        glm::mat4 view3 = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view3));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0);
        //! [3]

        //! [4] 右下
        glm::mat4 projection4 = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT / 2);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection4));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 72);
        glBindVertexArray(0);
        //! [4]

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, 0.0f, 0.2f); //摄像机向z轴平移0.2个单位（图像越来越小）
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos += glm::vec3(0.2f, 0.0f, 0.0f); //摄像机向x轴平移0.2个单位（图像向左边平移，大小不变）
    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
        std::cout << "1";
    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
        std::cout << "2";
    if (glfwGetKey(window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
        std::cout << "3";
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
    if (firstMove)
    {
        lastXPos = x;
        lastYPos = y;
        firstMove = false;
    }

    xOffset = x - lastXPos;
    yOffset = y - lastYPos;

    lastXPos = x;
    lastYPos = y;

    //std::cout << "X:"<<lastXPos << ",  Y:" << lastYPos << '\n';

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)  //获取当前鼠标按键的状态
    {
        //std::cout << xOffset << "," << yOffset << '\n';
        cameraPos += glm::vec3(-xOffset / 200, yOffset / 200, 0.0f);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) /*== GLFW_PRESS*/)  //获取当前鼠标按键的状态
    {
        fScal += 0.1f;
    }

}

//1.鼠标按键（右键，左键，滚轮等）
//2.两种状态（按下还是释放）
//3.用来判断是否按下了shift capslock ctrl等控制键时点击鼠标，和glfwGetKey不一样
void mousePress_callback(GLFWwindow* window, int btn, int action, int mods)
{
    if (btn == GLFW_MOUSE_BUTTON_LEFT && mods == GLFW_MOD_SHIFT)
    {
        //按下shift，再点击鼠标左键按钮就会执行该语句，按下和释放共两次
        std::cout << "pressed shift & mouse left btn\n";
    }

    if (btn == GLFW_MOUSE_BUTTON_LEFT && mods == GLFW_MOD_SHIFT && action == GLFW_RELEASE)
    {
        //按下shift，再点击鼠标左键按钮后松开时会执行该语句
        std::cout << "pressed shift & mouse left btn & release btn\n";
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // yoffset : 向自己滚动是负数，远离自己滚动是正数，
    // xoffset : 一直是0

    //std::cout << xoffset << "," << yoffset << "\n";

    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

#endif // TEST17

#ifdef TEST18

/*
可以共享的资源：

纹理；
shader；
program 着色器程序；
buffer 类对象，如 VBO、 EBO、 RBO 等 。
不可以共享的资源：

FBO 帧缓冲区对象（不属于 buffer 类）；
VAO 顶点数组对象（不属于 buffer 类）。
这里解释下，在不可以共享的资源中，FBO 和 VAO 属于资源管理型对象，FBO 负责管理几种缓冲区，本身不占用资源，VAO 负责管理 VBO 或 EBO ，本身也不占用资源。

作者：字节流动
链接：https://www.jianshu.com/p/fb346e3eadf7
来源：简书
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
*/

// 注意包含顺序，glfw必须再glad之后
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

static const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"uniform float color;"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"   fColor = vec3(color,0.0,0.0);"
"}\0";
static const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";


class Render
{
private:
    const uint32_t m_windoWidth;
    const uint32_t m_windowHeight;
public:
    Render();
    ~Render() = default;

public:
    void processInput(GLFWwindow* window);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void doRender(std::string);
};

Render::Render() :
    m_windowHeight(600),
    m_windoWidth(800)
{
}

void Render::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Render::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Render::doRender(std::string name)
{
    //初始化
    //glfwInit();
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_windoWidth, m_windowHeight, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw "error";
    }

    //设置当前上下文，回调函数（键盘，鼠标等）
    glfwMakeContextCurrent(window);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //gladLoadGL();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw "error";
    }

    //着色器 -> 创建 加载 编译 链接 绑定
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
         0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
         0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
    };

    //缓冲对象：创建 绑定类型 链接数据 配置数据
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

    //glfwGetCurrentContext
    //glfwMakeContextCurrent
    //glfwCreateWindow
    //glXCreateContext
    //glShareLists


    //循环绘制
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        auto time = glfwGetTime();

        std::cout << time << std::endl;

        glUniform1f(glGetUniformLocation(shaderProgram, "color"), (float)time / 20.0f);

        glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
        glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);


}


int main()
{

    //在主线程初始化glfw，一个程序只能初始化一次glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Render render1;
    //Render render2;
    //render1.doRender();
    //render2.doRender();

    Render render1;
    Render render2;

    std::thread t1(&Render::doRender, render1, "1111");
    std::thread t2(&Render::doRender, render2, "2222");
    t1.join();
    t2.join();


    //在所有线程调用完glfw后，再结束glfw
    glfwTerminate();

    return 0;
}

#endif //TEST18

#ifdef TEST19

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec3 aNormal;\n"

"out vec3 fColor;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"

"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"

"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   FragPos = vec3(model * vec4(aPos, 1.0f));\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"

"out vec4 FragColor;\n"

//材质
"struct Material {                 \n"
"    vec3 ambient;                 \n"
"    vec3 diffuse;                 \n"
"    vec3 specular;                 \n"
"    float shininess;             \n"
"};                                 \n"

//光照
"struct Light {                     \n"
"    vec3 position;                 \n"
"    vec3 ambient;                 \n"
"    vec3 diffuse;                 \n"
"    vec3 specular;                 \n"
"};                                 \n"

"uniform vec3 viewPos;             \n"
"uniform Material material;         \n"
"uniform Light light;             \n"

"void main()\n"
"{\n"
//ambient 环境光照
"    vec3 ambient = light.ambient * material.ambient;\n"

//diffuse 漫反射光照
"   vec3 norm = normalize(Normal);                                   \n"
"   vec3 lightDir = normalize(light.position - FragPos);           \n"
"   float diff = max(dot(norm, lightDir), 0.0);                       \n"
"   vec3 diffuse = light.diffuse * (diff * material.diffuse);       \n"

//specular 镜面光照
"   vec3 viewDir = normalize(viewPos - FragPos);                                \n"
"   vec3 reflectDir = reflect(-lightDir, norm);                                    \n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);    \n"
"   vec3 specular = light.specular * (spec * material.specular);                \n"

//result
"    vec3 result = fColor * (ambient + diffuse + specular);\n"
"   FragColor =  vec4(result, 1.0f);\n"
"}\n\0";

//顶点数据
float vertices[] = {
    //坐标                  //颜色               //法线
   -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
   -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,
   -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,     0.0f,  0.0f, -1.0f,

   -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,
   -0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,
   -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,     0.0f,  0.0f, 1.0f,

   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,     1.0f,  0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 0.0f,     1.0f,  0.0f,  0.0f,

   -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,
   -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 1.0f,     0.0f, -1.0f,  0.0f,

   -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f,
   -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 1.0f,     0.0f,  1.0f,  0.0f
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  //开启深度测试，后面还需要清除GL_DEPTH_BUFFER_BIT

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //坐标
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    //颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //法线
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //循环渲染
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //3d模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime()/*glm::radians(-45.0f)*/, glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //3d观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); //平移 向z轴反方向平移n个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //投影

        //立体效果
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        //光照
        glUniform3f(glGetUniformLocation(shaderProgram, "light.position"), 2.0f, 2.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.diffuse"), 2.0f, 2.0f, 2.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "light.specular"), 1.0f, 1.0f, 1.0f);

        //材质
        glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 1.0f, 0.5f, 0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 1.0f, 0.5f, 0.3f);
        glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 1.0f);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 36 * 9);
        // glBindVertexArray(0); // no need to unbind it every time 

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

//键盘输入
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

//窗口调整
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif // TEST19

#ifdef TEST22
//添加了三个uniform，模型矩阵，观察矩阵，投影矩阵，将这三个矩阵同gl_Position相乘（注意顺序），再开启深度缓冲就可以实现立体效果
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor =  vec4(fColor.r, fColor.g, fColor.b, 1.0f);\n"
"}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);  //开启深度测试，后面还需要清除GL_DEPTH_BUFFER_BIT

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLineWidth(5.0f);

        //3d模型矩阵
        //glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime()/*glm::radians(-45.0f)*/, glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
        //3d观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        //glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
        //3d投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);  //投影


        //! [0] 
        //! 只需要将3维坐标的x和y提取出来就是2维坐标
        glm::vec4 point(-0.5f, 0.5f, 0.0f, 1.0f);
        auto ret = model * point;
        //auto ret = glm::inverse(projection) * glm::inverse(view) * glm::inverse(model) * point;
        //auto ret = point * glm::inverse(projection) * glm::inverse(view) * glm::inverse(model) ;
        //auto ret = glm::inverse(model) * glm::inverse(view) * glm::inverse(projection) * point;

        float vertices2[] = { ret[0],ret[1],0.0f,0.0f,1.0f,0.0f };

        //auto ret1 = glm::inverse(projection) * glm::inverse(view) * glm::inverse(model) * model * view *projection * point;
        auto ret1 = projection * view * model * point * glm::inverse(model) * glm::inverse(view) * glm::inverse(projection);

        unsigned int VBO1, VAO1;
        glGenVertexArrays(1, &VAO1);
        glGenBuffers(1, &VBO1);

        glBindVertexArray(VAO1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        //![0] 

        //! [1]
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
        //! [1]

        //! [2]
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        glPointSize(8.0f);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
        //! [2]

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

#endif // TEST22

