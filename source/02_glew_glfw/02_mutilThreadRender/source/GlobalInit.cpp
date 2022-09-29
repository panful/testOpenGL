#include "GlobalInit.h"
#include <glfw/glfw3.h>
void GlobalInitFunc::operator()()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // 设置主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // 设置次版本号
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
