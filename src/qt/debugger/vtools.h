#ifndef __vtools_h
#define __vtools_h

#include <QString>
#include "../../types.h"
#include <QWidget>

namespace ui
{
	class VTools
	{
	public:
		static QString toHex(u08i number);
		static QString toHex(u16i number);
		static QString toBin(u08i number);
		static QString toFlags(u08i number);

		static bool getMemoryAddressInput(u16i * result, QWidget * parent = nullptr);
	};
}

#endif //defined(__vtools_h)