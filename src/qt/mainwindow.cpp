#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include "mainwindow.h"

namespace ui {
	EmulatorWindow::EmulatorWindow(QWidget * parent) 
		: QMainWindow(parent)
		, key_event_mapper(new KeyEventMapper(this))
		, screen_widget(new ScreenWidget(160, 144, this))
		, paused(false)
		, debugger(nullptr)
	{
		// Create window components
		createActions();
		createMenu();
		createToolbar();

		// Show the screen widget
		setCentralWidget(screen_widget);

		// Handle key presses
		installEventFilter(key_event_mapper);

		// Create a new io provider
		io_provider = std::make_shared<QtIoProvider>(screen_widget, key_event_mapper);

		// Listen for destruction of this window
		connect(this, SIGNAL(destroyed()), this, SLOT(onDestroyed()));
	}

	/// <summary>
	/// Window setup
	/// </summary>
	void EmulatorWindow::init(const std::string & wintitle)
	{
		resize(640, 480);
		setWindowTitle(tr(wintitle.c_str()));
		show();
	}

	/// <summary>
	/// Creates actions that can be used by the menu and/or toolbar.
	/// Has to be called <strong>before</strong> <code>create_menu</code> or <code>create_toolbar</code>
	/// </summary>
	void EmulatorWindow::createActions()
	{
		exit_action = new QAction(tr("&Exit"), this);
		exit_action->setIcon(QPixmap("icons/exit.png"));
		connect(exit_action, &QAction::triggered, this, &QWidget::close);

		open_action = new QAction(tr("&Open"), this);
		open_action->setIcon(QPixmap("icons/open.png"));
		connect(open_action, &QAction::triggered, this, &EmulatorWindow::openCart);

		pause_action = new QAction(tr("&Play/Pause"), this);
		pause_action->setDisabled(true);
		pause_action->setIcon(QPixmap("icons/play.png"));
		connect(pause_action, &QAction::triggered, this, &EmulatorWindow::togglePaused);
	}

	void EmulatorWindow::createMenu()
	{
		file_menu = menuBar()->addMenu(tr("&File"));
		file_menu->addAction(open_action);
		file_menu->addSeparator();
		file_menu->addAction(exit_action);
	}

	void EmulatorWindow::createToolbar()
	{
		toolbar = addToolBar(tr("main toolbar"));
		toolbar->addAction(open_action);
		toolbar->addSeparator();
		toolbar->addAction(pause_action);
	}

	bool EmulatorWindow::isClosed()
	{
		return !isVisible();
	}

	/// <summary>
	/// Open a cartridge and create a new emulation
	/// </summary>
	void EmulatorWindow::openCart()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open Rom"), "", tr("ROM Files (*.gbc *.gb)"));
		if (!file_name.isEmpty() && !file_name.isNull()) {
			emulator = std::make_shared<emu::Emulator>(io_provider, file_name.toStdString().c_str());
			emulator->initialize();
			// resume();
			pause();

			debugger = new VDebug(emulator, qobject_cast<QMainWindow *>(this));
		}
	}

	/// <summary>
	/// Toggles the play/pause state
	/// </summary>
	void EmulatorWindow::togglePaused()
	{
		if (paused)
		{
			resume();
		}
		else
		{
			pause();
		}
	}

	/// <summary>
	/// Resumes the (paused) emulator
	/// </summary>
	void EmulatorWindow::resume()
	{
		pause_action->setDisabled(false);
		pause_action->setIcon(QPixmap("icons/pause.png"));
		pause_action->setText(tr("Pause"));
		paused = false;
	}

	/// <summary>
	/// Pauses the emulator
	/// </summary>
	void EmulatorWindow::pause()
	{
		pause_action->setDisabled(false);
		pause_action->setIcon(QPixmap("icons/play.png"));
		pause_action->setText(tr("Resume"));
		paused = true;
	}

	/// <summary>
	/// Method that advances the emulation by one tick
	/// </summary>
	void EmulatorWindow::tick()
	{
		if (emulator && !paused)
		{
			emulator->tick();
		}
	}

	/// <summary>
	/// Slot that is called when the window is about to be destroyed
	/// </summary>
	void EmulatorWindow::onDestroyed()
	{
		// The io provider might still be used by the emulation. But since qt (i.e. this window) is in control
		// of the resources the io_provider uses, after destruction the resources would no longer be available.
		// So this way, we notify the io provider of its childrens destruction
		io_provider->destroy();
	}
}