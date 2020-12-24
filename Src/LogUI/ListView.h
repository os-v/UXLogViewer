//
//  @file ListView.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QTreeWidget>

class CListView : public QTreeView
{

	Q_OBJECT

public:

	enum { ENavTypeInvalid = -1, ENavTypeLineDown, ENavTypeLineUp, ENavTypePageDown, ENavTypePageUp, ENavTypeHome, ENavTypeEnd };

	explicit CListView(QWidget *parent = nullptr);
	~CListView();

	int GetVisibleRowsCount();

	void Repaint() {
		viewport()->repaint();
	}

signals:

	void OnNavigate(int eType);

protected:

	virtual void keyPressEvent(QKeyEvent *event);

};

#endif
