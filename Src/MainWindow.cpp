//
//  @file MainWindow.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QProgressDialog>
#include <QFileInfo>
#include <QStyle>
#include "LogUI/LogFile.h"
#include "LogUI/LogModel.h"
#include "Dialogs/FileDialog.h"
#include "Dialogs/ThemeDialog.h"
#include "Dialogs/SyncDialog.h"
#include "Dialogs/SettingsDialog.h"
#include "Dialogs/MessageDialog.h"
#include "Dialogs/AboutDialog.h"
#include "ProductInfo.h"
#include <QStandardPaths>
#include <QScreen>

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow)
{

	CAppConfig &pConfig = CAppConfig::Instance();

	ui->setupUi(this);

	m_pCtxMenuFilter = new QMenu();
	m_pCtxMenuFilter->addAction(ui->m_pMenuBarEditCopySelected);
	m_pCtxMenuFilter->addAction(ui->m_pMenuBarEditAddToResults);

	m_pCtxMenuResult = new QMenu();
	m_pCtxMenuResult->addAction(ui->m_pMenuBarEditCopySelected);
	m_pCtxMenuResult->addAction(ui->m_pMenuBarEditCopyResults);

	m_pLogWidget = new CLogWidget();
	this->setCentralWidget(m_pLogWidget);

	QAction* pFilterAction[FILTER_COUNT] = { ui->m_pMenuBarViewResultView, ui->m_pMenuBarViewFilterAView, ui->m_pMenuBarViewSubFilterAView, ui->m_pMenuBarViewFilterBView, ui->m_pMenuBarViewSubFilterBView };
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		m_pFilterAction[iFilter] = pFilterAction[iFilter];
		connect(m_pFilterAction[iFilter], SIGNAL(triggered()), this, SLOT(OnViewFilterChecked()));
	}

	static QString sFilterName[] = { "Result View", "Main Filter A", "Sub Filter A", "Main Filter B", "Sub Filter B" };
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		CLogWidget *pFiltered = iFilter == FILTER_MAIN_A || iFilter == FILTER_MAIN_B ? m_pLogWidget : (iFilter > 0 ? (CLogWidget*)m_pFilterView[iFilter - 1]->widget() : m_pLogWidget);
		m_pFilterView[iFilter] = AppendFilter(sFilterName[iFilter], pConfig.FilterState[iFilter], pConfig.FilterGeometry[iFilter], pConfig.GetFilterHeader(iFilter), iFilter, iFilter != 0, pFiltered);
		connect(m_pFilterView[iFilter], SIGNAL(visibilityChanged(bool)), this, SLOT(OnViewFilterChanged(bool)));
	}

	int nToolBarHeight = ScaleUI(ui->m_pToolBar->iconSize().width());
	//ui->m_pToolBar->setFixedHeight(nToolBarHeight);
	ui->m_pToolBar->setIconSize(QSize(nToolBarHeight, nToolBarHeight));
	//ui->m_pToolBar->setFixedHeight(128);
	//ui->m_pToolBar->setIconSize(QSize(128, 128));

	connect(ui->m_pMenuBarFileOpen, SIGNAL(triggered()), this, SLOT(OnFileOpen()));
	connect(ui->m_pMenuBarFileSave, SIGNAL(triggered()), this, SLOT(OnFileSave()));
	connect(ui->m_pMenuBarFileChangeTheme, SIGNAL(triggered()), this, SLOT(OnFileTheme()));
	connect(ui->m_pMenuBarFileSyncTheme, SIGNAL(triggered()), this, SLOT(OnFileSync()));
	connect(ui->m_pMenuBarFileSettings, SIGNAL(triggered()), this, SLOT(OnFileSettings()));
	connect(ui->m_pMenuBarFileExit, SIGNAL(triggered()), this, SLOT(OnFileExit()));

	connect(ui->m_pMenuBarEditClear, SIGNAL(triggered()), this, SLOT(OnEditClear()));
	connect(ui->m_pMenuBarEditCopySelected, SIGNAL(triggered()), this, SLOT(OnEditCopySelected()));
	connect(ui->m_pMenuBarEditCopyResults, SIGNAL(triggered()), this, SLOT(OnEditCopyResults()));
	connect(ui->m_pMenuBarEditAddToResults, SIGNAL(triggered()), this, SLOT(OnEditAddToResults()));
	connect(ui->m_pMenuBarEditMessage, SIGNAL(triggered()), this, SLOT(OnEditMessage()));

	connect(ui->m_pMenuBarHelpAbout, SIGNAL(triggered()), this, SLOT(OnHelpAbout()));

	connect(m_pLogWidget, SIGNAL(OnHeaderSectionResized(int, int)), this, SLOT(OnHeaderSectionResized(int, int)));
	connect(m_pLogWidget, SIGNAL(OnItemAltAction(bool)), this, SLOT(OnEditAltAction(bool)));
	connect(m_pLogWidget, SIGNAL(OnItemContextMenu(QPoint)), this, SLOT(OnItemContextMenu(QPoint)));
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		if(iFilter >= FILTER_MAIN_A)
			connect(m_pFilterView[iFilter]->widget(), SIGNAL(OnItemAltAction(bool)), this, SLOT(OnEditAltAction(bool)));
		connect(m_pFilterView[iFilter]->widget(), SIGNAL(OnItemContextMenu( QPoint)), this, SLOT(OnItemContextMenu(QPoint)));
	}

	if(!pConfig.IsLoaded())
	{
		if(!CAppConfig::IsMobile)
			showMaximized();
		for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		{
			m_pFilterView[iFilter]->setVisible(true);
			if(iFilter)
				QMainWindow::tabifyDockWidget(m_pFilterView[iFilter - 1], m_pFilterView[iFilter]);
		}
		m_pFilterView[FILTER_MAIN_A]->raise();
	}
	else
	{
		if(!CAppConfig::IsMobile)
			restoreGeometry(pConfig.MainGeometry);
		restoreState(pConfig.MainState);
		m_pLogWidget->RestoreState(pConfig.GetMainHeader());
		m_pLogWidget->SetLastColumnWidth(pConfig.ColumnWidth);
		for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
			m_pFilterView[iFilter]->setVisible(pConfig.FilterState[iFilter]);
	}

	FontSet(this, true);

	m_pLogWidget->Invalidate();
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->Invalidate();

	setWindowTitle(QString(PRODUCT_PRODUCTNAME " v") + PRODUCT_SVERSION);

	if(CAppConfig::IsMobile)
	{
		menuBar()->hide();
		statusBar()->hide();
		ui->m_pToolBar->setMovable(false);
		//resize(QGuiApplication::primaryScreen()->size());
	}

	m_pLogWidget->setFocus();

	if(CAppConfig::Instance().FileReopen && !CAppConfig::Instance().LastFilePath.isEmpty())
		OpenFile(CAppConfig::Instance().LastFilePath);

}

