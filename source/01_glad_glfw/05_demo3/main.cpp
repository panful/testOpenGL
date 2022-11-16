/*
1.入门，最简单的三角形
2.加载纹理（多个纹理对应多个顶点数组）  所有的纹理贴图都是插值运算
3.FBO帧缓冲
4.用例3的拷贝版，用来测试拾取 glReadPixels
5.分批顶点属性，位置，颜色，纹理等各有一个数组，而不是所有数据在一个数组中
6.鼠标点击画点
7.UBO:uniform缓冲对象（所有着色器中的相同uniform都是同一个，如果有多个着色器就需要多次设置）
8.几何着色器 Geometry Shader (可以在几何着色器中对输入的顶点进行处理，输入的是一个顶点，但是绘制的可以是根据这个顶点位置生成的其他图形）
9.最简单的EBO方式绘制
91.不使用VBO只使用EBO绘制
10.glfw获取当前视口大小
11.使用FBO时，窗口大小变化
12.采样器和纹理 genSamplers
13.自定义纹理数据（不通过读取图片而是给纹理数据数组的每一个元素赋值）
14.颜色插值
15.颜色映射表 绘制菱形，用自定义的纹理数据贴图
16.FBO使用glviewport，解决放大缩小问题，以及生成指定大小的纹理（1024*1）
17.颜色过渡模式(平滑，梯度） glsl中添加flat修饰符
18.一次生成多个vao
19.uniform buffer进阶使用 (如果使用uniform buffer设置变换矩阵，model需要单独设置uniform）
20.生成颜色
21.几何着色器
22.几何着色器，使用纹理
23.使用fbo渲染输出颜色
*/

#define TEST1

#ifdef TEST1

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

	//循环绘制
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST1

