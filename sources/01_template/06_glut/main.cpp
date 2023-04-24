#define TEST6

/*
1.右键菜单控制图形大小以及颜色
2.九宫格拾取
3.拾取（程序有一点问题）
4.加载纹理
5.鼠标拖动
6.模板缓冲区，实现挖空多边形
7.glReadPixel使用示例
*/



#ifdef TEST1

#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define DR 3.14159/180.0

int N = 4;
int num = 0;
int dir = 1;
int flag = 1;
double size = 10;
static GLfloat spin = 0.0;
GLfloat x, y;
int doubleb;

void triangle(GLfloat x, GLfloat y)
{ //画三角形，实际没用到……
    glPushMatrix();//将当前矩阵压栈（保存现场）
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + size / 2);
    glVertex2f(x - size / 2, y - size / 2);
    glVertex2f(x + size / 2, y - size / 2);
    glEnd();
    glPopMatrix();
    glutSwapBuffers();//双缓存技术的函数，作用为交换两个缓冲区的指针
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();//将当前矩阵压栈(保存现场)
    glBegin(GL_TRIANGLES);
    //设顶点坐标为(cos(spin),sin(spin))，其他两点坐标可由spin +/- 120°后得到
    glVertex2f(cos(DR * spin) * size + 30, sin(DR * spin) * size + 30);
    glVertex2f(cos(DR * (spin + 120)) * size + 30, sin(DR * (spin + 120)) * size + 30);
    glVertex2f(cos(DR * (spin - 120)) * size + 30, sin(DR * (spin - 120)) * size + 30);
    glVertex2f(cos(DR * spin) * size + 10, sin(DR * spin) * size - 30);
    glVertex2f(cos(DR * (spin + 120)) * size + 10, sin(DR * (spin + 120)) * size - 30);
    glVertex2f(cos(DR * (spin - 120)) * size + 10, sin(DR * (spin - 120)) * size - 30);
    glVertex2f(cos(DR * spin) * size - 20, sin(DR * spin) * size + 20);
    glVertex2f(cos(DR * (spin + 120)) * size - 20, sin(DR * (spin + 120)) * size + 20);
    glVertex2f(cos(DR * (spin - 120)) * size - 20, sin(DR * (spin - 120)) * size + 20);
    glVertex2f(cos(DR * spin) * size - 35, sin(DR * spin) * size - 20);
    glVertex2f(cos(DR * (spin + 120)) * size - 35, sin(DR * (spin + 120)) * size - 20);
    glVertex2f(cos(DR * (spin - 120)) * size - 35, sin(DR * (spin - 120)) * size - 20);
    glEnd();
    glPopMatrix();
    glutSwapBuffers();//双缓存技术的函数，作用为交换两个缓冲区的指针
}

void spinDisplay(void)
{ //角度变化
    if (dir) spin = spin + 0.05;
    else spin = spin - 0.05;
    if (spin > 360.0) spin = spin - 360.0;
    glutSetWindow(doubleb);
    glutPostRedisplay();
}

void myinit()
{ //初始化
    glViewport(0, 0, 500, 500);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)500, 0.0, (GLdouble)500, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glColor3f(1.0, 1.0, 1.0); //前景色为白色

}

void myMouse(int btn, int state, int x, int y)
{ //鼠标
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {//点击鼠标左键画三角形，但没起到作用
        if (num < N) triangle(x, y);
        num++;
    }
}

