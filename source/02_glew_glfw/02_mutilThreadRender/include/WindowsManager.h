#pragma once
#include <list>
#include <memory>
#include <thread>
#include <string>
#include <functional>
#include <unordered_map>
#include <array>

class MyProgram;
class MyWindow;
using DrawFunc = std::function<void(unsigned int width, unsigned int height)>;
enum class ProgramType
{
    BASE,
    MIXED,
};
struct WindowInfo
{
    std::string title;
    int width;
    int height;
    DrawFunc func;
    ProgramType programType;
    bool useFBO;
    std::array<float ,4> bgColor;
};
class WindowManager
{
public:
    
public:
    void wait();
    void createWindow(const WindowInfo& windowInfo);
    void createMainWindow(const WindowInfo& windowInfo);

    void setDependency(const std::string & main,std::initializer_list<const std::string> other);
private:
    std::list<std::unique_ptr<std::thread>> threads;
    std::unordered_map<std::string, std::shared_ptr<MyWindow>> windows;
    std::shared_ptr<MyWindow> mainWin;
};