#pragma once

#include <QQuickFramebufferObject>

class QFboItem :
    public QQuickFramebufferObject
{
    Q_OBJECT
public:
    QFboItem(QQuickItem* parent = nullptr);
    Renderer* createRenderer() const override;
};

