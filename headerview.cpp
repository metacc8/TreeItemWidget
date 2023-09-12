#include "headerview.h"

HeaderView::HeaderView(Qt::Orientation orientation, QWidget* parent)
	: QHeaderView(orientation, parent)
{
	connect(this, &QHeaderView::sectionClicked, this, &HeaderView::onSectionClicked);
}

HeaderView::~HeaderView()
{
}

void HeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
	if (!rect.isValid())
		return;
	//ʹ��QHeaderViewĬ��
	QHeaderView::paintSection(painter, rect, logicalIndex);

	TreeModel* tmodel = dynamic_cast<TreeModel*>(model());
	if (tmodel)
	{
		MultiColumnOrder sortOdrs = tmodel->orderColumns;
		for (int i = 0; i < sortOdrs.size(); i++)
		{
			QPair<int, Qt::SortOrder> sortCol = sortOdrs.at(i);
			if (sortCol.first == logicalIndex)
			{
				///��ʱֻ�����´���
				QStyleOptionHeader opt;
				initStyleOption(&opt);
				switch (sortCol.second)
				{
				case Qt::AscendingOrder:
					opt.sortIndicator = QStyleOptionHeader::SortDown; //
					break;
				case Qt::DescendingOrder:
					opt.sortIndicator = QStyleOptionHeader::SortUp;
					break;
				default:
					break;
				}
				opt.rect = rect;
				opt.section = logicalIndex;
				//��ȡ��ͷ����
				opt.text = model()->headerData(logicalIndex, Qt::Horizontal).toString();
				opt.textAlignment = defaultAlignment();
				opt.iconAlignment = Qt::AlignVCenter;
				style()->drawControl(QStyle::CE_Header, &opt, painter, this);
				return;
			}
		}
	}
}

///slot to sectionClicked()
void HeaderView::onSectionClicked(int logicalIndex)
{
	//HeadView->flipSortIndicator�����л����setSortIndicator��Ȼ��emit sortIndicatorChanged �ź�
	//TreeView ����sortIndicatorChanged�źź󣬻����sort������
	//����HeadView->flipSortIndicator���غ󣬲Ż�emit sectionClicked �źš�
	//��headView�У�������sectionClicked�źŵĴ�����onSectionClicked�����ñ�ͷ����������ʽ��
	//�������豣��Ч��ͬ��
	Qt::SortOrder sortOdr = QHeaderView::sortIndicatorOrder();
	QPair<int, Qt::SortOrder> colSort(logicalIndex, sortOdr);
	if (QApplication::keyboardModifiers() == Qt::ShiftModifier)
	{
		this->ableMultiSort = true;
		for (int i = 0; i < orderCols.size(); i++)
		{
			if (orderCols.at(i).first == logicalIndex)
			{
				orderCols.removeAt(i);
				break;
			}
		}
	}
	else
	{
		this->ableMultiSort = false;
		orderCols.clear();
	}
	orderCols.append(colSort); //������ζ������µ�sortIndicator����orderCols
}

void HeaderView::hideSectionAndEmitSignal(int logicalIndex)
{
	QHeaderView::hideSection(logicalIndex);
	emit hideSectionSignal(logicalIndex);
}

void HeaderView::showSectionAndEmitSignal(int logicalIndex)
{
	QHeaderView::showSection(logicalIndex);
	emit showSectionSignal(logicalIndex);
}

int HeaderView::firstVisiableLogicalIndex()
{
	for (int i = 0; i < count(); i++)
	{
		int idx = logicalIndex(i);
		if (!isSectionHidden(idx))
		{
			return idx;
		}
	}
	return 0;
}

void HeaderView::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == Qt::RightButton)
	{
		int logicalIndex = logicalIndexAt(e->pos());
		emit sectionRightClicked(logicalIndex);
	}
	else if (e->button() == Qt::LeftButton)
	{
		bLeftMousePressed = true;
		toHideIndex = logicalIndexAt(e->pos());
		if (toHideIndex > -1)
		{
			mstate = PressState;
		}
	}
	QHeaderView::mousePressEvent(e);
}

void HeaderView::mouseMoveEvent(QMouseEvent* e)
{
	bLeftMousePressed = false;
	int curpos, btmpos;
	if (orientation() == Qt::Orientation::Horizontal)
	{
		curpos = e->y();
		btmpos = QPoint(pos().x(), pos().y() + height()).y();
	}
	else
	{
		curpos = e->x();
		btmpos = QPoint(pos().x() + width(), pos().y()).x();
	}
	if (curpos > btmpos)
	{
		if (mstate == PressState && cursor() == Qt::ArrowCursor)///��ʱͨ�������״���ж��Ƿ�
		{
			mstate = MoveState;
			stageCursor = cursor();
			setCursor(QCursor(QPixmap(":Resources/blind.png")));
		}
	}
	else
	{
		if (mstate == MoveState)
		{
			mstate = PressState;
			setCursor(stageCursor);
		}
	}
	QHeaderView::mouseMoveEvent(e);
}

void HeaderView::mouseReleaseEvent(QMouseEvent* e)
{
	if (mstate == MoveState)
	{
		if (toHideIndex > -1 && toHideIndex < count())
		{
			hideSectionAndEmitSignal(toHideIndex);
		}
	}
	mstate = NoState;
	if (bLeftMousePressed)
	{
		bLeftMousePressed = false;
		int pos = orientation() == Qt::Horizontal ? e->x() : e->y();
		int logicalIndex = logicalIndexAt(pos);
		emit sectionSingleClicked(logicalIndex);
	}
	QHeaderView::mouseReleaseEvent(e);
}
