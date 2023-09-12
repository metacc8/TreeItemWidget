#include "treemodel.h"
#include "treeitem.h"
#include <queue>
#include <qmimedata.h>

TreeModel::TreeModel(QObject* parent)
	:QAbstractItemModel(parent)
{
	rootItem = NULL;
	setDefaultAttrOfModel();
}

TreeModel::TreeModel(TreeItem* rItem, QObject* parent)
	: QAbstractItemModel(parent)
{
	rootItem = rItem;
	attrOfColumns = QVector<QMap<int, QVariant>>(rootItem->columnCount(), QMap<int, QVariant>());
	setDefaultAttrOfModel();
}

TreeModel::TreeModel(LPCSTR  columns[], QObject* parent)
	:QAbstractItemModel(parent)
{
	QVector<QVariant> itemData;
//    for (int i = 0; columns[i] != NULL; i++)
//    {
//        itemData << STRING( columns[i]);
//    }
    itemData << STRING( "����" );itemData << STRING( "�Ա�" );itemData << STRING( "����" );itemData << STRING( "�绰" );

	rootItem = new TreeItem(itemData);
	attrOfColumns = QVector<QMap<int, QVariant>>(rootItem->columnCount(), QMap<int, QVariant>());
	setDefaultAttrOfModel();
}

TreeModel::TreeModel(QByteArray columns[], int cols, QObject* parent)
{
	QVector<QVariant> itemData;
	for (int i = 0; i < cols; i++)
	{
		itemData << STRING(columns[i]);
	}
	rootItem = new TreeItem(itemData);
	attrOfColumns = QVector<QMap<int, QVariant>>(rootItem->columnCount(), QMap<int, QVariant>());
	setDefaultAttrOfModel();
}

TreeModel::~TreeModel()
{
	if (rootItem)
	{
		delete rootItem;
	}
	itemVector.clear();///itemVector�е�Ԫ����delete rootItem������
	qDeleteAll(itemToDelVector);
	itemToDelVector.clear();
	qDeleteAll(itemHideVector);
	itemHideVector.clear();
	if (modelRecord)
	{
		delete modelRecord;
	}
	if (oldModelRecord)
	{
		delete oldModelRecord;
	}
}

QVariant TreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	QVariant d;
	TreeItem* item = getItem(index);
	if (item->isVirtualItem())///Ŀ¼�ڵ����⴦��Ŀ¼�ڵ��ֵ�����ڵ�һ�У�����index����Ϊ��ͼ�ɼ��ĵ�һ�У����򶼲���ʾ����
	{
		//if (head && head->logicalIndexAt(0) == index.column())
		//{
		//	//d = item->data(0, role);
		//}
		//if (!d.isValid() && !(d = attrOfColumns.value(index.column()).value(role)).isValid())	///�м���
		//	d = attrOfModel.value(role);	///model ȫ�ּ���
		if (role == Qt::FontRole)
		{
			d = attrOfModel.value(role);///ֻ����������Ϣ��������Ϣ��ɾ������ĳ����Ϣ�ᵼ��Ŀ¼�ڵ��޷����У�����ѡ��ʱtext����ʾ��
		}
		return d;
	}
	else
	{
		if (!(d = item->data(index.column(), role)).isValid())	///��Ԫ�񼶱�
			if (!(d = attrOfColumns.value(index.column()).value(role)).isValid())	///�м���
				d = attrOfModel.value(role);	///model ȫ�ּ���
	}

	switch (role)
	{
		//case Qt::DisplayRole:	//��ʾ���ı�
		//	return d;
	case Qt::EditRole:
		return item->data(index.column(), Qt::DisplayRole);
	case Qt::ForegroundRole:	//�ı���ɫ
	{
		quint32 color = d.toULongLong();
		return QColor((color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), (color & 0xff000000) >> 24);
	}
	case Qt::TextAlignmentRole:	//�ı�ͣ��λ��
	{
		int align = d.toInt();
		Rcspace::RcTextAlignment aenum = static_cast<Rcspace::RcTextAlignment>(align);
		return Rcspace::specialTextAlignment(aenum);
	}
	case Qt::DecorationRole:	//icon
	{
		int ienum = d.toInt();
		return Rcspace::specialIcon(static_cast<Rcspace::RcIconEnum>(ienum));
	}
	//case Qt::FontRole:	//����
	//	return d;
	case Qt::BackgroundRole:	//����ɫ
	{
		quint32 color = d.toULongLong();
		return QColor((color & 0xff0000) >> 16, (color & 0xff00) >> 8, (color & 0xff), (color & 0xff000000) >> 24);
	}
	//case Qt::CheckStateRole:	//��ѡ
	//	return d;
	default:
		return d;
	}
	return QVariant();
}


QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}


QModelIndex TreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	TreeItem* parentItem = getItem(parent);

	TreeItem* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}


QModelIndex TreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem* childItem = getItem(index);
	TreeItem* parentItem = childItem->parent();

	if (!parentItem || parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}


int TreeModel::rowCount(const QModelIndex& parent) const
{
	TreeItem* parentItem = getItem(parent);

	return parentItem->childCount();
}


int TreeModel::columnCount(const QModelIndex& parent) const
{
	return rootItem->columnCount();
}


Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags defaultFlags;
	if (!index.isValid())
		defaultFlags = 0;
	else
	{
		defaultFlags = QAbstractItemModel::flags(index);
	}
	if (editable && editableColVec.indexOf(index.column()) > -1)
	{
		defaultFlags |= Qt::ItemIsEditable;
	}
	if (dragndropable)
	{
		if (index.isValid())
		{
			defaultFlags |= Qt::ItemIsDragEnabled;
		}
		if (!index.isValid() || getItem(index)->isVirtualItem())
		{
			defaultFlags |= Qt::ItemIsDropEnabled;
		}
	}
	if (checkable)
	{
		defaultFlags |= Qt::ItemIsUserCheckable;
	}
	///���Item�����˲��ɱ༭,����EditRole
	TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
	if (item && item != rootItem && item->data(index.column(), EditEnableRole).isValid() && !item->data(index.column(), EditEnableRole).toBool())
	{
		defaultFlags &= ~Qt::ItemIsEditable;
		defaultFlags &= ~Qt::ItemIsUserCheckable;	///��userCheckableҲ��ΪEditEnableRole��һ����
	}
	if (!itemIsSelectable)
	{
		defaultFlags &= ~Qt::ItemIsSelectable;
	}
	return defaultFlags;
}