#ifdef TEST2

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";
#include <direct.h>
int main()
{

	auto ret = _getcwd(NULL, 0);
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//图1 -> 四边形
	float vertices1[] = {
		// positions          // colors               // texture coords
		-0.1f,  0.5f, 0.0f,   /*1.0f, 0.0f, 0.0f,*/   1.0f, 1.0f, // top right
		-0.1f, -0.5f, 0.0f,   /*0.0f, 1.0f, 0.0f,*/   1.0f, 0.0f, // bottom right
		-0.9f, -0.5f, 0.0f,   /*0.0f, 0.0f, 1.0f,*/   0.0f, 0.0f, // bottom left
		-0.9f,  0.5f, 0.0f,   /*1.0f, 1.0f, 0.0f,*/   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//图2 -> 三角形
	float vertices2[] = {
		//位置                   //纹理
		0.1f,  -0.5f, 0.0f,     0.0f,  0.0f,
		0.9f,  -0.5f, 0.0f,     1.0f,  0.0f,
		0.5f,   0.5f, 0.0f,     0.5f,  1.0f
	};

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//纹理1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//纹理2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//加载纹理图片1
	int width, height, nrChannels;

	//注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
	unsigned char* data2 = stbi_load("resources/barce.jpg", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	//绑定第二个纹理，一定注意该函数的位置
	glBindTexture(GL_TEXTURE_2D, texture2);

	//加载纹理图片2
	unsigned char* data1 = stbi_load("resources/wall.jpg", &width, &height, &nrChannels, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data1);
	stbi_image_free(data2);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind Texture
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render container
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#endif //TEST2

#ifdef TEST3
/*
可以将自定义的FBO理解为一个纹理，然后将这个纹理贴到默认的FBO上
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

//自定义FBO
const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

//默认FBO
const char* vs = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTtexture;\n"
"out vec2 aTexCoords;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   aTexCoords = aTtexture;\n"
"}\0";

const char* fs = "#version 330 core\n"
"in vec2 aTexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"   vec3 col = texture(screenTexture, aTexCoords).rgb;\n"
"   FragColor = vec4(col, 1.0f);\n"
"}\n\0";

unsigned int texColorBuffer;

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//自定义FBO着色器1
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//默认FBO着色器2
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vs, NULL);
	glCompileShader(vertexShader2);

	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fs, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	//自定义FBO
	float vertices1[] = {
		//pos                     //color
		 0.0f,   1.0f,   0.0f,     1.0f,   0.0f,   0.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f,   1.0f,   0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f,   0.0f,   1.0f
	};

	//默认FBO
	float vertices2[] = {
		// positions               texCoords
		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		-0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
		 0.8f,   0.8f,   0.0f,     1.0f, 1.0f
	};

	//自定义FBO对应的VAO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//默认FBO对应的VAO
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//FBO帧缓冲对象
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 400, 300, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//将纹理附加到当前绑定的帧缓冲对象（可以理解为将缓冲对象生成一个纹理，将来可以像加载纹理一样使用这个纹理）
	//1.帧缓冲的目标（绘制，读取或者两者皆可
	//2.附加的附件类型（注意参数末尾的0）
	//3.附加的纹理类型
	//4.要附件的纹理本身
	//5.多级渐远纹理的级别
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//渲染缓冲对象 和纹理是同一级的东西，也是帧缓冲附件类型其中之一，比纹理更好（如果需要从缓冲中采样数据建议使用纹理方式）
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	////创建一个深度和模板渲染缓冲对象
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	////附加这个渲染缓冲对象
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); //将渲染缓冲对象附加到帧缓冲的深度和模板附件上
	////渲染缓冲对象使用结束
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

		//自定义FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);


		//默认的FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram2);

		glBindVertexArray(VAO2);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);

	glDeleteFramebuffers(1, &FBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}

#endif //TEST3

#ifdef TEST4
/*
可以将自定义的FBO理解为一个纹理，然后将这个纹理贴到默认的FBO上
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//自定义FBO
const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
//"out int index;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
//"   index = gl_VertexID;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"in vec3 fColor;\n"
//"flat in int index;\n"
"out vec4 FragColor;\n"
//"uniform uint aDrawIndex;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

//默认FBO
const char* vs = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTtexture;\n"
"out vec2 aTexCoords;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   aTexCoords = aTtexture;\n"
"}\0";

const char* fs = "#version 330 core\n"
"in vec2 aTexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"   //vec3 col = texture(screenTexture, aTexCoords).rgb;\n"
"   //FragColor = vec4(col, 1.0f);\n"
"   FragColor = texture(screenTexture, aTexCoords);\n"
"}\n\0";

unsigned int FBO;
int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//自定义FBO着色器1
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader);
	glLinkProgram(shaderProgram1);

	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//默认FBO着色器2
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vs, NULL);
	glCompileShader(vertexShader2);

	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fs, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	//自定义FBO
	float vertices1[] = {
		//pos                     //color
		 0.0f,   1.0f,   0.0f,     1.0f,   0.0f,   0.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f,   1.0f,   0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f,   0.0f,   1.0f
	};

	//自定义FBO
	float vertices3[] = {
		//pos                     //color
		 0.0f,   1.0f,   0.0f,     0.0f,   1.0f,   0.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f,   0.0f,   1.0f,
		 1.0f,  -1.0f,   0.0f,     1.0f,   0.0f,   0.0f
	};

	//默认FBO
	float vertices2[] = {
		// positions               texCoords
		-1.0f,   1.0f,   0.0f,     0.0f, 1.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f, 0.0f,
		 1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,

		-1.0f,   1.0f,   0.0f,     0.0f, 1.0f,
		 1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,
		 1.0f,   1.0f,   0.0f,     1.0f, 1.0f
	};

	//自定义VAO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//VAO test
	unsigned int VBO3, VAO3;
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//默认VAO
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//FBO帧缓冲对象
	//unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 将纹理附加到当前绑定的帧缓冲对象
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//渲染缓冲对象
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); //将渲染缓冲对象附加到帧缓冲的深度和模板附件上



	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

		//自定义FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram1);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);


		//默认的FBO,要保证所有的渲染操作在主窗口中有视觉效果，我们需要再次激活默认帧缓冲，将它绑定到0。
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//先将自定义FBO生成的纹理贴到默认画布的最下面
		glUseProgram(shaderProgram2);
		glBindVertexArray(VAO2);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		//用背景覆盖自定义FBO纹理
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//绘制真正的图像
		glUseProgram(shaderProgram1);
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);
	glDeleteFramebuffers(1, &FBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	unsigned char* pixels = new unsigned char[4];
	glReadPixels(int(x), (int)(-y + 600.0), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	std::cout << "r:" << (int)pixels[0] << std::endl;  //一定要在此处将pixels转为int
	std::cout << "g:" << (int)pixels[1] << std::endl;
	std::cout << "b:" << (int)pixels[2] << std::endl;
	std::cout << "a:" << (int)pixels[3] << std::endl;

	std::cout << std::endl;

	glReadBuffer(GL_NONE);

	delete[] pixels;
	pixels = nullptr;
}

#endif //TEST4

#ifdef TEST5

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 myColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   myColor = aColor;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 myColor;\n"
"void main()\n"
"{\n"
"   if(gl_FragCoord.x < 400)\n"
"		FragColor = vec4(1.0f,0.0f,0.0f,1.0f);\n"
"   else"
"		FragColor = vec4(myColor, 1.0f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};
	//颜色数据
	float colors[] = {
		1.0f,  0.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  0.0f,  1.0f
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);  //先申请内存，不填充数据

	//分批设置数据
	//1.类型
	//2.开始位置
	//3.传入数据的大小
	//4.数据指针
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	//位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//颜色
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(vertices)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST5

#ifdef TEST6

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);
void mouse_enter_callback(GLFWwindow* window, int);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
//"layout (location = 0) in vec2 aPos;\n"
"uniform float xPos;\n"
"uniform float yPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(xPos,yPos, 0.0, 1.0);\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

double cursorPos[2] = { 0 };
unsigned int shaderProgram;

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetCursorEnterCallback(window, mouse_enter_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cursorPos), cursorPos, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(double), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(8);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 1);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);

		glUseProgram(shaderProgram);
		glUniform1f(glGetUniformLocation(shaderProgram, "xPos"), (float)(cursorPos[0] - 400) / 400);
		glUniform1f(glGetUniformLocation(shaderProgram, "yPos"), (float)(-cursorPos[1] + 300) / 300);

		std::cout << "enter:\t" << cursorPos[0] << "," << cursorPos[1] << "\n";
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		std::cout << "move:\t" << x << "," << y << "\n";
	}
}

void mouse_enter_callback(GLFWwindow* window, int pos)
{
	//std::cout << pos << '\n';
}
#endif //TEST6

#ifdef TEST7

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* FS1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"layout (std140) uniform uColor\n"
"{\n"
"	float uRed;\n"
"   float uGreen;\n"
"};\n"
"void main()\n"
"{\n"
"   FragColor = vec4(uRed, uGreen, 1.0f, 1.0f);\n"
"}\n\0";

const char* FS2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"layout (std140) uniform uColor\n"
"{\n"
"	float uRed;\n"
"   float uGreen;\n"
"};\n"
"layout (std140) uniform uBlue\n"
"{\n"
"   float myBlue;\n"
"};\n"
"void main()\n"
"{\n"
"   FragColor = vec4(uRed, uGreen, myBlue, 1.0f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//VS
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
	//FS1
	unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &FS1, NULL);
	glCompileShader(fragmentShader1);

	glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//FS2
	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &FS2, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Program1
	unsigned int shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);

	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//Program2
	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);


	//顶点数据
	float vertices1[] = {
		 0.8f,  -0.5f,  0.0f,
		 0.2f,  -0.5f,  0.0f,
		 0.5f,   0.5f,  0.0f
	};

	float vertices2[] = {
		-0.8f,  -0.5f,  0.0f,
		-0.2f,  -0.5f,  0.0f,
		-0.5f,   0.5f,  0.0f
	};

	//VAO VBO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//UBO uniform缓冲对象


	//glUniformBlockBinding(shaderProgram1, glGetUniformBlockIndex(shaderProgram1, "uColor"), 5);   //绑定点：6 ,数字是由使用者随意设置
	glUniformBlockBinding(shaderProgram2, glGetUniformBlockIndex(shaderProgram2, "uColor"), 6);   //绑定点：6
	glUniformBlockBinding(shaderProgram2, glGetUniformBlockIndex(shaderProgram2, "uBlue"), 4);   //绑定点：6


	unsigned int UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float), NULL, GL_STATIC_DRAW); //分配 2*sizeof(float) 个字节的内存

	unsigned int UBO2;
	glGenBuffers(1, &UBO2);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO2);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float), NULL, GL_STATIC_DRAW); //分配 2*sizeof(float) 个字节的内存

	glBindBufferRange(GL_UNIFORM_BUFFER, 6, UBO, 0, 2 * sizeof(float));  //绑定点：6，第二个参数和glUniformBlockBinding中的最后一个参数对应
	glBindBufferRange(GL_UNIFORM_BUFFER, 4, UBO2, 0, sizeof(float));    //绑定点：6，第二个参数和glUniformBlockBinding中的最后一个参数对应

	float fRed = 1.0f;
	float fGreen = 1.0f;
	float fBlue = 0.0f;

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//1.类型
	//2.从uniform的哪个字节开始
	//3.填充数据的字节大小
	//4.填充的数据指针
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), reinterpret_cast<void*>(&fRed));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float), sizeof(float), reinterpret_cast<void*>(&fGreen));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glDeleteBuffers(1, &UBO); //在绘制循环之后释放，此处不能释放

	glBindBuffer(GL_UNIFORM_BUFFER, UBO2);
	//1.类型
	//2.从uniform的哪个字节开始
	//3.填充数据的字节大小
	//4.填充的数据指针
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), reinterpret_cast<void*>(&fBlue));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glDeleteBuffers(1, &UBO); //在绘制循环之后释放，此处不能释放

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//triangle 1
		glUseProgram(shaderProgram1);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO1);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//triangle 2
		glUseProgram(shaderProgram2);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO2);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);

	glDeleteBuffers(1, &UBO);


	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST7

#ifdef TEST8

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* FS = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

//几何着色器,gl_Position是新生成的顶点，连接顶点时只包含新生成的，从顶点着色器输出的顶点并不会绘制
//const char* GS = "#version 330 core\n"
//"layout(points) in;\n"
//"layout(triangle_strip, max_vertices = 3) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数，超出个数的顶点将不会绘制
//"void main()\n"
//"{\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(-0.2, 0.0, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(0.2, 0.0, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.2, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	EndPrimitive();\n"
//"}\n";

const char* GS = "#version 330 core\n"
"layout(points) in;\n"
"layout(line_strip, max_vertices = 5) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数
"void main()\n"
"{\n"
"	gl_Position = gl_in[0].gl_Position + vec4(-0.3, 0.0, 0.0, 0.0);\n"
"	EmitVertex();\n"
"	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.3, 0.0, 0.0);\n"
"	EmitVertex();\n"

"	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);\n"
"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(-0.3, 0.0, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.0, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.0, 0.0);\n"
//"	EmitVertex();\n"
"	EndPrimitive();\n"
"}\n";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
	int success;
	char infoLog[512];
	//顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VS, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FS, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//几何着色器
	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &GS, NULL);
	glCompileShader(geometryShader);

	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);  //几何着色器
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left  
		 0.5f, -0.5f, 0.0f, // right 
		 0.0f,  0.5f, 0.0f  // top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
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

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(10);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_POINTS, 0, 3);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST8

#ifdef TEST9

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"void main()\n"
"{\n"
"   gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n"
"}\n\0";
#include <direct.h>
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 四边形
	float vertices[] = {
		-0.1f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,
		-0.9f, -0.5f, 0.0f,
		-0.9f,  0.5f, 0.0f,
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST9

#ifdef TEST91

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"void main()\n"
"{\n"
"   gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n"
"}\n\0";
#include <direct.h>
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 四边形
	float vertices[] = {
		-0.1f,  0.5f, 0.0f,
		-0.1f, -0.5f, 0.0f,
		-0.9f, -0.5f, 0.0f,
		-0.9f,  0.5f, 0.0f,
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	//unsigned int VBO, VAO, EBO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), vertices);
	//glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)indices);

		//glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST91

#ifdef TEST10

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		std::cout << width << "," << height << "\n";

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST10

#ifdef TEST11
/*
可以将自定义的FBO理解为一个纹理，然后将这个纹理贴到默认的FBO上
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

//自定义FBO
const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

//默认FBO
const char* vs = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTtexture;\n"
"out vec2 aTexCoords;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   aTexCoords = aTtexture;\n"
"}\0";

const char* fs = "#version 330 core\n"
"in vec2 aTexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"   vec3 col = texture(screenTexture, aTexCoords).rgb;\n"
"   FragColor = vec4(col, 1.0f);\n"
"}\n\0";

unsigned int texColorBuffer;
unsigned int FBO;

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//自定义FBO着色器1
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//默认FBO着色器2
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vs, NULL);
	glCompileShader(vertexShader2);

	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fs, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	//自定义FBO
	float vertices1[] = {
		//pos                     //color
		 0.0f,   1.0f,   0.0f,     1.0f,   0.0f,   0.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f,   1.0f,   0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f,   0.0f,   1.0f
	};

	//默认FBO
	float vertices2[] = {
		// positions               texCoords
		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		-0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
		 0.8f,   0.8f,   0.0f,     1.0f, 1.0f
	};

	//自定义FBO对应的VAO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//默认FBO对应的VAO
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//FBO帧缓冲对象

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//将纹理附加到当前绑定的帧缓冲对象（可以理解为将缓冲对象生成一个纹理，将来可以像加载纹理一样使用这个纹理）
	//1.帧缓冲的目标（绘制，读取或者两者皆可
	//2.附加的附件类型（注意参数末尾的0）
	//3.附加的纹理类型
	//4.要附件的纹理本身
	//5.多级渐远纹理的级别
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//渲染缓冲对象 和纹理是同一级的东西，也是帧缓冲附件类型其中之一，比纹理更好（如果需要从缓冲中采样数据建议使用纹理方式）
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	////创建一个深度和模板渲染缓冲对象
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	////附加这个渲染缓冲对象
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); //将渲染缓冲对象附加到帧缓冲的深度和模板附件上
	////渲染缓冲对象使用结束
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

		//自定义FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);


		//默认的FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram2);

		glBindVertexArray(VAO2);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);

	glDeleteFramebuffers(1, &FBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//将纹理附加到当前绑定的帧缓冲对象（可以理解为将缓冲对象生成一个纹理，将来可以像加载纹理一样使用这个纹理）
	//1.帧缓冲的目标（绘制，读取或者两者皆可
	//2.附加的附件类型（注意参数末尾的0）
	//3.附加的纹理类型
	//4.要附件的纹理本身
	//5.多级渐远纹理的级别
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
}

#endif //TEST11

#ifdef TEST12

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";
#include <direct.h>
int main()
{

	auto ret = _getcwd(NULL, 0);
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//图1 -> 四边形
	float vertices1[] = {
		// positions          // colors               // texture coords
		-0.1f,  0.5f, 0.0f,   /*1.0f, 0.0f, 0.0f,*/    2.0f,   2.0f, // top right
		-0.1f, -0.5f, 0.0f,   /*0.0f, 1.0f, 0.0f,*/    2.0f,  -2.0f, // bottom right
		-0.9f, -0.5f, 0.0f,   /*0.0f, 0.0f, 1.0f,*/   -2.0f,  -2.0f, // bottom left
		-0.9f,  0.5f, 0.0f,   /*1.0f, 1.0f, 0.0f,*/   -2.0f,   2.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//图2 -> 三角形
	float vertices2[] = {
		//位置                   //纹理
		0.1f,  -0.5f, 0.0f,     0.0f,  0.0f,  //左下
		0.9f,  -0.5f, 0.0f,     2.0f,  0.0f,  //右下
		0.5f,   0.5f, 0.0f,     0.5f,  1.0f   //中上
	};

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//纹理1 三角形
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//纹理2 矩形
	unsigned int texture2;
	glGenTextures(1, &texture2);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//采样器开始
	unsigned int samplerID;
	glGenSamplers(1, &samplerID);

	glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glActiveTexture(texture1);
	//glBindSampler(texture1, samplerID);
	//glBindSampler(texture2, samplerID);
	//采样器结束   这部分代码和glTexParameteri那四行效果一样，采样器可以应用到很多纹理上。如果直接设置，则需要每个纹理都设置，造成很大的开销

	//加载纹理图片1
	int width, height, nrChannels;

	//注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
	unsigned char* data2 = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	//绑定第二个纹理，一定注意该函数的位置
	glBindTexture(GL_TEXTURE_2D, texture2);

	//加载纹理图片2
	unsigned char* data1 = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data1);
	stbi_image_free(data2);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind Texture
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render container
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#endif // TEST12

#ifdef TEST13

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 2) in vec2 aTexCoord;\n"
"layout (location = 2) in float aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//"   TexCoord = aTexCoord;\n"
"   TexCoord = vec2(aTexCoord,0.0);\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";
#include <direct.h>
int main()
{

	auto ret = _getcwd(NULL, 0);
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//图1 -> 四边形
	float vertices1[] = {
		// positions          // colors               // texture coords
		-0.1f,  0.5f, 0.0f,   /*1.0f, 0.0f, 0.0f,*/   1.0f, //1.0f, // top right
		-0.1f, -0.5f, 0.0f,   /*0.0f, 1.0f, 0.0f,*/   1.0f, //0.0f, // bottom right
		-0.9f, -0.5f, 0.0f,   /*0.0f, 0.0f, 1.0f,*/   0.0f, //0.0f, // bottom left
		-0.9f,  0.5f, 0.0f,   /*1.0f, 1.0f, 0.0f,*/   0.0f, //1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//图2 -> 三角形
	float vertices2[] = {
		//位置                   //纹理
		0.1f,  -0.5f, 0.0f,  0.0f,  // 0.0f,  0.0f,
		0.9f,  -0.5f, 0.0f,  0.5f,  // 1.0f,  0.0f,
		0.5f,   0.5f, 0.0f,  1.0f  // 0.5f,  1.0f
	};

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//纹理1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//纹理2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//加载纹理图片1
	int width, height, nrChannels;

	//注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
	unsigned char* data2 = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

	//unsigned int data3[1024] = { 0 };
	//for (int i = 0; i < 1024; i++)
	//{
	//	*reinterpret_cast<uint32_t*>(&data3[i]) = (uint32_t)((float)i / 1024 * 255);
	//}

	unsigned char data3[1024 * 4] = { 0 };
	for (int i = 0; i < 1024 * 4; i += 4)
	{
		data3[i + 0] = (float)(i / 1024.0 / 4 * 255);
		data3[i + 1] = (float)(i / 1024.0 / 4 * 255);
		data3[i + 2] = 0;
		data3[i + 3] = 0;
	}
	//unsigned char data3[] = {
	//	255,    0,    0,    255,
	//	  0,  255,    0,    255,
	//	  0,    0,  255,    255
	//};
	if (data3)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_INT, data3);
		//auto ret = glGetError();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data3);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	//绑定第二个纹理，一定注意该函数的位置
	glBindTexture(GL_TEXTURE_2D, texture2);

	//加载纹理图片2
	unsigned char* data1 = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data1);
	stbi_image_free(data2);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind Texture
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render container
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#endif // TEST13

#ifdef TEST14

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 800;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
	//	 0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
	//	 0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
	//};

	float vertices1[] = {
		-0.8f,  0.01f,  0.0f,     0.1f,  0.9f,  0.9f,
		 0.8f,  0.01f,  0.0f,     0.9f,  0.1f,  0.1f,
	};

	float vertices2[]{
		-0.8f,  0.0f,  0.0f,    0.1f,  0.9f,  0.9f,
		-0.6f,  0.0f,  0.0f,    0.2f,  0.8f,  0.8f,
		-0.4f,  0.0f,  0.0f,    0.3f,  0.7f,  0.7f,
		-0.2f,  0.0f,  0.0f,    0.4f,  0.6f,  0.6f,
		 0.0f,  0.0f,  0.0f,    0.5f,  0.5f,  0.5f,
		 0.2f,  0.0f,  0.0f,    0.6f,  0.4f,  0.4f,
		 0.4f,  0.0f,  0.0f,    0.7f,  0.3f,  0.3f,
		 0.6f,  0.0f,  0.0f,    0.8f,  0.2f,  0.2f,
		 0.8f,  0.0f,  0.0f,    0.9f,  0.1f,  0.1f,
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);

	glBindVertexArray(VAO1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		//glEnable(GL_SMOOTH)
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10.0f);
		glPointSize(10.0f);

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(VAO2);
		glDrawArrays(GL_POINTS, 0, 9);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	double x, y;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &x, &y);

		//std::vector< unsigned char > pixels(1 * 1 * 4);

		unsigned char* pixels = new unsigned char[4];
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);  //y坐标需要转换

		std::cout << "r:" << (int)pixels[0] << std::endl;  //一定要在此处将pixels转为int
		std::cout << "g:" << (int)pixels[1] << std::endl;
		std::cout << "b:" << (int)pixels[2] << std::endl;
		std::cout << "a:" << (int)pixels[3] << std::endl;

		std::cout << std::endl;

		delete[] pixels;
		pixels = nullptr;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST14

