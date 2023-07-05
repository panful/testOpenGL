/*
 * 1. 利用transform feedback将顶点着色器的输出获取到cpu
 */

#define TEST1

#ifdef TEST1

#include <common.hpp>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 outPos;
out vec2 outTex;

void main()                                
{
   outPos = vec3(aPos) + vec3(1.0);
   outTex = vec2(aPos.xy) + vec2(2.0);
   gl_Position = vec4(aPos, 1.0);
}
)";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Transform Feedback Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    //--------------------------------------------------------------------------------------------
    // shader program
    GLuint shaderProgram = glCreateProgram();

    // 必须在link之前设置反馈变量
    GLchar const* varyings[] = { "outPos", "outTex" };
    glTransformFeedbackVaryings(shaderProgram, 2, varyings, GL_INTERLEAVED_ATTRIBS);

    glAttachShader(shaderProgram, vertexShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
        return -1;
    }
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);

    //--------------------------------------------------------------------------------------------
    // clang-format off
    float vertices[] = {
         2.0f,  0.0f,  0.0f,
         3.0f,  0.0f,  0.0f,
         4.0f,  0.0f,  0.0f,
    };
    // clang-format on

    GLuint VAO { 0 }, VBO { 0 };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 创建 Transform Feedback 缓冲区
    // sizeof(float) * 3 * (3 + 2) 表示顶点着色器输出3个顶点，每个顶点一个outPos(vec3)和outTex(vec2)
    GLuint tfBuffer { 0 };
    glGenBuffers(1, &tfBuffer);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(float) * 3 * (3 + 2), NULL, GL_STATIC_READ);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 创建 Transform Feedback 对象，并绑定缓冲区
    GLuint tfObject { 0 };
    glGenTransformFeedbacks(1, &tfObject);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tfBuffer);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    //--------------------------------------------------------------------------------------------
    // 启动变换反馈，在绘制结束后停止变换反馈
    glUseProgram(shaderProgram);

    glBindVertexArray(VAO);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfObject);

    // 关闭光栅化，不关闭也不影响，因为不使用片段着色器，所以最好关闭
    glEnable(GL_RASTERIZER_DISCARD);

    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glEndTransformFeedback();

    // 开启光栅化
    glDisable(GL_RASTERIZER_DISCARD);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBindVertexArray(0);

    //--------------------------------------------------------------------------------------------
    // 方式1：读取 Transform Feedback 缓冲区数据
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    void* rawData = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(float) * 3 * (3 + 2), GL_MAP_READ_BIT);
    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    float* p = reinterpret_cast<float*>(rawData);
    for (int i = 0; i < 3; i++)
    {
        std::cout << "outPos " << i << " : " << p[i * 5 + 0] << ", " << p[i * 5 + 1] << ", " << p[i * 5 + 2] << '\t';
        std::cout << "outTex " << i << " : " << p[i * 5 + 3] << ", " << p[i * 5 + 4] << '\n';
    }

    //--------------------------------------------------------------------------------------------
    // 方式2：读取 Transform Feedback 缓冲区数据
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, tfBuffer);
    GLfloat p2[15](0.0f);
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(p2), p2);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    std::cout << "-----------------------------------------\n";
    for (int i = 0; i < 3; i++)
    {
        std::cout << "outPos " << i << " : " << p2[i * 5 + 0] << ", " << p2[i * 5 + 1] << ", " << p2[i * 5 + 2] << '\t';
        std::cout << "outTex " << i << " : " << p2[i * 5 + 3] << ", " << p2[i * 5 + 4] << '\n';
    }

    // remember to delete buffers
    glfwTerminate();
    return 0;
}

#endif // TEST1
