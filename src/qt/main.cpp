#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	ui::EmulatorWindow window;
	window.init(std::string("KarouGB"));

	while (!window.isClosed())
	{
		app.processEvents();
		window.tick();
	}
}