void TreeModel::sort(int column, Qt::SortOrder order)
{
	//HeadView->flipSortIndicator�����л����setSortIndicator��Ȼ��emit sortIndicatorChanged �ź�
	//TreeView ����sortIndicatorChanged�źź󣬻���ô˴���sort������
	//����HeadView->flipSortIndicator���غ󣬲Ż�emit sectionClicked �źš�
	//��headView�У�������sectionClicked�źŵĴ�����onSectionClicked�����ñ�ͷ����������ʽ��
	//�������豣��Ч��ͬ��

	if (column < 0 || column >= columnCount()) return;
	sortColumn = column;
	QPair<int, Qt::SortOrder> colSort(column, order);
	bool bHasSorted = false;
	if (QApplication::keyboardModifiers() == Qt::ShiftModifier) ///shift �����£����������������⣺���������ֶ�һ�����������
	{
		this->ableMultiColumnSort = true;
		for (auto& it : orderColumns)
		{
			if (it.first == column)
			{
				//for (int i = 0; i < orderColumns.size(); i++)
				//{
				//	if (orderColumns.at(i).first == column)
				//	{
				//		orderColumns.removeAt(i);
				//		break;
				//	}
				//}
				bHasSorted = true;
				if (it.second == order)///����˳��һ�£�ֱ������
				{
					return;
				}
				else
				{
					it.second = order;
				}
			}
		}
	}
	else
	{
		this->ableMultiColumnSort = false;
		MultiColumnOrder temp;
		temp.swap(orderColumns);
		MultiColumnOrder::iterator it, ite = temp.end();
		for (it = temp.begin(); it != ite; it++)	///�������������ֶΣ��Լ���ǰ�����ֶ�
		{
			//if (groupColumns.indexOf((*it).first) > -1 && column != (*it).first)
			//{
			//	orderColumns.append(*it);
			//}
			if (groupColumns.indexOf((*it).first) > -1 || column == (*it).first)
			{
				if (column == it->first)
				{
					bHasSorted = true;
					it->second = order;
				}
				orderColumns.append(*it);
			}
		}
	}
	//orderColumns.append(colSort);///������ζ���ӵ�ĩβ
	if (!bHasSorted)
		orderColumns.append(colSort);///δ��������������ӵ�ĩβ
	emit layoutAboutToBeChanged();
	sortChildrenItems(rootItem, column, order/*, true*/);
	emit layoutChanged();
}


bool TreeModel::insertColumns(int position, int columns, const QModelIndex& parent)
{
	bool success;
	beginInsertColumns(parent, position, position + columns - 1);
	success = rootItem->insertColumns(position, columns);
	endInsertColumns();

	///���������С������еĴ洢ֵ
	for (int i = 0; i < orderColumns.size(); i++)
	{
		QPair<int, Qt::SortOrder> sord = orderColumns.at(i);
		if (sord.first >= position)
		{
			sord.first += columns;	///position֮����к���
		}
	}
	for (int i = 0; i < groupColumns.size(); i++)
	{
		int gcol = groupColumns.at(i);
		if (gcol >= position)
		{
			gcol += columns;
		}
	}
	return success;
}


bool TreeModel::removeColumns(int position, int columns, const QModelIndex& parent)
{
	bool success;

	///ɾ���л�Ӱ�쵽ԭ��������ͷ��飬�ȴ���ɾ��������ͷ��飬Ȼ������ɾ���в���
	///�����������С������еĴ洢ֵ
	for (int i = 0; i < groupColumns.size(); i++)	///ɾ������
	{
		int gcol = groupColumns.at(i);
		if (gcol >= position && gcol < (position + columns))
		{
			delGroupColumn(gcol);
		}
	}
	MultiColumnOrder sortOrds;
	for (int i = 0; i < orderColumns.size(); i++)
	{
		QPair<int, Qt::SortOrder> odr = orderColumns.at(i);
		if (odr.first < position || odr.first >= position + columns)
		{
			sortOrds.append(odr);	///���治��ɾ����������
		}
	}
	sortOrds.swap(orderColumns);
	doSortByColumns();	///��������

	/////////////ɾ����
	beginRemoveColumns(parent, position, position + columns - 1);
	success = rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	///���������С������еĴ洢ֵ
	for (int i = 0; i < groupColumns.size(); i++)	///ɾ������
	{
		int gcol = groupColumns.at(i);
		if (gcol > position)
		{
			groupColumns[i] = gcol + columns;
		}
	}
	for (int i = 0; i < orderColumns.size(); i++)
	{
		QPair<int, Qt::SortOrder> odr = orderColumns.at(i);
		if (odr.first > position)
		{
			odr.first += columns;
		}
	}
	return success;
}


bool TreeModel::insertRows(int position, int rows, const QModelIndex& parent)
{
	TreeItem* parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}


