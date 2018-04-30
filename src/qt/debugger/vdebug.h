#ifndef __vdebug_h
#define __vdebug_h

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <memory>
#include "../../emulator.h"
#include "vcpustatusview.h"
#include "vmemorymodel.h"
#include "vmemoryview.h"

namespace ui {
	class VDebug : public QMainWindow
	{
		Q_OBJECT
	private:

		std::shared_ptr<emu::Emulator> emulator;

		QAction * stepAction;

		QAction * syncMemAction;

		QToolBar * debugControls;

		VCPUStatusView * cpuStatusView;
		VMemoryModel * memoryModel;
		VMemoryView * memoryView;

		void createActions();
		void createToolbar();

		void tickEmulator();
	public:

		VDebug(std::shared_ptr<emu::Emulator> & emulator, QMainWindow * parent = nullptr);
	};
}

#endif