#include "vcpustatusview.h"
#include <QFormLayout>
#include <QLabel>
#include <QHBoxLayout>
#include "vtools.h"

namespace ui
{
	VCPUStatusView::VCPUStatusView(std::shared_ptr<emu::Emulator> & emulator, QWidget * parent)
		: QWidget(parent)
		, emulator(emulator)
	{
		regA = new QLabel();
		regB = new QLabel();
		regC = new QLabel();
		regD = new QLabel();
		regE = new QLabel();
		regH = new QLabel();
		regL = new QLabel();
		regSP = new QLabel();
		regPC = new QLabel();
		flags = new QLabel();

		interruptEnable = new QLabel();
		interruptFlag = new QLabel();
		haltFlag = new QLabel();
		imeFlag = new QLabel();

		QHBoxLayout * hbox = new QHBoxLayout;

		QFormLayout * layout = new QFormLayout;
		layout->addRow(new QLabel(tr("IRE")), interruptEnable);
		layout->addRow(new QLabel(tr("IRF")), interruptFlag);
		layout->addRow(new QLabel(tr("IME")), imeFlag);
		layout->addRow(new QLabel(tr("HLT")), haltFlag);
		hbox->addLayout(layout);

		layout = new QFormLayout;
		layout->addRow(new QLabel(tr("PC")), regPC);
		layout->addRow(new QLabel(tr("SP")), regSP);
		layout->addRow(new QLabel(tr("AF")), flags);
		hbox->addLayout(layout);

		layout = new QFormLayout;
		layout->addRow(new QLabel(tr("A")), regA);
		layout->addRow(new QLabel(tr("B")), regB);
		layout->addRow(new QLabel(tr("C")), regC);
		layout->addRow(new QLabel(tr("D")), regD);
		hbox->addLayout(layout);

		layout = new QFormLayout;
		layout->addRow(new QLabel(tr("E")), regE);
		layout->addRow(new QLabel(tr("H")), regH);
		layout->addRow(new QLabel(tr("L")), regL);
		hbox->addLayout(layout);

		setLayout(hbox);
		update();
	}

	void VCPUStatusView::update()
	{
		regA->setText(VTools::toHex(emulator->cpuContext->A));
		regB->setText(VTools::toHex(emulator->cpuContext->B));
		regC->setText(VTools::toHex(emulator->cpuContext->C));
		regD->setText(VTools::toHex(emulator->cpuContext->D));

		regE->setText(VTools::toHex(emulator->cpuContext->E));
		regH->setText(VTools::toHex(emulator->cpuContext->H));
		regL->setText(VTools::toHex(emulator->cpuContext->L));

		regPC->setText(VTools::toHex(emulator->cpuContext->PC));
		regSP->setText(VTools::toHex(emulator->cpuContext->SP));

		interruptEnable->setText(VTools::toBin(emulator->cpu->reg_ie));
		interruptFlag->setText(VTools::toBin(emulator->cpu->reg_if));
		imeFlag->setText(QString::number(emulator->cpuContext->ime));
		haltFlag->setText(QString::number(emulator->cpuContext->halt));
		flags->setText(VTools::toFlags(emulator->cpuContext->AF));
	}
}