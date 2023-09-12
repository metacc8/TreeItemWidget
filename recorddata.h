#pragma once
#include <qvariant.h>

///������չTreeItem��װ��displayRole����
class RecordData
{
public:
	RecordData();
	virtual ~RecordData();
	virtual QVariant data(int column, int role = Qt::DisplayRole) = 0;
	virtual bool setData(int column, const QVariant& value, int role = Qt::EditRole) = 0;
};