void myKey(unsigned char key, int x, int y)
{ //键盘
    if (key == ' ')
    { //如果按下空格键，则控制旋转和暂停
        flag ^= 1;
        if (flag) glutIdleFunc(spinDisplay);
        else glutIdleFunc(NULL);
    }
    //按下C字母改变旋转方向
    else if (key == 'c' || key == 'C') dir ^= 1;
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-50.0, 50.0, -50.0 * (GLfloat)h / (GLfloat)w,
            50.0 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-50.0 * (GLfloat)w / (GLfloat)h,
            50.0 * (GLfloat)w / (GLfloat)h, -50.0, 50.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void myTimer(int v)
{
    glutSetWindow(doubleb);
    glutPostRedisplay();
    glutTimerFunc(100, myTimer, v);
}

void main_menu(int id)
{ //主菜单
    switch (id)
    {
    case 1: exit(0);
        break;
    }
}

void size_menu(int id)
{ //大小菜单
    switch (id)
    {
    case 2: size = 2 * size;
        break;
    case 3: if (size > 1) size = size / 2;
        break;
    }
}

void color_menu(int id)
{ //颜色菜单
    switch (id)
    {
    case 4: glColor3f(1.0, 0.0, 0.0); //红色 
        break;
    case 5: glColor3f(0.0, 0.0, 1.0); //蓝色 
        break;
    case 6: glColor3f(1.0, 1.0, 0.0); //黄色 
        break;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    doubleb = glutCreateWindow("Triangle");
    /*右键菜单*/
    /*大小子菜单*/
    int sub_menu1 = glutCreateMenu(size_menu);
    glutAddMenuEntry("Increase size", 2);
    glutAddMenuEntry("Decrease size", 3);
    /*颜色子菜单*/
    int sub_menu2 = glutCreateMenu(color_menu);
    glutAddMenuEntry("red", 4);
    glutAddMenuEntry("blue", 5);
    glutAddMenuEntry("yellow", 6);
    /*主菜单*/
    glutCreateMenu(main_menu);
    glutAddMenuEntry("Quit", 1);
    glutAddSubMenu("Size", sub_menu1); //添加子菜单 
    glutAddSubMenu("Color", sub_menu2);    //添加子菜单
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    myinit();
    glutDisplayFunc(display);
    glutMouseFunc(myMouse); //鼠标事件
    glutKeyboardFunc(myKey); //键盘事件
    glutReshapeFunc(myReshape);
    glutIdleFunc(spinDisplay);
    glutTimerFunc(100, myTimer, 60);

    glutMainLoop();
    return 0;
}

#endif // TEST1

#ifdef TEST2

#include <GL/glut.h> 

int board[3][3];   //存储几个方块的颜色

#define BUFSIZE 512
//处理点击记录:
//hits为产生的点击的数量，buffer中存储点击记录，每个点击记录由四个项目组成
void processHits(GLint hits, GLuint buffer[])
{
    unsigned int i, j;
    GLuint ii, jj, names, * ptr;

    ptr = (GLuint*)buffer;

    for (i = 0; i < hits; i++)    //处理每一个点击记录
    {
        //某一个点击记录来说，由四个项目组成：
        //（1）当点击发生时，名字堆栈中的名称数量
        //（2）自上一个点击记录之后，与视景体相交的所有顶点的最小和最大窗口坐标z值
        //（3）当点击发生时，名称堆栈的内容，从最底部的元素开始
        names = *ptr;      //获得名字堆栈中的名称数量
        ptr += 3;               //跳过前三个记录

        for (j = 0; j < names; j++) //开始处理名字堆栈中的内容，获取被选中的方块的index
        {
            //对应于绘制方块时，压入名字堆栈中的内容

            if (j == 0)        //x方向上的index
                ii = *ptr;
            else if (j == 1)   //y方向上的index
                jj = *ptr;
            ptr++;
        }
    }
    board[ii][jj] = (board[ii][jj] + 1) % 3;   //改变被选中方块的颜色
}



//绘制所有方块，参数有GL_RENDER和GL_SELECT两种模式
void drawSquares(GLenum mode)
{
    GLuint i, j;

    for (i = 0; i < 3; i++)
    {
        if (mode == GL_SELECT)       //如果是在选择模式下，将名字堆栈的首元素换成x方向上的索引
            glLoadName(i);

        for (j = 0; j < 3; j++)
        {
            if (mode == GL_SELECT)    //将y方向上的索引压入名字堆栈
                glPushName(j);

            //绘制方块，在GL_SELECT模式下，某一个方块会被选中，因此，会产生一个点击记录

            //该点击被记录时，名字堆栈中有两个名称，分别是i和j的值，也就是被选中方块的索引
            glColor3f((GLfloat)i / 3.0, (GLfloat)j / 3.0, (GLfloat)board[i][j] / 3.0);
            glRecti(i, j, i + 1, j + 1);

            if (mode == GL_SELECT) //弹出名字
                glPopName();
        }
    }
}

//当鼠标左键点击窗口时，进入选择模式开始绘制；绘制之后，处理点击记录
void pickSquares(int button, int state, int x, int y)
{
    GLuint selectBuf[BUFSIZE] = { 0 };   //存储点击记录
    GLint hits;                                 //点击记录的个数
    GLint viewport[4];                    //视口

    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    glGetIntegerv(GL_VIEWPORT, viewport);  //获得视口

    glSelectBuffer(BUFSIZE, selectBuf);    //指定存储点击记录的数组
    glRenderMode(GL_SELECT);          //进入选择模式

    glInitNames();           //初始化名字堆栈并压入初始元素
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    //设置挑选矩阵，挑选区域的中心坐标是(x,viewport[3]-y)，大小是(5,5)
    gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);
    //设置投影矩阵
    gluOrtho2D(0.0, 3.0, 0.0, 3.0);

    //在选择模式下绘制方块
    drawSquares(GL_SELECT);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glFlush();        //绘制结束

   //处理点击记录

    hits = glRenderMode(GL_RENDER); //获取记录下的点击的个数
    processHits(hits, selectBuf);           //处理点击记录selectBuf

    glutPostRedisplay();
}


void init()
{
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    for (int i = 0; i < 3; i++)            //初始化9个方块的颜色
        for (int j = 0; j < 3; j++)
            board[i][j] = 0;
}

void display()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSquares(GL_RENDER);  //基本绘制
    glFlush();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 3.0, 0.0, 3.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("pick");
    init();
    glutMouseFunc(pickSquares);    //当鼠标点击时，调用pickSquares，进入选择模式进行绘制
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);      //display只完成基本的绘制
    glutMainLoop();
    return 0;
}

