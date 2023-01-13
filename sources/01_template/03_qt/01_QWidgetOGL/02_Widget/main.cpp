#include <QApplication>
//#include <QWidget>
#include "MyOglWidget.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	MyOglWidget w;
	w.show();

	//QWidget* w = new QWidget();
	//w->show();

	app.exec();
}