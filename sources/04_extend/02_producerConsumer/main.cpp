/**
 * 01. 异步执行OpenGL
 * 02. 生产者消费者模型
 */

#define TEST2

#ifdef TEST1

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>
#include <string>
#include <thread>

class OpenGL
{
public:
    OpenGL() noexcept
    {
        std::cout << "OpenGL\tthread id: " << std::this_thread::get_id() << '\n';

        Init();
        TestDraw();
        Run();
    }

private:
    void Init() noexcept
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            return;
        }
    }

    void Run() noexcept
    {
        while (!glfwWindowShouldClose(m_window))
        {
            glClearColor(.1f, .2f, .3f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(m_shaderProgram);
            glBindVertexArray(m_vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);

            glfwPollEvents();
            glfwSwapBuffers(m_window);
        }
    }

    void TestDraw()
    {
        const char* VS = R"(#version 330 core
                            layout (location = 0) in vec3 aPos;
                            void main()
                            { 
                                gl_Position = vec4(aPos, 1.f);
                            })";

        const char* FS = R"(#version 330 core
                            out vec4 FragColor;
                            void main()
                            {
                                FragColor = vec4(0.f,1.f,0.f,1.f);
                            })";

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
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &FS, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);

        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDetachShader(m_shaderProgram, vertexShader);
        glDetachShader(m_shaderProgram, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // clang-format off
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f,
        };
        // clang-format on

        // Buffer
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &VBO);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

private:
    int m_windowWidth { 800 };
    int m_windowHeight { 600 };
    std::string m_windowTitle { "Test" };
    GLFWwindow* m_window { nullptr };
    uint32_t m_vao { 0 };
    uint32_t m_shaderProgram { 0 };
};

class AsyncOpenGL
{
public:
    AsyncOpenGL()
    {
        std::thread t([]() { OpenGL opengl; });
        t.join();
    }
};

int main()
{
    std::cout << "Main\tthread id: " << std::this_thread::get_id() << '\n';
    AsyncOpenGL asyncOgl;

    return 0;
}

#endif // TEST1

#ifdef TEST2

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <condition_variable>
#include <iostream>
#include <list>
#include <string>
#include <thread>

class OpenGL
{
public:
    OpenGL() noexcept
    {
        std::cout << "OpenGL()\n";
        Executor();
    }

    ~OpenGL() noexcept
    {
        std::cout << "~OpenGL()\n";
        if (m_taskThread.joinable())
        {
            m_taskThread.join();
        }
    }

    void AddTask(int t)
    {
        std::cout << "AddTask\tthread id: " << std::this_thread::get_id() << '\n';
        // 此处可以考虑加锁，避免任务排序错误
        m_tasks.emplace_back(t);
        m_cv.notify_one();
    }

private:
    void Executor()
    {
        m_taskThread = std::thread(&OpenGL::DoTask, this);
    }

    void DoTask()
    {
        std::cout << "DoTask\tthread id: " << std::this_thread::get_id() << '\n';

        while (true)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait(lock, [this]() { return !this->m_tasks.empty(); });

            switch (m_tasks.front())
            {
            case 0:
                Init();
                break;
            case 1:
                CreateVAO();
                break;
            case 2:
                Draw();
                break;
            case 3:
                Close();
                return;
            default:
                std::cerr << "task error\n";
                break;
            }

            m_tasks.pop_front();
        }
    }

private:
    void Init() noexcept
    {
        std::cout << "Init\tthread id: " << std::this_thread::get_id() << '\n';

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
        if (!m_window)
        {
            std::cerr << "failed to create GLFW window\n";
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            return;
        }
    }

    void Draw() noexcept
    {
        static int r = 0;
        static int g = 1;
        static int b = 2;

        std::cout << "Draw\tthread id: " << std::this_thread::get_id() << '\n';

        glClearColor(.1f, .2f, .3f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(m_shaderProgram);
        glUniform3f(glGetUniformLocation(m_shaderProgram, "uColor"), static_cast<float>(r), static_cast<float>(g), static_cast<float>(b));
        r = (r + 1) % 2;
        g = (g + 1) % 2;
        b = (b + 1) % 2;

        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    void Close() noexcept
    {
        std::cout << "Close\tthread id: " << std::this_thread::get_id() << '\n';

        glDeleteProgram(m_shaderProgram);
        glDeleteBuffers(1, &m_vao);
        glfwDestroyWindow(m_window);
    }

    void CreateVAO() noexcept
    {
        std::cout << "VAO\tthread id: " << std::this_thread::get_id() << '\n';

        const char* VS = R"(#version 330 core
                            layout (location = 0) in vec3 aPos;
                            void main()
                            { 
                                gl_Position = vec4(aPos, 1.f);
                            })";

        const char* FS = R"(#version 330 core
                            uniform vec3 uColor;
                            out vec4 FragColor;
                            void main()
                            {
                                FragColor = vec4(uColor, 1.f);
                            })";

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
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &FS, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);

        glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDetachShader(m_shaderProgram, vertexShader);
        glDetachShader(m_shaderProgram, fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // clang-format off
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f,
        };
        // clang-format on

        // Buffer
        unsigned int VBO { 0 };
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &VBO);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

private:
    int m_windowWidth { 800 };
    int m_windowHeight { 600 };
    std::string m_windowTitle { "Test" };
    GLFWwindow* m_window { nullptr };
    uint32_t m_vao { 0 };
    uint32_t m_shaderProgram { 0 };

private:
    std::thread m_taskThread;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::list<int> m_tasks;
};

void init(OpenGL& ogl)
{
    ogl.AddTask(0);
}

void create(OpenGL& ogl)
{
    ogl.AddTask(1);
}

void draw(OpenGL& ogl)
{
    ogl.AddTask(2);
}

void close(OpenGL& ogl)
{
    ogl.AddTask(3);
}

// 在不同的线程中添加任务，在独立的一个线程中消费任务，执行OpenGL函数

int main()
{
    std::cout << "Main\tthread id: " << std::this_thread::get_id() << '\n';

    OpenGL opengl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t0(init, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t1(create, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t20(draw, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t21(draw, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t22(draw, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::thread t3(close, std::ref(opengl));
    std::this_thread::sleep_for(std::chrono::seconds(1));

    t0.join();
    t1.join();
    t20.join();
    t21.join();
    t22.join();
    t3.join();

    return 0;
}

#endif // TEST2
