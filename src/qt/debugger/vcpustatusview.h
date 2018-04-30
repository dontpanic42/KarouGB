#ifndef __vcpustatusview_h
#define __vcpustatusview_h

#include <QWidget>
#include <QLabel>
#include <memory>
#include "../../emulator.h"

namespace ui
{
	class VCPUStatusView : public QWidget
	{
	private:
		std::shared_ptr<emu::Emulator> emulator;

		QLabel * regA;
		QLabel * regB;
		QLabel * regC;
		QLabel * regD;
		QLabel * regE;
		QLabel * regH;
		QLabel * regL;

		QLabel * regPC;
		QLabel * regSP;

		QLabel * interruptEnable;
		QLabel * interruptFlag;
		QLabel * haltFlag;
		QLabel * imeFlag;
	public:
		VCPUStatusView(std::shared_ptr<emu::Emulator> & emulator, QWidget * parent = nullptr);

		void update();
	};
}

#endif //defined(__vcpustatusview_h)