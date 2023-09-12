#include "treeitemwidget.h"
#include "qheaderview.h"
#include <vector>

TreeItemWidget::TreeItemWidget(QWidget* parent)
	: QWidget(parent)
{
	dataModel = nullptr; //dataModel ��ָ��
	totalModel = nullptr;
	setupDispUi(); // view�����ʼ��
}

TreeItemWidget::~TreeItemWidget()
{
	///headView,dataView,vboxLayout������ʾɾ������������ʱ�Զ�����
	///��ʾdelete model
	if (dataModel)
	{
		disconnect(dataModel, &TreeModel::dataAltered, this, &TreeItemWidget::onModelDataAltered);
		delete dataModel;
	}
	if (totalModel && totalModel != dataModel)	//��ָֹ�빲��
	{
		delete totalModel;
	}
	//delete menu
	if (headMenu)
	{
		delete headMenu;
	}
	if (dataMenu && dataMenu != headMenu)	//��ָֹ�빲��
	{
		delete dataMenu;
	}
}

void TreeItemWidget::hideSections(vector<int>& hidenLogicalIndexes)
{
	std::vector<int> ::iterator itr = hidenLogicalIndexes.begin();
	QHeaderView* headView = dataView->header();
	HeaderView* castHead = dynamic_cast<HeaderView*>(headView);
	if (castHead)
	{
		for (; itr != hidenLogicalIndexes.end(); itr++)
			castHead->hideSectionAndEmitSignal(*itr);
	}
	else
	{
		for (; itr != hidenLogicalIndexes.end(); itr++)
			headView->hideSection(*itr);
	}
}

/// �������õ����е�˳��
void TreeItemWidget::adjSectionViewShow(map<int, pair<int, bool>>& visualLogicalIndexPair)	///�����������������ݵ���ʾ map<visualIndex, <logicalIndex, visible>>
{
	///visualLogicalIndexPair����visualIndex���������Ұ��������е��ֶ�
	map<int, pair<int, bool>>::iterator it = visualLogicalIndexPair.begin(), ite = visualLogicalIndexPair.end();
	for (; it != ite; it++)
	{
		headView->moveSection(headView->visualIndex(it->second.first), it->first);
		if (it->second.second)
		{
			headView->showSectionAndEmitSignal(it->second.first);
		}
		else
		{
			headView->hideSectionAndEmitSignal(it->second.first);
		}
	}
}

void TreeItemWidget::addGroupColumn(int column)
{
	//TreeModel ������beginResetModel()/endResetModel()
	dataModel->addGroupColumn(column);
}

void TreeItemWidget::delGroupColumn(int column)
{
	///TreeModel ������beginResetModel()/endResetModel()
	dataModel->delGroupColumn(column);
}

void TreeItemWidget::setupDispUi(void)
{
	vboxLayout = new QVBoxLayout();
	vboxLayout->setSpacing(0);
	vboxLayout->setContentsMargins(0, 0, 0, 0);
	dataView = new TreeView(this);
	vboxLayout->addWidget(dataView, 1);
	/// dataView 
	dataView->setSelectionMode(QAbstractItemView::ExtendedSelection); // �������Ŀѡ��
	dataView->setAlternatingRowColors(true);
	dataView->setSelectionBehavior(QAbstractItemView::SelectRows); //Ĭ��ѡ��Ϊ��
	dataView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	dataView->setAnimated(false);
	dataView->setAllColumnsShowFocus(true);
	dataView->setAlternatingRowColors(false);
	dataView->footTreeView()->setRootIsDecorated(false);///����top-level�ڵ��۵�iconΪ����ʾ��һ������в���������νṹ
	//dataView->setTreePosition(-1);
	//dataView->setTreePosition(headView->firstVisiableLogicalIndex());
	///headView
	headView = new HeaderView(Qt::Horizontal, this);
	///ʹ���Զ���������Ӧ�߼����sectionSingleClicked�źţ��������Ҫ�Զ���������Ӧ�߼���ȡ����
	connect(headView, &HeaderView::sectionSingleClicked, this, &TreeItemWidget::onHeadViewSectionSingleClicked);
	////�ź�ת��
	connect(headView, &HeaderView::hideSectionSignal, this, &TreeItemWidget::onHeadSectionHide);
	connect(headView, &HeaderView::showSectionSignal, this, &TreeItemWidget::onHeadSectionShow);

	dataView->setHeaderAndReconnectSignals(headView);
	dataView->setSortingEnabled(false);//Ĭ��ֵ����false�����Բ������ã�����ֻ��Ϊ��ǿ����������Ϊfalse�������ܲ���Qt�Դ����߼�
	headView->setFirstSectionMovable(true);
	this->setLayout(vboxLayout);

	//headView->setSectionResizeMode(QHeaderView::Interactive);
	//conncet signals & slots
	connect(dataView, &QAbstractItemView::doubleClicked, this, &TreeItemWidget::onRowDoubleClicked);
	connect(dataView, &QTreeView::expanded, this, &TreeItemWidget::onItemExpanded);
	connect(dataView, &QTreeView::collapsed, this, &TreeItemWidget::onItemCollapsed);
	connect(dataView, &QAbstractItemView::clicked, this, &TreeItemWidget::onItemClicked);
	connect(dataView, &TreeView::mousePressed, this, &TreeItemWidget::onDataViewMousePressed);
	connect(headView, &HeaderView::sectionRightClicked, this, &TreeItemWidget::onHeaderSectionRightClicked);
	///menu
	headMenu = nullptr;
	dataMenu = nullptr;
}

