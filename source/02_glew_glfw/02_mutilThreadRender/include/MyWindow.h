#pragma once    
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <string>
#include <memory>
#include <array>
#include <unordered_map>
#include <thread>
#include <functional>
#include <atomic>
#include <sstream>
#include "Transform.h"


class MyTextureInterface;
class MyProgram;
class FBObject;

using DrawFunc = std::function<void(unsigned int width, unsigned int height)>;

class MyWindow
{
public:
    MyWindow()=delete;

    MyWindow(const std::string& title, unsigned int width, unsigned int height, DrawFunc func,const std::array<float,4>& bgColor, std::shared_ptr<MyWindow> sharedWin);

    ~MyWindow();

    void processEvent();

    void setProgram(std::shared_ptr<MyProgram> prog);

    void createFBO();

    std::shared_ptr<FBObject> getFBO() const;

    void pushFBO(std::shared_ptr<FBObject> texture);

    void start();

    GLFWwindow* window() const;

    const std::string& getTitle() const;

    const std::string& getCreateThread() const;

    const std::string& getRunThread() const;

    double getFPS() const;

    double getSleepTime() const;

    void update(const Transformation& trans);

    const Transformation& getTransformation() const;

    bool isDone() const;

    void setDone(bool d);

    void render();
private:
    void createVAO();

    bool draw();

    void initGL();

    void waitForNotify();

    void processEventInternal();

    void processKeyEvent();

    void processMouseEvent();

    void sleep();
private:
    GLFWwindow* win{};
    std::string title{};
    DrawFunc drawFunc{};
    unsigned int width{};
    unsigned int height{};
    std::array<float, 4> backgroundColor{};
    std::shared_ptr<MyProgram> program{};
    std::shared_ptr< FBObject> fboTarget{};
    int lastMouseX{}, lastMouseY{};
    Transformation transformation;
    std::atomic_bool jobDone{};
    std::list<std::shared_ptr< FBObject>> dependentFBO;
    std::atomic_bool startFlag{};
    std::string createThread{};
    std::string runThread{};
    double fps{};
    double sleepTime{};
};