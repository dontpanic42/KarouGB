#ifndef __mainwindow_h
#define __mainwindow_h

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <memory>
#include "screenwidget.h"
#include "keyeventmapper.h"
#include "qtioprovider.h"
#include "../io/io_provider.h"
#include "../types.h"
#include "../emulator.h"

namespace ui {

	class EmulatorWindow 
		: public QMainWindow
	{

		Q_OBJECT

	private:
		QMenu * file_menu;
		QToolBar * toolbar;
		QAction * exit_action;
		QAction * open_action;
		QAction * pause_action;

		ScreenWidget * screen_widget;
		KeyEventMapper * key_event_mapper;

		std::unique_ptr<emu::Emulator> emulator = nullptr;
		std::shared_ptr<QtIoProvider> io_provider = nullptr;

		bool paused;

		void createActions();
		void createMenu();
		void createToolbar();
		void openCart();
		void togglePaused();
		void resume();
		void pause();

	public:
		explicit EmulatorWindow(QWidget * parent = nullptr);

		void init(const std::string & wintitle);
		bool isClosed();

		void tick();
	public slots:
		void onDestroyed();
	};

}
#endif //defined(__mainwindow_h)