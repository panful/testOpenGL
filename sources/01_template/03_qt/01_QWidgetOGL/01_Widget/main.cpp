#include <QApplication>
//#include <QWidget>
#include "GLWidget.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	GLWidget w;
	w.show();

	//QWidget* w = new QWidget();
	//w->show();

	app.exec();
}