bool TreeModel::removeRows(int position, int rows, const QModelIndex& parent)
{
	TreeItem* parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

bool TreeModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	TreeItem* pitem = static_cast<TreeItem*>(index.internalPointer());
	if (pitem == nullptr || pitem == rootItem)
	{
		return true;
	}
	bool success = pitem->setData(index.column(), value, role);
	int childCnt = pitem->childCount();
	emit dataChanged(index, index, QVector<int>(1, role));
	if (childCnt)
	{
		for (int i = 0; i < childCnt; i++)
		{
			setDataUnaffectParentCheckState(createIndex(index.row(), index.column(), pitem->child(i)), value, role);
		}
		emit dataChanged(createIndex(0, index.column(), pitem->child(0)), createIndex(childCnt - 1, index.column(), pitem->child(childCnt - 1)), QVector<int>(1, role));
	}
	if (role == Qt::CheckStateRole)	///checkstate �Ը��ڵ��Ӱ��
	{
		setCheckStateAscend(parent(index));
	}

	///���źŲ�����view��ʾ����������ظ��ڵ㡢�ӽڵ����ݸı�����ٷ��ͣ����ӽڵ���ص��ⲿ�����Ƿ�Ҫ�������ⲿ�߼�ȷ��
	emit dataAltered(pitem, index.column(), QVector<int>(1, role));
	return success;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	bool success = false;
	if (rootItem)
	{
		success = rootItem->setData(section, value);
	}
	emit headerDataChanged(Qt::Horizontal, section, section);
	return success;
}

bool TreeModel::setItemData(const QModelIndex& index, const QMap<int, QVariant>& roles)
{
	bool success = false;
	TreeItem* item = getItem(index);
	if (item && item != rootItem)
	{
		for (auto it = roles.cbegin(); it != roles.cend(); it++)
		{
			success = item->setData(index.column(), it.value(), it.key());
		}
	}
	return success;
}

