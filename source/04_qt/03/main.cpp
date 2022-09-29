/*
1.QPainter使用示例
2.渲染文字
3.用例2文字图片放在main函数外部生成
*/

#define TEST2

#ifdef TEST1

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QString>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QImage img(QSize(800, 400), QImage::Format_ARGB32);  //第二个参数为图片模式，rgb，rgba等等
    img.fill(QColor(0, 0, 0, 0));   //填充为透明 Qt::transparent

    QPainter painter;

    //字体,QGuiApplication实例必须存在才能使用QFont
    //QFont font("宋体", 15, QFont::Bold, true);

    //画笔
    QPen pen;
    pen.setColor(Qt::red); //设置画笔颜色

    //画刷
    QBrush brush;
    brush.setColor(Qt::blue);

    painter.begin(&img);  //begin会重置painter，画笔画刷等等需要在begin之后设置才能生效

    painter.setPen(pen);
    //painter.setFont(font); //QPainter有默认字体
    painter.drawRect(QRect(0, 0, 799, 399));  //画一个矩形

    painter.drawText(QRect(0, 0, 10, 10), QString::fromLocal8Bit("1"));  //必须有字体，无论是使用默认的还是自定义的，因此必须实例化QApplication
    painter.drawText(QRect(100, 0, 10, 10), QString::fromLocal8Bit("2"));
    painter.drawText(QRect(200, 0, 10, 10), QString::fromLocal8Bit("3"));
    painter.drawText(QRect(300, 0, 10, 10), QString::fromLocal8Bit("4"));
    painter.drawText(QRect(400, 0, 10, 10), QString::fromLocal8Bit("5"));
    painter.drawText(QRect(500, 0, 10, 10), QString::fromLocal8Bit("6"));
    painter.drawText(QRect(600, 0, 10, 10), QString::fromLocal8Bit("7"));
    painter.drawText(QRect(700, 0, 10, 10), QString::fromLocal8Bit("8"));
    painter.drawText(QRect(800, 0, 10, 10), QString::fromLocal8Bit("9"));  //出了边界绘制不出来

    painter.end();
    img.save("C:\\Users\\yangpan\\Desktop\\aaa.png");

    auto ret = img.bits();

    int i = 0;
    //MyWidget* w = new MyWidget();
    //w->show();

    //return 0;
    return app.exec();
}

#endif // TEST1

#ifdef TEST2

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <QApplication>
#include <QPainter>
#include <QWidget>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";

const char* vs_1 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.xyz, 1.0);\n"
"}\0";
const char* fs_1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f,1.0f,0.0f,0.5f);\n"
"}\n\0";

