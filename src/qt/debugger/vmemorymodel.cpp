#include "vmemorymodel.h"
#include "vtools.h"
#include "../../debug_translation.h"
#include <iostream>

namespace ui
{
	VMemoryModel::VMemoryModel(std::shared_ptr<emu::Emulator> & emulator)
		: QAbstractTableModel()
		, emulator(emulator)
	{

	}

	/// <summary>
	/// Returns the number of rows
	/// </summary>
	int VMemoryModel::rowCount(const QModelIndex & parent) const
	{
		return emulator->mmu->MEMORY_SIZE;
	}

	/// <summary>
	/// Returns the number of columns
	/// </summary>
	int VMemoryModel::columnCount(const QModelIndex & parent) const
	{
		return TableColumnLast;
	}

	/// <summary>
	/// Returns data for the given row/column combination
	/// </summary>
	QVariant VMemoryModel::data(const QModelIndex & index, int role) const
	{
		if (role == Qt::DisplayRole)
		{
			switch(index.column())
			{
				case TableColumnValue:
				{
					return QString("0x") + VTools::toHex(emulator->mmu->rb(index.row()));
				}

				case TableColumnTranslation:
				{
					u08i current = emulator->mmu->rb(index.row());
					if (current == 0xCB)
					{
						return QString("");
					}

					if (index.row() > 0 && emulator->mmu->rb(index.row() - 1) == 0xCB)
					{
						return emu::op_translation_CB[current];
					}

					return emu::op_translation_00[current];
				}

				default:
				{
					return QVariant::Invalid;
				}
			}
			
		}

		return QVariant::Invalid;
	}

	/// <summary>
	/// Updates all data
	/// </summary>
	void VMemoryModel::update()
	{
		emit dataChanged(QModelIndex(), QModelIndex());
	}
}