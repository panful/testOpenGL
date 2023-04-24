#include "MyWindow.h"
#include <iostream>
#include <chrono>
#include <mutex>
#include <thread>
#include <sstream>
#include "MyShader.h"
#include "FBObject.h"
#include "MyTexture.h"

#define KEYPRESS(key) glfwGetKey(win, key) == GLFW_PRESS
#define MOUSEPRESS(mousebutton) glfwGetMouseButton(win, mousebutton) == GLFW_PRESS
std::mutex mymutex;
class TimePrinter
{
public:
    TimePrinter(const std::string &label):prefix(label)
    {
        start = std::chrono::system_clock::now();
    }
    long long  getMilSec()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    }
    ~TimePrinter() 
    {
        //printf("%s draw time :%d\n", prefix.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count());
    }
    std::chrono::time_point<std::chrono::system_clock> start{};
    std::string prefix;
};
MyWindow::MyWindow(const std::string& title, unsigned int w, unsigned int h, DrawFunc func, const std::array<float,4>& bgColor, std::shared_ptr<MyWindow> sharedWin) :
    width(w), height(h), drawFunc(func), lastMouseX(0), lastMouseY(0), backgroundColor(bgColor), title(title),createThread(""),runThread(""),fps(0.),transformation()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    createThread = ss.str();
    GLFWwindow * sharing = sharedWin ? sharedWin->win : nullptr;
    win = glfwCreateWindow(w, h, title.c_str(), 0, sharing);
    if (win)
    {
        glfwMakeContextCurrent(win);
        glewInit();
        glfwSetWindowSizeLimits(win, w, h, w, h);
        createVAO();
    }
    else
    {
        printf("%s\n",glewGetErrorString(glfwGetError(nullptr)));
    }
}
void MyWindow::processEvent()
{
    processEventInternal();
}

bool MyWindow::draw()
{
    if (fboTarget)
    {
        drawFunc(width, height);
    }
    else
    {

        unsigned int texBindingIndex = 0;
        if (dependentFBO.size() )
        {
            auto fboIter = dependentFBO.begin();
            texBindingIndex = 0;
            while (fboIter != dependentFBO.end())
            {
                program->setTexture(texBindingIndex, (*fboIter)->getColorTex());
                ++texBindingIndex;
                program->setTexture(texBindingIndex, (*fboIter)->getDepthTex());
                ++texBindingIndex;
                ++fboIter;    
            }
        }

        drawFunc(width, height);
    }
    return true;
}

void MyWindow::setProgram(std::shared_ptr<MyProgram> prog)
{
    program = prog;
}

void MyWindow::createVAO()
{
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void MyWindow::createFBO()
{
    fboTarget = std::make_shared<FBObject>();
    fboTarget->create(width,height );
}

std::shared_ptr<FBObject> MyWindow::getFBO() const
{
    return fboTarget;
}
glm::mat4 gmat{};
void MyWindow::initGL()
{
    program->use(); 
    if (fboTarget)
    {
        fboTarget->use();
    }
    glViewport(0, 0, width, height);
    if (backgroundColor.size() == 4)
    {
        glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
    }
    else
    {
        glClearColor(0.3f, 0.8f, 0.3f, 1.f);
    }
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    auto transMat = transformation.getModelViewMatrix();
    auto projectionMat = glm::ortho(-(width / 2.f), width / 2.f, -(height / 2.f), height / 2.f, 1e3f, -1e3f);
    
    if (title == "main canvas")
    {
        transMat = glm::mat4{ 1.f };
    }
    projectionMat = projectionMat;
    program->setMatrix("mvp", projectionMat*transMat);
    
}

void MyWindow::waitForNotify()
{
    while (jobDone && !glfwWindowShouldClose(win))
    {//sleep untile main thread call update(false)
        //sleep();
    }
}

void MyWindow::processEventInternal()
{
    processKeyEvent();
    processMouseEvent();
}

void MyWindow::processKeyEvent()
{
    if (KEYPRESS(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(win, true);
    }
    if (KEYPRESS(GLFW_KEY_Q))
    {
        transformation.zoomAction(1);
    }
    if (KEYPRESS(GLFW_KEY_E))
    {
        transformation.zoomAction(-1);
    }
    if (KEYPRESS(GLFW_KEY_R))
    {
        transformation.reset();
    }
    if (KEYPRESS(GLFW_KEY_T))
    {
        transformation.rotate(1,1);
    }
}

void MyWindow::processMouseEvent()
{
    
    double x, y;
    glfwGetCursorPos(win, &x, &y);
    if (MOUSEPRESS(GLFW_MOUSE_BUTTON_RIGHT))
    {
        int deltaX = (int)x - lastMouseX;
        int deltaY = -((int)y - lastMouseY);
        transformation.translate(deltaX, deltaY);
    }
    if (MOUSEPRESS(GLFW_MOUSE_BUTTON_LEFT))
    {
        int deltaX = (int)x - lastMouseX;
        int deltaY = -((int)y - lastMouseY);
        transformation.rotate(deltaX, deltaY);
    }
    lastMouseX = (int)x;
    lastMouseY = (int)y;
}

void MyWindow::sleep()
{
    std::this_thread::sleep_for(std::chrono::microseconds(1));
}

void MyWindow::pushFBO(std::shared_ptr<FBObject> fbo)
{
    dependentFBO.emplace_back(fbo);
}

void MyWindow::start()
{
    startFlag = true;
}

GLFWwindow* MyWindow::window() const
{
    return win;
}

const std::string& MyWindow::getTitle() const
{
    return title;
}

const std::string& MyWindow::getCreateThread() const
{
    return createThread;
}

const std::string& MyWindow::getRunThread() const
{
    return runThread;
}

double MyWindow::getFPS() const
{
    return fps;
}

double MyWindow::getSleepTime() const
{
    return sleepTime;
}

void MyWindow::update(const Transformation& trans)
{
    transformation = trans;
    gmat = transformation.getModelViewMatrix();
    if (fboTarget)
    {
        fboTarget->setReady(true);
    }
}

const Transformation& MyWindow::getTransformation() const
{
    return transformation;
}

bool MyWindow::isDone() const
{
    return jobDone;
}

void MyWindow::setDone(bool d)
{
    jobDone = d;
}

void MyWindow::render()
{
    while (!startFlag && !glfwWindowShouldClose(win))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    glfwMakeContextCurrent(win);

    std::stringstream ss;
    ss << std::this_thread::get_id();
    runThread = ss.str();
    double starttime{};
    double sleepStartTime{};
    while (!glfwWindowShouldClose(win))
    {
        starttime = glfwGetTime();
        sleepStartTime = glfwGetTime();
        waitForNotify();
        sleepTime = (glfwGetTime() - sleepStartTime) * 1000;
        initGL();
        if (draw())
        {
            if (fboTarget)
            {
                glFinish();
            }
            else
            {
                glfwSwapBuffers(win);
            }
            jobDone = true;
        }
        fps = 1. / ((glfwGetTime() - starttime));
    }
}



MyWindow::~MyWindow()
{

    
}
