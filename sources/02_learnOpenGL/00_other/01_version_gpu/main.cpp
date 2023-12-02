/**
 * 01.获取OpenGL版本，双显卡切换 https://blog.csdn.net/hhy321/article/details/122322475
 */

#define TEST1

#ifdef TEST1

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

int main(int argc, char** argv)
{
    const unsigned int SCR_WIDTH  = 800;
    const unsigned int SCR_HEIGHT = 600;

    if (!glfwInit())
    {
        return -1;
    }
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 设置窗口隐藏
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLFW", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    printf("OpenGL vendor string: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL renderer string: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version string: %s\n", glGetString(GL_VERSION));

    auto version = glGetString(GL_VERSION);
    std::cout << "version\tmajor: " << version[0] << ", minor: " << version[2] << '\n';

    glfwTerminate();
    return 0;
}
#endif // TEST1
