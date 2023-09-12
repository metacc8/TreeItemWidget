#ifndef __TREEMODEL_H__
#define __TREEMODEL_H__

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <qapplication.h>
#include "stdafx.h"
#include <qfont.h>
#include "rcnamespace.h"
#include <qheaderview.h>
#include "modelrecord.h"

#define EditEnableRole	64	////�Զ���Role�����ý�ɫ��������Ϊ0ʱ�����ε�Ԫ��Ŀɱ༭���ԣ���ʹ��Ԫ��������Ϊ�ɱ༭
template <class T>
class NodeType
{
public:
	NodeType(T val) :value(val) {}
	~NodeType() {
		qDeleteAll(childNodes);
		childNodes.clear();
	}

public:
	T value;
	QVector<NodeType*> childNodes;
};

typedef QList<QPair<int, Qt::SortOrder>> MultiColumnOrder;
class TreeItem;

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TreeModel(QObject* parent = 0);
	TreeModel(TreeItem* rItem, QObject* parent = 0);
    TreeModel(LPCSTR  columns[], QObject* parent = 0);
	TreeModel(QByteArray columns[], int cols, QObject* parent = 0);
	virtual ~TreeModel();

	// override 
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	void sort(int column, Qt::SortOrder order) override;
	bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
	bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
	bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;
	bool setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles) override;
	/* sort feature support */
	int sortColumnNum();
	MultiColumnOrder sortMultiColumns();
	bool isMultiColumnSort();
	void setSortAbled(bool enable);
	virtual void resetModelData(TreeItem* rItem); // ����ģ������
	void addGroupColumn(int column); //���Ӱ��ֶη��飬ֻ�ܰ�������ֶ�˳�����η���
	void delGroupColumn(int column); //ɾ�����ֶη��飬ɾ��˳����Ҫ������ӵ�˳��
	void doGroupByColumns(); /*����groupColumnsִ�з��飬���ܻ��õ�*/
	void doGroupByColumns(const QVector<int>& colVec); /*����groupColumnsִ�з��飬���ܻ��õ�*/
	void doSortByColumns();/*����orderColumnsִ�����򣬿��ܻ��õ�*/
	void doSortByColumns(MultiColumnOrder sortOrders);/*����orderColumnsִ�����򣬿��ܻ��õ�*/
	///����record����һ��ֵ��varƥ�䣬���ص�һ��ƥ���index
	QModelIndex findRecordMatchVal(const QVariant& var);
	///����findRecord������������
	QModelIndex findRecordMatchValByColumn(const QVariant& var, int column);
	TreeItem* returnRootItem();
	void resetModelDataWithItemList(QVector<TreeItem*>& itemList);
	void appendRows(const QVector<TreeItem*>& itemList);
	QVector<int> getGroupColumns();
	void resetGroupColumns(QVector<int> cols);
	///������ز�������ӿ�
	TreeItem* getItemAt(int i);
	int itemCounts();
	void appendItem(TreeItem* item);///itemֻ��ӵ�itemVector������ͬ����rootItem��
	void appendItemSync(TreeItem* item);///itemͬ����ӵ�itemVector��rootItem�£�������view�������޷���ĳ���������ͼ�´���ʱ�����ֶ�����updateIndex
	void truncateItems(int count);
	void updateIndex();	///�������е����򡢷����������
	void removeAllItems();
	void removeItemAt(int row);////ֻɾ��itemVector�µĽڵ�
	void removeItemSyncAt(int row);////ͬ��ɾ��itemVector��rootItem�µĽڵ�
	////������ʾ��������
	///����������
	void setTextAlignment(int column, Rcspace::RcTextAlignment aenum);
	void setBackground(int column, unsigned long colorVal);
	void setForeground(int column, unsigned long colorVal);
	void setIcon(int column, Rcspace::RcIconEnum ienum);
	///ȫ����������
	void setFont(QFont font);
	void setTextAlignment(Rcspace::RcTextAlignment aenum);
	void setBackground(unsigned long colorVal);
	void setForeground(unsigned long colorVal);
	void setIcon(Rcspace::RcIconEnum ienum);
	///editable
	void setEditable(bool edit);
	bool isEditable();
	///drag&drop
	void setDragAndDropAble(bool able);
	bool isDragAndDropAble();
	Qt::DropActions supportedDragActions() const override;
	Qt::DropActions	supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indexes) const override;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
	////checkable
	void setCheckable(bool check);
	bool isCheckable();
	////isItemSelectable�����������ã�Ŀǰֻ��AutoComboBox�����õ�������֧��model���𣬲�֧�ֵ�Ԫ�񼶱�
	void setItemIsSelectable(bool selectable);
	bool isItemIsSelectable();
	///��ȡitem�������б��е�λ��
	int getItemIndex(TreeItem* pitem);
	QModelIndex index(int row, int column, TreeItem* item);
	///����ĳ���Ƿ�ɱ༭
	void setColumnEditable(int col, bool edit);
	void setAllColumnEditable(bool edit);
	bool getColumnEditable(int col);
	////����item,��У��position����Ч��
	void switchItems(int posLeft, int posRight, TreeItem* parent);
	///��������
	void setAttributeGlobal(QVariant var, int role);
	void setAttributeColumn(QVariant var, int column, int role);
	void removeAttributeColumn(int column, int role);
	///
	void setHeaderView(QHeaderView* h);
	QHeaderView* getHeaderView();
	bool reloadData();
	///Ԥ����ռ�
	void reserveItemSpace(int capacity);
	void setModelRecord(ModelRecord* record);
	///treeNode�ṩ�������νṹ��ͼ��model����������///treeNode�൱��model��rootItem���������صĸ��ڵ�
	void setModelRecord(ModelRecord* record, NodeType<int>* treeNode);
