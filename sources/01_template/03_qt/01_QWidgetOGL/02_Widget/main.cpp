#include <QApplication>
#include "MyOglWidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MyOglWidget w;
    w.show();

    return app.exec();
}