TreeItem* TreeModel::getItem(const QModelIndex& index) const
{
	if (index.isValid()) {
		TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
}

int TreeModel::sortColumnNum()
{
	return sortColumn;
}

MultiColumnOrder TreeModel::sortMultiColumns()
{
	return orderColumns;
}

bool TreeModel::isMultiColumnSort()
{
	return ableMultiColumnSort;
}

void TreeModel::setSortAbled(bool enable)
{
	if (!enable)
		orderColumns.clear();
	return;
}

void TreeModel::resetModelData(TreeItem* rItem) // ����ģ������
{
	beginResetModel();
	if (rootItem && rootItem != rItem)
	{
		delete rootItem;
	}
	rootItem = rItem;
	//rootItem->setModel(this);
	endResetModel();
}

////���Ӱ��ֶη���
void TreeModel::addGroupColumn(int column) //���ӷ���
{
	if (groupColumns.indexOf(column) > -1)
	{
		return;
	}
	///
	MultiColumnOrder::iterator it, ite = orderColumns.end();
	for (it = orderColumns.begin(); it != ite; it++)
	{
		if ((*it).first == column)
			break;
	}
	if (it == ite)	///û�в�������Ĭ������Ϊ��������
	{
		sort(column, Qt::AscendingOrder);
	}

	TreeItem* p = rootItem;
	beginResetModel();	///use resetModel is safer than layoutChanged & RemoveRows
	std::queue<TreeItem*> itemQueue;
	itemQueue.push(p);
	while (!itemQueue.empty())
	{
		TreeItem* item = itemQueue.front();
		itemQueue.pop();
		if (item->childCount() > 0 && item->child(0)->isVirtualItem()) //Ŀ¼�ڵ������
		{
			for (int i = 0; i < item->childCount(); i++)
			{
				itemQueue.push(item->child(i));
			}
		}
		if (item->childCount() > 0 && !item->child(0)->isVirtualItem()) //���һ��Ŀ¼�ڵ�������
		{
			QMap<QVariant, TreeItem*> varItemMap;
			QVector<TreeItem*> allChild = item->takeChildAll();
			for (int i = 0; i < allChild.size(); i++)
			{
				TreeItem* sunItem = allChild.at(i);
				QVariant colVar = sunItem->data(column);
				if (varItemMap.contains(colVar))
				{
					varItemMap.value(colVar)->appendChild(sunItem);
				}
				else
				{
					QVector<QVariant> data(item->columnCount());
					data[0] = rootItem->data(column).toString() + ":" + sunItem->data(column).toString();//Ŀ¼��ʾ��ʽ������: ��ֵ��
					//while (data.count() < item->columnCount())
					//{
					//	data << QVariant(); // Ŀ¼�ڵ������ж���Ϊ��
					//}

					//TreeItem* nItem = new TreeItem(data, this, item);
					TreeItem* nItem = new TreeItem(data, item);
					nItem->setData(0, sunItem->data(column), Qt::UserRole);	////item[0] UserRole�洢sunItem��displayRole������ɾ������mergeʱ�Ƚ�
					nItem->setForeground(0, RGB(120, 120, 120));
					if (sunItem->isCheckable(0))
					{
						nItem->setCheckable(0, true);
						sunItem->setChecked(0, false);
					}
					nItem->setVirtual(true);	////����ʱ������Ŀ¼�ڵ㣬��������Ϊtrue
					item->appendChild(nItem);
					nItem->appendChild(sunItem);
					varItemMap.insert(colVar, nItem);
				}
			}
		}
	}
	endResetModel();
	groupColumns.append(column);
	return;
}

/////ɾ�����ֶη��飬ɾ��˳����Ҫ������ӵ�˳��
void TreeModel::delGroupColumn(int column) //ɾ��level���ڵ��µ������ӽڵ㣬ͬʱ����ڵ����ӽڵ�
{
	int level = groupColumns.indexOf(column);
	if (level < 0)
	{
		return;
	}

	TreeItem* p = rootItem;
	beginResetModel();	///use resetModel is safer than layoutChanged & RemoveRows
	std::queue<TreeItem*> itemQueue;
	itemQueue.push(p);
	while (!itemQueue.empty())
	{
		TreeItem* item = itemQueue.front();
		itemQueue.pop();
		if (item->levelOfItem() == level) // �ҵ������ڵ㣬��Ӧ�����нڵ�ĸ��׽ڵ㣬Ҫɾ���Ľڵ��Ǹýڵ���ӽڵ�
		{
			QVector<TreeItem*> allChild = item->takeChildAll();
			for (int i = 0; i < allChild.size(); i++)
			{
				item->appendChildList(allChild.at(i)->takeChildAll());
			}
			qDeleteAll(allChild);
			mergeSameGroupedItems(item);
		}
		else if (item->levelOfItem() < level) // ��û����level�㣬��������
		{
			for (int i = 0; i < item->childCount(); ++i)
			{
				itemQueue.push(item->child(i));
			}
		}
	}
	endResetModel();
	groupColumns.removeAt(level);
	for (int i = 0; i < orderColumns.size(); i++)
	{
		if (orderColumns.at(i).first == column)
			orderColumns.removeAt(i);
	}
	doSortByColumns();
	return;
}

////����groupColumnsִ�з��飬���ܻ��õ�
//һ��Ҫ����û�з�����������
void TreeModel::doGroupByColumns()
{
	QVector<int> gcols;
	gcols.swap(groupColumns);
	QVector<int>::iterator itr = gcols.begin();
	for (; itr != gcols.end(); itr++)
	{
		addGroupColumn(*itr);
	}
}

////����groupColumnsִ�з��飬���ܻ��õ�
//һ��Ҫ����û�з�����������
void TreeModel::doGroupByColumns(const QVector<int>& colVec)
{
	groupColumns.clear();
	QVector<int>::const_iterator itr = colVec.begin();
	for (; itr != colVec.end(); itr++)
	{
		addGroupColumn(*itr);
	}
}

////����orderColumnsִ�����򣬿��ܻ��õ�
//������������û������ʱ���ã��ָ�ԭ����״̬
void TreeModel::doSortByColumns()
{
	MultiColumnOrder sorders = MultiColumnOrder();
	orderColumns.swap(sorders);
	for (int i = 0; i < sorders.size(); i++)
	{
		QPair<int, Qt::SortOrder> odr = sorders.at(i);
		sortPrivate(odr.first, odr.second);
	}
}

void TreeModel::doSortByColumns(MultiColumnOrder sortOrders)
{
	orderColumns.swap(sortOrders);
	doSortByColumns();
}

///����record����һ��ֵ��varƥ�䣬���ص�һ��ƥ���index,���򷵻�
QModelIndex TreeModel::findRecordMatchVal(const QVariant& var)
{
	return findRecordNoColumn(rootItem, var);
}
///����findRecord������������
QModelIndex TreeModel::findRecordMatchValByColumn(const QVariant& var, int column)
{
	return findRecordByColumn(rootItem, var, column);
}

TreeItem* TreeModel::returnRootItem()
{
	return rootItem;
}

void TreeModel::resetModelDataWithItemList(QVector<TreeItem*>& itemList)
{
	//if (rootItem)
	//{
	//	beginResetModel();

	//	rootItem->removeChildren(0, rootItem->childCount());
	//	rootItem->setChildren(itemList);
	//	itemVector.clear();
	//	itemVector = itemList;
	//	endResetModel();
	//}
	qDeleteAll(itemToDelVector);
	itemToDelVector.clear();
	itemVector.swap(itemToDelVector);
	itemVector.swap(itemList);
	updateIndex();
}
void TreeModel::appendRows(const QVector<TreeItem*>& itemList)
{
	if (rootItem)
	{
		beginResetModel();
		rootItem->appendChildList(itemList);
		itemVector.append(itemList);
		endResetModel();
	}
}

QVector<int> TreeModel::getGroupColumns()
{
	return groupColumns;
}
void TreeModel::resetGroupColumns(QVector<int> cols)
{
	groupColumns = cols;
}

void TreeModel::setHeader(TreeItem* root)
{
	if (rootItem)
	{
		delete rootItem;
	}
	rootItem = root;
}

///merge item �ڵ�����ͬ�ķ���Ŀ¼
void TreeModel::mergeSameGroupedItems(TreeItem* item)
{
	std::queue<TreeItem*> itemQueue;
	std::map<QVariant, TreeItem*> itemMap;
	TreeItem* pitem, * citem;
	itemQueue.push(item);
	while (!itemQueue.empty())
	{
		pitem = itemQueue.front();
		itemQueue.pop();
		//pitem���ӽڵ�Ϊ�Ƿ������ɵ�Ŀ¼�ڵ������账��
		if (pitem->childCount() > 0 && pitem->child(0)->isVirtualItem())	//�������ɵ�Ŀ¼�ڵ㲻���ԭʼ���ݽڵ�λ��ͬһ�߶ȣ�����ж�child(0)�Ƿ�Ϊ�鼴��
		{
			std::map<QVariant, TreeItem*>::iterator it;
			for (int i = 0; i < pitem->childCount();)
			{
				citem = pitem->child(i);
				QVariant value = citem->data(0, Qt::UserRole);
				if ((it = itemMap.find(value)) == itemMap.end())
				{
					itemMap[value] = citem;
					i++;	//û�з���merge���ӽڵ㲻�ᱻɾ����i++
				}
				else
				{
					itemMap[value]->appendChildList(citem->takeChildAll());
					delete pitem->takeChildAt(i);	//���ӽڵ���ӽڵ㱻merge��֮ǰ��ͬgroup���ֵܽڵ��£������һ���շ��飬��Ҫɾ��
				}
			}
			for (int i = 0; i < pitem->childCount(); i++)
			{
				itemQueue.push(pitem->child(i));	//��merge֮����ӽڵ������У���ÿ���ӽڵ���merge
			}
			itemMap.clear();	//��գ�������һ��
		}
	}
}

void TreeModel::sortPrivate(int column, Qt::SortOrder order)	///
{
	if (column < 0 || column >= columnCount()) return;
	sortColumn = column;
	QPair<int, Qt::SortOrder> colSort(column, order);
	orderColumns.append(colSort);
	//layoutAboutToBeChanged and layoutChanged should be call before and after sort
	emit layoutAboutToBeChanged();
	sortChildrenItems(rootItem, column, order);
	emit layoutChanged();
}

TreeItem* TreeModel::getItemAt(int i)
{
	if (i < 0 || i >= itemVector.size())
	{
		return nullptr;
	}
	return itemVector[i];
}

int TreeModel::itemCounts()
{
	return itemVector.size();
}

///����modelȫ��Ĭ������
void TreeModel::setDefaultAttrOfModel()
{
	attrOfModel[Qt::TextAlignmentRole] = 0;
	attrOfModel[Qt::DecorationRole] = Rcspace::noIcon;
	attrOfModel[Qt::BackgroundRole] = 0x00ffffff;	///white, rbg=0xffffff,a=0x00
	attrOfModel[Qt::ForegroundRole] = 0xff000000;	///black, rgb=0x000000,a=0xff
	attrOfModel[Qt::FontRole] = QFont();
}
void TreeModel::removeItem(TreeItem* item)
{
	if (!item || item == rootItem)
	{
		return;
	}
	const int row = item->childNumber();
	QModelIndex index = createIndex(row, 0, item);
	beginRemoveRows(index.parent(), row, row);
	item->parent()->takeChildAt(row);
	endRemoveRows();
}
void TreeModel::setCheckStateAscend(const QModelIndex& index)
{
	if (!index.isValid())
	{
		return;
	}
	TreeItem* pItem = static_cast<TreeItem*>(index.internalPointer());
	if (!pItem || pItem == rootItem)
	{
		return;
	}
	int nChild = pItem->childCount();
	int nChecked = 0, nUnchecked = 0;
	Qt::CheckState ostate, nstate;
	ostate = pItem->data(0, Qt::CheckStateRole).value<Qt::CheckState>();
	for (int i = 0; i < nChild; i++)
	{
		Qt::CheckState check = pItem->child(i)->data(0, Qt::CheckStateRole).value<Qt::CheckState>();
		if (check == Qt::Checked)
		{
			nChecked++;
		}
		else if (check == Qt::Unchecked)
		{
			nUnchecked++;
		}
	}
	nstate = nChecked == nChild ? Qt::Checked : nUnchecked == nChild ? Qt::Unchecked : Qt::PartiallyChecked;
	pItem->setCheckState(index.column(), nstate);
	if (nstate != ostate)
	{
		emit dataChanged(index, index, QVector<int>(1, Qt::CheckStateRole));
		setCheckStateAscend(parent(index));
	}
}
bool TreeModel::setDataUnaffectParentCheckState(const QModelIndex& index, const QVariant& value, int role)
{
	TreeItem* pitem = static_cast<TreeItem*>(index.internalPointer());
	if (pitem == nullptr || pitem == rootItem)
	{
		return true;
	}
	bool success = pitem->setData(index.column(), value, role);
	int childCnt = pitem->childCount();
	if (childCnt)
	{
		for (int i = 0; i < childCnt; i++)
		{
			setDataUnaffectParentCheckState(createIndex(index.row(), index.column(), pitem->child(i)), value, role);
		}
		emit dataChanged(createIndex(0, index.column(), pitem->child(0)), createIndex(childCnt - 1, index.column(), pitem->child(childCnt - 1)), QVector<int>(1, role));
	}
	return success;
}
void TreeModel::sortChildrenItems(TreeItem* parent, int column, Qt::SortOrder order)
{
	if (!parent->childCount())
	{
		return;
	}
	//auto compare = std::bind(&TreeModel::itemLessThan, this, std::placeholders::_1, std::placeholders::_2);
	MultiColumnOrder& sortOdr = orderColumns;
	int indexOfColumn = sortOdr.size() - 1;///��ǰ�������������������е�λ��
	for (int i = 0; i < sortOdr.size(); i++)
	{
		if (sortOdr.at(i).first == column)
		{
			indexOfColumn = i;
			break;
		}
	}
	std::queue<TreeItem*> itemQueue;
	TreeItem* pitem;
	QVector<int>& groupCols = groupColumns;
	int level;
	itemQueue.push(parent);
	if ((level = groupCols.indexOf(column)) < 0)	///Ҷ�ӽڵ�����
	{
		auto compare = std::bind(order == Qt::AscendingOrder ? &TreeModel::itemLessThan : &TreeModel::itemGreaterThan, this, std::placeholders::_1, std::placeholders::_2);
		while (!itemQueue.empty())
		{
			pitem = itemQueue.front();
			itemQueue.pop();
			if (pitem->childCount() > 0 && pitem->child(0)->isVirtualItem()/*->childCount() > 0*/)	///
			{
				for (int i = 0; i < pitem->childCount(); i++)
				{
					itemQueue.push(pitem->child(i));
				}
			}
			else ///��pitem���ӽڵ�����
			{
				QVector<TreeItem*>& itemVec = pitem->childItems;
				std::map<TreeItem*, int> bfIndexMap, afIndexMap;
				if (sortOdr.size() > 1)	///��������
				{
					int lastSortCol = sortOdr.at(sortOdr.size() - 2).first;	///��һ�����ֶ�
					QVector<TreeItem*>::iterator it1 = itemVec.begin(), it2 = it1, ite = itemVec.end(), it3;
					while (it2 != ite)
					{
						while (it2 != ite && multiColEqual(*it1, *it2, indexOfColumn))	///�ڸ���֮ǰ�ֶ�����Ļ����϶�Qvector�ֲ�����  
							it2++;
						for (it3 = it1; it3 != it2; it3++)
						{
							bfIndexMap[*it3] = it3 - itemVec.begin();	///��������֮ǰ���±�
						}
						qStableSort(it1, it2, compare);	///����
						for (it3 = it1; it3 != it2; it3++)
						{
							afIndexMap[*it3] = it3 - itemVec.begin();	///��������֮����±�
						}
						it1 = it2;
					}
					QModelIndexList fromList, toList;
					std::map<TreeItem*, int>::iterator itm, itme;
					itme = bfIndexMap.end();
					for (itm = bfIndexMap.begin(); itm != itme; itm++)	///map ��֤�˱�����������
					{
						toList.append(createIndex(itm->second, 0, itm->first));
					}
					itme = afIndexMap.end();
					for (itm = afIndexMap.begin(); itm != itme; itm++)
					{
						toList.append(createIndex(itm->second, 0, itm->first));
					}
					changePersistentIndexList(fromList, toList);	////
				}
				else ///��������
				{
					QVector<TreeItem*>::iterator it = itemVec.begin(), ite = itemVec.end();
					int index = 0;
					for (; it != ite; it++, index++)
					{
						bfIndexMap[*it] = index;
					}
					qStableSort(itemVec.begin(), itemVec.end(), compare);	///��Qvector��ȫ������
					for (it = itemVec.begin(), index = 0; it != ite; it++, index++)
					{
						afIndexMap[*it] = index;
					}
					QModelIndexList fromList, toList;
					std::map<TreeItem*, int>::iterator itm, itme;
					itme = bfIndexMap.end();
					for (itm = bfIndexMap.begin(); itm != itme; itm++)	///map ��֤�˱�����������
					{
						fromList.append(createIndex(itm->second, 0, itm->first));
					}
					itme = afIndexMap.end();
					for (itm = afIndexMap.begin(); itm != itme; itm++)
					{
						toList.append(createIndex(itm->second, 0, itm->first));
					}
					changePersistentIndexList(fromList, toList);	////
				}
			}
		}
	}
	else	///Ŀ¼�ڵ�����
	{
		auto compare = std::bind(order == Qt::AscendingOrder ? &TreeModel::directItemLessThan : &TreeModel::directItemGreaterThan, this, std::placeholders::_1, std::placeholders::_2);
		while (!itemQueue.empty())
		{
			pitem = itemQueue.front();
			itemQueue.pop();
			if (pitem->itemLevel < level)
			{
				for (int i = 0; i < pitem->childCount(); i++)
				{
					itemQueue.push(pitem->child(i));
				}
			}
			else if (pitem->itemLevel == level)///���ӽڵ�����(��Ҫ�����Ŀ¼�ڵ�����)����
			{
				QVector<TreeItem*>& itemVec = pitem->childItems;
				std::map<TreeItem*, int> bfIndexMap, afIndexMap;
				QVector<TreeItem*>::iterator it = itemVec.begin(), ite = itemVec.end();
				int i = 0;
				for (; it != ite; it++, i++)
				{
					bfIndexMap[*it] = i;
				}
				qStableSort(itemVec.begin(), itemVec.end(), compare);
				for (it = itemVec.begin(), i = 0; it != ite; it++, i++)
				{
					afIndexMap[*it] = i;
				}
				QModelIndexList fromList, toList;
				std::map<TreeItem*, int>::iterator itm, itme;
				itme = bfIndexMap.end();
				for (itm = bfIndexMap.begin(); itm != itme; itm++)	///map ��֤�˱�����������
				{
					fromList.append(createIndex(itm->second, 0, itm->first));
				}
				itme = afIndexMap.end();
				for (itm = afIndexMap.begin(); itm != itme; itm++)
				{
					toList.append(createIndex(itm->second, 0, itm->first));
				}
				changePersistentIndexList(fromList, toList);	////
			}
			else
			{
				///nothing todo 
			}
		}
	}
}
bool TreeModel::itemLessThan(TreeItem* left, TreeItem* right)
{
	int sortCol = sortColumn < 0 ? 0 : sortColumn;
	return TreeItem::isVariantLessThen(left->data(sortCol), right->data(sortCol));
}
bool TreeModel::itemGreaterThan(TreeItem* left, TreeItem* right)
{
	int sortCol = sortColumn < 0 ? 0 : sortColumn;
	return TreeItem::isVariantLessThen(right->data(sortCol), left->data(sortCol));
}
bool TreeModel::directItemLessThan(TreeItem* left, TreeItem* right)
{
	return TreeItem::isVariantLessThen(left->data(0), right->data(0));

}
bool TreeModel::directItemGreaterThan(TreeItem* left, TreeItem* right)
{
	return TreeItem::isVariantLessThen(right->data(0), left->data(0));
}
bool TreeModel::multiColEqual(TreeItem* left, TreeItem* right, int cmpSize)
{
	for (int i = 0; i < cmpSize; i++)
	{
		if (!TreeItem::isVariantEqualTo(left->data(orderColumns[i].first), right->data(orderColumns[i].first)))
			return false;
	}
	return true;
}
QModelIndex TreeModel::findRecordByColumn(TreeItem* parent, const QVariant& var, int column)
{
	QModelIndex index;
	if (parent)
	{
		TreeItem* item;
		for (int i = 0; i < parent->childCount(); i++)
		{
			item = parent->child(i);
			if (item->isVirtualItem())///Ŀ¼�ڵ㲻�ڲ��ҷ�Χ�ڣ�ֱ������
				index = findRecordByColumn(item, var, column);
			else if (item->data(column) == var)
				index = createIndex(i, column, item);
			else
				index = findRecordByColumn(item, var, column);
			if (index.isValid())
				break;
		}
	}
	return index;
}
QModelIndex TreeModel::findRecordNoColumn(TreeItem* parent, const QVariant& var)
{
	QModelIndex index;
	if (parent)
	{
		TreeItem* item;
		for (int i = 0; i < parent->childCount(); i++)
		{
			item = parent->child(i);
			if (item->isVirtualItem())
				index = findRecordNoColumn(item, var);
			else
			{
				for (int j = 0; j < columnCount(); j++)
				{
					if (item->data(j) == var)
					{
						index = createIndex(i, j, item);
						break;
					}
				}
			}
			if (index.isValid())
				break;
		}
	}
	return index;
}
void TreeModel::appendItem(TreeItem* item)
{
	itemVector.append(item);
}

void TreeModel::appendItemSync(TreeItem* item)
{
	itemVector.append(item);
	rootItem->appendChild(item);
}

void TreeModel::truncateItems(int count)
{
	while (--count >= 0 && itemVector.size() > 0)
	{
		itemToDelVector << itemVector.takeLast();
	}
}
void TreeModel::removeAllItems()
{
	//itemToDelVector << itemVector;
	//itemVector.clear();
	itemVector.swap(itemToDelVector);
}
void TreeModel::removeItemAt(int row)
{
	itemToDelVector << itemVector.takeAt(row);
}
void TreeModel::removeItemSyncAt(int row)
{
	itemToDelVector << itemVector.takeAt(row);
	rootItem->setChildren(itemVector);
}
///�������е����򡢷����������
void TreeModel::updateIndex()
{
	beginResetModel();
	TreeItem* pitem;
	std::queue<TreeItem*> itemQueue;
	itemQueue.push(rootItem);
	while (!itemQueue.empty())
	{
		pitem = itemQueue.front();
		itemQueue.pop();
		if (pitem->childCount() > 0 && pitem->child(0)->isVirtualItem())	///�ӽڵ�ΪĿ¼�ڵ�������
		{
			QVector<TreeItem*> sunList = pitem->takeChildAll();
			for (int i = 0; i < sunList.size(); i++)
			{
				itemQueue.push(sunList.at(i));	///
			}
		}
		else
		{
			pitem->detachChildren();	///�ֽڵ�Ϊ��Ŀ¼�ڵ㣬ֱ�ӽ���ӹ�ϵ
		}
		if (pitem != rootItem)
		{
			delete pitem;			///ɾ��Ŀ¼�ڵ�
		}
	}
	rootItem->setChildren(itemVector);///дʱ��ֵ&������ֵ
	//rootItem->appendChildList(itemVector);////��������
	doGroupByColumns();
	doSortByColumns();
	endResetModel();
	qDeleteAll(itemToDelVector);
	itemToDelVector.clear();
	if (oldModelRecord)
	{
		delete oldModelRecord;
		oldModelRecord = nullptr;
	}
}

////������ʾ��������
///����������
void TreeModel::setTextAlignment(int column, Rcspace::RcTextAlignment aenum)
{
	if (column < 0 || column >= attrOfColumns.size())
	{
		return;
	}
	attrOfColumns[column][Qt::TextAlignmentRole] = aenum;
}
void TreeModel::setBackground(int column, unsigned long colorVal)
{
	if (column < 0 || column >= attrOfColumns.size())
	{
		return;
	}
	attrOfColumns[column][Qt::BackgroundRole] = (qulonglong)colorVal;
}
void TreeModel::setForeground(int column, unsigned long colorVal)
{
	if (column < 0 || column >= attrOfColumns.size())
	{
		return;
	}
	attrOfColumns[column][Qt::ForegroundRole] = (qulonglong)colorVal;
}
void TreeModel::setIcon(int column, Rcspace::RcIconEnum ienum)
{
	if (column < 0 || column >= attrOfColumns.size())
	{
		return;
	}
	attrOfColumns[column][Qt::DecorationRole] = ienum;
}
///ȫ����������
void TreeModel::setFont(QFont font)
{
	attrOfModel[Qt::FontRole] = font;
}
void TreeModel::setTextAlignment(Rcspace::RcTextAlignment aenum)
{
	attrOfModel[Qt::TextAlignmentRole] = aenum;
}
void TreeModel::setBackground(unsigned long colorVal)
{
	attrOfModel[Qt::BackgroundRole] = (qulonglong)colorVal;
}
void TreeModel::setForeground(unsigned long colorVal)
{
	attrOfModel[Qt::ForegroundRole] = (qulonglong)colorVal;
}
void TreeModel::setIcon(Rcspace::RcIconEnum ienum)
{
	attrOfModel[Qt::DecorationRole] = ienum;
}
void TreeModel::setEditable(bool edit)
{
	editable = edit;
}
bool TreeModel::isEditable()
{
	return editable;
}
void TreeModel::setDragAndDropAble(bool able)
{
	dragndropable = able;
}
bool TreeModel::isDragAndDropAble()
{
	return dragndropable;
}
Qt::DropActions TreeModel::supportedDragActions() const
{
	return Qt::MoveAction;
}
Qt::DropActions	TreeModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

static const char sTreeItemMimeType[] = "application/x-treeitem";
QStringList TreeModel::mimeTypes() const
{
	return QStringList() << sTreeItemMimeType;
}
QMimeData* TreeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData;
	QByteArray data; //a kind of RAW format for datas

	//QDataStream is independant on the OS or proc architecture
	//serialization of C++'s basic data types, like char, short, int, char *, etc.
	//Serialization of more complex data is accomplished
	//by breaking up the data into primitive units.
	QDataStream stream(&data, QIODevice::WriteOnly);
	QList<TreeItem*> nodes;

	//
	foreach(const QModelIndex & index, indexes) {
		TreeItem* node = getItem(index);
		if (!nodes.contains(node))
			nodes << node;
	}
	stream << QCoreApplication::applicationPid();
	stream << nodes.count();
	foreach(TreeItem * node, nodes) {
		stream << reinterpret_cast<qlonglong>(node);
	}
	mimeData->setData(sTreeItemMimeType, data);
	return mimeData;
}
bool TreeModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_ASSERT(action == Qt::MoveAction);
	Q_UNUSED(column);
	//test if the data type is the good one
	if (!mimeData->hasFormat(sTreeItemMimeType)) {
		return false;
	}
	QByteArray data = mimeData->data(sTreeItemMimeType);
	QDataStream stream(&data, QIODevice::ReadOnly);
	qint64 senderPid;
	stream >> senderPid;
	if (senderPid != QCoreApplication::applicationPid()) {
		// Let's not cast pointers that come from another process...
		return false;
	}
	TreeItem* parentNode = getItem(parent);
	Q_ASSERT(parentNode);
	int count;
	stream >> count;
	if (row == -1) {	///��ֹdropΪ�µ��ӽڵ�
		return false;
	}
	for (int i = 0; i < count; ++i) {
		// Decode data from the QMimeData
		qlonglong nodePtr;
		stream >> nodePtr;
		TreeItem* node = reinterpret_cast<TreeItem*>(nodePtr);
		if (parentNode != node->parent())	///�������Ŀ¼drop
		{
			return false;
		}
		if (node->childNumber() < row)	///������������ɾ���ɽڵ㣬Ŀ��λ��>node��ǰλ��ʱ��row--
			--row;
		// Remove from old position
		removeItem(node);
		beginInsertRows(parent, row, row);
		parentNode->insertChildAt(row, node);
		endInsertRows();
		++row;
	}
	return true;
}
void TreeModel::setCheckable(bool check)
{
	checkable = check;
}
bool TreeModel::isCheckable()
{
	return checkable;
}
void TreeModel::setItemIsSelectable(bool selectable)
{
	itemIsSelectable = selectable;
}
bool TreeModel::isItemIsSelectable()
{
	return itemIsSelectable;
}
int TreeModel::getItemIndex(TreeItem* pitem)
{
	return itemVector.indexOf(pitem);
}
QModelIndex TreeModel::index(int row, int column, TreeItem* item)
{
	if (item == nullptr || item == rootItem)
	{
		return QModelIndex();
	}
	return createIndex(row, column, item);
}
void TreeModel::setColumnEditable(int col, bool edit)
{
	if (edit && editableColVec.indexOf(col) < 0)
	{
		editableColVec.append(col);
	}
	else if (!edit && editableColVec.indexOf(col) > -1)
	{
		editableColVec.remove(editableColVec.indexOf(col));
	}
}
void TreeModel::setAllColumnEditable(bool edit)
{
	editableColVec.clear();
	if (edit)
	{
		for (int i = 0; i < columnCount(); i++)
		{
			editableColVec.append(i);
		}
	}
}
bool TreeModel::getColumnEditable(int col)
{
	if (editableColVec.indexOf(col) > -1)
	{
		return true;
	}
	return false;
}
void TreeModel::switchItems(int posLeft, int posRight, TreeItem* parent)
{
	if (!parent || parent == rootItem)
	{
		TreeItem* tmp;
		tmp = itemVector.at(posLeft);
		itemVector[posLeft] = itemVector.at(posRight);
		itemVector[posRight] = tmp;
	}
	else
	{
		parent->switchChild(posLeft, posRight);
	}
}

