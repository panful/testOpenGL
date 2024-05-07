// clang-format off
#include <windows.h>
#include <glad/glad.h>
#include <GL/gl.h>
// clang-format on

// 全局变量
HWND hWnd  = NULL; // 窗口句柄
HDC hDC    = NULL; // 设备上下文
HGLRC hRC  = NULL; // OpenGL渲染上下文
int width  = 800;  // 窗口宽度
int height = 600;  // 窗口高度

// 窗口过程函数，处理窗口消息
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// 初始化OpenGL
void initOpenGL()
{
    // 设置像素格式
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.iLayerType = PFD_MAIN_PLANE;

    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);

    // 创建OpenGL渲染上下文并激活
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    gladLoadGL();

    // 设置OpenGL选项
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

// 绘制三角形
void drawTriangle()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(.5f, .5f, .0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.0f, 0.5f, 0.0f);
    glEnd();

    SwapBuffers(hDC);
}

// 主循环
void mainLoop()
{
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            drawTriangle();
        }
    }
}

// WinMain：程序入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;

    // 注册窗口类
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = WindowProcedure;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "OpenGL";

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 创建窗口
    hWnd =
        CreateWindow("OpenGL", "WinOGL", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 0, width, height, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 获取设备上下文
    hDC = GetDC(hWnd);

    // 初始化OpenGL
    initOpenGL();

    // 进入主循环
    mainLoop();

    // 清理资源
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
    DestroyWindow(hWnd);
    UnregisterClass("OpenGL", hInstance);

    return 0;
}