void TreeItemWidget::setupDataModel(TreeModel* dModel)
{
	if (dModel == this->dataModel)
	{
		return;
	}
	if (dataModel)
	{
		delete dataModel;
	}
	dataModel = dModel;
	dataView->setModel(dataModel); // setModel����deleteԭ�е�model
	dataModel->setHeaderView(headView);
	connect(dataView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TreeItemWidget::onSelectionChanged);
	connect(dataModel, &TreeModel::dataAltered, this, &TreeItemWidget::onModelDataAltered);
	syncAttrToModel();
}

void TreeItemWidget::setupTotalModel(TreeModel* tModel)
{
	if (tModel == this->totalModel)
	{
		return;
	}
	if (totalModel)
	{
		delete totalModel;
	}
	totalModel = tModel;
	totalModel->setHeaderView(headView);
	dataView->setFootViewModel(tModel);
}

TreeModel* TreeItemWidget::getDataModel()
{
	return dataModel;
}

TreeModel* TreeItemWidget::getTotalModel()
{
	return totalModel;
}
QVector<QByteArray> TreeItemWidget::getColumnNames()
{
	QVector<QByteArray> namesArr;
	if (dataModel)
	{
		TreeItem* ritem = dataModel->returnRootItem();
		if (ritem)
		{
			for (int i = 0; i < ritem->columnCount(); i++)
			{
				namesArr << BYTEARRAY(ritem->data(i).toString());
			}
		}
	}
	return namesArr;
}
///����ĳ������ֵ�ҵ���һ�������ø���ֵ�ĵ�Ԫ��
QModelIndex TreeItemWidget::findRecord(const QVariant& var)
{
	return dataModel->findRecordMatchVal(var);
}
///���أ��޶���ƥ��ֵ����
QModelIndex TreeItemWidget::findRecord(const QVariant& var, int column)
{
	return dataModel->findRecordMatchValByColumn(var, column);
}
///ѡ��ĳ�в����������϶�ֱ�����пɼ�
void TreeItemWidget::setFocusOnIndex(const QModelIndex& idx)
{
	if (!idx.isValid())
	{
		return;
	}
	////Ĭ�������setCurrentIndexҲ��ѡ�и�index
	dataView->setFocus();
	dataView->scrollTo(idx, QAbstractItemView::EnsureVisible);
	dataView->setCurrentIndex(idx);
}
void TreeItemWidget::ensureVisible(const QModelIndex& idx)
{
	dataView->scrollTo(idx, QAbstractItemView::EnsureVisible);
}
//void TreeItemWidget::openHeaderViewCustomContextMenu()
//{
//	headView->setContextMenuPolicy(Qt::CustomContextMenu);
//	connect(headView, &HeaderView::customContextMenuRequested, this, &TreeItemWidget::onHeaderContextMenu);
//}
void TreeItemWidget::openDataViewCustomContextMenu()
{
	dataView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(dataView, &TreeView::customContextMenuRequested, this, &TreeItemWidget::onDataRowContextMenu);
}
TreeItem* TreeItemWidget::currentItem()
{
	QModelIndex index = dataView->currentIndex();
	if (index.isValid())
	{
		return static_cast<TreeItem*>(index.internalPointer());
	}
	return nullptr;
}
///��������չʾ����
void TreeItemWidget::updateDisplay()
{
	dataModel->updateIndex();
	if (shouldRootDecorate)
	{
		if (!dataView->rootIsDecorated())
		{
			dataView->setRootIsDecorated(true);
		}
	}
	else
	{
		if (dataModel->getGroupColumns().size())
		{
			if (!dataView->rootIsDecorated())
			{
				dataView->setRootIsDecorated(true);
			}
		}
		else
		{
			if (dataView->rootIsDecorated())
			{
				dataView->setRootIsDecorated(false);
			}
		}
	}
	dataView->updateGeometry();
}
//���»�������չʾ��
void TreeItemWidget::updateTotalDisplay()
{
	if (totalModel)
	{
		totalModel->updateIndex();
		//dataView->setFootViewGeometry();
	}
}
void TreeItemWidget::expandAll()
{
	return dataView->expandAll();
}
void TreeItemWidget::expand(TreeItem* item)
{
	QModelIndex index = dataModel->index(item->childNumber(), 0, item);
	return dataView->expand(index);
}
void TreeItemWidget::expandRecursively(TreeItem* item, int depth)
{
	QModelIndex index = dataModel->index(item->childNumber(), 0, item);
	dataView->expand(index);
	if (depth != 0)
	{
		for (int i = 0; i < item->childCount(); i++)
		{
			expandRecursively(item->child(i), depth - 1);
		}
	}
}
void TreeItemWidget::collapseAll()
{
	return dataView->collapseAll();
}
void TreeItemWidget::collapse(TreeItem* item)
{
	QModelIndex index = dataModel->index(item->childNumber(), 0, item);
	return dataView->collapse(index);
}
bool TreeItemWidget::isExpanded(TreeItem* item)
{
	QModelIndex index = dataModel->index(item->childNumber(), 0, item);
	return dataView->isExpanded(index);
}
QList<TreeItem*> TreeItemWidget::selectedRows()
{
	QModelIndexList indexes = dataView->selectionModel()->selectedRows();
	QList<TreeItem*> list;
	for (int i = 0; i < indexes.size(); i++)
	{
		list.append((TreeItem*)indexes.at(i).internalPointer());
	}
	return list;
}
QModelIndexList TreeItemWidget::selectedIndexes()
{
	return  dataView->selectionModel()->selectedRows();
}
void TreeItemWidget::selectItems(const QVector<TreeItem*>& items)
{
	if (items.size() == 0)return;
	dataView->clearSelection();
	QItemSelectionModel* selModel = dataView->selectionModel();
	for (int i = 0; i < items.size(); i++)
	{
		selModel->select(dataModel->index(items.at(i)->childNumber(), 0, items.at(i)), QItemSelectionModel::SelectionFlag::Select);
	}
}
void TreeItemWidget::selectItems(const QVector<TreeItem*>& items, const QVector<int>& rows)
{
	if (items.size() == 0)return;
	if (items.size() != rows.size())return;
	dataView->clearSelection();
	QItemSelectionModel* selModel = dataView->selectionModel();
	for (int i = 0; i < items.size(); i++)
	{
		selModel->select(dataModel->index(rows.at(i), 0, items.at(i)), QItemSelectionModel::SelectionFlag::Select);
	}
}
void TreeItemWidget::setGroupable(bool group)
{
	groupAbled = group;
}
bool TreeItemWidget::isGroupable()
{
	return groupAbled;
}
void TreeItemWidget::setDragAndDropAble(bool able)
{
	dragndropable = able;
	if (dataModel && dataView)
	{
		dataModel->setDragAndDropAble(able);
		dataView->setDragEnabled(able);
		dataView->setAcceptDrops(able);
		dataView->setDropIndicatorShown(able);
	}
}
bool TreeItemWidget::isDragAndDropAble()
{
	return dragndropable;
}
void TreeItemWidget::setCheckable(bool check)
{
	checkable = check;
	if (dataModel)
	{
		dataModel->setCheckable(check);
	}
}
bool TreeItemWidget::isCheckable()
{
	return checkable;
}
void TreeItemWidget::setEditable(bool edit)
{
	editable = edit;
	if (dataModel)
	{
		dataModel->setEditable(edit);
	}
}
bool TreeItemWidget::isEditable()
{
	return editable;
}
void TreeItemWidget::setSortable(bool sort)
{
	sortable = sort;
	if (dataModel)
	{
		dataModel->setSortAbled(sort);
	}
	//if (dataView)
	//{
	//	dataView->setSortingEnabled(sort);
	//}
}
bool TreeItemWidget::isSortable()
{
	return sortable;
}
void TreeItemWidget::setShouldRootDecorate(bool should)
{
	shouldRootDecorate = should;
}
void TreeItemWidget::syncAttrToModel()	////��������datamodelʱͬ���������µ�datamodel
{
	if (dataModel)
	{
		dataModel->setCheckable(checkable);		////checkable
		if (dataView)
		{
			//dataView->setSortingEnabled(sortable);	////sortable
			dataModel->setDragAndDropAble(dragndropable);	///drag & dropable
			dataView->setDragEnabled(dragndropable);
			dataView->setAcceptDrops(dragndropable);
			dataView->setDropIndicatorShown(dragndropable);
		}
		dataModel->setSortAbled(sortable);		///sortable
		dataModel->setEditable(editable);		///editable
	}
}
void TreeItemWidget::onModelDataAltered(TreeItem* pitem, int column, const QVector<int>& roles)
{
	emit itemDataChanged(pitem, column, roles);
}
void TreeItemWidget::onItemExpanded(const QModelIndex& index)
{
	emit expandedOrCollapsed(index, 1);
}
void TreeItemWidget::onItemCollapsed(const QModelIndex& index)
{
	emit expandedOrCollapsed(index, 0);
}
void TreeItemWidget::onItemClicked(const QModelIndex& index)
{
	emit itemClicked(index);
}
void TreeItemWidget::onDataViewMousePressed()
{
	emit mousePressed();
}
void TreeItemWidget::onHeadViewSectionSingleClicked(int logicalIndex)
{
	if (!sortable)///��ֹ����ֱ�ӷ���
	{
		return;
	}
	if (dataModel)
	{
		MultiColumnOrder sorts = dataModel->sortMultiColumns();
		Qt::SortOrder order = Qt::SortOrder::AscendingOrder;
		for (auto it : sorts)
		{
			if (it.first == logicalIndex)///֮ǰ�Ѿ����ڶԸ��е����򣬷�ת����˳��
			{
				order = it.second == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
				break;
			}
		}
		dataModel->sort(logicalIndex, order);
	}
}
void TreeItemWidget::onHeadSectionShow(int logicalIndex)
{
	emit sectionShowed(logicalIndex, true);
}
void TreeItemWidget::onHeadSectionHide(int logicalIndex)
{
	emit sectionShowed(logicalIndex, false);
}
///deprecated,don't use this, use right clicked slot
void TreeItemWidget::onHeaderContextMenu(const QPoint& pos)
{
}
void TreeItemWidget::onDataRowContextMenu(const QPoint& pos)
{
	//����ʹ��
	QModelIndexList indexes = dataView->selectionModel()->selectedRows();
	TreeItem* tmp;
	QByteArray str;
	for (int i = 0; i < indexes.size(); i++)
	{
		//tmp = (TreeItem*)indexes.at(i).internalPointer();
		str = BYTEARRAY(dataModel->data(indexes.at(i), Qt::DisplayRole).toString());
		if (indexes.at(i).child(0, 0).isValid())
		{
			int k = 0;
		}
	}
	int j = 0;
}
void TreeItemWidget::onActionOfHeaderSection(bool checked)
{
	QAction* action = qobject_cast<QAction*>(sender());
	QString secname = action->text();
	TreeItem* rItem = dataModel->returnRootItem();
	int i = 0;
	while (i < rItem->columnCount() && secname != rItem->data(i).toString())
	{
		++i;
	}
	if (i < rItem->columnCount())
	{
		headView->setSectionHidden(i, checked ? false : true);
		dataView->footTreeView()->header()->setSectionHidden(i, checked ? false : true);
		//dataView->setTreePosition(headView->logicalIndexAt(0));
		dataView->setTreePosition(headView->firstVisiableLogicalIndex());
		emit sectionShowed(i, checked);
	}
}
void TreeItemWidget::onRowDoubleClicked(const QModelIndex& index)
{
	emit(rowDoubleClicked(index));
}
void TreeItemWidget::onHeaderGroup(bool checked)
{
	if (checked)
	{
		dataModel->addGroupColumn(rightClickSection);
		if (!dataView->rootIsDecorated())
		{
			dataView->setRootIsDecorated(true);		///�����з������Ҫ����Ϊtrue
		}
	}
	else
	{
		dataModel->delGroupColumn(rightClickSection);
		if (dataModel->getGroupColumns().size() == 0 && !shouldRootDecorate && dataView->rootIsDecorated())
		{
			dataView->setRootIsDecorated(false);	///���ݷ����и�����shouldRootDecorate���Թ�ͬ����
		}
	}
	dataView->expandAll();
}
void TreeItemWidget::onHeaderSectionRightClicked(int logicalIndex)
{
	rightClickSection = logicalIndex;	///��¼ÿ������Ҽ�����λ��
	if (headMenu == nullptr)headMenu = new QMenu(this);
	headMenu->clear();	///ɾ��֮ǰ������action��actionû�б���������ʹ��ʱ���Զ�delete
	TreeItem* rItem = dataModel->returnRootItem();
	QAction* action;
	action = headMenu->addAction(QIcon(), STRING("�Զ������п�"));
	connect(action, &QAction::triggered, this, &TreeItemWidget::onAutoSetHeaderSectionWidth);
	action = headMenu->addAction(QIcon(), STRING("��������п�"));
	connect(action, &QAction::triggered, this, &TreeItemWidget::onOptimalSetHeaderSectionWidth);
	if (dataModel)
	{
		MultiColumnOrder sortcols = dataModel->sortMultiColumns();
		QVector<int> groupCols = dataModel->getGroupColumns();
		for (auto it : sortcols)
		{
			if (it.first == logicalIndex)///��ͨ���򣬷Ƿ�������
			{
				if (groupCols.indexOf(logicalIndex) < 0)
				{
					action = headMenu->addAction(QIcon(), STRING("ȡ������"));
					connect(action, &QAction::triggered, this, &TreeItemWidget::onActionCancelSort);
				}
				break;
			}
		}
		if (groupAbled)
		{
			action = headMenu->addAction(QIcon(), STRING("��������"));
			connect(action, &QAction::triggered, this, &TreeItemWidget::onHeaderGroup);
			action->setCheckable(true);
			if (groupCols.indexOf(logicalIndex) > -1)
			{
				action->setChecked(true);
			}
			else
			{
				action->setChecked(false);
			}
		}
	}

	headMenu->addSeparator();
	map<int, int> visualLogicalmap;
	for (int i = 0; i < rItem->columnCount(); i++)
	{
		visualLogicalmap[headView->visualIndex(i)] = i;
	}
	for (auto it = visualLogicalmap.begin(); it != visualLogicalmap.end(); it++)
	{
		action = headMenu->addAction(QIcon(), rItem->data(it->second).toString());
		connect(action, &QAction::triggered, this, &TreeItemWidget::onActionOfHeaderSection);
		action->setCheckable(true);
		if (!headView->isSectionHidden(it->second))
		{
			action->setChecked(true);
		}
		else
		{
			action->setChecked(false);
		}
	}

	headMenu->exec(QCursor::pos());
}
void TreeItemWidget::onAutoSetHeaderSectionWidth(bool checked)
{
	headView->resizeSections(QHeaderView::Stretch);
}
void TreeItemWidget::onOptimalSetHeaderSectionWidth(bool checked)
{
	///������������Ӧ
	QVector<int> nTotalSize;
	if (totalModel && dataView->footTreeView()->isVisible())
	{
		TreeViewAdapt* footView = dataView->footTreeView();
		for (int i = 0; i < dataModel->columnCount(); i++)
		{
			nTotalSize.append(footView->sizeHintForColumn(i));
		}
	}
	headView->resizeSections(QHeaderView::ResizeToContents);
	if (nTotalSize.size() > 0)
	{
		for (int i = 0; i < nTotalSize.size(); i++)
		{
			if (headView->isSectionHidden(i))
				continue;
			if (headView->sectionSize(i) < nTotalSize.at(i))
			{
				headView->resizeSection(i, nTotalSize.at(i));
			}
		}
	}
}
///dataMenu��Ӧaction����Ӧ
void TreeItemWidget::onDataMenuAction(bool checked)
{
}
void TreeItemWidget::onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
	emit selectionChanged(selected, deselected);
}

void TreeItemWidget::onActionCancelSort()
{
	MultiColumnOrder sortcols = dataModel->sortMultiColumns();
	for (int i = 0; i < sortcols.size(); i++)
	{
		if (sortcols[i].first == rightClickSection)
		{
			sortcols.removeAt(i);
			break;
		}
	}
	dataModel->reloadData();
	dataModel->doGroupByColumns();
	dataModel->doSortByColumns(sortcols);
}
