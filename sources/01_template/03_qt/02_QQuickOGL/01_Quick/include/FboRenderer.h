#pragma once
#include <QOpenGLFunctions_3_3_Core>
#include <QQuickFramebufferObject>

class FboRenderer :
    public QQuickFramebufferObject::Renderer,
    protected QOpenGLFunctions_3_3_Core
{
public:
    FboRenderer();
protected:
    void render() override;
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
private:
    void init();
    void doRender();
private:
    QOpenGLFunctions_3_3_Core* mGraphicFunc;
    unsigned int VAO, VBO, mProgram;
};

