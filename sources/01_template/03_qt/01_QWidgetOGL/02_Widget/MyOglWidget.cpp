#include "MyOglWidget.h"

//GLSL
//#version表示使用GLSL的版本
//out表示是输出参数
//in表示是输入参数

//读取shader代码；建立、编译shader；向shader中传递数据

static const char* VS =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 fragmentColor;"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  //顶点着色器好像必须有一个gl_Position，这应该是顶点的坐标
"   fragmentColor = aColor;\n"
"}\0";

static const char* FS = "#version 330 core\n"
"in vec3 fragmentColor;\n"   //这里的输入由顶点着色器VS传入
"out vec3 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = fragmentColor;\n"
"}\n\0";

//5个三维的点，每行前三个是坐标xyz，后三个是颜色rgb
float vertices[] = {  
    //位置                  //颜色
    -0.5f,  0.5f, 0.0f,     1.0f, 0.0f ,0.0f,
    -0.5f, -0.5f, 0.0f,     1.0f, 0.0f ,0.0f,
     0.5f, -0.5f, 0.0f,     0.0f, 1.0f ,0.0f,
     0.5f,  0.5f, 0.0f,     0.0f, 1.0f ,0.0f,
     0.0f,  0.0f, 0.0f,     0.0f, 0.0f ,0.0f
};

//顶点索引数组
unsigned int indices[] = {
    0, 1, 4,  //第一个三角形
    2, 3, 4   //第二个三角形
};

void MyOglWidget::initializeGL()
{
    initializeOpenGLFunctions();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VS, NULL);  //读取shader
    glCompileShader(vertexShader);               //编译shader

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FS, NULL);
    glCompileShader(fragmentShader);
    
    //给shader中的uniform变量赋值。注意赋值函数使用的位置，只有当一个program启用的时候，里面的uniform变量才会被分配索引，所以过早的对uniform变量赋值会长生错误
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgram);
    glDeleteProgram(shaderProgram);
}

void MyOglWidget::resizeGL(int w, int h)
{

}

void MyOglWidget::paintGL()
{
    //创建对象  生成缓冲对象的名称
    //1.生成缓冲对象的数量
    //2.用来存储缓冲对象名称的数组
    //glCreateBuffers和glGenBuffers作用一样，前者opengl4.5才支持，后者所有版本支持
    GLuint VAO, VBO, EBO;  //创建显存空间，并分配VBO的id
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);  //
    glGenBuffers(1, &EBO);  //EBO用来绑定顶点的索引，使用EBO时必须使用glDrawElements绘制

    //绑定类型 绑定缓冲对象的名称
    //1.缓冲对象的类型，
    //2.要绑定的缓冲对象的名称，也就是我们在上一个函数里生成的名称，使用该函数将缓冲对象绑定到OpenGL上下文环境中以便使用
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); //顶点数据绑定一个VBO，（同一类型的数据只能绑定一个VBO）第一个参数指定绑定的数据类型
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);  //EBO绑定和填充时的参数为GL_ELEMENT_ARRAY_BUFFER

    //填充数据
    //1.目标缓冲对象，符号常量必须为GL_ARRAY_BUFFER或GL_ELEMENT_ARRAY_BUFFER。
    //2.缓冲区对象的新数据存储的大小（以字节为单位）
    //3.复制到数据存储区以进行初始化的数据的指针，如果不复制数据，则指定NULL。
    //4.指定数据存储的预期使用模式。 符号常量必须为GL_STREAM_DRAW，GL_STATIC_DRAW或GL_DYNAMIC_DRAW
    //glBufferData把用户定义的数据传输到当前绑定的显存缓冲区（VBO)中，顶点数据传输到GPU后，通过glVertexAttribPointer告诉GPU如何解释这些数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //配置
    //glVertexAttribPointer的参数含义：
    // 1. 指定我们要配置的顶点属性，和layout的数字是对应的
    // 2. 指定顶点属性的大小(坐标xyz共有三个,颜色rgb也有三个）
    // 3. 指定数据的类型
    // 4. 是否希望数据被标准化(Normalize)(映射到[0,1],有符号数则为[-1,1]  是否有法线
    // 5. 步长(Stride),连续的顶点属性组之间的间隔,从这个属性第二次出现的地方到整个数组0位置之间有多少字节,可以理解为一组数据有多大，
    //    比如一组数据包括坐标xyz，颜色rgb那么步长就为6，要乘字节长度，例如：6 * sizeof(GLfloat)
    // 6. 数据指针,数据开始的位置，比如一组数据包括xyz,rgb，那么xyz开始的位置为0，rgb开始的位置为3，要乘字节长度，例如：reinterpret_cast<void*>(3 * sizeof(GLfloat))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    //glEnableVertexAttribArray:开启顶点着色器的属性，不开启的时候，顶点在着色器端是不可见的，也就意味着画不出图像
    //参数好像和glVertexAttribPointer的 第一个参数有对应的关系
    //layout(location = 1)  在着色器中设置的属性 location = 1 和这个 1 有关系
    glEnableVertexAttribArray(0);
    //配置颜色，glVertexAttribPointer应该是给shader中的输入参数赋值
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1); //，用来开启顶点属性(默认是关闭)，参数用来指定哪一个顶点属性（也就是序号，不是true或false），调用该函数前必须调用glVertexAttribPointer，否则程序会崩溃



    //绘制1
    //1.绘图模式
    //2.依次从索引数组中读取几个顶点来进行绘制（这里一个点有3个坐标，3个颜色，因此为6）
    //3.索引数组中存放的元素的类型
    //4.指向索引数组的指针，在前面填充部分已经设置了
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //前面使用了EBO，所以这里使用该函数绘制
    //绘制2
    //glDrawArrays:当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
    // glDrawArrays(GLenum mode, GLint first, GLsizei count) 参数：
    // 1. mode，绘制方式，GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN
    // 2. first，从数组缓存中的哪一位开始绘制，表示从指定数据数组的哪一个点开始画起，一般为0，
    // 3. count，数组中顶点的数量
    // 备注（重要）： 调用该函数之前，需要调用glEnableVertexAttribArray、glVertexAttribPointer等设置顶点属性和数据
    //glPointSize(10);
    //glDrawArrays(GL_POINTS, 0, 5);

    //卸载
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

}

//glBufferData  顶点一次  序列一次
//glVertexAttribPointer 顶点一次  颜色一次