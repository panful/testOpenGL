
#include <QGuiApplication>

#include <QtQuick/QQuickView>

#include "fboinsgrenderer.h"

// Qt官方示例

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGLRhi);

    qmlRegisterType<FboInSGRenderer>("SceneGraphRendering", 1, 0, "MyRenderer"); //模块名和qml中的import对应

    QQuickView view;
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("01_03_02_03.qml"));
    view.show();

    return app.exec();
}
