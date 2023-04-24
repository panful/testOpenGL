#include "ViewerItem.h"
#include "FboRender.h"

#include <QOpenGLFunctions_3_3_Core>

ViewerItem::ViewerItem(QQuickItem* parent)
    :QQuickFramebufferObject(parent),
    m_fbo(nullptr)
{
    setMirrorVertically(true);  //上下翻转坐标
    //setFocus(true);
    setAcceptedMouseButtons(Qt::MouseButton::AllButtons);  //设置接收所有鼠标按键事件，如果不设置，鼠标回调函数不会响应
}

QQuickFramebufferObject::Renderer* ViewerItem::createRenderer() const
{
    //m_fbo = FboRender::getInstance();
    m_fbo = new FboRender(/*reinterpret_cast<void*>(new QOpenGLFunctions_3_3_Core)*/);

    //connect(this, &ViewerItem::sigMouseEvent, m_fbo, &FboRender::testLeft);

    return m_fbo;
}

void ViewerItem::testFunc()
{
    update();

    m_fbo->testLeft();
    m_fbo->testRight();
}

void ViewerItem::mousePressEvent(QMouseEvent* event)
{
    update();

    if (event->button() == Qt::LeftButton)
    {
        m_fbo->testLeft();
    }

    if (event->button() == Qt::RightButton)
    {
        m_fbo->testRight();
    }
    
}