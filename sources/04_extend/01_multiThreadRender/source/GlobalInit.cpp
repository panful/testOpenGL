#include "GlobalInit.h"
#include <glfw/glfw3.h>
void GlobalInitFunc::operator()()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                  // �������汾��
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                  // ���ôΰ汾��
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
