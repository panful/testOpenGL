#include "QFboItem.h"
#include "FboRenderer.h"
#include <QDebug>

QFboItem::QFboItem(QQuickItem* parent)
    :QQuickFramebufferObject(parent)
{
    qDebug() << "QQuick Framebuffer Object";

    setAcceptedMouseButtons(Qt::MouseButton::AllButtons);
    setTextureFollowsItemSize(true);
    setFocus(true);
    setMirrorVertically(true);  //上下翻转坐标
}

QQuickFramebufferObject::Renderer* QFboItem::createRenderer() const
{
    qDebug() << "createRenderer()";

    return new FboRenderer();
}
