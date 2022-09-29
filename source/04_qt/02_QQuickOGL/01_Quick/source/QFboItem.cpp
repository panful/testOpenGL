#include "QFboItem.h"
#include "FboRenderer.h"

#include <QOpenGLFunctions_3_3_Core>

QFboItem::QFboItem(QQuickItem* parent)
    :QQuickFramebufferObject(parent)
{
    setMirrorVertically(true);  //上下翻转坐标
}

QQuickFramebufferObject::Renderer* QFboItem::createRenderer() const
{
    return new FboRenderer();
}
