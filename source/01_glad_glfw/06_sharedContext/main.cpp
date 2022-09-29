

// 20 21 23 共享上下文，多线程
#ifdef TEST20

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

GLFWwindow* myContext = nullptr;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void render(uint32_t w, uint32_t h, std::string name, float color, GLFWwindow* context = nullptr)
{
	const char* VS = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"uniform float theColor;\n"
		"out vec3 fColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"   fColor = aColor;\n"
		"   fColor = vec3(theColor,0.0f,0.0f);\n"
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

	myContext = glfwCreateWindow(w, h, name.c_str(), NULL, context);

	if (myContext == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(myContext);
	glfwSetFramebufferSizeCallback(myContext, framebuffer_size_callback);
	//gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
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
	while (!glfwWindowShouldClose(myContext))
	{
		processInput(myContext);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）

		glUniform1f(glGetUniformLocation(shaderProgram, "theColor"), color);

		glBindVertexArray(VAO);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(myContext);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
}

int main()
{
	//初始化glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//render(800, 600, "test", nullptr);


	std::thread t1(render, 800, 600, "111", 0.5f, nullptr);
	std::thread t2(render, 800, 600, "222", 1.0f, myContext);
	t1.join();
	t2.join();

	glfwTerminate();

	return 0;
}

#endif // TEST20

#ifdef TEST21

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>

class OpenGLContext
{
public:
	OpenGLContext();
	~OpenGLContext() = default;
	GLFWwindow* getContext() const;
	const uint32_t getVBO() const { return m_vbo; }

public:
	const uint32_t m_windoWidth;
	const uint32_t m_windowHeight;
	const std::string m_windowName;

	uint32_t m_vbo;
	uint32_t m_ebo;
	uint32_t m_shaderProgram;

	GLFWwindow* m_window;
};

OpenGLContext::OpenGLContext() :
	m_windoWidth(800),
	m_windowHeight(600),
	m_windowName("Test Window"),
	m_vbo(0),
	m_ebo(0),
	m_shaderProgram(0)
{
	m_window = glfwCreateWindow(m_windoWidth, m_windowHeight, m_windowName.c_str(), NULL, NULL);

	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw "error";
	}

	const char* VS = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"out vec3 fColor;\n"
		"uniform float color;"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"   fColor = aColor;\n"
		"   fColor = vec3(color,0.0,0.0);"
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



	//设置当前上下文，回调函数（键盘，鼠标等）
	glfwMakeContextCurrent(m_window);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//gladLoadGL();
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw "error";
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

	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);

	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDetachShader(m_shaderProgram, vertexShader);
	glDetachShader(m_shaderProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//顶点数据
	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f,  0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f,  0.0f,  0.0f,  1.0f,// top   
	};

	//缓冲对象：创建 绑定类型 链接数据 配置数据
	//unsigned int VBO, VAO;
	//glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &m_vbo);

	//glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

GLFWwindow* OpenGLContext::getContext() const
{
	return glfwGetCurrentContext();
}


class Draw
{
public:
	Draw(OpenGLContext* context);
	void genVAO();
	void render();
private:
	uint32_t m_vao;
	uint32_t m_shaderProgram;
	uint32_t m_texture;

	OpenGLContext* m_context;

	GLFWwindow* m_window;
};

Draw::Draw(OpenGLContext* context) :
	m_vao(0),
	m_shaderProgram(context->m_shaderProgram),
	m_texture(0),
	m_context(context),
	m_window(context->getContext())
{
	genVAO();

	//glfwMakeContextCurrent(nullptr);
}

void Draw::render()
{
	//glfwMakeContextCurrent(m_window);
	//循环绘制
	while (!glfwWindowShouldClose(m_window))
	{
		//processInput(m_window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		auto time = glfwGetTime();

		std::cout << time << std::endl;

		glUniform1f(glGetUniformLocation(m_shaderProgram, "color"), (float)time / 20.0f);

		glUseProgram(m_shaderProgram);  //修改着色器之前必须执行该函数，此处没有修改因此放在循环外也可以（修改一次uniform之前必须调用该函数）
		glBindVertexArray(m_vao);       //调用绘制函数前，必须绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &m_vao);
}

void Draw::genVAO()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_context->m_vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//主线程创建上下文并生成shader、program、vbo、ebo、texture（没有vao和fbo）
//子线程使用createWindow，getCurrentContext，makeCurrentContext，生成vao，调用draw

int main()
{
	//在主线程初始化glfw，一个程序只能初始化一次glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	OpenGLContext* context1 = new OpenGLContext();
	OpenGLContext* context2 = new OpenGLContext();

	Draw d1(context1);
	Draw d2(context2);

	d1.render();
	d2.render();

	std::thread t1(&Draw::render, d1);
	std::thread t2(&Draw::render, d1);

	t1.join();
	t2.join();


	//在所有线程调用完glfw后，再结束glfw
	glfwTerminate();
}

#endif // TEST21

#ifdef TEST23

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <array>

//主线程创建上下文并生成shader、program、vbo、ebo、texture（没有vao和fbo）
//子线程使用createWindow，getCurrentContext，makeCurrentContext，生成vao，调用draw

void initGLFW()
{
	//在主线程初始化glfw，一个程序只能初始化一次glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

class Window
{
private:
	GLFWwindow* m_window;
	uint32_t m_VAO;

	uint32_t m_program;  //如果共享了上下文，则新的Window不再生成program
	uint32_t m_VBO;

public:
	Window() = delete;
	Window(std::string title, uint32_t w, uint32_t h, Window* win = nullptr);
	~Window() = default;
public:
	uint32_t shaderProgram(const std::string& vs, const std::string& fs);
	uint32_t bufferObject(const uint32_t verCount, const std::vector<float> vertices);

	void render();
private:
	void createVAO();
};

uint32_t Window::shaderProgram(const std::string& vs, const std::string& fs)
{
	auto VS = vs.c_str();
	auto FS = fs.c_str();

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

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl; \
	}

	glDetachShader(m_program, vertexShader);
	glDetachShader(m_program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return m_program;
}

uint32_t Window::bufferObject(const uint32_t verCount, const std::vector<float> vertices)
{
	//uint32_t vbo = 0;
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//if (!indices.empty())
	//{
	//	//生成ebo
	//	uint32_t ebo;
	//	glGenBuffers(1, &m_ebo);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//}
	return m_VBO;
}


Window::Window(std::string title, uint32_t w, uint32_t h, Window* win) :
	m_window(nullptr),
	m_VAO(0)
{
	GLFWwindow* shareContext = win ? win->m_window : nullptr;
	m_window = glfwCreateWindow(w, h, title.c_str(), 0, shareContext);

	// 设置当前上下文
	glfwMakeContextCurrent(m_window);

	// 加载glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::logic_error("Failed to initialize GLAD");
	}

	// 如果没有共享的上下文，则在此处创建shader vbo texture等资源
	if (!win)
	{
		std::string VS = "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = vec4(aPos.xyz, 1.0);\n"
			"}\0";
		std::string FS = "#version 330 core\n"
			"out vec4 FragColor;\n"
			"void main()\n"
			"{\n"
			"FragColor = vec4(0.0f,1.0f,0.0f,1.0f);\n"
			"}\n\0";
		/*m_program = */shaderProgram(VS, FS);

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		std::vector<float> vertices = {
			0.0f,0.5f,0.0f,
			-0.5f,-0.5f,0.0f,
			0.5f,-0.5f,0.0f };
		/*m_VBO = */bufferObject(3, vertices);
	}
	else
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, win->m_VBO);
		m_program = win->m_program;
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Window::render()
{
	//glfwMakeContextCurrent(m_window);
	while (true)
	{
		glViewport(0, 0, 800, 600);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(m_VAO);
		glUseProgram(m_program);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void Window::createVAO()
{

}


int main()
{
	// 初始化glfw
	initGLFW();

	Window* win1 = new Window("111", 800, 600, nullptr);
	//win1->render();
	Window* win2 = new Window("222", 800, 600, win1);
	//win2->render();

	std::thread t1(&Window::render, win1);
	std::thread t2(&Window::render, win2);

	t1.join();
	t2.join();


	return 0;
}

#endif // TEST23