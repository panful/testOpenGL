#include <QApplication>
#include "GLWidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    GLWidget w;
    w.show();

    return app.exec();
}
