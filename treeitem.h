#ifndef __TREEITEM_H__
#define __TREEITEM_H__

#include <QList>
#include <QVariant>
#include <QVector>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qabstractitemmodel.h>
#include "treemodel.h"
#include "rcnamespace.h"
#include "recorddata.h"

/////////////TreeItemData////////////////
class TreeItemData
{
public:
	QMap<int, QVariant> dataMap;	///QMap<Role,Value>
	inline TreeItemData() : dataMap(QMap<int, QVariant>()) {}
	inline TreeItemData(const TreeItemData& other)
	{
		dataMap = other.dataMap;
	}
	inline bool operator==(const TreeItemData& other) const	///�����ô��������Ƚ�
	{
		return dataMap == other.dataMap;
	}
	inline void setData(QVariant data, int role)
	{
		dataMap[role] = data;
	}
	inline QVariant data(int role) const
	{
		return dataMap.value(role);
	}
	inline void eraseData(int role)
	{
		QMap<int, QVariant>::iterator it = dataMap.find(role);
		if (it != dataMap.end())
		{
			dataMap.erase(it);
		}
	}
};

/////////////TreeItem//////////////////
class TreeItem
{
public:
	TreeItem();
	TreeItem(TreeItem* parent);
	TreeItem(RecordData* record);
private:
	TreeItem(TreeItem* parent, RecordData* record);
public:
	explicit TreeItem(const QVector<QVariant>& data, TreeItem* parent = 0, int role = Qt::DisplayRole);
	virtual ~TreeItem();

	TreeItem* child(int number);
	int childCount() const;
	int rowCount() const;
	int columnCount() const;
	QVariant data(int column, int role = Qt::DisplayRole) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	TreeItem* parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const; // index in parent->childItems
	bool setData(int column, const QVariant& value, int role = Qt::DisplayRole);
	bool addChild(TreeItem* child);

	static bool isVariantLessThen(const QVariant& left, const QVariant& right);
	static bool isVariantEqualTo(const QVariant& left, const QVariant& right);
	static bool isVariantGreaterThan(const QVariant& left, const QVariant& right);
	QVector<TreeItem*> fatChildItems(); // ��ȡ�ֵܽڵ㣬�����Լ�
	QVector<TreeItem*> broOtherItems(); // ��ȡ�ֵܽڵ㣬�������Լ�
	QVector<TreeItem*> childrenItems(); //��ȡ�ӽڵ��б�����
	TreeItem* takeChildAt(int position); //�Ƴ�ָ��λ�õ��ӽڵ�
	void appendChild(TreeItem* child); // ��ĩβ�����ӽڵ�
	void prependChild(TreeItem* child); //��ͷ�������ӽڵ�
	void insertChildAt(int position, TreeItem* child); // ��ָ��λ�ò����ӽڵ�
	QVector<TreeItem*> takeChildAll(); /*�Ƴ������ӽڵ�*/
	void setParentItem(TreeItem* parent);/*���ø��׽ڵ�*/
	void appendChildList(const QVector<TreeItem*>& childList); /*��ĩβ���������ӽڵ�*/
	void setChildren(const QVector<TreeItem*>& itemVector);
	int levelOfItem();
	void setItemLevel(int level);
	void detachChildren();	///��󣨲����٣��ӽڵ㣬���ӽڵ�ĸ��ڵ㲢û������
	///���ò�����ʽ
	void setForeground(int col, Qt::GlobalColor clr);
	void setForeground(int col, unsigned long colorVal);
	void setForeground(int col, int r, int g, int b, int a);
	QVariant foreground(int col);
	void setBackground(int col, Qt::GlobalColor clr);
	void setBackground(int col, unsigned long colorVal);
	void setBackground(int col, int r, int g, int b, int a);
	QVariant background(int col);
	void setTextAlign(int col, Rcspace::RcTextAlignment aenum);
	QVariant textAlign(int col);
	void setIcon(int col, Rcspace::RcIconEnum ienum);
	QVariant cellIcon(int col);
	///��ڵ�����
	void setVirtual(bool bVirtual);
	bool isVirtualItem();
	///check state settings
	void setCheckable(int col, bool check);
	void setChecked(int col, bool check);
	void setCheckState(int col, Qt::CheckState state);
	bool isCheckable(int col);
	bool isChecked(int col);
	///erase data roles
	void eraseData(int col, int role);
	void switchChild(int m, int n);
private:
	///recordData��أ�ֻ����Ԫ����
	void setRecordData(RecordData* record);
	RecordData* innerRecordData();
private:
	QVector<TreeItem*> childItems;	///ʹ��QVector
	QVector<TreeItemData> itemData;
	RecordData* recordData = nullptr;///������ʾdisplayRole
	TreeItem* parentItem;
	int itemLevel;	///ֻ��Ŀ¼�ڵ�ʹ�ã���������
	bool isVirtual = false;	///�Ƿ���ڵ㣨����ʱ���ɵ�Ŀ¼�ڵ㣩������ʱ����Ŀ¼�ڵ���ֶ�����Ϊtrue

	friend class TreeModel;
};

#endif // __TREEITEM_H__
