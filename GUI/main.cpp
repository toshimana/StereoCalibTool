#include "StereoCalibToolGUI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	StereoCalibToolGUI w;
	w.show();
	return a.exec();
}
