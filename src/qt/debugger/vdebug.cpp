#include "vdebug.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include "vmemoryview.h"
#include "vtools.h"

namespace ui
{
	VDebug::VDebug(std::shared_ptr<emu::Emulator> & emulator, QMainWindow * parent)
		: QMainWindow(parent)
		, emulator(emulator)
	{

		memoryView = new VMemoryView();
		memoryModel = new VMemoryModel(emulator);
		memoryView->setModel(memoryModel);

		QVBoxLayout * mainLayout = new QVBoxLayout;
		mainLayout->addWidget(memoryView);

		cpuStatusView = new VCPUStatusView(emulator);
		mainLayout->addWidget(cpuStatusView);

		QWidget * qwidget = new QWidget;
		qwidget->setLayout(mainLayout);

		setCentralWidget(qwidget);

		createActions();
		createToolbar();

		setWindowTitle(tr("Debugger"));
		resize(640, 480);
		show();
	}

	void VDebug::createToolbar()
	{
		debugControls = addToolBar(tr("debugger toolbar"));
		debugControls->addAction(stepAction);
		debugControls->addSeparator();
		debugControls->addAction(syncMemAction);
		debugControls->addAction(goToPCAction);
		debugControls->addAction(goToAddressAction);
	}

	void VDebug::createActions()
	{
		stepAction = new QAction(tr("&Step"), this);
		stepAction->setIcon(QPixmap("icons/step.png"));
		connect(stepAction, &QAction::triggered, this, &VDebug::tickEmulator);

		syncMemAction = new QAction(tr("&Sync"), this);
		syncMemAction->setIcon(QPixmap("icons/sync.png"));
		syncMemAction->setCheckable(true);
		syncMemAction->setChecked(true);

		goToAddressAction = new QAction(tr("&GoTo Memory Address"), this);
		goToAddressAction->setIcon(QPixmap("icons/goto.png"));
		connect(goToAddressAction, &QAction::triggered, this, &VDebug::goToAddress);

		goToPCAction = new QAction(tr("GoTo &PC Address"), this);
		goToPCAction->setIcon(QPixmap("icons/goto-pc.png"));
		connect(goToPCAction, &QAction::triggered, this, &VDebug::goToPC);
	}

	/// <summary>
	/// Advance the emulation one tick
	/// </summary>
	void VDebug::tickEmulator()
	{
		emulator->tick();
		cpuStatusView->update();
		memoryModel->update();

		// If memory synchronization is enabled, set the memory view to the address in PC
		if (syncMemAction->isChecked())
		{
			goToPC();
		}
	}

	/// <summary>
	/// Prompts the user to input an address and shows it in the memory view
	/// </summary>
	void VDebug::goToAddress()
	{
		u16i address;
		if (VTools::getMemoryAddressInput(&address, this))
		{
			memoryView->select(address);
		}
	}

	/// <summary>
	/// Shows the address the PC points to in the memory view
	/// </summary>
	void VDebug::goToPC()
	{
		memoryView->select(emulator->cpuContext->PC);
	}
}