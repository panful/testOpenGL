#include "GLWidget.h"

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.5, 0.5, 0.5, 0);  //设置背景色
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    const GLfloat vers[] = {
        (0.25), (0.25), (0.0),
        (0.75), (0.25), (0.0),
        (0.25), (0.75), (0.0),
        (0.75), (0.75), (0.0),
    };

    //glVertexPointer(3, GL_FIXED, 0, vers);

    glDrawArrays(GL_LINE_STRIP, 0, 4);

    //GLfloat position[9] = {
    //    -0.8f, -0.8f, 0.0f,
    //    0.8f, -0.8f, 0.0f,
    //    0.0f, 0.8f, 0.0f };



    //单独vao绘制  //这个要包含opengl32.lib
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glVertexPointer(3, GL_FLOAT, 0, position);/*size：指定每个顶点对应的坐标个数,可取2,3,4
    //                       type：指定数组中每个顶点坐标的数据类型:GL_BYTE, GL_SHORT,GL_FIXED,GL_FLOAT；
    //                       stride:指定连续顶点间的字节排列方式，即<从一个数据的开始到下一个数据的开始,
    //                        所相隔的字节数>，为0表示数组中的顶点被认为按照紧凑方式排列，默认为0；
    //                       pointer:指定了数组中第一个顶点的首地址，默认值为0。*/
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDisableClientState(GL_VERTEX_ARRAY);


    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)NULL);
    //glEnableVertexAttribArray(0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);


    /***单独vbo绘制***/
    //GLuint vboNum;
    //glGenBuffers(1, &vboNum);
    //glBindBuffer(GL_ARRAY_BUFFER, vboNum);
    //glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), position, GL_STATIC_DRAW);

    //glEnableClientState(GL_VERTEX_ARRAY);
    //glVertexPointer(3, GL_FLOAT, 0, 0);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    //glDisableClientState(GL_VERTEX_ARRAY);


    /***使用glVertexAttribPointer()、vbo和vao传递数据***/
    /***vbo***/
    //GLuint vboNum;
    //glGenBuffers(1, &vboNum);
    //glBindBuffer(GL_ARRAY_BUFFER, vboNum);
    //glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), position, GL_STATIC_DRAW);

    ///***vao***/
    //GLuint vaoNum;
    //glGenVertexArrays(1, &vaoNum);
    //glBindVertexArray(vaoNum);
    //glEnableVertexAttribArray(0);

    //glBindBuffer(GL_ARRAY_BUFFER, vboNum);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)NULL);

    //glBindVertexArray(vaoNum);
    //glDrawArrays(GL_TRIANGLES, 0, 3);


    //GLfloat arr_c3f_v3f[] = {
    //1, 0, 0, 0, 1, 0,
    //0, 1, 0, 1, 0, 0,
    //0, 0, 1, -1, 0, 0,
    //};
    //GLuint index_list[] = { 0, 1, 2 };
    //glInterleavedArrays(GL_C3F_V3F, 0, arr_c3f_v3f);
    //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, index_list);


    //glBegin(GL_POLYGON);
    //glColor3f(1, 0, 0);
    //glVertex2f(0, 0.5);
    //glColor3f(0, 1, 0);
    //glVertex2f(-0.5, -0.5);
    //glColor3f(0, 0, 1);
    //glVertex2f(0.5, -0.5);
    //glEnd();

//#define PI 3.1415926f
//    double d = 0.0;
//
//    while (d < 2 * PI)
//    {
//        GLfloat x1 = 0.5 * sin(d);
//        GLfloat y1 = 0.5 * cos(d);
//        GLfloat x2 = sin(d - PI / 9.0);
//        GLfloat y2 = cos(d - PI / 9.0);
//        GLfloat x3 = sin(d + PI / 9.0);
//        GLfloat y3 = cos(d + PI / 9.0);
//
//        glBegin(GL_TRIANGLES);
//        glVertex2f(y1, x1);
//        glVertex2f(y2, x2);
//        glVertex2f(y3, x3);
//        glEnd();
//        d += (PI / 9.0);
//    }


//    glRotatef(30.0, 1, 0, 0);
//    glRotatef(30.0, 0, 1, 0);
//
//
//    glClearStencil(0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//
//    glEnable(GL_STENCIL_TEST);
//    glEnable(GL_DEPTH_TEST);
//    glStencilFunc(GL_NEVER, 0x0, 0x0);
//    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
//
//    glColor3f(1.0f, 1.0f, 1.0f);
//    GLdouble dRadius = 0.2;
//    //在模板缓冲区绘制(因为模板测试失败不能在颜色缓冲区写入)
//
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0, 0, -0.5);
//#define PI 3.1415926f
//    for (double d = 0.0; d < 2 * PI; d += 0.1)
//    {
//        glVertex3f(dRadius * cos(d), dRadius * sin(d), -0.5);
//    }
//    glVertex3f(dRadius * cos(0.0), dRadius * sin(0.0), -0.5);
//    glEnd();
//
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0, 0, 0.5);
//
//    for (double d = 0.0; d < 2 * PI; d += 0.1)
//    {
//        glVertex3f(dRadius * cos(d), dRadius * sin(d), 0.5);
//    }
//    glVertex3f(dRadius * cos(0.0), dRadius * sin(0.0), 0.5);
//    glEnd();
//    //现在与颜色缓冲区在模板缓冲区对应处有线的地方不会绘制
//    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//
//    glColor3f(1.0, 1.0, 0.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.5, 0.5, -0.5);
//    glVertex3f(-0.5, 0.5, -0.5);
//    glVertex3f(-0.5, -0.5, -0.5);
//    glVertex3f(0.5, -0.5, -0.5);
//    glEnd();
//
//    glColor3f(0.0, 1.0, 1.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.5, 0.5, 0.5);
//    glVertex3f(-0.5, 0.5, 0.5);
//    glVertex3f(-0.5, -0.5, 0.5);
//    glVertex3f(0.5, -0.5, 0.5);
//    glEnd();
//
//    glColor3f(0.0, 1.0, 0.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.5, 0.5, 0.5);
//    glVertex3f(0.5, 0.5, -0.5);
//    glVertex3f(-0.5, 0.5, -0.5);
//    glVertex3f(-0.5, 0.5, 0.5);
//    glEnd();
//
//    glColor3f(1.0, 0.0, 0.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.5, -0.5, 0.5);
//    glVertex3f(0.5, -0.5, -0.5);
//    glVertex3f(-0.5, -0.5, -0.5);
//    glVertex3f(-0.5, -0.5, 0.5);
//    glEnd();
//
//    glColor3f(0.0, 1.0, 0.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(0.5, 0.5, 0.5);
//    glVertex3f(0.5, 0.5, -0.5);
//    glVertex3f(0.5, -0.5, -0.5);
//    glVertex3f(0.5, -0.5, 0.5);
//    glEnd();
//
//    glColor3f(1.0, 0.0, 0.0);
//    glBegin(GL_TRIANGLE_FAN);
//    glVertex3f(-0.5, 0.5, 0.5);
//    glVertex3f(-0.5, 0.5, -0.5);
//    glVertex3f(-0.5, -0.5, -0.5);
//    glVertex3f(-0.5, -0.5, 0.5);
//    glEnd();



    //glBegin(GL_LINES);
    //glVertex2f(-0.5, -0.5);
    //glVertex2f(0.5, 0.5);
    //glEnd();
}

void GLWidget::resizeGL()
{
}
