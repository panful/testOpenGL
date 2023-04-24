#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "QFboItem.h"

int main(int argc, char* argv[])
{
    // 窗口加一个按钮之后就不会闪烁，原因未知

    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    //QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    QGuiApplication app(argc, argv);

    //把item类型注册到qml中<Cpp类型>(qml模块名,主版本,次版本,qml类型)
    qmlRegisterType<QFboItem>("LearnOpenGL", 1, 0, "MyRenderer"); //模块名和qml中的import对应

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("01_03_02_01.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
            {
                qDebug() << "error";
                QCoreApplication::exit(-1);
            }
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
