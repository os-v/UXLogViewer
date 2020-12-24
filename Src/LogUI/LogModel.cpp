//
//  @file LogModel.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogModel.h"
#include "AppConfig.h"
#include <QColor>
#include <QSize>
#include "LogFile.h"
#include "LogTheme.h"

CLogModel::CLogModel(ILogFile *pLogFile, QObject *parent)
	: QAbstractItemModel(parent), m_pLogFile(pLogFile), m_nRowsCount(0)
{
}

CLogModel::~CLogModel()
{
}

void CLogModel::UpdateHeader()
{

	emit headerDataChanged(Qt::Vertical, 0, m_pLogFile->GetTheme()->GetColCount());

	//emit dataChanged(createIndex(0, 0), createIndex(m_pLogFile->GetSize(), 0));

}

void CLogModel::UpdateView(ILogFile *pLogFile, int nRows)
{

	if(pLogFile)
		m_pLogFile = pLogFile;

	m_nRowsCount = nRows;

	beginInsertRows(QModelIndex(), 0, nRows ? nRows : m_pLogFile->GetSize());
	endInsertRows();

	//emit dataChanged(createIndex(0, 0), createIndex(m_pLogFile->GetSize(), 0));
	//emit headerDataChanged(Qt::Vertical, 0, m_pLogFile->GetTheme()->GetColCount());

}

QModelIndex CLogModel::index(int row, int column, const QModelIndex &parent) const
{

	if(parent.isValid() && parent.column() != 0)
		return QModelIndex();

	if(parent.internalId() == 0)
		return createIndex(row, column);

	return QModelIndex();
}

QModelIndex CLogModel::parent(const QModelIndex &child) const
{
	return QModelIndex();
}

int CLogModel::rowCount(const QModelIndex &parent) const
{
	if(parent.internalId() != 0)
		return 0;
	return m_nRowsCount ? m_nRowsCount : (int)m_pLogFile->GetSize();
}

int CLogModel::columnCount(const QModelIndex &parent) const
{
	return m_pLogFile->GetTheme()->GetColCount();
}

QVariant CLogModel::data(const QModelIndex &index, int role) const
{

	if(!index.isValid())
		return QVariant();

	if(role != Qt::DisplayRole)//  && role != Qt::EditRole)
		return QVariant();

	int nRow = index.row();
	if(nRow < m_pLogFile->GetRecord())
		return QVariant();

	QString *pCols = m_pLogFile->GetFrameLine(nRow);
	if(!pCols)
		return QVariant();

	return pCols[index.column()];
}

QVariant CLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{

	CLogTheme *pTheme = m_pLogFile->GetTheme();

	if(orientation == Qt::Horizontal && role == Qt::DisplayRole)			//Qt::ItemDataRole
		return pTheme->FieldName[section];

	return QVariant();
}

