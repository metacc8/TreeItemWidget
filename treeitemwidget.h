#pragma once

#include "treemodel.h"
#include <qtreeview.h>
#include "stdafx.h"
#include "headerview.h"
#include <qboxlayout.h>
#include "treeview.h"
#include <qabstractitemmodel.h>
#include <qmenu.h>

using namespace std;

class TreeItemWidget : public QWidget
{
	Q_OBJECT

public:
	TreeItemWidget(QWidget* parent = nullptr);
	virtual ~TreeItemWidget();
	void hideSections(std::vector<int>& hidenLogicalIndexes); /// ����������
	void adjSectionViewShow(map<int, pair<int, bool>>& visualLogicalIndexPair);	///�����������������ݵ���ʾ map<visualIndex, <logicalIndex, visible>>
private:
	void addGroupColumn(int column);
	void delGroupColumn(int column);
	void setupDispUi(void);
public:
	/*model set and get*/
	void setupDataModel(TreeModel* dModel);
	void setupTotalModel(TreeModel* tModel);
	TreeModel* getDataModel();
	TreeModel* getTotalModel();
	QVector<QByteArray> getColumnNames();
	///ѡ���в���
	QModelIndex findRecord(const QVariant& var);	///����ĳ������ֵ�ҵ���һ�������ø���ֵ�ĵ�Ԫ��
	QModelIndex findRecord(const QVariant& var, int column);	///���أ��޶���ƥ��ֵ����
	void setFocusOnIndex(const QModelIndex& idx); ///ѡ��ĳ�в����������϶�ֱ�����пɼ�
	void ensureVisible(const QModelIndex& idx);
	///�Ҽ��˵����
	//void openHeaderViewCustomContextMenu();			///deprecated
	void openDataViewCustomContextMenu();
	TreeItem* currentItem();
	void updateDisplay();	///��������չʾ����
	void updateTotalDisplay();	//���»�������չʾ��
	void expandAll();
	void expand(TreeItem* item);
	void expandRecursively(TreeItem* item, int depth = -1);
	void collapseAll();
	void collapse(TreeItem* item);
	bool isExpanded(TreeItem* item);
	QList<TreeItem*> selectedRows();
	QModelIndexList selectedIndexes();
	void selectItems(const QVector<TreeItem*>& items);
	void selectItems(const QVector<TreeItem*>& items, const QVector<int>& rows);//�����кź��кŶ�Ӧ��itemѡ�У�ʡȥitem->childNumber()�ĵ���
	///groupable
	void setGroupable(bool group);
	bool isGroupable();
	////drag & drop 
	void setDragAndDropAble(bool able);
	bool isDragAndDropAble();
	////checkable
	void setCheckable(bool check);
	bool isCheckable();
	///editable
	void setEditable(bool edit);
	bool isEditable();
	///sortable
	void setSortable(bool sort);
	bool isSortable();
	///shouldRootDecorate
	void setShouldRootDecorate(bool should);
private:
	void syncAttrToModel();
signals:
	void mousePressed();
	void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void itemDataChanged(TreeItem* pitem, int column, const QVector<int>& roles = QVector<int>());
	void rowDoubleClicked(const QModelIndex& index);
	void expandedOrCollapsed(const QModelIndex& index, int nType);	//0-�۵���1-չ��
	void itemClicked(const QModelIndex& index);
	void sectionShowed(int logicalIndex, bool show);
private slots:
	void onModelDataAltered(TreeItem* pitem, int column, const QVector<int>& roles = QVector<int>());
	void onItemExpanded(const QModelIndex& index);
	void onItemCollapsed(const QModelIndex& index);
	void onItemClicked(const QModelIndex& index);
	void onDataViewMousePressed();
	void onHeadViewSectionSingleClicked(int logicalIndex);
	void onHeadSectionShow(int logicalIndex);
	void onHeadSectionHide(int logicalIndex);
public slots:
	virtual void onHeaderContextMenu(const QPoint& pos);	///deprecated
	virtual void onDataRowContextMenu(const QPoint& pos);	///������ʾ���Ҽ��˵�
	virtual void onActionOfHeaderSection(bool checked);	///��ͷ�Ҽ��˵���ʾ�����action
	virtual void onRowDoubleClicked(const QModelIndex& index);	///������Ŀ˫������
	virtual void onHeaderGroup(bool checked);	///��ͷ�ֶΰ�������
	virtual void onHeaderSectionRightClicked(int logicalIndex);	///��ͷ�Ҽ��˵�
	void onAutoSetHeaderSectionWidth(bool checked = false);	///�Զ������п�
	void onOptimalSetHeaderSectionWidth(bool checked = false);	///��������п�
	virtual void onDataMenuAction(bool checked);	///dataMenu��Ӧaction����Ӧ
	void onSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	virtual void onActionCancelSort();///ȡ������
public:
	HeaderView* headView; /// ��ͷ��ͼ
	TreeView* dataView;
	QVBoxLayout* vboxLayout; /// vboxLayout Ϊ dataView �� totalView ����
	QMenu* headMenu;
	QMenu* dataMenu;
private:
	bool editable = false;		///edit
	bool dragndropable = false;	///drag & drop
	bool checkable = false;		///checkable
	bool sortable = false;		///sortable
	TreeModel* dataModel;
	TreeModel* totalModel;
	int rightClickSection;	///��ע����Ҽ�������section
	bool groupAbled = true;
	///dataView->rootIsDecorated���ԣ�Ĭ��Ϊfalse���ɴ������ֶ����á���model���ݽڵ�ֲ�ʱ�����ֶ�����Ϊtrue��������Ҫ����
	///dataView���ݸ�ֵ���Ƿ�����з���������rootIsDecorated����
	bool shouldRootDecorate = false;	
};
