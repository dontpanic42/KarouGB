#include "vmemoryview.h"
#include <QHeaderView>

namespace ui
{
	VMemoryView::VMemoryView(QWidget * parent)
		: QTableView(parent)
	{
		setSelectionBehavior(QTableView::SelectRows);
		horizontalHeader()->setStretchLastSection(true);

		verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		verticalHeader()->setDefaultSectionSize(20);
	}

	/// <summary>
	/// Select the given address in the memory view
	/// </summary>
	void VMemoryView::select(u16i addr)
	{
		QModelIndex index = model()->index(addr, 0);
		setCurrentIndex(index);
		scrollTo(index);
	}
}