#ifdef TEST15

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <iostream>
#include <list>

struct Color
{
	float r, g, b, a; // 每个分量的取值范围为[0.0, 1.0]
};

/// 颜色映射表
class ColorMap
{
public:
	bool IsEmpty() const;

	// 根据param，将节点插入mColorNodes中
	void AddNode(float param, Color color) { mColorNodes.emplace_back(ColorNode{ param,color }); }
	void Pop() {}

	Color Lookup(float param) const {}

	//const size_t size() const { return mColorNodes.size(); }

	void Clear() {}
private:
	struct ColorNode
	{
		float param;
		Color color;
	};
public:
	std::list<ColorNode> mColorNodes;
};

unsigned char* GenTexture(ColorMap cm)
{
	///< 生成纹理的结果
	unsigned char* result = new unsigned char[1024 * 4];
	///< 用来保存前一个特征点的序号
	int preIndex = 0;
	///< 用来保存前一个特征点的颜色
	Color preColor = cm.mColorNodes.begin()->color;
	for (auto it = cm.mColorNodes.begin(); it != cm.mColorNodes.end(); it++)
	{
		///< 输出颜色的序号
		const int index = static_cast<const int>(it->param * 1023);
		///< 给定的特征点直接使用，不需要计算
		result[index * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
		result[index * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
		result[index * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
		result[index * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);

		if (it == cm.mColorNodes.begin())
			continue;

		///< 两个特征点之间序号的间隔
		int distance = index - preIndex - 1;
		///< 两个特征点之间颜色的间隔
		float rStep = it->color.r - preColor.r;
		float gStep = it->color.g - preColor.g;
		float bStep = it->color.b - preColor.b;
		float aStep = it->color.a - preColor.a;
		///< 两个特征点之间的所有插值
		for (int i = preIndex + 1; i < index; i++)
		{
			if (false)
			{
				result[i * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
				result[i * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
				result[i * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
				result[i * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);
			}
			else
			{
				result[i * 4 + 0] = static_cast<unsigned char>((preColor.r + (i - preIndex) * rStep / distance) * 255);
				result[i * 4 + 1] = static_cast<unsigned char>((preColor.g + (i - preIndex) * gStep / distance) * 255);
				result[i * 4 + 2] = static_cast<unsigned char>((preColor.b + (i - preIndex) * bStep / distance) * 255);
				result[i * 4 + 3] = static_cast<unsigned char>((preColor.a + (i - preIndex) * aStep / distance) * 255);
			}

		}
		preIndex = index;
		preColor = it->color;
	}
	return result;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 2) in float aTexCoord;\n"
//"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   TexCoord = vec2(aTexCoord,0.0);\n"
//"   TexCoord = aTexCoord;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture,TexCoord);\n"
"}\n\0";
#include <direct.h>
int main()
{
	auto ret = _getcwd(NULL, 0);
	// glfw: initialize and configure
	// ------------------------------
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

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile our shader zprogram
	// ------------------------------------
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//float vertices2[] = {
	//	//位置                    //param        //纹理
	//	0.1f,  -0.5f, 0.0f,       0.0f,  0.0f,
	//	0.9f,  -0.5f, 0.0f,       1.0f,  0.0f,
	//	0.5f,   0.5f, 0.0f,       0.5f,  1.0f,
	//};

	//float vertices2[] = {
	//	//pos                  //param 
	//	-0.5f,  -0.5f, 0.0f,     1.0f,  //左下
	//	 0.5f,  -0.5f, 0.0f,     0.0f,  //右下
	//	 0.0f,   0.5f, 0.0f,     0.5f   //中上
	//};

	//float vertices2[] = {
	//	//pos                  //param 
	//	-0.5f,  -0.5f, 0.0f,     0.0f,  //左下
	//	-0.5f,   0.5f, 0.0f,     0.0f,  //右下
	//	 0.5f,   -0.5f, 0.0f,    1.0f,   //中上

	//	 0.5,    0.5,   0.0,      1.0f,
	//	-0.5f,   0.5f,  0.0f,     0.0f,  //右下
	//	 0.5f,  -0.5f,  0.0f,      1.0f   //中上
	//};

	float vertices2[] = {
		 0.0f,   0.0f,  0.0f,   0.1f,
		 0.5f,   0.0f,  0.0f,   0.8f,
		 0.0f,   0.5f,  0.0f,   0.8f,

		 0.0f,   0.0f,  0.0f,   0.1f,
		-0.5f,   0.0f,  0.0f,   0.8f,
		 0.0f,  -0.5f,  0.0f,   0.8f,

		 0.0f,   0.0f,  0.0f,   0.1f,
		-0.5f,   0.0f,  0.0f,   0.8f,
		 0.0f,   0.5f,  0.0f,   0.8f,

		 0.0f,   0.0f,  0.0f,   0.1f,
		 0.5f,   0.0f,  0.0f,   0.8f,
		 0.0f,  -0.5f,  0.0f,   0.8f,
	};

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//纹理1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glShade(GL_SMOOTH);
	//glShadeModel
	//加载纹理图片1
	int width, height, nrChannels;

	//注意调用glBindTexture之后紧接着就要加载纹理图片，如果连续绑定多个纹理，紧接着再加载纹理图片，就会出错。
	//unsigned char* data2 = stbi_load("container.jpg", &width, &height, &nrChannels, 0);

	ColorMap cm;

	cm.AddNode(0.0, Color{ 1.0, 0.0, 0.0, 1.0 });
	cm.AddNode(0.3, Color{ 0.0, 1.0, 0.0, 1.0 });
	cm.AddNode(0.5, Color{ 0.0, 0.0, 1.0, 1.0 });
	cm.AddNode(0.6, Color{ 0.0, 1.0, 0.0, 1.0 });
	cm.AddNode(0.8, Color{ 1.0, 0.0, 0.0, 1.0 });
	cm.AddNode(1.0, Color{ 1.0, 1.0, 1.0, 1.0 });

	unsigned char* retx = GenTexture(cm);

	unsigned char data2[] = {
		255,    0,    0,    255,
		  0,  255,    0,    255,
		  0,    0,  255,    255,
		255,    0,    0,    255,
	};

	if (retx)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, retx);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, 12);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif // TEST15

#ifdef TEST16
/*
可以将自定义的FBO理解为一个纹理，然后将这个纹理贴到默认的FBO上
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

//自定义FBO
const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

//默认FBO
const char* vs = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTtexture;\n"
"out vec2 aTexCoords;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   aTexCoords = aTtexture;\n"
"}\0";

const char* fs = "#version 330 core\n"
"in vec2 aTexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"   vec3 col = texture(screenTexture, aTexCoords).rgb;\n"
"   FragColor = vec4(col, 1.0f);\n"
"}\n\0";

unsigned int texColorBuffer;

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//自定义FBO着色器1
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//默认FBO着色器2
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vs, NULL);
	glCompileShader(vertexShader2);

	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fs, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	//自定义FBO
	float vertices1[] = {
		//pos                     //color
		-1.0f,    0.0f,   0.0f,    1.0f,  0.0f,  0.0f,
		-0.5f,    0.0f,   0.0f,    1.0f,  1.0f,  0.0f,
		 0.0f,    0.0f,   0.0f,    0.0f,  1.0f,  0.0f,
		 0.5f,    0.0f,   0.0f,    0.0f,  1.0f,  1.0f,
		 1.0f,    0.0f,   0.0f,    0.0f,  0.0f,  1.0f,

	};

	//默认FBO
	float vertices2[] = {
		// positions               texCoords
		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		-0.8f,  -0.8f,   0.0f,     0.0f, 0.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,

		-0.8f,   0.8f,   0.0f,     0.0f, 1.0f,
		 0.8f,  -0.8f,   0.0f,     1.0f, 0.0f,
		 0.8f,   0.8f,   0.0f,     1.0f, 1.0f
	};

	//自定义FBO对应的VAO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//默认FBO对应的VAO
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//FBO帧缓冲对象
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//自定义FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glViewport(0, 0, 1024, 1);
		glUseProgram(shaderProgram);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
		glBindVertexArray(0);


		//默认的FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram2);

		glBindVertexArray(VAO2);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);

	glDeleteFramebuffers(1, &FBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;
}

#endif //TEST16

#ifdef TEST17

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"flat out vec3 fColor;\n"
//"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"flat in vec3 fColor;\n"
//"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.9f, -0.5f, 0.0f,			1.0f,  0.0f,  0.0f,
		-0.9f,  0.5f, 0.0f,			1.0f,  0.0f,  0.0f,

		-0.8f, -0.5f, 0.0f,			1.0f,  0.0f,  0.0f,
		-0.8f,  0.5f, 0.0f,			1.0f,  0.0f,  0.0f,

		-0.3f, -0.5f, 0.0f,			1.0f,  1.0f,  0.0f,
		-0.3f,  0.5f, 0.0f,			1.0f,  1.0f,  0.0f,

		 0.0f, -0.5f, 0.0f,			0.0f,  1.0f,  0.0f,
		 0.0f,  0.5f, 0.0f,			0.0f,  1.0f,  0.0f,

		 0.3f, -0.5f, 0.0f,			0.0f,  1.0f,  1.0f,
		 0.3f,  0.5f, 0.0f,			0.0f,  1.0f,  1.0f,

		 0.8f, -0.5f, 0.0f,			0.0f,  0.0f,  1.0f,
		 0.8f,  0.5f, 0.0f,			0.0f,  0.0f,  1.0f,
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式


	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 12);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST17

#ifdef TEST18

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   //vec4 color = vec4(fColor,0.0);\n"
"   //if(color.a<1.0) discard;\n"
"   FragColor = vec4(fColor, 0.5f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO, vao;
	glGenVertexArrays(1, &VAO);
	//glGenVertexArrays(5, &vao);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式


	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST18

#ifdef TEST19

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* FS1 = "#version 330 core\n"
"out vec4 FragColor;\n"
"layout (std140) uniform uColor\n"
"{\n"
"	float uRed;\n"
"   float uGreen;\n"
"};\n"
"void main()\n"
"{\n"
"   FragColor = vec4(uRed, uGreen, 1.0f, 1.0f);\n"
"}\n\0";

const char* FS2 = "#version 330 core\n"
"out vec4 FragColor;\n"
"layout (std140) uniform uColor\n"
"{\n"
"	float uRed;\n"
"   float uGreen;\n"
"};\n"
"layout (std140) uniform uBlue\n"
"{\n"
"   float myBlue;\n"
"};\n"
"void main()\n"
"{\n"
"   FragColor = vec4(uRed, uGreen, myBlue, 1.0f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//VS
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
	//FS1
	unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &FS1, NULL);
	glCompileShader(fragmentShader1);

	glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//FS2
	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &FS2, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//Program1
	unsigned int shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);

	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	//Program2
	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader1);
	glDeleteShader(fragmentShader2);


	//顶点数据
	float vertices1[] = {
		 0.8f,  -0.5f,  0.0f,
		 0.2f,  -0.5f,  0.0f,
		 0.5f,   0.5f,  0.0f
	};

	float vertices2[] = {
		-0.8f,  -0.5f,  0.0f,
		-0.2f,  -0.5f,  0.0f,
		-0.5f,   0.5f,  0.0f
	};

	//VAO VBO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//UBO uniform缓冲对象
	//shader1右边三角形，shader2左边三角形

	glUniformBlockBinding(shaderProgram1, glGetUniformBlockIndex(shaderProgram1, "uColor"), 6);   //绑定点：6 ,数字是由使用者随意设置
	glUniformBlockBinding(shaderProgram2, glGetUniformBlockIndex(shaderProgram2, "uColor"), 6);   //绑定点：6
	glUniformBlockBinding(shaderProgram2, glGetUniformBlockIndex(shaderProgram2, "uBlue"), 5);    //绑定点：6


	unsigned int UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float), NULL, GL_STATIC_DRAW); //分配 2*sizeof(float) 个字节的内存

	unsigned int UBO2;
	glGenBuffers(1, &UBO2);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO2);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(float), NULL, GL_STATIC_DRAW); //分配 2*sizeof(float) 个字节的内存

	glBindBufferRange(GL_UNIFORM_BUFFER, 6, UBO, 0, 2 * sizeof(float));  //绑定点：6，第二个参数和glUniformBlockBinding中的最后一个参数对应
	glBindBufferRange(GL_UNIFORM_BUFFER, 5, UBO2, 0, sizeof(float));     //绑定点：6，第二个参数和glUniformBlockBinding中的最后一个参数对应

	float fRed = 1.0f;
	float fGreen = 1.0f;
	float fBlue = 0.0f;




	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), reinterpret_cast<void*>(&fRed));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float), sizeof(float), reinterpret_cast<void*>(&fGreen));

	glBindBuffer(GL_UNIFORM_BUFFER, UBO2);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), reinterpret_cast<void*>(&fBlue));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glDeleteBuffers(1, &UBO); //在绘制循环之后释放，此处不能释放

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//triangle 1
		glUseProgram(shaderProgram1);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO1);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//triangle 2
		glUseProgram(shaderProgram2);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO2);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);

	glDeleteBuffers(1, &UBO);


	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST19

#ifdef TEST20

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <thread>
#include <vector>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float red;\n"
"uniform float green;\n"
"uniform float blue;\n"
"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = vec3(red,green,blue);\n"
"}\0";
const char* FS = "#version 330 core\n"
"in vec3 fColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
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
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, //1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f, //0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f, //0.0f,  0.0f,  1.0f,// top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //100ms

		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）

		//std::cout << myColor[index][0] << " " << myColor[index][1] << " " << myColor[index][2] << "\n";
		glUniform1f(glGetUniformLocation(shaderProgram, "red"), distribution(generator));
		glUniform1f(glGetUniformLocation(shaderProgram, "green"), distribution(generator));
		glUniform1f(glGetUniformLocation(shaderProgram, "blue"), distribution(generator));

		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif // TEST20

#ifdef TEST21

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out VS_OUT{vec3 color;} vs_out;\n"
"void main()\n"
"{\n"
"   vs_out.color = aColor;\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* FS = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 fColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
//"   FragColor = vec4(1.0f,0.0f,0.0f,1.0f);\n"
"}\n\0";

// 几何着色器,gl_Position是新生成的顶点，连接顶点时只包含新生成的，从顶点着色器输出的顶点并不会绘制
// gl_in[index]:index是第几个顶点，例如传入的是线段，那么就有两个顶点，如果是点就只有一个顶点
const char* GS = "#version 330 core\n"
"layout(lines) in;\n"
"layout(line_strip, max_vertices = 5) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数

"in VS_OUT{\n"
"	vec3 color;\n"
"} gs_in[];\n"  //注意此处是gs_in，是自定义数据，不是gl_in[]

"out vec3 fColor;\n"
"void main()\n"
"{\n"
"   fColor = gs_in[0].color;\n"
"	gl_Position = gl_in[0].gl_Position;\n"
"	EmitVertex();\n"
"	gl_Position = gl_in[1].gl_Position;\n"
"	EmitVertex();\n"

"   fColor = vec3(0.0f,0.0f,1.0f);\n"  //箭头顶端设置为蓝色，上一个EmitVertex之后的所有点都将使用这个颜色
"	gl_Position = gl_in[1].gl_Position + vec4(-0.3, 0.3, 0.0, 0.0);\n"
"	EmitVertex();\n"
"	gl_Position = gl_in[1].gl_Position;\n"
"	EmitVertex();\n"
"	gl_Position = gl_in[1].gl_Position + vec4(-0.3, -0.3, 0.0, 0.0);\n"
"	EmitVertex();\n"

"	EndPrimitive();\n"
"}\n";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
	int success;
	char infoLog[512];
	//顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VS, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FS, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//几何着色器
	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &GS, NULL);
	glCompileShader(geometryShader);

	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);  //几何着色器
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(10);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_LINES, 0, 4);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST21

#ifdef TEST22

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <list>
struct Color
{
	float r, g, b, a; // 每个分量的取值范围为[0.0, 1.0]
};

/// 颜色映射表
class ColorMap
{
public:
	bool IsEmpty() const;

	// 根据param，将节点插入mColorNodes中
	void AddNode(float param, Color color) { mColorNodes.emplace_back(ColorNode{ param,color }); }
	void Pop() {}

	Color Lookup(float param) const {}

	//const size_t size() const { return mColorNodes.size(); }

	void Clear() {}
private:
	struct ColorNode
	{
		float param;
		Color color;
	};
public:
	std::list<ColorNode> mColorNodes;
};

unsigned char* GenTexture(ColorMap myCM)
{
	//归一化
	ColorMap cm = myCM;
	auto start = cm.mColorNodes.front().param;
	auto end = cm.mColorNodes.back().param;

	for (auto& elem : cm.mColorNodes)
	{
		elem.param = elem.param / (end - start);
	}


	///< 生成纹理的结果
	unsigned char* result = new unsigned char[1024 * 4];
	///< 用来保存前一个特征点的序号
	int preIndex = 0;
	///< 用来保存前一个特征点的颜色
	Color preColor = cm.mColorNodes.begin()->color;
	for (auto it = cm.mColorNodes.begin(); it != cm.mColorNodes.end(); it++)
	{
		///< 输出颜色的序号
		const int index = static_cast<const int>(it->param * 1023);
		///< 给定的特征点直接使用，不需要计算
		result[index * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
		result[index * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
		result[index * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
		result[index * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);

		if (it == cm.mColorNodes.begin())
			continue;

		///< 两个特征点之间序号的间隔
		int distance = index - preIndex - 1;
		///< 两个特征点之间颜色的间隔
		float rStep = it->color.r - preColor.r;
		float gStep = it->color.g - preColor.g;
		float bStep = it->color.b - preColor.b;
		float aStep = it->color.a - preColor.a;
		///< 两个特征点之间的所有插值
		for (int i = preIndex + 1; i < index; i++)
		{
			if (false)
			{
				result[i * 4 + 0] = static_cast<unsigned char>((it->color.r) * 255);
				result[i * 4 + 1] = static_cast<unsigned char>((it->color.g) * 255);
				result[i * 4 + 2] = static_cast<unsigned char>((it->color.b) * 255);
				result[i * 4 + 3] = static_cast<unsigned char>((it->color.a) * 255);
			}
			else
			{
				result[i * 4 + 0] = static_cast<unsigned char>((preColor.r + (i - preIndex) * rStep / distance) * 255);
				result[i * 4 + 1] = static_cast<unsigned char>((preColor.g + (i - preIndex) * gStep / distance) * 255);
				result[i * 4 + 2] = static_cast<unsigned char>((preColor.b + (i - preIndex) * bStep / distance) * 255);
				result[i * 4 + 3] = static_cast<unsigned char>((preColor.a + (i - preIndex) * aStep / distance) * 255);
			}

		}
		preIndex = index;
		preColor = it->color;
	}
	return result;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* VS = "#version 330 core\n"
"layout (location = 0) in vec4 aPosParam;\n"
"out VS_OUT{vec2 TexCoord;} vs_out;\n"
"void main()\n"
"{\n"
"   vs_out.TexCoord = vec2(aPosParam.w,0.0f);\n"
"   gl_Position = vec4(aPosParam.xyz, 1.0f);\n"
"}\0";
const char* FS = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 fTexCoord;\n"
"uniform sampler2D myTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(myTexture,fTexCoord);\n"
"}\n\0";

// 几何着色器,gl_Position是新生成的顶点，连接顶点时只包含新生成的，从顶点着色器输出的顶点并不会绘制
// gl_in[index]:index是第几个顶点，例如传入的是线段，那么就有两个顶点，如果是点就只有一个顶点
const char* GS = "#version 330 core\n"
"layout(lines) in;\n"
"layout(line_strip, max_vertices = 7) out;\n"  //第一个参数是图元类型，第二个参数是新的图元的顶点个数

"in VS_OUT{\n"
"	vec2 TexCoord;\n"
"} gs_in[];\n"  //注意此处是gs_in，是自定义数据，不是gl_in[]

"out vec2 fTexCoord;\n"
"void main()\n"
"{\n"
"   float xNorm = gl_in[1].gl_Position.x - gl_in[0].gl_Position.x;\n"
"   float yNorm = gl_in[1].gl_Position.y - gl_in[0].gl_Position.y;\n"
"   float zNorm = gl_in[1].gl_Position.z - gl_in[0].gl_Position.z;\n"

"   fTexCoord = gs_in[0].TexCoord;\n"  //纹理坐标

"	gl_Position = gl_in[0].gl_Position;\n"
"	EmitVertex();\n"
"	gl_Position = gl_in[1].gl_Position;\n"
"	EmitVertex();\n"
//"	gl_Position = gl_in[1].gl_Position + vec4(gl_in[1].gl_Position.x/xNorm*0.8, 0.1, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[1].gl_Position;\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[1].gl_Position + vec4(gl_in[1].gl_Position.x/xNorm*0.8, -0.1, 0.0, 0.0);\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[1].gl_Position;\n"
//"	EmitVertex();\n"
//"	gl_Position = gl_in[1].gl_Position + vec4(0.0, 0.0, 0.1, 0.0);\n"
//"	EmitVertex();\n"

"	EndPrimitive();\n"
"}\n";

int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//着色器 -> 创建 加载 编译 链接 绑定
	int success;
	char infoLog[512];
	//顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VS, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FS, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//几何着色器
	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &GS, NULL);
	glCompileShader(geometryShader);

	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);  //几何着色器
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	//顶点数据
	//float vertices[] = {
	//	-0.5f, -0.5f, 0.0f,  0.0f,
	//	 0.5f, -0.5f, 0.0f,  0.0f,

	//	-0.5f,  0.5f, 0.0f,  0.3f,
	//	 0.5f,  0.5f, 0.0f,  0.3f,
	//};

	struct vertex
	{
		float x;
		float y;
		float z;

		vertex(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) :
			x(_x),
			y(_y),
			z(_z)
		{}

		vertex operator/(int n)
		{
			if (n <= 0)
			{
				return *this;
			}

			return vertex(this->x / n, this->y / n, this->z / n);
		}
	};

	int verCount = 6;
	float vertices[] =
	{
		-0.8f,   0.5f,   0.0f,
		-0.8f,  -0.5f,   0.0f,

		 0.0f,  -0.5f,   0.0f,
		 0.0f,   0.5f,   0.0f,

		 0.8f,   0.5f,   0.0f,
		 0.8f,  -0.5f,   0.0f,
	};

	int indices[] =
	{
		2,
		4,0,1,2,3,
		4,2,3,4,5,
	};

	float fieldDatax[] =
	{
		-0.1f,0.1f,
	};

	float fieldDatay[] =
	{
		0.2f,0.2f,
	};

	float fieldDataz[] =
	{
		-0.1f,-0.1f,
	};

	size_t start = 0;
	size_t end = 1;

	std::vector<vertex> coreVertex;  //所有格心顶点

	for (size_t i = 0; i < indices[0]; i++)  //  拓扑的个数
	{
		start = end;
		end += indices[end] + 1;

		std::vector<int> vecIndex;  //拓扑数据中的单个拓扑
		for (size_t j = start; j < end; j++)
		{
			vecIndex.emplace_back(indices[j]);
		}

		vertex core;   //格心

		for (size_t k = 1; k < vecIndex.size(); k++)
		{
			size_t index = vecIndex[k] * 3;

			core.x += vertices[index + 0];
			core.y += vertices[index + 1];
			core.z += vertices[index + 2];
		}

		coreVertex.emplace_back(core / vecIndex[0]);
	}

	std::vector<float> lines;
	float maxParam = 1.0f;
	float minParam = 0.0f;

	//构造主轴顶点数据并对xyz归一化，获取w分量的最大最小值
	for (size_t i = 0; i < indices[0]; i++)
	{
		auto vecX = fieldDatax[i] - coreVertex[i].x;
		auto vecY = fieldDatay[i] - coreVertex[i].y;
		auto vecZ = fieldDataz[i] - coreVertex[i].z;
		//模长，用来求颜色参数
		auto length = std::hypot(vecX, vecY, vecZ);

		maxParam = std::max(maxParam, length);
		minParam = std::min(minParam, length);
		//格心
		lines.emplace_back(coreVertex[i].x);
		lines.emplace_back(coreVertex[i].y);
		lines.emplace_back(coreVertex[i].z);

		lines.emplace_back(length); //w

		//方向
		lines.emplace_back(fieldDatax[i] + vecX / length); //x
		lines.emplace_back(fieldDatay[i] + vecY / length); //y
		lines.emplace_back(fieldDataz[i] + vecZ / length); //z

		lines.emplace_back(length); //w
	}

	auto size = lines.size();

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, lines.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//纹理
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	ColorMap cm;

	cm.AddNode(0.0, Color{ 0.0, 0.0, 1.0, 1.0 });
	cm.AddNode(0.3, Color{ 1.0, 1.0, 0.0, 1.0 });
	cm.AddNode(0.5, Color{ 0.0, 1.0, 0.0, 1.0 });
	cm.AddNode(0.8, Color{ 0.0, 1.0, 1.0, 1.0 });
	cm.AddNode(1.0, Color{ 1.0, 1.0, 0.0, 1.0 });

	unsigned char* retx = GenTexture(cm);

	if (retx)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, retx);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	//循环绘制
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(10);

		glBindTexture(GL_TEXTURE_2D, texture1);
		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_LINES, 0, 4);

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif //TEST22

#ifdef TEST23
/*
可以将自定义的FBO理解为一个纹理，然后将这个纹理贴到默认的FBO上
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double x, double y);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//自定义FBO
const char* vs_FBO = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fColor;\n"
//"out int index;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   fColor = aColor;\n"
//"   index = gl_VertexID;\n"
"}\0";
const char* fs_FBO = "#version 330 core\n"
"in vec3 fColor;\n"
//"flat in int index;\n"
"out vec4 FragColor;\n"
//"uniform uint aDrawIndex;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(fColor, 1.0f);\n"
"}\n\0";

//默认FBO
const char* vs = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTtexture;\n"
"out vec2 aTexCoords;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   aTexCoords = aTtexture;\n"
"}\0";

const char* fs = "#version 330 core\n"
"in vec2 aTexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D screenTexture;\n"
"void main()\n"
"{\n"
"   //vec3 col = texture(screenTexture, aTexCoords).rgb;\n"
"   //FragColor = vec4(col, 1.0f);\n"
"   FragColor = texture(screenTexture, aTexCoords);\n"
"}\n\0";

unsigned int FBO;
int main()
{
	//初始化
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

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//自定义FBO着色器1
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vs_FBO, NULL);
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
	glShaderSource(fragmentShader, 1, &fs_FBO, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader);
	glAttachShader(shaderProgram1, fragmentShader);
	glLinkProgram(shaderProgram1);

	glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//默认FBO着色器2
	unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader2, 1, &vs, NULL);
	glCompileShader(vertexShader2);

	glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fs, NULL);
	glCompileShader(fragmentShader2);

	glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader2);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

	glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader2);
	glDeleteShader(fragmentShader2);

	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	//自定义FBO
	float vertices1[] = {
		//pos                     //color
		-1.0f,   1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		-1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f, 1.0f,0.0f,

		-1.0f,   1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f, 1.0f,0.0f,
		 1.0f,   1.0f,   0.0f,     0.0f, 1.0f,0.0f,
	};

	//自定义FBO
	float vertices3[] = {
		//pos                     //color
		-1.0f,   1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		-1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f, 1.0f,0.0f,

		-1.0f,   1.0f,   0.0f,     1.0f, 0.0f,0.0f,
		 1.0f,  -1.0f,   0.0f,     0.0f, 1.0f,0.0f,
		 1.0f,   1.0f,   0.0f,     0.0f, 1.0f,0.0f,
	};

	//默认FBO
	float vertices2[] = {
		// positions               texCoords
		-1.0f,   1.0f,   0.0f,     0.0f, 1.0f,
		-1.0f,  -1.0f,   0.0f,     0.0f, 0.0f,
		 1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,

		-1.0f,   1.0f,   0.0f,     0.0f, 1.0f,
		 1.0f,  -1.0f,   0.0f,     1.0f, 0.0f,
		 1.0f,   1.0f,   0.0f,     1.0f, 1.0f
	};

	//自定义VAO
	unsigned int VBO1, VAO1;
	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//VAO test
	unsigned int VBO3, VAO3;
	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices3), vertices3, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//默认VAO
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//FBO帧缓冲对象
	//unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	//纹理
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 将纹理附加到当前绑定的帧缓冲对象
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	//渲染缓冲对象
	//unsigned int rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); //将渲染缓冲对象附加到帧缓冲的深度和模板附件上



	//检查帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  //线框模式

		//自定义FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.5f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram1);

		glBindVertexArray(VAO1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		//默认的FBO,要保证所有的渲染操作在主窗口中有视觉效果，我们需要再次激活默认帧缓冲，将它绑定到0。
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//先将自定义FBO生成的纹理贴到默认画布的最下面
		glUseProgram(shaderProgram2);
		glBindVertexArray(VAO2);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		//用背景覆盖自定义FBO纹理
		glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//绘制真正的图像
		glUseProgram(shaderProgram1);
		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteProgram(shaderProgram1);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteProgram(shaderProgram2);
	glDeleteFramebuffers(1, &FBO);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	for (int i = 0; i < SCR_WIDTH; i++)
	{
		unsigned char* pixels = new unsigned char[4]();
		glReadPixels(i, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		std::cout << "r:" << (int)pixels[0] << "\t";  //一定要在此处将pixels转为int
		std::cout << "g:" << (int)pixels[1] << "\t";
		std::cout << "b:" << (int)pixels[2] << "\t";
		std::cout << "a:" << (int)pixels[3] << "\t";

		std::cout << std::endl;

		delete[] pixels;
		pixels = nullptr;
	}

	glReadBuffer(GL_NONE);
}

#endif //TEST23