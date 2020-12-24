//
//  @file LogModel.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractItemModel>

#include "LogFile.h"

class CLogModel : public QAbstractItemModel
{

	Q_OBJECT

public:

	CLogModel(ILogFile *pLogFile, QObject *parent = 0);
	~CLogModel();

	void UpdateHeader();
	void UpdateView(ILogFile *pLogFile = 0, int nRows = 0);

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

protected:

	ILogFile *m_pLogFile;
	int m_nRowsCount;

};

#endif