private:
	TreeItem* buildItemWithModel(TreeItem* pitem, NodeType<int>* node);
protected:
	void setHeader(TreeItem* root);
private:
	TreeItem* getItem(const QModelIndex& index) const;
	void mergeSameGroupedItems(TreeItem* item);
	void sortPrivate(int column, Qt::SortOrder order);
	void setDefaultAttrOfModel();///����modelȫ��Ĭ������
	void removeItem(TreeItem* item);
	void setCheckStateAscend(const QModelIndex& index);
	bool setDataUnaffectParentCheckState(const QModelIndex& index, const QVariant& value, int role);
	void sortChildrenItems(TreeItem* parent, int column, Qt::SortOrder order);
	bool itemLessThan(TreeItem* left, TreeItem* right);
	bool itemGreaterThan(TreeItem* left, TreeItem* right);
	bool directItemLessThan(TreeItem* left, TreeItem* right);
	bool directItemGreaterThan(TreeItem* left, TreeItem* right);
	bool multiColEqual(TreeItem* left, TreeItem* right, int cmpSize);
	QModelIndex findRecordByColumn(TreeItem* parent, const QVariant& var, int column);
	QModelIndex findRecordNoColumn(TreeItem* parent, const QVariant& var);
signals:
	void dataAltered(TreeItem* pitem, int column, const QVector<int>& roles = QVector<int>());	///���������õ�dataChanged�ź�����
private:
	QVector<int> editableColVec;	///����༭����
	bool editable = false;		///�Ƿ�ɱ༭�ܿ���,��Ԫ���Ƿ�ɱ༭����editable��editableColVecͬʱ������ editable==true && editableColVec.indexOf(col) > -1 ʱ�ɱ༭
	bool dragndropable = false;	///drag & drop
	bool checkable = false;	///checkable
	bool itemIsSelectable = true;///Qt::ItemIsSelectable
	bool ableMultiColumnSort = false; // bool ���ͳ�ʼ����Ҫ�ֶ�����ֵ
	TreeItem* rootItem = nullptr;	///index root item, for display 
	QVector<TreeItem*> itemVector;	///�洢���ݽڵ㣬��������ͷ���ʱֻ�ؽ�Ŀ¼�ڵ㣬���ݽڵ㱣��
	QVector<TreeItem*> itemToDelVector;	///��ɾ�������ݽڵ�
	QVector<TreeItem*> itemHideVector;///�ݲ���
	int sortColumn = -1;
	MultiColumnOrder orderColumns; // support multicolumn sort
	QVector<int> groupColumns;

	///���ڵ�Ԫ�����ԣ����ΰ��� ��Ԫ��->��->modelȫ�� ��˳������ȡ
	///�����ȡ����ǰ���������ΪinValid����������˳���ȡ��һ����ֵ
	QMap<int, QVariant> attrOfModel;	///moldeȫ������
	QVector<QMap<int, QVariant>> attrOfColumns;	///�м�������
	///�洢��ͷ
	QHeaderView* head = nullptr;
	///���ټ���
	ModelRecord* modelRecord = nullptr;
	ModelRecord* oldModelRecord = nullptr;///�滻modelRecord��Ϊ�ݴ�����滻��modelRecord��������ɾ����updateIndex����ɾ��

	friend class TreeItem;
	friend class HeaderView;
};

#endif
