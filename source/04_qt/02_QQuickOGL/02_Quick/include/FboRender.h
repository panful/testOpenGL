#pragma once

#include <QOpenGLFunctions_3_3_Core>
#include <QQuickFramebufferObject>

class FboRender :
    public QObject,
    public QQuickFramebufferObject::Renderer,
    protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    //static FboRender* m_instance;
    //static FboRender* getInstance()
    //{
    //    if (m_instance)
    //        return m_instance;
    //}

    FboRender(/*void* GraphicFunc*/);

    void testLeft() { m_red = 1.0f; m_green = 0.0f; }
    void testRight() { m_green = 1.0f; m_red = 0.0f; }
protected:
    void render() override;
    void synchronize(QQuickFramebufferObject* fbo) override;
    QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override;
private:
    void init();
    void doRender();
private:
    QOpenGLFunctions_3_3_Core* mGraphicFunc;
    unsigned int VAO, VBO, mProgram;

    float m_red;
    float m_green;
};

