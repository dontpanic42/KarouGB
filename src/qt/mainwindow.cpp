#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QFileDialog>
#include "mainwindow.h"

namespace ui {
	EmulatorWindow::EmulatorWindow(QWidget *parent) 
	: QMainWindow(parent)
	{
		create_actions();
		create_menu();
		create_toolbar();

		screenWidget = new ScreenWidget(160, 144, this);
		setCentralWidget(screenWidget);
	}

	void EmulatorWindow::init(const std::string & wintitle)
	{
		resize(640, 480);
		setWindowTitle(tr(wintitle.c_str()));
		show();
	}

	void EmulatorWindow::create_actions()
	{
		exit_action = new QAction(tr("&Exit"), this);
		connect(exit_action, &QAction::triggered, this, &QWidget::close);

		open_action = new QAction(tr("&Open"), this);
		open_action->setIcon(QPixmap("icons/icn_play.png"));
		connect(open_action, &QAction::triggered, this, &EmulatorWindow::open_cart);
	}

	void EmulatorWindow::create_menu()
	{
		file_menu = menuBar()->addMenu(tr("&File"));
		file_menu->addAction(open_action);
		file_menu->addSeparator();
		file_menu->addAction(exit_action);
	}

	void EmulatorWindow::create_toolbar()
	{
		toolbar = addToolBar(tr("main toolbar"));
		toolbar->addAction(open_action);
	}

	bool EmulatorWindow::isClosed()
	{
		return !isVisible();
	}

	void EmulatorWindow::open_cart()
	{
		QString file_name = QFileDialog::getOpenFileName(this, tr("Open Rom"), "", tr("ROM Files (*.cgb *.gb)"));
		emulator = std::move(std::make_unique<emu::Emulator>(std::shared_ptr<IOProvider>(this), file_name.toStdString().c_str()));
		emulator->initialize();
	}

	void EmulatorWindow::tick()
	{
		if (emulator)
		{
			emulator->tick();
		}
	}

	void EmulatorWindow::handleError(const std::exception & exception)
	{
		QMessageBox msgBox;
		msgBox.setText("An error occured.");
		msgBox.setInformativeText("An error occured during emulation");
		msgBox.setDetailedText(exception.what());
		msgBox.setIcon(QMessageBox::Icon::Critical);
		msgBox.show();
	}

	void EmulatorWindow::draw(u08i x, u08i y, u08i r, u08i g, u08i b)
	{
		screenWidget->draw(x, y, r, g, b);
	}

	void EmulatorWindow::display()
	{
		screenWidget->flip_buffers();
	}

	void EmulatorWindow::poll()
	{
		// TBD (?)
	}

	void EmulatorWindow::registerButtonCallback(Button btn, on_press_t onPress, on_release_t onRelease)
	{
		// TBD
	}
}