#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <vector>

#include "Shader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void setBackground(float red, float green, float blue, float alpha);

    void show();
    void shader(const std::string& vs, const std::string& fs);
    void loadModel(std::vector<std::vector<float>>);
    

private:
    void init();
    static void resizeWindow(GLFWwindow* window, int width, int height);
    static void processInput(GLFWwindow* winodw);
    static void mouseCallback(GLFWwindow* window, double x, double y);
    void buffer();
private:
    GLFWwindow* m_window;
    Shader* m_shaderProgram;
    Model* m_model;
    Buffer* m_buffer;
    float* vertices;

    std::vector<float> m_bkColor;
};

Window::Window(int width, int height, const std::string& title)
    :m_window(nullptr),
    m_shaderProgram(nullptr),
    m_buffer(nullptr),
    vertices(nullptr),
    m_bkColor({ 0.5f,0.5f,0.5f,1.0f })
{
    //vertices
    init();

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, resizeWindow);
    glfwSetCursorPosCallback(m_window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    buffer();

    //shader(m_vertexShader, m_fragmentShader);

    //loadModel();

}

Window::~Window()
{
    if (m_window != nullptr)
    {
        delete m_window;
        m_window = nullptr;
    }

    if (m_shaderProgram != nullptr)
    {
        delete m_shaderProgram;
        m_shaderProgram = nullptr;
    }

    if (m_model != nullptr)
    {
        delete m_model;
        m_model = nullptr;
    }

}

void Window::setBackground(float red, float green, float blue, float alpha)
{
    m_bkColor = std::vector<float>{ red,green,blue,alpha };
}

void Window::show()
{
    if (m_window)
    {
        while (!glfwWindowShouldClose(m_window))
        {
            processInput(m_window);

            glBindFramebuffer(GL_FRAMEBUFFER, m_buffer->m_FBO);

            glClearColor(m_bkColor[0], m_bkColor[1], m_bkColor[2], m_bkColor[3]);
            glClear(GL_COLOR_BUFFER_BIT);

            m_shaderProgram->use();

            //3d模型矩阵
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 0.0f)); //旋转，绕x，y轴顺时针旋转45°（可以同时看到多个立方体的表面），第二个参数设置为时间可以一直旋转
            //3d观察矩阵
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)); //平移 向z轴反方向平移5个单位（即向屏幕里面平移，图形会越来越小）
            //3d投影矩阵
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)800.0 / (float)600.0, 0.1f, 100.0f); //投影
            m_shaderProgram->setMat4("view", view);
            m_shaderProgram->setMat4("projection", projection);
            m_shaderProgram->setMat4("model", model);

            m_model->draw();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glBindTexture(GL_TEXTURE, m_buffer->m_texBuffer);
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }
    }
}

void Window::shader(const std::string& vs, const std::string& fs)
{
    m_shaderProgram = new Shader(vs, fs);
}

void Window::loadModel(std::vector<std::vector<float>> data)
{
    m_model = new Model(data);
}

void Window::buffer()
{
    m_buffer = new Buffer();
}

void Window::init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::resizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::processInput(GLFWwindow* winodw)
{
}

void Window::mouseCallback(GLFWwindow* window, double x, double y)
{

}