int main(int argc, char** argv)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
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

    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
    glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //shader program
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

    //vertex shader
    unsigned int v1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v1, 1, &vs_1, NULL);
    glCompileShader(v1);

    glGetShaderiv(v1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int f1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f1, 1, &fs_1, NULL);
    glCompileShader(f1);
    glGetShaderiv(f1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(f1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //shader program
    unsigned int program1 = glCreateProgram();
    glAttachShader(program1, v1);
    glAttachShader(program1, f1);
    glLinkProgram(program1);
    glGetProgramiv(program1, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(v1);
    glDeleteShader(f1);

    //矩形
    float vertices2[] = {
        //位置                   //纹理
        -0.5f,-0.5f,0.0f,        0.0f,1.0f,
         0.5f, 0.5f,0.0f,        1.0f,0.0f,
        -0.5f, 0.5f,0.0f,        0.0f,0.0f,

        -0.5f,-0.5f,0.0f,        0.0f,1.0f,
         0.5f, 0.5f,0.0f,        1.0f,0.0f,
         0.5f,-0.5f,0.0f,        1.0f,1.0f,
    };

    float vertices1[] = {
        -0.8f,-0.8f,0.0f,
         0.0f, 0.8f,0.0f,
         0.8f,-0.8f,0.0f,
    };

    unsigned int VBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //使用QPainter绘制文字并生成图片数据
    QApplication app(argc, argv);

    int width = SCR_WIDTH / 2;
    int height = SCR_HEIGHT / 2;

    QImage img(QSize(width, height), QImage::Format::Format_RGBA8888);
    //img.fill(Qt::transparent);
    img.fill(QColor(0, 0, 0, 0));

    QPainter p;
    QPen pen;
    QBrush brush;
    brush.setColor(Qt::blue);

    pen.setColor(Qt::red);
    p.begin(&img);
    p.setPen(pen);
    p.drawText(QRect(0, 0, 100, 100), "AAA");
    p.drawText(QRect(100, 0, 100, 100), "BBB");
    p.drawText(QRect(0, 100, 100, 100), "AAA");
    p.drawText(QRect(100, 100, 100, 100), "BBB");
    p.drawRect(QRect(0, 0, width - 1, height - 1));
    //p.fillRect(QRect(0, 0, 199, 9), brush);
    p.end();
    auto data = img.bits();

    img.save("C:\\Users\\yangpan\\Desktop\\test.png");

    unsigned char data3[1024 * 4] = { 0 };
    for (int i = 0; i < 1024 * 4; i += 4)
    {
        data3[i + 0] = 255;
        data3[i + 1] = 0;
        data3[i + 2] = 0;
        if (i > 512 * 4)
            data3[i + 3] = 125;
        else
            data3[i + 3] = 0;
    }

    int widthx, heightx, nrChannels;
    auto ret = stbi_load("C:\\Users\\yangpan\\Desktop\\test.png", &widthx, &heightx, &nrChannels, 0);

    //注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    //stbi_image_free(ret);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program1);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // render container
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

#endif //TEST2

#ifdef TEST3

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <QApplication>
#include <QPainter>
#include <QWidget>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";

const char* vs_1 = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.xyz, 1.0);\n"
"}\0";
const char* fs_1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f,1.0f,0.0f,0.5f);\n"
"}\n\0";

class Test
{
    uint8_t* image;
    QImage img;
public:
    Test();
    uint8_t* get() const
    {
        return image;
    }
    QImage getImg() const
    {
        return img;
    }
public:
    void fun(int argc, char** argv);
};

Test::Test()
{
    int width = SCR_WIDTH / 2;
    int height = SCR_HEIGHT / 2;
    img = QImage(QSize(width, height), QImage::Format::Format_RGBA8888);
}

void Test::fun(int argc, char** argv)
{
    //使用QPainter绘制文字并生成图片数据
    QApplication app(argc, argv);
    //img.fill(Qt::transparent);
    img.fill(QColor(0, 0, 0, 0));

    QPainter p;
    QPen pen;
    QBrush brush;
    brush.setColor(Qt::blue);

    pen.setColor(Qt::red);
    p.begin(&img);
    p.setPen(pen);
    p.drawText(QRect(0, 0, 100, 100), "AAA");
    p.drawText(QRect(100, 0, 100, 100), "BBB");
    p.drawText(QRect(0, 100, 100, 100), "AAA");
    p.drawText(QRect(100, 100, 100, 100), "BBB");
    p.drawRect(QRect(0, 0, 399, 299));
    //p.fillRect(QRect(0, 0, 199, 9), brush);
    p.end();
    auto data = img.bits();

    image = data;
    img.save("C:\\Users\\yangpan\\Desktop\\test.png");
    int i = 0;

}


int main(int argc, char** argv)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
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

    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
    glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //shader program
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

    //vertex shader
    unsigned int v1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v1, 1, &vs_1, NULL);
    glCompileShader(v1);

    glGetShaderiv(v1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(v1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int f1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f1, 1, &fs_1, NULL);
    glCompileShader(f1);
    glGetShaderiv(f1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(f1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    //shader program
    unsigned int program1 = glCreateProgram();
    glAttachShader(program1, v1);
    glAttachShader(program1, f1);
    glLinkProgram(program1);
    glGetProgramiv(program1, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(v1);
    glDeleteShader(f1);

    //矩形
    float vertices2[] = {
        //位置                   //纹理
        -0.5f,-0.5f,0.0f,        0.0f,1.0f,
         0.5f, 0.5f,0.0f,        1.0f,0.0f,
        -0.5f, 0.5f,0.0f,        0.0f,0.0f,

        -0.5f,-0.5f,0.0f,        0.0f,1.0f,
         0.5f, 0.5f,0.0f,        1.0f,0.0f,
         0.5f,-0.5f,0.0f,        1.0f,1.0f,
    };

    float vertices1[] = {
        -0.8f,-0.8f,0.0f,
         0.0f, 0.8f,0.0f,
         0.8f,-0.8f,0.0f,
    };

    unsigned int VBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int VBO2, VAO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint8_t* data = nullptr;
    Test t;
    t.fun(argc, argv);
    data = t.get();

    auto img = t.getImg();
    auto sss = img.bits();


    unsigned char data3[1024 * 4] = { 0 };
    for (int i = 0; i < 1024 * 4; i += 4)
    {
        data3[i + 0] = 255;
        data3[i + 1] = 0;
        data3[i + 2] = 0;
        if (i > 512 * 4)
            data3[i + 3] = 125;
        else
            data3[i + 3] = 0;
    }

    int widthx, heightx, nrChannels;
    auto ret = stbi_load("C:\\Users\\yangpan\\Desktop\\test.png", &widthx, &heightx, &nrChannels, 0);

    //注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthx, heightx, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    //stbi_image_free(ret);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program1);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // render container
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

#endif //TEST3