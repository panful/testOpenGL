
/**
 * 1. glfw鼠标、键盘、窗口事件回调函数
 */

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <iostream>

// 窗口大小
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 键盘
void processInput(GLFWwindow* window);
// 鼠标位置
void cursorPosCB(GLFWwindow* window, double xpos, double ypos);
// 键盘按下抬起
void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods);
// 鼠标滚轮
void scrollCB(GLFWwindow* window, double xoffset, double yoffset);
// 鼠标按下抬起
void mouseCB(GLFWwindow* window, int button, int action, int mods);

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core                     \n"
                 "layout (location = 0) in vec3 aPos;                    \n"
                 "void main()                                            \n"
                 "{                                                      \n"
                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);    \n"
                 "}                                                      \n\0";

const char* FS = "#version 330 core                     \n"
                 "void main()                                            \n"
                 "{                                                      \n"
                 "   gl_FragColor = vec4(0.0f,0.8f,0.0f,1.0f);           \n"
                 "}                                                      \n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursorPosCB);
    glfwSetKeyCallback(window, keyCB);
    glfwSetScrollCallback(window, scrollCB);
    glfwSetMouseButtonCallback(window, mouseCB);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Shader & Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f, -0.5f, 0.0f,  // right
        0.0f, 0.5f, 0.0f,   // top
    };

    // Buffer
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    // glfwGetKey获取某个按键的状态
    // 类似的还有 glfwGetMouseButton()等等
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    std::cout << "framebuffer size: " << width << '\t' << height << '\n';
    glViewport(0, 0, width, height);
}

void cursorPosCB(GLFWwindow* window, double xpos, double ypos)
{
    std::cout << "cursor pos: " << xpos << '\t' << ypos << '\n';
}

/// @brief 键盘按下抬起事件
/// @param window   glfw窗口
/// @param key      glfw键码值
/// @param scancode 平添相关的键码值
/// @param action   按下、抬起、重复按下
/// @param mods     控制键是否按下
void keyCB(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // std::cout << "key: " << key << '\t' << scancode << '\t' << action << '\t' << mods << '\n';

    std::string strAction;
    std::string strKey;
    std::string strMods;

    switch (action)
    {
    case GLFW_PRESS:
        strAction = "press\t";
        break;
    case GLFW_RELEASE:
        strAction = "release";
        break;
    case GLFW_REPEAT:
        strAction = "repeat";
        break;
    default:
        strAction = "error";
        break;
    }

    switch (key)
    {
    case GLFW_KEY_0:
    case GLFW_KEY_1:
    case GLFW_KEY_2:
    case GLFW_KEY_3:
    case GLFW_KEY_4:
    case GLFW_KEY_5:
    case GLFW_KEY_6:
    case GLFW_KEY_7:
    case GLFW_KEY_8:
    case GLFW_KEY_9:
        strKey = "number";
        break;
    case GLFW_KEY_A:
    case GLFW_KEY_B:
    case GLFW_KEY_C:
    case GLFW_KEY_D:
    case GLFW_KEY_E:
    // ...
    case GLFW_KEY_Z:
        strKey = "alpha";
        break;
    case GLFW_KEY_RIGHT:
    case GLFW_KEY_LEFT:
    case GLFW_KEY_DOWN:
    case GLFW_KEY_UP:
        strKey = "direction";
        break;
    default:
        strKey = "other";
        break;
    }

    switch (mods)
    {
    case GLFW_MOD_SHIFT:
        strMods = "shift";
        break;
    case GLFW_MOD_CONTROL:
        strMods = "control";
        break;
    case GLFW_MOD_ALT:
        strMods = "alt";
        break;
    case GLFW_MOD_SUPER:
        strMods = "super";
        break;
    case GLFW_MOD_CAPS_LOCK:
        strMods = "caps";
        break;
    case GLFW_MOD_NUM_LOCK:
        strMods = "num";
        break;
    default:
        break;
    }

    std::cout << "key: " << strKey << "\t action: " << strAction << "\t mods: " << strMods << '\n';
}

void scrollCB(GLFWwindow* window, double xoffset, double yoffset)
{
    // std::cout << "scroll: " << xoffset << '\t' << yoffset << '\n';

    if (xoffset > 0.0)
    {
        std::cout << "aleft\n";
    }
    else if (xoffset < 0.0)
    {
        std::cout << "rightward\n";
    }

    if (yoffset > 0.0)
    {
        std::cout << "forward\n";
    }
    else if (yoffset < 0.0)
    {
        std::cout << "backward\n";
    }
}

/// @brief 鼠标按下抬起回调函数
/// @param window glfw窗口
/// @param button 鼠标按钮：左键、右键、中键等
/// @param action 按下、释放
/// @param mods   控制键是否按下：shift ctrl alt等
void mouseCB(GLFWwindow* window, int button, int action, int mods)
{
    // std::cout << "mouse: " << button << '\t' << action << '\t' << mods << '\n';

    switch (action)
    {
    case GLFW_PRESS:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            std::cout << "left button down\n";
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            std::cout << "middle button down\n";
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            std::cout << "right button down\n";
            break;
        default:
            std::cout << "other button down\n";
        }
        break;
    case GLFW_RELEASE:
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            std::cout << "left button up\n";
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            std::cout << "middle button up\n";
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            std::cout << "right button up\n";
            break;
        default:
            std::cout << "other button up\n";
        }
        break;
    default:
        std::cout << "other action\n";
        break;
    }
}
