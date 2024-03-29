//
//  @file LogWidget.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QFileSystemWatcher>

#include "LogFile.h"
#include "LogFileRaw.h"
#include "LogFileFlt.h"

namespace Ui {
	class CLogWidget;
}

class CLogModel;
class CLogWidget;

class CLogWidget : public QWidget
{

	Q_OBJECT

public:

	explicit CLogWidget(QWidget *parent = nullptr, int nID = -1, bool fSearchBar = true, CLogWidget *pFiltered = 0);
	~CLogWidget();

	ILogFile *GetLogFile() {
		return m_pFiltered || m_pLogFileFlt->IsFiltered() ? (ILogFile*)m_pLogFileFlt :  (ILogFile*)m_pLogFileRaw;
	}

	CLogModel *GetLogModel() {
		return m_pLogModel;
	}

	void Invalidate();

	QByteArray SaveState();
	bool RestoreState(QByteArray pState);
	void ResizeColumnsToContents();
	void ResizeColumn(int iSection, int nSize);
	int GetLastColumnWidth();
	void SetLastColumnWidth(int nWidth);

	void ResetData();
	void ThemeUpdated(QByteArray pState, int nLastColumnWidth);
	void LoadFile(QString sFileName);
	void SaveFile(QString sFileName);
	bool ApplyFilter(QString sFilter);
	void SelectItem(qint64 nOffset, qint64 nVOffset);
	void CopyToClipboard(bool fSelectedOnly);
	void AddToResults(CLogWidget *pActive);
	QString GetSelectedMessage();

	qint64 GetSelected() {
		return m_nSelected;
	}

private:

	int m_nAction;
	int m_nViewID;

	Ui::CLogWidget *ui;

	QFileSystemWatcher m_pFileWatcher;

	CLogModel *m_pLogModel;
	CLogWidget *m_pFiltered;
	CLogFileRaw *m_pLogFileRaw;
	CLogFileFlt *m_pLogFileFlt;

	qint64 m_nSelected;

	void UpdateSelected(qint64 nSelect, int nSelected);

	virtual void resizeEvent(QResizeEvent *event);

signals:

	void OnHeaderSectionResized(int iSection, int nSize);

	void OnItemAltAction(bool fAltMod);
	void OnItemContextMenu(QPoint pt);

private slots:

	void OnFileChanged(const QString& sPath);

	void OnVScrollValuechanged(int nValue);
	void OnVScrollSliderMoved(int nValue);
	void OnVScrollActionTriggered(int nAction);

	void OnHeaderContextMenu(QPoint pt);
	void OnHeaderSectionDoubleClicked(int logicalIndex);
	void OnHeaderSectionResized(int logicalIndex, int oldSize, int newSize);

	void OnItemContextMenuClicked(QPoint pt);
	void OnItemClicked(const QModelIndex &index);
	void OnItemDoubleClicked(const QModelIndex &index);

	void OnNavigate(int eType);

	void OnSearchTriggered();

};

#endif
