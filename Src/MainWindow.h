//
//  @file MainWindow.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDockWidget>
#include "LogUI/LogWidget.h"
#include "LogUI/LogFile.h"
#include "LogUI/LogTheme.h"
#include "AppConfig.h"

namespace Ui {
	class CMainWindow;
}

class CMainWindow : public QMainWindow
{

	Q_OBJECT

public:

	explicit CMainWindow(QWidget *parent = nullptr);
	~CMainWindow();

private:

	Ui::CMainWindow *ui;

	QFont m_pFontMain;
	QFont m_pFontFixed;

	QMenu *m_pCtxMenuFilter;
	QMenu *m_pCtxMenuResult;

	CLogWidget *m_pLogWidget;

	QAction* m_pFilterAction[FILTER_COUNT];
	QDockWidget* m_pFilterView[FILTER_COUNT];

	void UpdateConfig();

	QDockWidget *AppendFilter(QString sName, bool fVisible, QByteArray &pGeometry, QByteArray pHeader, int nViewID, bool fFilterBar, CLogWidget *pFiltered);

	CLogWidget *FindFocusedLogWidget();

	QString PromptFilePath(bool fSave);

private slots:

	void OnFileOpen();
	void OnFileSave();
	void OnFileTheme();
	void OnFileSync();
	void OnFileSettings();
	void OnFileExit();

	void OnEditClear();
	void OnEditCopySelected();
	void OnEditCopyResults();
	void OnEditAddToResults();
	void OnEditMessage();
	void OnEditAltAction(bool fAltMode);

	void OnViewFilterChecked();
	void OnViewFilterChanged(bool visible);

	void OnHelpAbout();

	void OnHeaderSectionResized(int iSection, int nSize);
	void OnItemContextMenu(QPoint pt);

};

#endif
