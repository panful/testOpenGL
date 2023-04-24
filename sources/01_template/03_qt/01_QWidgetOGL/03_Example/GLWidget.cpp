
#include "glwidget.h"
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QCoreApplication>
#include <math.h>

bool GLWidget::m_transparent = false;

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
    m_xRot(0),
    m_yRot(0),
    m_zRot(0),
    m_program(0)
{
    m_core = QSurfaceFormat::defaultFormat().profile() == QSurfaceFormat::CoreProfile;
    // --transparent causes the clear color to be transparent. Therefore, on systems that
    // support it, the widget will become transparent apart from the logo.
    //透明使透明色透明。因此，在支持它的系统上，除了logo外，widget将变得透明。
    if (m_transparent) {
        QSurfaceFormat fmt = format();
        fmt.setAlphaBufferSize(8);    //设置颜色缓冲区的alpha通道的所需大小
        setFormat(fmt); //设置请求的曲面格式。如果未通过此函数显式设置格式，则将使用QSurfaceFormat:：defaultFormat（）返回的格式。
    }
}

GLWidget::~GLWidget()
{
    cleanup();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);  //保存小部件的建议最小大小
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400); //保存窗口的建议大小,可以认为是设置窗口大小，有时和设置的大小不一样（好像会根据屏幕自动调整，不确定，个人猜测）
}

static void qNormalizeAngle(int& angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::cleanup()
{
    if (m_program == nullptr)
        return;
    makeCurrent(); //通过使相应的上下文成为当前上下文并在该上下文中绑定帧缓冲区对象，准备呈现此小部件的OpenGL内容。
                   //在大多数情况下，不必调用此函数，因为它是在调用paintGL（）之前自动调用的。
    m_logoVbo.destroy();
    delete m_program;
    m_program = 0;
    doneCurrent();
}

static const char* vertexShaderSourceCore =
"#version 150\n"
"layout (location = 0) in vec4 vertex;\n"
"layout (location = 1) in vec3 normal;\n"
"out vec3 vert;\n"
"out vec3 vertNormal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"}\n";

static const char* fragmentShaderSourceCore =
"#version 150\n"
"layout (location = 1) in highp vec3 vert;\n"
"layout (location = 0) in highp vec3 vertNormal;\n"
"out highp vec4 fragColor;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
"   fragColor = vec4(col, 1.0);\n"
"}\n";

static const char* vertexShaderSource =
"attribute vec4 vertex;\n"
"attribute vec3 normal;\n"
"varying vec3 vert;\n"
"varying vec3 vertNormal;\n"
"uniform mat4 projMatrix;\n"
"uniform mat4 mvMatrix;\n"
"uniform mat3 normalMatrix;\n"
"void main() {\n"
"   vert = vertex.xyz;\n"
"   vertNormal = normalMatrix * normal;\n"
"   gl_Position = projMatrix * mvMatrix * vertex;\n"
"}\n";

static const char* fragmentShaderSource =
"varying highp vec3 vert;\n"
"varying highp vec3 vertNormal;\n"
"uniform highp vec3 lightPos;\n"
"void main() {\n"
"   highp vec3 L = normalize(lightPos - vert);\n"
"   highp float NL = max(dot(normalize(vertNormal), L), 0.0);\n"
"   highp vec3 color = vec3(0.39, 1.0, 0.0);\n"           //这里可以设置"QT"的颜色
"   highp vec3 col = clamp(color * 0.2 + color * 0.8 * NL, 0.0, 1.0);\n"
//"   highp vec3 col = clamp(1,1,0);\n"
"   gl_FragColor = vec4(col, 1.0);\n"
"}\n";

void GLWidget::initializeGL()
{
    // In this example the widget's corresponding top-level window can change
    // several times during the widget's lifetime. Whenever this happens, the
    // QOpenGLWidget's associated context is destroyed and a new one is created.
    // Therefore we have to be prepared to clean up the resources on the
    // aboutToBeDestroyed() signal, instead of the destructor. The emission of
    // the signal will be followed by an invocation of initializeGL() where we
    // can recreate all resources.

    //在本例中，小部件对应的顶级窗口在小部件的生命周期内可以更改几次。每当发生这种情况时，
    //QOpenGLWidget的关联上下文就会被销毁，并创建一个新的上下文。
    //因此，我们必须准备清理aboutToBeDestroyed（）信号上的资源，
    //而不是析构函数。信号发出之后将调用initializeGL（），在这里我们可以重新创建所有资源。

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &GLWidget::cleanup);

    initializeOpenGLFunctions();
    //为当前上下文初始化OpenGL函数解析。
    //调用此函数后，QOpenGLFunctions对象只能与当前上下文以及与其共享的其他上下文一起使用。再次调用InitializePenglFunctions（）以更改对象的上下文关联。

    glClearColor(1, 0, 0, m_transparent ? 0 : 1);

    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_core ? vertexShaderSourceCore : vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_core ? fragmentShaderSourceCore : fragmentShaderSource);
    m_program->bindAttributeLocation("vertex", 0);
    m_program->bindAttributeLocation("normal", 1);    //设置传入的变量
    m_program->link();


    // uniformLocation:返回此着色器程序的参数列表中统一变量名的位置
    m_program->bind();
    m_projMatrixLoc = m_program->uniformLocation("projMatrix");
    m_mvMatrixLoc = m_program->uniformLocation("mvMatrix");
    m_normalMatrixLoc = m_program->uniformLocation("normalMatrix");
    m_lightPosLoc = m_program->uniformLocation("lightPos");

    // Create a vertex array object. In OpenGL ES 2.0 and OpenGL 2.x
    // implementations this is optional and support may not be present
    // at all. Nonetheless the below code works in all cases and makes
    // sure there is a VAO when one is needed.
    //创建顶点数组对象。在OpenGLES2.0和OpenGL2.x实现中，这是可选的，
    //可能根本不存在支持。尽管如此，下面的代码在所有情况下都有效，并确保在需要时有一个VAO。

    m_vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);

    // Setup our vertex buffer object.
    m_logoVbo.create();
    m_logoVbo.bind();
    m_logoVbo.allocate(m_logo.constData(), m_logo.count() * sizeof(GLfloat));

    // Store the vertex attribute bindings for the program.
    //存储程序的顶点属性绑定。
    setupVertexAttribs();

    // Our camera never changes in this example.
    //在这个例子中，我们的相机从不改变。
    m_camera.setToIdentity();
    m_camera.translate(0, 0, -1);

    // Light position is fixed.
    //灯光位置固定。
    m_program->setUniformValue(m_lightPosLoc, QVector3D(0, 0, 70));  // QVector3D:三维空间中的向量或顶点

    m_program->release();
}

