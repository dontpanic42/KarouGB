#include "vtools.h"

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
}