void TreeModel::setAttributeGlobal(QVariant var, int role)
{
	attrOfModel[role] = var;
}

void TreeModel::setAttributeColumn(QVariant var, int column, int role)
{
	attrOfColumns[column][role] = var;
}

void TreeModel::removeAttributeColumn(int column, int role)
{
	auto it = attrOfColumns[column].find(role);
	if (it != attrOfColumns[column].end())
	{
		attrOfColumns[column].erase(it);
	}
}

void TreeModel::setHeaderView(QHeaderView* h)
{
	head = h;
}

QHeaderView* TreeModel::getHeaderView()
{
	return head;
}
bool TreeModel::reloadData()
{
	beginResetModel();
	TreeItem* pitem;
	std::queue<TreeItem*> itemQueue;
	itemQueue.push(rootItem);
	while (!itemQueue.empty())
	{
		pitem = itemQueue.front();
		itemQueue.pop();
		if (pitem->childCount() > 0 && pitem->child(0)->isVirtualItem())	///�ӽڵ�ΪĿ¼�ڵ�������
		{
			QVector<TreeItem*> sunList = pitem->takeChildAll();
			for (int i = 0; i < sunList.size(); i++)
			{
				itemQueue.push(sunList.at(i));	///
			}
		}
		else
		{
			pitem->detachChildren();	///�ֽڵ�Ϊ��Ŀ¼�ڵ㣬ֱ�ӽ���ӹ�ϵ
		}
		if (pitem != rootItem)
		{
			delete pitem;			///ɾ��Ŀ¼�ڵ�
		}
	}
	rootItem->setChildren(itemVector);
	//rootItem->appendChildList(itemVector);///��������
	endResetModel();
	qDeleteAll(itemToDelVector);
	itemToDelVector.clear();
	return true;
}

