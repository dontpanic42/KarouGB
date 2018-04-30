#ifndef __vmemoryview_h
#define __vmemoryview_h

#include <QTableView>
#include "../../types.h"

namespace ui 
{
	class VMemoryView : public QTableView
	{
	public:
		VMemoryView(QWidget * parent = nullptr);
		void select(u16i addr);
	};
}

#endif __vmemoryview_h //defined(__vmemoryview_h)