#endif // TEST2

#ifdef TEST3
#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>

void Init()
{
    glClearColor(0.25f, 0.25f, 0.25f, 0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
}

void draw(GLenum nEnum)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (GL_SELECT == nEnum)
    {
        glPushName(1);
    }
    glColor3f(1.0, 0.0, 0.0); // 红色矩形
    glRectf(100, 100, 200, 200);

    if (GL_SELECT == nEnum)
    {
        glPushName(2);
    }
    glColor3f(0.0, 1.0, 0.0); // 绿色矩形
    glRectf(250, 100, 350, 200);

    if (GL_SELECT == nEnum)
    {
        glPushName(3);
    }
    glColor3f(0.0, 0.0, 1.0); // 蓝色矩形
    glRectf(175, 175, 275, 275);

    if (GL_SELECT == nEnum)
    {
        glPushName(4);
    }
    glColor3f(1.0, 1.0, 0.0); // 黄色线
    glBegin(GL_LINES);
    glVertex3f(50.0, 300.0, 0.0);
    glVertex3f(300.0, 300.0, 0.0);
    glEnd();

    glFinish();
    glFlush();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    draw(GL_RENDER);
}

void Reshape(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 400, 0, 400, 10, -10);

    return;
}

void mouseFunc(int button, int state, int x, int y)
{
    if (GLUT_LEFT_BUTTON == button && GLUT_DOWN == state)
    {
        GLuint nBuffer[512];
        glSelectBuffer(512, nBuffer);
        glRenderMode(GL_SELECT);
        glInitNames();
        glPushName(0);

        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLint  nViewport[4];
        glGetIntegerv(GL_VIEWPORT, nViewport);
        gluPickMatrix(x, nViewport[3] - y, 10, 10, nViewport);
        glOrtho(0, 400, 0, 400, 10, -10);
        draw(GL_SELECT);
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        int nPicks = glRenderMode(GL_RENDER);
        printf("\nSelect Num:%d--", nPicks);

        GLuint* ptr = nBuffer;
        GLuint name;
        for (int i = 0; i < nPicks; i++)
        {
            name = *ptr;
            ptr += 3;
            ptr += name - 1;
            switch (*ptr)
            {
            case 1:
                printf("--red--");
                break;
            case 2:
                printf("--gree--");
                break;
            case 3:
                printf("--blue--");
                break;
            case 4:
                printf("--yellow--");
                break;
            }
            ptr++;
        }
    }
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 300);
    Init();
    glutCreateWindow("OpenGL Test Yulinxx");
    glutDisplayFunc(display);
    glutReshapeFunc(Reshape);
    glutMouseFunc(mouseFunc);
    glutMainLoop();
    return 0;
}
#endif // TEST3

