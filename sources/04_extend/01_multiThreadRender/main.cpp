
#include "gllear.h"
#include "GlobalInit.h"
#include "WindowsManager.h"
#include "MyWindow.h"
#include "ProgramInit.h"
#include <tuple>
#include <sstream>
#include <chrono>


unsigned int vertexbuffer{};
unsigned int clbuffer{};
unsigned int uvbuffer{};

unsigned int rectVertexBuffer{};
unsigned int rectUVBuffer{};
GLuint textureID = 0;
void BindData()
{
    float vertices[] = {
            -1.f,        -1.f,         0.f,
             0.f,         1.f,         0.f,
             1.f,        -1.f,         0.f,
            -0.9f,        0.3f,        0.3f,
             0.2f,        0.6f,         1.f,
             0.5f,       -0.4f,       -0.6f,
    };
    for (auto& v : vertices)
    {
        v *= 400;
    }

    float cls[] = {
            0.8f,    0.6f,    0.1f,
            0.6f,    0.4f,    0.2f,
            0.4f,    0.7f,    0.8f,
            0.f,    1.f,    1.f,
            0.f,    1.f,    1.f,
            1.f,    0.f,    1.f,
    };

    //创建顶点缓冲对象 顶点数组对象 索引缓冲对象
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &clbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, clbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cls), cls, GL_STATIC_DRAW);



}

void BindRectData()
{
    float vertices[] = {
            -1.f, -1.f, 0.f,
             1.f, -1.f, 0.f,
             1.f,  1.f, 0.f,
            -1.f,  1.f, 0.f,

    };
    for (auto& v : vertices)
    {
        v *= 400.f;
    }
    float uv[] = {
            0.f, 0.f,
            1.f, 0.f,
            1.f, 1.f,
            0.f, 1.f
    };

    //创建顶点缓冲对象 顶点数组对象 索引缓冲对象
    glGenBuffers(1, &rectVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    if (auto err = glGetError())
    {
        std::cout << glewGetErrorString(err) << std::endl;;
    }
    glGenBuffers(1, &rectUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rectUVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
    if (auto err = glGetError())
    {
        std::cout << glewGetErrorString(err) << std::endl;;
    }
}

void doSomeThing()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

void DrawRect(unsigned int width, unsigned int height)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, rectVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, rectUVBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
void drawObject1(unsigned int width, unsigned int height)
{
    doSomeThing();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, clbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 3, 3);

}
void drawObject2(unsigned int width, unsigned int height)
{
    doSomeThing();
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, clbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawBoth(unsigned int width, unsigned int height)
{
    drawObject1(width, height);
    drawObject2(width, height);
}

void drawNothing(unsigned int width, unsigned int height)
{

}

DrawFunc drawFuncs[3]{ DrawRect, drawObject1, drawObject2 };
std::string winTitle[]{ "main canvas", "canvas1", "canvas2" };
ProgramType type[]{ ProgramType::MIXED, ProgramType::BASE, ProgramType::BASE };
const bool useFbo[] = { false, true, true };
std::array<float, 4> bgColor[] = { { 1.f, 0.f, 0.f, 1.f },{ 0.f, 1.f, 0.f, 1.f }, { 0.f, 0.f, 1.f, 1.f } };
const unsigned int width = 800;
const unsigned int height = 800;

void run()
{
    WindowManager wm;
    wm.createMainWindow({ winTitle[0], width, height, drawFuncs[0], type[0], useFbo[0], bgColor[0] });
    for (size_t i = 1; i < 3; i++)
    {
        wm.createWindow({ winTitle[i], width, height, drawFuncs[i], type[i], useFbo[i], bgColor[i] });
    }
    wm.setDependency(winTitle[0], { winTitle[1],winTitle[2] });

    wm.wait();
}

void singleThreadRun()
{
    drawFuncs[0] = drawBoth;
    type[0] = ProgramType::BASE;
    drawFuncs[1] = drawNothing;
    drawFuncs[2] = drawNothing;
    winTitle[0] = "singleMain";
    run();
}

void doubleThreadRun()
{
    drawFuncs[0] = DrawRect;
    type[0] = ProgramType::MIXED;
    winTitle[0] = "main canvas";
    run();
}


int main()
{

    GlobalInitFunc func;
    func();
    singleThreadRun();
    //doubleThreadRun();
    return 0;
}
