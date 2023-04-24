#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class GLWidget :
    public QOpenGLWidget,
    protected QOpenGLFunctions
{
public:
    void paintGL();
    void initializeGL();
    void resizeGL();
};
