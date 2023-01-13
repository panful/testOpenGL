#include "FboRender.h"
#include <QOpenGLFramebufferObject>

#include "ViewerItem.h"

#include <iostream>
#include <exception>

//FboRender* FboRender::m_instance = new FboRender();

FboRender::FboRender(/*void* func*/)
	:/*mGraphicFunc(func),*/
	VAO(0),
	VBO(0),
	mProgram(0),
	m_red(0.0f),
	m_green(0.0f)
{
	//reinterpret_cast<QOpenGLFunctions_3_3_Core*>(mGraphicFunc)->
	//mGraphicFunc = new QOpenGLFunctions_3_3_Core();
	/*mGraphicFunc->*/initializeOpenGLFunctions();

	//QOpenGLContext context;
	//context.makeCurrent();
	//context.swapBuffers();
	//wglMakeCurrent();

	try
	{
		init();
	}
	catch (std::exception e)
	{
		std::cout << e.what();
	}
}

void FboRender::render()
{
	doRender();

	update();
}

void FboRender::synchronize(QQuickFramebufferObject* fbo)
{
	//  响应该函数有以下方法
	//  1.在qml中设置item的大小随窗口大小改变，即不能固定viewitem的大小，缩放窗口就会调用该函数
	//  2.在viewitem中调用update函数

	auto color = fbo->property("color").value<QColor>();
	auto colorRed = color.red();

	// 获取ViewItem中的数据
	auto test1 = fbo->property("myTest").value<TestData>();

	auto test2 = fbo->property("myTest1").value<int>();


	auto width = fbo->width();
	auto height = fbo->height();
}

QOpenGLFramebufferObject* FboRender::createFramebufferObject(const QSize& size)
{
	QOpenGLFramebufferObjectFormat format;
	format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	format.setSamples(4);
	return new QOpenGLFramebufferObject(size, format);
}

void FboRender::init()
{
	const char* VS = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"
		"uniform float uRed;\n"
		"uniform float uGreen;\n"
		"out vec3 fColor;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		//"   fColor = aColor;\n"
		"   fColor = vec3(uRed,uGreen,0.0f);\n"
		"}\0";
	const char* FS = "#version 330 core\n"
		"in vec3 fColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(fColor, 0.5f);\n"
		"}\n\0";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &VS, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		throw std::logic_error("vertex shader error!\n");
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FS, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		throw std::logic_error("fragment shader error!\n");
	}

	mProgram = glCreateProgram();
	glAttachShader(mProgram, vertexShader);
	glAttachShader(mProgram, fragmentShader);
	glLinkProgram(mProgram);

	glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(mProgram, 512, NULL, infoLog);
		throw std::logic_error("program error!\n");
	}
	glDetachShader(mProgram, vertexShader);
	glDetachShader(mProgram, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,		1.0f,  0.0f,  0.0f,// left  
		 0.5f, -0.5f, 0.0f,		0.0f,  1.0f,  0.0f,// right 
		 0.0f,  0.5f, 0.0f,		0.0f,  0.0f,  1.0f,// top   
	};

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
}

void FboRender::doRender()
{
	//渲染窗口的大小，是在qml里设置的
	auto width = framebufferObject()->width();
	auto height = framebufferObject()->height();


	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //必须清除深度缓冲，否则窗口变换后，绘制不出来图形。

	glUseProgram(mProgram);

	glUniform1f(glGetUniformLocation(mProgram, "uRed"), m_red);
	glUniform1f(glGetUniformLocation(mProgram, "uGreen"), m_green);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0); //没有这句，窗口变化后图形绘制不出来。
}
