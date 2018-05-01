#include "vtools.h"
#include "../../cpu/cpu.h"
#include <QInputDialog>
#include <QSpinBox>
#include <limits>
#include <iostream>

namespace ui
{

	QString VTools::toHex(u08i number)
	{
		return QString("%1").arg(number, 2, 16, QChar('0'));
	}

	QString VTools::toHex(u16i number)
	{
		return QString("%1").arg(number, 4, 16, QChar('0'));
	}

	QString VTools::toBin(u08i number)
	{
		return QString("%1").arg(number, 8, 2, QChar('0'));
	}

	QString VTools::toFlags(u08i number)
	{
		using namespace emu::cpu::flag;

		QString result = QString("");
		result += (number & ZERO) ? QString("Z ") : QString("- ");
		result += (number & SUBTRACT) ? QString("N ") : QString("- ");
		result += (number & HALFCARRY) ? QString("H ") : QString("- ");
		result += (number & CARRY) ? QString("C") : QString("-");
		return result;
	}

	/// <summary>
	/// Opens (and blocks) until the user enters a memory address in hex format.
	/// </summary>
	bool VTools::getMemoryAddressInput(u16i * result, QWidget * parent)
	{
		QInputDialog dialog(parent, Qt::WindowFlags());
		dialog.setWindowTitle(QObject::tr("Input memory address"));
		dialog.setLabelText(QObject::tr("Input memory address (in hex, e.g. FFC3)"));
		dialog.setIntRange(0, USHRT_MAX);
		dialog.setIntStep(1);
		dialog.setInputMode(QInputDialog::IntInput);

		QSpinBox * spinBox = dialog.findChild<QSpinBox*>();
		spinBox->setDisplayIntegerBase(16);

		bool ret = dialog.exec() == QDialog::Accepted;
		if (ret && result)
		{
			*result = (u16i) dialog.intValue();
		}

		return ret;
	}
}