CMainWindow::~CMainWindow()
{

	UpdateConfig();

	delete ui;

}

void CMainWindow::UpdateConfig()
{

	CAppConfig &pConfig = CAppConfig::Instance();

	pConfig.MainGeometry = saveGeometry();
	pConfig.MainState = saveState();
	if(pConfig.MainHeader.count() <= pConfig.ThemeSelected)
		for(int iTheme = pConfig.MainHeader.count() - 1; iTheme < pConfig.ThemeSelected; iTheme++, pConfig.MainHeader.push_back(QByteArray()));
	pConfig.MainHeader[pConfig.ThemeSelected] = m_pLogWidget->SaveState();

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		pConfig.FilterState[iFilter] = !m_pFilterView[iFilter]->isHidden();
		pConfig.FilterGeometry[iFilter] = m_pFilterView[iFilter]->saveGeometry();
		pConfig.FilterHeader[iFilter][pConfig.ThemeSelected] = ((CLogWidget*)m_pFilterView[iFilter]->widget())->SaveState();
	}

	pConfig.ThemeDefs = CLogTheme::Instance().GetThemesDefs();

}

QDockWidget *CMainWindow::AppendFilter(QString sName, bool fVisible, QByteArray &pGeometry, QByteArray pHeader, int nViewID, bool fFilterBar, CLogWidget *pFiltered)
{

	CLogWidget *pLogWidget = 0;
	QDockWidget *pView = new QDockWidget(this);
	pView->setWindowTitle(sName);
	pView->setObjectName(sName);
	pView->setWidget(pLogWidget = new CLogWidget(pView, nViewID, fFilterBar, pFiltered));
	//pView->setFloating(true);
	//pView->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	if(CAppConfig::IsMobile)
		pView->setFeatures(QDockWidget::NoDockWidgetFeatures);

	this->addDockWidget(Qt::BottomDockWidgetArea, pView, Qt::Vertical);

	//pView->restoreGeometry(pGeometry);
	pLogWidget->RestoreState(pHeader);

	//pView->setVisible(fVisible);

	return pView;
}

