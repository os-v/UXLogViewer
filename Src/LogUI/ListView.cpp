//
//  @file ListView.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "ListView.h"
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QWheelEvent>

class GridDelegate : public QStyledItemDelegate
{
public:
	explicit GridDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) { }
	void paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
	{
		painter->save();
		painter->setPen(QPen(QBrush(Qt::gray), 0.1));
		//painter->setPen(QColor(0xFF002000));
		//painter->drawRect(option.rect);
		painter->drawLine(option.rect.left(), option.rect.top(), option.rect.left(), option.rect.bottom());
		painter->drawLine(option.rect.right(), option.rect.top(), option.rect.right(), option.rect.bottom());
		painter->restore();
		QStyledItemDelegate::paint(painter, option, index);
	}
};

CListView::CListView(QWidget *parent) :
	QTreeView(parent)
{

	setItemDelegate(new GridDelegate(this));

}

CListView::~CListView()
{

}

int CListView::GetVisibleRowsCount()
{

	int nFirstItem = indexAt(rect().topLeft()).row();
	int nLastItem = indexAt(rect().bottomLeft()).row();

	return verticalScrollBar()->pageStep();
}

void CListView::keyPressEvent(QKeyEvent *event)
{

	if(event->key() == Qt::Key_Down)
		emit OnNavigate(ENavTypeLineDown);
	else if(event->key() == Qt::Key_Up)
		emit OnNavigate(ENavTypeLineUp);
	else if(event->key() == Qt::Key_PageDown)
		emit OnNavigate(ENavTypePageDown);
	else if(event->key() == Qt::Key_PageUp)
		emit OnNavigate(ENavTypePageUp);
	else if(event->key() == Qt::Key_End)
		emit OnNavigate(ENavTypeEnd);
	else if(event->key() == Qt::Key_Home)
		emit OnNavigate(ENavTypeHome);

}
