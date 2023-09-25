/**
 * 01. ImGui基础使用
 * 02. ImGui常用控件：文本、按钮、复选框、单选框、下拉列表、颜色选择框、滑杆、输入框
 */

#define TEST2

#ifdef TEST1

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

const char* VS = R"(#version 330 core
                    layout (location = 0) in vec3 aPos;
                    void main()
                    {
                    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
                    }
                    )";

const char* FS = R"(#version 330 core
                    void main()
                    {
                       gl_FragColor = vec4(0.0f,0.8f,0.0f,1.0f);
                    }
                    )";

int main()
{
    // 初始化GLFW窗口和OpenGL上下文
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "IMGUI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // 加载glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //---------------------------------------------------------------------------------
    // 初始化Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    //---------------------------------------------------------------------------------
    // Shader & Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success { 0 };
    char infoLog[512] { 0 };
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

    //---------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    // clang-format on

    unsigned int VBO { 0 }, VAO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //---------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // 处理输入事件
        glfwPollEvents();

        // 开始新的一帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //---------------------------------------------------------------------------------
        // ImGui::Begin() ImGui::End()之间是一个对话框的内容
        {
            // 绘制GUI元素
            ImGui::Begin("Hello, World");
            ImGui::Text("This is a simple GUI");
            if (ImGui::Button("Test Button"))
            {
                // 当按钮被点击时执行操作
                std::cout << "clicked button\n";
            }
            ImGui::End();
        }

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制自定义图元
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 准备渲染GUI需要用到的数据
        ImGui::Render();

        // 绘制GUI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 清理资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 关闭窗口和释放相关资源
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

const char* VS = R"(#version 330 core
                    layout (location = 0) in vec3 aPos;
                    void main()
                    {
                    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
                    }
                    )";

const char* FS = R"(#version 330 core
                    void main()
                    {
                       gl_FragColor = vec4(0.0f,0.8f,0.0f,1.0f);
                    }
                    )";

int main()
{
    // 初始化GLFW窗口和OpenGL上下文
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "IMGUI", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // 加载glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //---------------------------------------------------------------------------------
    // 初始化Dear ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    //---------------------------------------------------------------------------------
    // Shader & Program
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);
    glCompileShader(vertexShader);

    int success { 0 };
    char infoLog[512] { 0 };
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

    //---------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    // clang-format on

    unsigned int VBO { 0 }, VAO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //---------------------------------------------------------------------------------
    bool open { true };
    float window_x { 0.f }, window_y { 0.f };
    int comboIndex { 0 };
    float slider { 50.f };
    bool checkBox0 { false }, checkBox1 { false }, checkBox2 { false };
    float inputFloat3[3] { 0.f };
    int radio { 0 };
    float color[3] { 1.f };
    while (!glfwWindowShouldClose(window))
    {
        // 处理输入事件
        glfwPollEvents();

        // 开始新的一帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //---------------------------------------------------------------------------------
        {
            if (open)
            {
                ImGui::Begin("Window 0", &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
                {
                    ImGui::SetWindowPos({ window_x, window_y });
                    ImGui::TextColored({ 1.f, 1.f, 0.f, 1.f }, "Text");
                }
                ImGui::End();
            }

            ImGui::Begin("Window 1");
            {
                // 显示文本
                ImGui::Text("This is a simple GUI");

                // 按钮
                if (ImGui::Button("Test Button"))
                {
                    // 当按钮被点击时会进入
                    std::cout << "clicked button\n";
                }

                // 下拉列表框
                const char* const str[] { "0", "1", "2", "3" };
                if (ImGui::Combo("Combo", &comboIndex, str, 4))
                {
                    // 当index改变时会进入
                    std::cout << "index\t" << comboIndex << '\n';
                }

                // 滑杆
                if (ImGui::SliderFloat("Slider", &slider, 10.f, 100.f))
                {
                    std::cout << "Slider\t" << slider << '\n';
                }

                // 复选框
                if (ImGui::Checkbox("Checkbox_0", &checkBox0))
                {
                    std::cout << "Checkbox_0\t" << checkBox0 << '\n';
                }

                if (ImGui::Checkbox("Checkbox_1", &checkBox1))
                {
                    std::cout << "Checkbox_1\t" << checkBox1 << '\n';
                }

                if (ImGui::Checkbox("Checkbox_2", &checkBox2))
                {
                    std::cout << "Checkbox_2\t" << checkBox2 << '\n';
                }

                // 输入框
                if (ImGui::InputFloat3("InputFloat3", inputFloat3))
                {
                    std::cout << "InputFlot3\t" << inputFloat3[0] << '\t' << inputFloat3[1] << '\t' << inputFloat3[2] << '\n';
                }

                // 单选框
                if (ImGui::RadioButton("Radio_0", &radio, 0))
                {
                    std::cout << "Radio\t" << radio << '\n';
                }
                if (ImGui::SameLine(), ImGui::RadioButton("Radio_1", &radio, 1))
                {
                    std::cout << "Radio\t" << radio << '\n';
                }
                if (ImGui::SameLine(), ImGui::RadioButton("Radio_2", &radio, 2))
                {
                    std::cout << "Radio\t" << radio << '\n';
                }

                // 颜色选择框
                if (ImGui::ColorEdit3("ColorEdit3", color))
                {
                    std::cout << "ColorEdit3\t" << color[0] << '\t' << color[1] << '\t' << color[2] << '\n';
                }
            }
            ImGui::End();
        }

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制自定义图元
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // 准备渲染GUI需要用到的数据
        ImGui::Render();

        // 绘制GUI
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 清理资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 关闭窗口和释放相关资源
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

#endif // TEST2