void GLWidget::setupVertexAttribs()
{
    m_logoVbo.bind();
    // currentContext  返回在当前线程中调用makeCurrent的最后一个上下文，如果没有当前上下文，则返回0
    // functions       获取此上下文的QOpenGLFunctions实例
    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    //glEnableVertexAttribArray:开启顶点着色器的属性，不开启的时候，顶点在着色器端是不可见的，也就意味着画不出图像
    f->glEnableVertexAttribArray(0);  // 参数0好像和glVertexAttribPointer的 第一个参数 0有关系
    f->glEnableVertexAttribArray(1);  // layout(location = 1)  在着色器中设置的属性 location = 1 和这个 1 有关系
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    //glVertexAttribPointer的参数含义：
    // 1. 指定我们要配置的顶点属性，和layout的数字是对应的
    // 2. 指定顶点属性的大小(坐标xyz共有三个,颜色rgb也有三个）
    // 3. 指定数据的类型
    // 4. 是否希望数据被标准化(Normalize)(映射到[0,1],有符号数则为[-1,1]  是否有法线
    // 5. 步长(Stride),连续的顶点属性组之间的间隔,从这个属性第二次出现的地方到整个数组0位置之间有多少字节,可以理解为一组数据有多大，
    //    比如一组数据包括坐标xyz，颜色rgb那么步长就为6，要乘字节长度，例如：6 * sizeof(GLfloat)
    // 6. 数据指针,数据开始的位置，比如一组数据包括xyz,rgb，那么xyz开始的位置为0，rgb开始的位置为3，要乘字节长度，例如：reinterpret_cast<void*>(3 * sizeof(GLfloat))
    m_logoVbo.release();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);   //开启深度测试（开启后立体图形不可以看到遮住的部分）
    glEnable(GL_CULL_FACE);    //根据函数glCullFace要求启用隐藏图形材料的面

    m_world.setToIdentity();  //注释这句代码后，鼠标拖动转的特别快
    m_world.rotate(180.0f - (m_xRot / 16.0f), 1, 0, 0);
    m_world.rotate(m_yRot / 16.0f, 0, 1, 0);  //绕着y轴旋转(m_yRot/16.0f)度
    m_world.rotate(m_zRot / 16.0f, 0, 0, 1);  //绕着z轴旋转(m_zRot/16.0f)度

    QOpenGLVertexArrayObject::Binder vaoBinder(&m_vao);
    m_program->bind();
    // setUniformValue:将当前上下文中位置处的统一变量设置为4x4矩阵值
    m_program->setUniformValue(m_projMatrixLoc, m_proj);
    m_program->setUniformValue(m_mvMatrixLoc, m_camera * m_world);
    // normalMatrix:返回与此4x4变换对应的正规矩阵。正规矩阵是这个4x4矩阵左上3x3部分的逆的转置。如果3x3子矩阵不可逆，则此函数返回恒等式。
    QMatrix3x3 normalMatrix = m_world.normalMatrix();
    m_program->setUniformValue(m_normalMatrixLoc, normalMatrix);

    //glDrawArrays:当采用顶点数组方式绘制图形时，使用该函数。该函数根据顶点数组中的坐标数据和指定的模式，进行绘制。
    // glDrawArrays(GLenum mode, GLint first, GLsizei count) 参数：
    // 1. mode，绘制方式，GL_POINTS、GL_LINES、GL_LINE_LOOP、GL_LINE_STRIP、GL_TRIANGLES、GL_TRIANGLE_STRIP、GL_TRIANGLE_FAN
    // 2. first，从数组缓存中的哪一位开始绘制，表示从指定数据数组的哪一个点开始画起，一般为0，
    // 3. count，数组中顶点的数量
    // 备注（重要）： 调用该函数之前，需要调用glEnableVertexAttribArray、glVertexAttribPointer等设置顶点属性和数据
    glDrawArrays(GL_TRIANGLES, 0, m_logo.vertexCount());

    m_program->release();
}

void GLWidget::resizeGL(int w, int h)
{
    m_proj.setToIdentity();
    m_proj.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}

void GLWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent* event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 5 * dy);
        setYRotation(m_yRot + 5 * dx);
    }
    else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 5 * dy);
        setZRotation(m_zRot + 5 * dx);
    }
    m_lastPos = event->pos();
}
