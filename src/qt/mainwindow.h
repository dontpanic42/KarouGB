#ifndef __mainwindow_h
#define __mainwindow_h

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <memory>
#include "screenwidget.h"
#include "../io/io_provider.h"
#include "../types.h"
#include "../emulator.h"

namespace ui {

	class EmulatorWindow 
		: public QMainWindow
		, public emu::IOProvider 
	{

		Q_OBJECT

	private:
		QMenu * file_menu;
		QToolBar * toolbar;
		QAction * exit_action;
		QAction * open_action;

		ScreenWidget * screenWidget;

		std::unique_ptr<emu::Emulator> emulator = nullptr;

		void create_actions();
		void create_menu();
		void create_toolbar();
		void open_cart();

	public:

		explicit EmulatorWindow(QWidget * parent = nullptr);

		bool isClosed();

		void tick();

		// IOProvider stuff

		void init(const std::string & wintitle);
		void handleError(const std::exception & exception);
		void draw(u08i x, u08i y, u08i r, u08i g, u08i b);
		void display();

		void poll();
		void registerButtonCallback(Button btn, on_press_t onPress, on_release_t onRelease);
	};

}
#endif //defined(__mainwindow_h)