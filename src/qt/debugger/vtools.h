#ifndef __vtools_h
#define __vtools_h

#include <QString>
#include "../../types.h"

namespace ui
{
	class VTools
	{
	public:
		static QString toHex(u08i number);
		static QString toHex(u16i number);

		static QString toBin(u08i number);
	};
}

#endif //defined(__vtools_h)