CLogWidget *CMainWindow::FindFocusedLogWidget()
{

	QWidget *pFocused = QApplication::focusWidget();

	if(pFocused && !qobject_cast<CLogWidget*>(pFocused))
		pFocused = pFocused->parentWidget();

	if(!qobject_cast<CLogWidget*>(pFocused))
		return m_pLogWidget;

	return (CLogWidget*)pFocused;
}

QString CMainWindow::PromptFilePath(bool fSave)
{

	bool fStaticFolder = false;
	bool fUseCustomDialog = fSave ? CAppConfig::Instance().FileSaveUI : CAppConfig::Instance().FileOpenUI;

	QString sFolder = CAppConfig::Instance().RecentFolder;

#ifdef Q_OS_IOS
	if(fSave)
	{
		fStaticFolder = true;
		fUseCustomDialog = true;
		sFolder = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).last();
	}
#endif
	
	if(!fUseCustomDialog && fSave)
		return QFileDialog::getSaveFileName(this, QString(), sFolder);

	if(!fUseCustomDialog && !fSave)
		return QFileDialog::getOpenFileName(this, QString(), sFolder);

	return CFileDialog::PromptFileName(this, fSave, QString(), sFolder, fStaticFolder);
}

void CMainWindow::OpenFile(const QString& sFileName)
{

	LogMessage("CMainWindow::OpenFile() -> %ls", sFileName.utf16());

	if(sFileName.isEmpty())
		return;

	QFileInfo pFileInfo(sFileName);
	CAppConfig::Instance().LastFilePath = sFileName;
	CAppConfig::Instance().RecentFolder = pFileInfo.absolutePath();
	CAppConfig::Instance().ColumnWidth = m_pLogWidget->GetLastColumnWidth();
	CAppConfig::Instance().Save();

	m_pLogWidget->ResetData();
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->ResetData();

	m_pLogWidget->LoadFile(sFileName);

	setWindowTitle(PRODUCT_PRODUCTNAME " v" PRODUCT_SVERSION " [" + pFileInfo.fileName() + "]");

}

void CMainWindow::OnFileOpen()
{

	QString sFileName = PromptFilePath(false);

	OpenFile(sFileName);

}

void CMainWindow::OnFileSave()
{

	QString sFileName = PromptFilePath(true);
	if(sFileName.isEmpty())
		return;

	FindFocusedLogWidget()->SaveFile(sFileName);

}

void CMainWindow::OnFileTheme()
{

	UpdateConfig();

	CThemeDialog pDlg;
	//pDlg.setWindowModality(Qt::WindowModal);
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();
	else
		pDlg.show();

	int eResult = pDlg.exec();
	if(eResult != QDialog::Accepted)
		return;

	CAppConfig &pConfig = CAppConfig::Instance();

	m_pLogWidget->ThemeUpdated(pConfig.GetMainHeader(), pConfig.ColumnWidth);

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->ThemeUpdated(pConfig.GetFilterHeader(iFilter), pConfig.ColumnWidth);

}

