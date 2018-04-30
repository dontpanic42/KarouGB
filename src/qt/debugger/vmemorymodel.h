#ifndef __vmemorymodel_cpp
#define __vmemorymodel_cpp

#include <QAbstractTableModel>
#include <memory>
#include "../../emulator.h"


namespace ui
{
	class VMemoryModel : public QAbstractTableModel
	{
	private:
		std::shared_ptr<emu::Emulator> emulator;

		enum TableColumn
		{
			TableColumnValue = 0,
			TableColumnTranslation = 1,
			TableColumnLast = 2
		};
	public:
		VMemoryModel(std::shared_ptr<emu::Emulator> & emulator);

		int rowCount(const QModelIndex & parent) const;
		int columnCount(const QModelIndex & parent) const;
		QVariant data(const QModelIndex & index, int role) const;

		void update();
	};
}

#endif //defined(__cmemorymodel_cpp)