#ifdef TEST4
#define _CRT_SECURE_NO_WARNINGS
#define WindowWidth  400
#define WindowHeight 400
#define WindowTitle  "OpenGL纹理测试"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
//#pragma comment(lib,"glut32.lib")

//定义两个纹理对象编号
GLuint texGround;
GLuint texWall;

#define BMP_Header_Length 54  //图像数据在内存块中的偏移量
static GLfloat angle = 0.0f;   //旋转角度

// 函数power_of_two用于判断一个整数是不是2的整数次幂
int power_of_two(int n)
{
    if (n <= 0)
        return 0;
    return (n & (n - 1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint load_texture(const char* file_name)
{
    GLint width, height, total_bytes;
    GLubyte* pixels = 0;
    GLuint last_texture_ID = 0, texture_ID = 0;

    // 打开文件，如果失败，返回
    FILE* pFile = fopen(file_name, "rb");
    if (pFile == 0)
        return 0;

    // 读取文件中图象的宽度和高度
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&width, 4, 1, pFile);
    fread(&height, 4, 1, pFile);
    fseek(pFile, BMP_Header_Length, SEEK_SET);

    // 计算每行像素所占字节数，并根据此数据计算总像素字节数
    {
        GLint line_bytes = width * 3;
        while (line_bytes % 4 != 0)
            ++line_bytes;
        total_bytes = line_bytes * height;
    }

    // 根据总像素字节数分配内存
    pixels = (GLubyte*)malloc(total_bytes);
    if (pixels == 0)
    {
        fclose(pFile);
        return 0;
    }

    // 读取像素数据
    if (fread(pixels, total_bytes, 1, pFile) <= 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放
    // 若图像宽高超过了OpenGL规定的最大值，也缩放
    {
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        if (!power_of_two(width)
            || !power_of_two(height)
            || width > max
            || height > max)
        {
            const GLint new_width = 256;
            const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
            GLint new_line_bytes, new_total_bytes;
            GLubyte* new_pixels = 0;

            // 计算每行需要的字节数和总字节数
            new_line_bytes = new_width * 3;
            while (new_line_bytes % 4 != 0)
                ++new_line_bytes;
            new_total_bytes = new_line_bytes * new_height;

            // 分配内存
            new_pixels = (GLubyte*)malloc(new_total_bytes);
            if (new_pixels == 0)
            {
                free(pixels);
                fclose(pFile);
                return 0;
            }

            // 进行像素缩放
            gluScaleImage(GL_RGB,
                width, height, GL_UNSIGNED_BYTE, pixels,
                new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

            // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
            free(pixels);
            pixels = new_pixels;
            width = new_width;
            height = new_height;
        }
    }

    // 分配一个新的纹理编号
    glGenTextures(1, &texture_ID);
    if (texture_ID == 0)
    {
        free(pixels);
        fclose(pFile);
        return 0;
    }

    // 绑定新的纹理，载入纹理并设置纹理参数
    // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
    GLint lastTextureID = last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
    glBindTexture(GL_TEXTURE_2D, texture_ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定
    free(pixels);
    return texture_ID;
}


void display(void)
{
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置视角
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(75, 1, 1, 21);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //gluLookAt(-4, 7, 7, 0, 0, 0, 0, 0, 1);

    //glRotatef(angle, 0.0f, 0.0f, 1.0f); //旋转

    glBindTexture(GL_TEXTURE_2D, texGround);
    //glRectf(0, 0, 0.8, 0.8);
    glBegin(GL_QUADS);
    //glTexCoord2f(-0.25f, -0.25f); glVertex2f(-0.5, -0.5);
    //glTexCoord2f(0.75, -0.25); glVertex2f(0.5, -0.5);
    //glTexCoord2f(0.5, 0.75); glVertex2f(0, 0.5);

    glTexCoord2f(0, 0); glVertex2f(-0.5, -0.5);
    glTexCoord2f(1, 0); glVertex2f(0.5, -0.5);
    glTexCoord2f(1, 1); glVertex2f(0.5, 0.5);
    glTexCoord2f(0, 1); glVertex2f(-0.5, 0.5);

    glEnd();



    // 绘制底面以及纹理
    //glBindTexture(GL_TEXTURE_2D, texGround);
    //glBegin(GL_QUADS);
    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, -8.0f, 0.0f);
    //glTexCoord2f(0.0f, 3.0f); glVertex3f(-8.0f, 8.0f, 0.0f);
    //glTexCoord2f(3.0f, 3.0f); glVertex3f(8.0f, 8.0f, 0.0f);
    //glTexCoord2f(3.0f, 0.0f); glVertex3f(8.0f, -8.0f, 0.0f);
    //glEnd();
    //// 绘制立面
    //glBindTexture(GL_TEXTURE_2D, texWall);
    //glBegin(GL_QUADS);
    //glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, -3.0f, 0.0f);
    //glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, -3.0f, 5.0f);
    //glTexCoord2f(2.0f, 1.0f); glVertex3f(6.0f, -3.0f, 5.0f);
    //glTexCoord2f(2.0f, 0.0f); glVertex3f(6.0f, -3.0f, 0.0f);
    //glEnd();

    //绘制另外一个立面
    //glBegin(GL_QUADS);
    //glTexCoord2f(2.0f, 0.0f); glVertex3f(6.0f, -3.0f, 0.0f);
    //glTexCoord2f(0.0f, 0.0f); glVertex3f(6.0f, 9.0f, 0.0f);
    //glTexCoord2f(0.0f, 1.0f); glVertex3f(6.0f, 9.0f, 5.0f);
    //glTexCoord2f(2.0f, 1.0f); glVertex3f(6.0f, -3.0f, 5.0f);
    //glEnd();

    glutSwapBuffers();
}

void myIdle(void)
{
    angle += 1.8f;
    if (angle >= 360.0f)
        angle = 0.0f;
    display();
}

int main(int argc, char* argv[])
{
    // GLUT初始化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WindowWidth, WindowHeight);
    glutCreateWindow(WindowTitle);
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);    // 启用纹理
    texGround = load_texture("1.bmp");  //加载纹理
    texWall = load_texture("1.bmp‪");
    glutDisplayFunc(&display);   //注册函数 
    glutIdleFunc(&myIdle);
    glutMainLoop(); //循环调用
    return 0;
}
#endif // TEST4

#ifdef TEST5
//https://blog.csdn.net/Snow_Me/article/details/53649085
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <GL/glut.h>
using namespace std;

GLdouble newMat[4][2] = { 0,0, 100,0, 100,100, 0,100 };
const GLdouble mat[4][2] = { 0,0, 100,0, 100,100, 0,100 };

void init() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);
}
bool isEqual() {
    for (int i = 0; i < 4; ++i) {
        if (fabs(mat[i][0] - newMat[i][0]) >= 1e-6) return false;
        if (fabs(mat[i][1] - newMat[i][1]) >= 1e-6) return false;
    }
    return true;
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    for (int i = 0; i < 4; ++i)
        glVertex2d(mat[i][0], mat[i][1]);
    glEnd();
    glFlush();

    if (!isEqual()) {
        glBegin(GL_POLYGON);
        glColor3f(0.0, 1.0, 0.0); //移动后用绿色矩阵表示
        for (int i = 0; i < 4; ++i)
            glVertex2d(newMat[i][0], newMat[i][1]);
        glEnd();
        glFlush();
    }
}
bool isInner(int x, int y) {
    unsigned short int c = 0;
    if (x < newMat[0][0]) c |= 1; c <<= 1;
    if (x > newMat[1][0]) c |= 1; c <<= 1;
    if (y > newMat[2][1]) c |= 1; c <<= 1;
    if (y < newMat[0][1]) c |= 1;
    if (c == 0) return true;
    else return false;
}
void mouse_process(int button, int state, int x, int y) {
    /*
    if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
    }
    */
}
void mouse_process_active(int x, int y) {
    y = 600 - y; //因为获取的鼠标位置
    if (isInner(x, y)) {
        double avgx = (newMat[0][0] + newMat[1][0]) / 2.0;
        double avgy = (newMat[0][1] + newMat[3][1]) / 2.0;

        double dx = x - avgx;
        double dy = y - avgy;
        for (int i = 0; i < 4; ++i) {
            newMat[i][0] += dx;
            newMat[i][1] += dy;

            if (newMat[i][0] > 800) {
                int dx = newMat[i][0] - 800;
                for (int j = 0; j < 4; ++j)
                    newMat[j][0] -= dx;
                MessageBox(NULL, TEXT("Coordinate out of range"), TEXT("Warning"), MB_ICONWARNING | MB_OK);
                Sleep(1000);
            }
            else if (newMat[i][0] < 0) {
                int dx = 0 - newMat[i][0];
                for (int j = 0; j < 4; ++j)
                    newMat[j][0] += dx;
                MessageBox(NULL, TEXT("Coordinate out of range"), TEXT("Warning"), MB_ICONWARNING | MB_OK);
                Sleep(1000);
            }
            else if (newMat[i][1] > 600) {
                int dy = newMat[i][1] - 600;
                for (int j = 0; j < 4; ++j)
                    newMat[j][1] -= dy;
                MessageBox(NULL, TEXT("Coordinate out of range"), TEXT("Warning"), MB_ICONWARNING | MB_OK);
                Sleep(1000);
            }
            else if (newMat[i][1] < 0) {
                int dy = 0 - newMat[i][1];
                for (int j = 0; j < 4; ++j)
                    newMat[j][1] += dy;
                MessageBox(NULL, TEXT("Coordinate out of range"), TEXT("Warning"), MB_ICONWARNING | MB_OK);
                Sleep(1000);
            }
        }
        glutPostRedisplay();
    }
}
void mouse_process_passtive(int x, int y) {}
int main(int argv, char** argc) {
    glutInit(&argv, argc);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Lab 3");
    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse_process);
    glutMotionFunc(mouse_process_active);
    glutPassiveMotionFunc(mouse_process_passtive);

    glutMainLoop();
    return 0;
}

