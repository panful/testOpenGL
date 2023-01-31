#include "FboRenderer.h"
#include <QOpenGLFramebufferObject>

#include <iostream>
#include <exception>

FboRenderer::FboRenderer():
    VAO(0),
    VBO(0),
    mProgram(0)
{
    mGraphicFunc = new QOpenGLFunctions_3_3_Core();

    if (initializeOpenGLFunctions())
    {
        std::cout << "init opengl success\n";
    }
    else
    {
        std::cout << "init opengl failed\n";
    }

    try
    {
        init();
    }
    catch (std::exception e)
    {
        std::cout << e.what();
    }
}

void FboRenderer::render()
{
    doRender();

    update();
}

QOpenGLFramebufferObject* FboRenderer::createFramebufferObject(const QSize& size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(4);
    return new QOpenGLFramebufferObject(size, format);
}

void FboRenderer::init()
{
    const char* VS = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "out vec3 fColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "   fColor = aColor;\n"
        "}\0";
    const char* FS = "#version 330 core\n"
        "in vec3 fColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(fColor, 0.5f);\n"
        "}\n\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        throw std::logic_error("vertex shader error!\n");
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        throw std::logic_error("fragment shader error!\n");
    }

    mProgram = glCreateProgram();
    glAttachShader(mProgram, vertexShader);
    glAttachShader(mProgram, fragmentShader);
    glLinkProgram(mProgram);

    glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
        throw std::logic_error("program error!\n");
    }
    glDetachShader(mProgram, vertexShader);
    glDetachShader(mProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,        1.0f,  0.0f,  0.0f,// left  
         0.5f, -0.5f, 0.0f,        0.0f,  1.0f,  0.0f,// right 
         0.0f,  0.5f, 0.0f,        0.0f,  0.0f,  1.0f,// top   
    };

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
}

void FboRenderer::doRender()
{
    //渲染窗口的大小，是在qml里设置的
    auto width = framebufferObject()->width();
    auto height = framebufferObject()->height();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //必须清除深度缓冲，否则窗口变换后，绘制不出来图形。

    glUseProgram(mProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0); //没有这句，窗口变化后图形绘制不出来。
}