void TreeModel::reserveItemSpace(int capacity)
{
	if (capacity > itemVector.capacity())
	{
		itemVector.reserve(capacity);
	}
}

void TreeModel::setModelRecord(ModelRecord* record)
{
	if (record && record != modelRecord)
	{
		if (oldModelRecord)
		{
			delete oldModelRecord;
		}
		oldModelRecord = modelRecord;
		modelRecord = record;
		int nSize = modelRecord->recordSize(), itemSize = itemVector.size();
        reserveItemSpace(nSize);
        int i = 0, nMin = std::min(nSize, itemSize);
		for (; i < nMin; i++)
		{
			itemVector[i]->setRecordData(modelRecord->recordAt(i));
		}
		while (i < nSize)
		{
			itemVector.push_back(new TreeItem(rootItem, modelRecord->recordAt(i)));
			i++;
		}
		if (i < itemSize)
		{
			truncateItems(itemSize - i);
		}
	}
}

void TreeModel::setModelRecord(ModelRecord* record, NodeType<int>* treeNode)
{
	if (record && record != modelRecord)
	{
		if (oldModelRecord)
		{
			delete oldModelRecord;
		}
		oldModelRecord = modelRecord;
		modelRecord = record;
		int nSize = treeNode->childNodes.size(), itemSize = itemVector.size();
		reserveItemSpace(nSize);
        int i = 0, nMin = std::min(nSize, itemSize);

		for (; i < nMin; i++)
		{
			buildItemWithModel(itemVector[i], treeNode->childNodes[i]);
		}
		while (i < nSize)
		{
			itemVector.push_back(buildItemWithModel(new TreeItem(rootItem), treeNode->childNodes[i]));
			i++;
		}
		if (i < itemSize)
		{
			truncateItems(itemSize - i);
		}
	}
}

TreeItem* TreeModel::buildItemWithModel(TreeItem* pitem, NodeType<int>* node)
{
	pitem->setRecordData(modelRecord->recordAt(node->value));
	int i = 0;
	for (; i < node->childNodes.size(); i++)
	{
		if (i < pitem->childCount())
		{
			buildItemWithModel(pitem->child(i), node->childNodes[i]);
		}
		else
		{
			pitem->appendChild(buildItemWithModel(new TreeItem(pitem), node->childNodes[i]));
		}

	}
	if (i < pitem->childCount())
	{
		pitem->removeChildren(i, pitem->childCount() - i);
	}
	return pitem;
}