#endif // TEST5

#ifdef TEST6
//https://blog.csdn.net/csxiaoshui/article/details/23457273
#include <GL/glut.h> 

void display()
{
    glClearColor(1.0, 1.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NEVER, 0x0, 0x0);
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);

    glColor3f(1.0f, 1.0f, 1.0f);
    GLdouble dRadius = 0.2;
    //在模板缓冲区绘制(因为模板测试失败不能在颜色缓冲区写入)
    glRectf(-0.2, -0.2, 0.2, 0.2);

    //现在与颜色缓冲区在模板缓冲区对应处有线的地方不会绘制
    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glColor3f(0.0, 1.0, 1.0);
    glRectf(-0.8, -0.8, 0.8, 0.8);

    glFlush();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("模板缓冲");

    glutDisplayFunc(display);      //display只完成基本的绘制
    glutMainLoop();
    return 0;
}


#endif // TEST6

#ifdef TEST7

#include <GL/glut.h>
#include <vector>
#include <iostream>

using namespace std;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glScalef(5, 5, 5);
    glBegin(GL_QUADS);
    glColor3ub(255, 0, 0);
    glVertex2f(-1, -1);
    glColor3ub(0, 255, 0);
    glVertex2f(1, -1);
    glColor3ub(0, 0, 255);
    glVertex2f(1, 1);
    glColor3ub(255, 255, 255);
    glVertex2f(-1, 1);
    glEnd();

    glutSwapBuffers();
}

void mouse(int x, int y)
{
    // 4 bytes per pixel (RGBA), 1x1 bitmap
    vector< unsigned char > pixels(1 * 1 * 4);
    glReadPixels
    (
        x, glutGet(GLUT_WINDOW_HEIGHT) - y,
        1, 1,
        GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]
    );

    cout << "r:" << (int)pixels[0] << endl;  //转为int
    cout << "g:" << (int)pixels[1] << endl;
    cout << "b:" << (int)pixels[2] << endl;
    cout << "a:" << (int)pixels[3] << endl;
    cout << endl;
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

    glutInitWindowSize(800, 600);
    glutCreateWindow("glReadPixels()");

    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouse);
    glutMainLoop();
    return 0;
}

#endif // TEST7