void CMainWindow::OnFileSync()
{

	CSyncDialog pDlg;
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();
	else
		pDlg.show();

	int eResult = pDlg.exec();
	if(eResult != QDialog::Accepted)
		return;

	CAppConfig &pConfig = CAppConfig::Instance();

	m_pLogWidget->ThemeUpdated(pConfig.GetMainHeader(), pConfig.ColumnWidth);

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->ThemeUpdated(pConfig.GetFilterHeader(iFilter), pConfig.ColumnWidth);

}

void CMainWindow::OnFileSettings()
{

	CSettingsDialog pDlg;
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();
	else
		pDlg.show();

	int eResult = pDlg.exec();
	if(eResult != QDialog::Accepted)
		return;

	FontInit();
	FontSet(this, true);
	m_pLogWidget->SetLastColumnWidth(CAppConfig::Instance().ColumnWidth);
	m_pLogWidget->Invalidate();
	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->Invalidate();

}

void CMainWindow::OnFileExit()
{

	close();

}

void CMainWindow::OnEditClear()
{

	CLogWidget *pWidget = FindFocusedLogWidget();

	for(int iFilter = FILTER_COUNT - 1; pWidget == m_pLogWidget && iFilter >= 0; iFilter--)
		((CLogWidget*)m_pFilterView[iFilter]->widget())->ResetData();

	pWidget->ResetData();

}

void CMainWindow::OnEditCopySelected()
{

	FindFocusedLogWidget()->CopyToClipboard(true);

}

void CMainWindow::OnEditCopyResults()
{

	((CLogWidget*)m_pFilterView[FILTER_RESULT]->widget())->CopyToClipboard(false);

}

void CMainWindow::OnEditAddToResults()
{

	((CLogWidget*)m_pFilterView[FILTER_RESULT]->widget())->AddToResults(FindFocusedLogWidget());

}

void CMainWindow::OnEditMessage()
{

	CMessageDialog pDlg(FindFocusedLogWidget()->GetSelectedMessage());
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();
	else
		pDlg.show();

	pDlg.exec();

}

void CMainWindow::OnEditAltAction(bool fAltMode)
{

	if(fAltMode)
		OnEditAddToResults();
	else
		OnEditMessage();

}

void CMainWindow::OnViewFilterChecked()
{

	int iAction = ArrayFindIndex<QAction*>(m_pFilterAction, FILTER_COUNT, (QAction*)sender());
	if(iAction != -1)
		m_pFilterView[iAction]->setVisible(m_pFilterAction[iAction]->isChecked());

}

void CMainWindow::OnViewFilterChanged(bool visible)
{

	int iFilter = ArrayFindIndex<QDockWidget*>(m_pFilterView, FILTER_COUNT, (QDockWidget*)sender());
	if(iFilter == -1)
		return;

	bool fChecked = m_pFilterView[iFilter]->toggleViewAction()->isChecked();
	if(fChecked  != m_pFilterAction[iFilter]->isChecked())
		m_pFilterAction[iFilter]->setChecked(fChecked);

}

void CMainWindow::OnHelpAbout()
{

	CAboutDialog pDlg;
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();
	else
		pDlg.show();

	pDlg.exec();

}

void CMainWindow::OnHeaderSectionResized(int iSection, int nSize)
{

	CAppConfig::Instance().ColumnWidth = m_pLogWidget->GetLastColumnWidth();

	if(sender() != m_pLogWidget)
		((CLogWidget*)m_pLogWidget)->ResizeColumn(iSection, nSize);

	for(int iFilter = FILTER_MAIN_A; iFilter < FILTER_COUNT; iFilter++)
	{
		if(sender() != m_pFilterView[iFilter]->widget())
			((CLogWidget*)m_pFilterView[iFilter]->widget())->ResizeColumn(iSection, nSize);
	}

}

void CMainWindow::OnItemContextMenu(QPoint pt)
{

	CLogWidget *pWidget = (CLogWidget*)sender();
	if(pWidget == ((CLogWidget*)m_pFilterView[FILTER_RESULT]->widget()))
		m_pCtxMenuResult->popup(pt);
	else
		m_pCtxMenuFilter->popup(pt);

}

