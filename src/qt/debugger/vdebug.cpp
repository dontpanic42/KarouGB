#include "vdebug.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include <QActionGroup>
#include "vmemoryview.h"

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
		debugControls->addAction(syncMemAction);
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
			memoryView->select(emulator->cpuContext->PC);
		}
	}
}