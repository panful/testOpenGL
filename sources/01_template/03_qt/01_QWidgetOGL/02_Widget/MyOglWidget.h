#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>

class MyOglWidget :
    public QOpenGLWidget,
    protected QOpenGLFunctions_4_5_Core
{
public:
    void paintGL();
    void initializeGL();
    void resizeGL(int w,int h);
};

