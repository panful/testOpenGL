#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include  "ViewerItem.h"

int main(int argc, char* argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);  //缩放窗口不闪烁，使用了opengl调用这个参数程序会崩，暂时不知道原因
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QGuiApplication app(argc, argv);

    //把item类型注册到qml中<Cpp类型>(qml模块名,主版本,次版本,qml类型)
    qmlRegisterType<ViewerItem>("LearnOpenGL", 1, 0, "ViewerItem"); //模块名和qml中的import对应
    qmlRegisterSingletonType<ViewerItem>("LearnOpenGL", 1, 0, "Item", [](QQmlEngine*, QJSEngine*)->QObject* {return new ViewerItem(); });

    QQmlApplicationEngine engine;

    //engine.rootContext()->setContextProperty("Item", new ViewerItem());

    const QUrl url(QStringLiteral("01_03_02_02.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
