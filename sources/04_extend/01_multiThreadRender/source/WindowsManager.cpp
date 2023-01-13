#include "WindowsManager.h"
#include "MyWindow.h"
#include "GlobalInit.h"
#include "ProgramInit.h"
#include "MyShader.h"
#include <mutex>
#include <iomanip>
#include <windows.h>
#include <dos.h>


void cls()
{
    COORD coor = { 0,0 };   
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coor);
    CONSOLE_CURSOR_INFO cinfo;
    cinfo.bVisible = 0;
    cinfo.dwSize = 1;
    HANDLE fd = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(fd, &cinfo);
}

void WindowManager::wait()
{
    bool allClosed{ false };
    double deltaTime{};
    double nextPrintTime = glfwGetTime() + 1;
    while (true)
    {
        //主窗口处理事件
        mainWin->processEvent();
        //close all when one is closed
        allClosed = false;
        for (auto& it : windows)
        {
            if (glfwWindowShouldClose(it.second->window()))
            {
                allClosed = true;
                break;
            }
        }
        if (allClosed)
        {
            for (auto& it : windows)
            {
                glfwSetWindowShouldClose(it.second->window(), true);
            }
            break;
        }

        //print fps
        if (glfwGetTime() > nextPrintTime)
        {
            std::stringstream ss;
            ss << std::setw(15) << "canvas name" << std::setw(15) << "createdthread" << std::setw(15) << "runingthread" <<
                std::setw(15) << "fps" << std::setw(15) << "sleeptime" << std::setw(15) << "deltaTime" << std::endl;
            for (auto& it : windows)
            {
                ss << std::setw(15) << it.first << std::setw(15) << it.second->getCreateThread() <<
                    std::setw(15) << it.second->getRunThread() <<
                    std::setw(15) << it.second->getFPS() <<
                    std::setw(15) << it.second->getSleepTime() <<
                    std::setw(15) << deltaTime*1000 << std::endl;
            }
            cls();
            std::cout << ss.str();
            nextPrintTime= glfwGetTime()+0.16;
        }

        //sync the rendering threads, 
        bool alldone = {true};
        for (auto& it : windows)
        {
            alldone = alldone && it.second->isDone();
        }
        if (alldone)
        {
            auto trans = mainWin->getTransformation();
            for (auto& it : windows)
            {
                it.second->update(trans);
            }
            for (auto& it : windows)
            {
                it.second->setDone(false);
            }
        }
        deltaTime = glfwGetTime() ;
        glfwPollEvents();
        deltaTime = glfwGetTime() - deltaTime;
    }
    
    for (auto & thread:threads)
    {
        thread->join();
    }
    
}

extern void BindData();
extern void BindRectData();
void WindowManager::createWindow(const WindowInfo& windowInfo)
{
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    std::shared_ptr<MyWindow> win = std::make_shared<MyWindow>(windowInfo.title,
        windowInfo.width, windowInfo.width, windowInfo.func, windowInfo.bgColor, mainWin);
    std::shared_ptr<MyProgram> programBase, programMixed;
    programBase = std::make_shared<MyProgram>();
    programMixed = std::make_shared<MyProgram>();
    ProgramInitFunc initFunc;
    initFunc(programBase, programMixed);
    if (windowInfo.programType == ProgramType::BASE)
    {
        BindData();
        win->setProgram(programBase);
    }
    else
    {
        BindRectData();
        win->setProgram(programMixed);
    }
    if (windowInfo.useFBO)
    {
        win->createFBO();
    }
    windows[windowInfo.title] = win;
    glfwMakeContextCurrent(0);
    threads.emplace_back(std::make_unique<std::thread>([win]()
        {    
            win->render();
        }));
}

void WindowManager::createMainWindow(const WindowInfo& windowInfo)
{
    std::shared_ptr<MyWindow> win = std::make_shared<MyWindow>(windowInfo.title,
        windowInfo.width, windowInfo.width, windowInfo.func, windowInfo.bgColor, nullptr);
    std::shared_ptr<MyProgram> programBase, programMixed;
    programBase = std::make_shared<MyProgram>();
    programMixed = std::make_shared<MyProgram>();
    ProgramInitFunc initFunc;
    initFunc(programBase, programMixed);
    if (windowInfo.programType == ProgramType::BASE)
    {
        BindData();
        win->setProgram(programBase);
    }
    else
    {
        BindRectData();
        win->setProgram(programMixed);
    }
    if (windowInfo.useFBO)
    {
        win->createFBO();

    }
    windows[windowInfo.title] = win;
    glfwMakeContextCurrent(0);
    mainWin = win;
    
    threads.emplace_back(std::make_unique<std::thread>([win]()
        {
            win->render();
        }));
}

void WindowManager::setDependency(const std::string& main, std::initializer_list<const std::string> other)
{
    for (auto title : other)
    {
        mainWin->pushFBO(windows[title]->getFBO());
    }
    for (auto & it : windows)
    {
        ( (it.second))->start();
    }
}
