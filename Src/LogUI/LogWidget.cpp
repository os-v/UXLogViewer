//
//  @file LogWidget.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogWidget.h"
#include "ui_LogWidget.h"
#include "LogTheme.h"
#include "LogModel.h"
#include "../AppConfig.h"
#include <QScrollBar>
#include <QLineEdit>
#include <QClipboard>
#include <QMenu>

#define QAbstractSlider_SliderMoved				10
#define QAbstractSlider_SliderSkipAction		11

CLogWidget::CLogWidget(QWidget *parent, int nID, bool fSearchBar, CLogWidget *pFiltered) :
	QWidget(parent),
	ui(new Ui::CLogWidget)
{

	m_nAction = QAbstractSlider::SliderNoAction;

	ui->setupUi(this);

	m_nViewID = nID;

	m_pFiltered = pFiltered;
	m_pLogFileRaw = new CLogFileRaw();
	m_pLogFileFlt = new CLogFileFlt();
	m_pLogModel = new CLogModel(GetLogFile());

	m_nSelected = -1;

	ui->m_pTreeView->setModel(m_pLogModel);
	//ui->m_pTreeView->setSelectionModel(new CLogModelSel(m_pLogFileRaw));

	ui->m_pTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->m_pTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);

	ui->m_pTreeView->header()->setSectionsClickable(true);
	//ui->m_pTreeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	connect(ui->m_pTreeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(OnVScrollValuechanged(int)), Qt::QueuedConnection);
	connect(ui->m_pTreeView->verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(OnVScrollSliderMoved(int)), Qt::QueuedConnection);
	connect(ui->m_pTreeView->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(OnVScrollActionTriggered(int)), Qt::QueuedConnection);

	connect(ui->m_pTreeView->header(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnHeaderContextMenu(QPoint)));
	connect(ui->m_pTreeView->header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(OnHeaderSectionDoubleClicked(int)));
	connect(ui->m_pTreeView->header(), SIGNAL(sectionResized(int, int, int)), this, SLOT(OnHeaderSectionResized(int, int, int)));

	connect(ui->m_pTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnItemContextMenuClicked(QPoint)));
	connect(ui->m_pTreeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnItemClicked(const QModelIndex&)), Qt::QueuedConnection);
	connect(ui->m_pTreeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(OnItemDoubleClicked(const QModelIndex&)), Qt::QueuedConnection);

	connect(ui->m_pTreeView, SIGNAL(OnNavigate(int)), this, SLOT(OnNavigate(int)));

	connect(ui->m_pComboSearch->lineEdit(), SIGNAL(returnPressed()), this, SLOT(OnSearchTriggered()), Qt::QueuedConnection);

	ui->m_pComboSearch->lineEdit()->setPlaceholderText("Please enter filter");
	ui->m_pComboSearch->addItems(CAppConfig::Instance().GetFilterSearch(m_nViewID));

	if(!fSearchBar)
		ui->m_pComboSearch->setVisible(false);

	//ui->m_pComboSearch->lineEdit()->setInputMethodHints(Qt::ImhLowercaseOnly);//ImhPreferLowercase);

}

CLogWidget::~CLogWidget()
{

	delete ui;

	SafeDelete(m_pLogFileRaw);
	SafeDelete(m_pLogFileFlt);
	SafeDelete(m_pLogModel);

}

void CLogWidget::UpdateFont()
{

	ui->m_pTreeView->setFont(*FontGet(true));

}

QByteArray CLogWidget::SaveState()
{
	return ui->m_pTreeView->header()->saveState();
}

bool CLogWidget::RestoreState(QByteArray pState)
{
	return ui->m_pTreeView->header()->restoreState(pState);
}

void CLogWidget::ResizeColumnsToContents()
{

	QHeaderView *pHeader = ui->m_pTreeView->header();

	pHeader->resizeSections(QHeaderView::ResizeToContents);

	int iLastColumn = pHeader->count() - 1;
	int nSizeHint = pHeader->sizeHintForColumn(iLastColumn);
	pHeader->resizeSection(iLastColumn, nSizeHint);
	ui->m_pTreeView->resizeColumnToContents(iLastColumn);

}

void CLogWidget::ResizeColumn(int iSection, int nSize)
{

	QHeaderView *pHeader = ui->m_pTreeView->header();
	pHeader->resizeSection(iSection, nSize);

}

void CLogWidget::ResetData()
{

	if(m_pLogFileRaw)
		m_pLogFileRaw->Reset();

	if(m_pLogFileFlt)
		m_pLogFileFlt->Reset();

	m_pLogModel->UpdateView(0, DEFAULT_ROWSCOUNT);
	int nFrameSize = ui->m_pTreeView->GetVisibleRowsCount();

	if(m_nViewID == FILTER_RESULT)
		m_pLogFileFlt->Create(m_pFiltered->GetLogFile(), "", nFrameSize, true);
	else
		m_pLogFileFlt->SetFrameSize(nFrameSize);

	m_pLogModel->UpdateView();

}

void CLogWidget::ThemeUpdated(QByteArray pState)
{

	m_pLogFileRaw->UpdateTheme(&CLogTheme::Instance());
	m_pLogFileFlt->UpdateTheme(&CLogTheme::Instance());

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();
	int nScrollPos = pScroll->value();

	m_pLogModel->UpdateHeader();
	//m_pLogModel->UpdateView();

	QHeaderView *pHeader = ui->m_pTreeView->header();
	if(pState.size())
		pHeader->restoreState(pState);
	else
		pHeader->resizeSections(QHeaderView::ResizeToContents);

	GetLogFile()->ReadFrame();

	int nSelected = GetLogFile()->FindRecord(m_nSelected);
	if(nSelected != -1)
	{
		QModelIndex pIndex = m_pLogModel->index(nSelected, 0);
		ui->m_pTreeView->selectionModel()->setCurrentIndex(pIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Select);
	}

}

void CLogWidget::LoadFile(QString sFileName)
{

	LogMessage("CLogWidget::LoadFile()");

	m_pLogFileFlt->Reset();

	m_pLogModel->UpdateView(0, DEFAULT_ROWSCOUNT);
	int nFrameSize = ui->m_pTreeView->GetVisibleRowsCount();
	//m_pLogModel->UpdateView();

	m_pLogFileRaw->Destroy();
	m_pLogFileRaw->Create(sFileName, nFrameSize, &CLogTheme::Instance());
	m_pLogFileRaw->MoveTo(0);
	m_pLogModel->UpdateView(m_pLogFileRaw);

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();
	pScroll->setValue(m_pLogFileRaw->GetRecord());

	//ui->m_pTreeView->resizeColumnsToContents();

}

void CLogWidget::SaveFile(QString sFileName)
{

	GetLogFile()->SaveFile(sFileName);

}

bool CLogWidget::ApplyFilter(QString sFilter)
{

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();

	pScroll->setValue(0);

	m_pLogModel->UpdateView(0, DEFAULT_ROWSCOUNT);
	int nFrameSize = ui->m_pTreeView->GetVisibleRowsCount();

	if(sFilter == "" || !m_pLogFileFlt->Create(m_pFiltered ? m_pFiltered->GetLogFile() : m_pLogFileRaw, sFilter, nFrameSize, false))
	{
		m_pLogFileFlt->Reset();
		m_pLogFileRaw->MoveTo(0);
		m_pLogModel->UpdateView(m_pLogFileRaw);
		return false;
	}

	m_pLogFileFlt->MoveTo(0);
	m_pLogModel->UpdateView(m_pLogFileFlt);

	return true;
}

void CLogWidget::SelectItem(qint64 nROffset, qint64 nVOffset)
{

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();

	ILogFile *pLogFile = GetLogFile();

	int nRecord = pLogFile->FindRecord(nROffset);
	if(nRecord == -1)
	{
		int nScrollPos = pLogFile->MoveAt(nVOffset == -1 ? nROffset : nVOffset);
		if(nScrollPos <= pScroll->maximum() - pLogFile->GetFrameSize())
			nScrollPos = pLogFile->MoveOn(-pLogFile->GetFrameSize() / 2);
		pScroll->setValue(nScrollPos);
		nRecord = pLogFile->FindRecord(nROffset);
	}

	QModelIndex pIndex = m_pLogModel->index(nRecord, 0);
	ui->m_pTreeView->selectionModel()->setCurrentIndex(pIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Select);

	m_nSelected = nROffset;
	LogMessage("SelectItem(%d)", (int)m_nSelected);

	if(m_pFiltered)
		OnItemDoubleClicked(pIndex);

	ui->m_pTreeView->Repaint();

}

void CLogWidget::CopyToClipboard(bool fSelectedOnly)
{

	if(fSelectedOnly)
	{

		QString sLine;
		QString *pLine = &sLine;
		GetLogFile()->PollRecords(&m_nSelected, &pLine, 0, 1);

		QApplication::clipboard()->clear();
		QApplication::clipboard()->setText(sLine);

	}
	else if(m_nViewID == FILTER_RESULT)
	{

		QStringList pRecords;
		m_pLogFileFlt->EnumRecords(pRecords);

		QString sRecords = pRecords.join("\r\n");

		QApplication::clipboard()->clear();
		QApplication::clipboard()->setText(sRecords);

	}

}

void CLogWidget::AddToResults(CLogWidget *pActive)
{

	qint64 nSelected = pActive->GetSelected();

	QBuffer *pBuffer = m_pLogFileFlt->GetBuffer();

	if(pBuffer)
	{
		m_pLogFileFlt->InsertRecord(nSelected);
		int nRowsCount = ui->m_pTreeView->GetVisibleRowsCount();
		if(m_pLogFileFlt->GetFrameSize() != nRowsCount)
		{
			QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();
			m_pLogFileFlt->SetFrameSize(nRowsCount);
			m_pLogFileFlt->MoveTo(pScroll->value());
		}
		else
			m_pLogFileFlt->ReadFrame();
		m_pLogModel->UpdateView(m_pLogFileFlt);
	}

}

QString CLogWidget::GetSelectedMessage()
{

	CLogTheme *pTheme = GetLogFile()->GetTheme();

	QString *pLine = new QString[pTheme->GetColCount()];
	GetLogFile()->PollRecords(&m_nSelected, &pLine, pTheme, 1);

	QString sResult = pLine[pTheme->GetColCount() - 1];

	delete[] pLine;

	return sResult;
}

void CLogWidget::resizeEvent(QResizeEvent *event)
{

	QWidget::resizeEvent(event);

	ILogFile *pLogFile = GetLogFile();
	if(!pLogFile)
		return;

	int nRowsCount = ui->m_pTreeView->GetVisibleRowsCount();
	if(pLogFile->GetFrameSize() != nRowsCount)
	{
		QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();
		pLogFile->SetFrameSize(nRowsCount);
		pLogFile->ReadFrame();
	}

}

void CLogWidget::OnVScrollValuechanged(int nValue)
{

	LogMessage("OnVScrollValuechanged(%d, %d)", nValue, m_nAction);

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();

	ILogFile *pLogFile = GetLogFile();
	if(!pLogFile)
		return;

	int nOffset = pScroll->value();
	int nMove = (int)((int)nOffset - (int)pLogFile->GetRecord());
	if(!nMove)
	{
		if(!nOffset && pLogFile->GetOffset())
			nMove = -1;
		else
		{
			m_nAction = QAbstractSlider::SliderNoAction;
			return;
		}
	}

	LogMessage("OnVScrollValuechanged() -> off:%d, record:%d", nOffset, pLogFile->GetRecord());
	if(m_nAction == QAbstractSlider::SliderPageStepAdd || m_nAction == QAbstractSlider::SliderPageStepSub)
		nOffset = pLogFile->MoveOn(m_nAction == QAbstractSlider::SliderPageStepAdd ? pScroll->pageStep() : -pScroll->pageStep());
	else if(m_nAction == QAbstractSlider_SliderMoved/* || m_nAction == QAbstractSlider::SliderNoAction*/)
		nOffset = pLogFile->MoveTo(nOffset);
	else if(m_nAction == QAbstractSlider::SliderNoAction || m_nAction == QAbstractSlider::SliderMove || m_nAction == QAbstractSlider::SliderSingleStepAdd || m_nAction == QAbstractSlider::SliderSingleStepSub)
		nOffset = pLogFile->MoveOn(nMove);
	//else if(m_nAction == QAbstractSlider::SliderToMinimum)
	//else if(m_nAction == QAbstractSlider::SliderToMaximum)

	LogMessage("OnVScrollValuechanged() -> off:%d, record:%d, readed:%d", nOffset, pLogFile->GetRecord(), pLogFile->GetReaded());

	pScroll->setValue(nOffset);

	int nSelected = pLogFile->FindRecord(m_nSelected);
	quint64 nPrevSelected = m_nSelected;
	LogMessage("OnVScrollValuechanged() -> sel:%d, msel:%d", nSelected, (int)m_nSelected);
	QModelIndex pIndex = m_pLogModel->index(nSelected != -1 ? nSelected : -1, 0);
	ui->m_pTreeView->selectionModel()->setCurrentIndex(pIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Select);
	m_nSelected = nPrevSelected;
	LogMessage("OnVScrollValuechanged() -> %d", (int)nPrevSelected);

	//if(m_nAction != QAbstractSlider_SliderMoved)// && m_nAction != QAbstractSlider::SliderPageStepAdd && m_nAction != QAbstractSlider::SliderPageStepSub)
		m_nAction = QAbstractSlider::SliderNoAction;

	ui->m_pTreeView->Repaint();

}

void CLogWidget::OnVScrollSliderMoved(int nValue)
{

	LogMessage("OnVScrollSliderMoved(%d)", nValue);

	m_nAction = QAbstractSlider_SliderMoved;

	OnVScrollValuechanged(nValue);

}

void CLogWidget::OnVScrollActionTriggered(int nAction)
{

	LogMessage("OnVScrollActionTriggered(%d, %d)", nAction, m_nAction);

	if(m_nAction != QAbstractSlider_SliderMoved && m_nAction != QAbstractSlider::SliderPageStepAdd && m_nAction != QAbstractSlider::SliderPageStepSub)
		m_nAction = nAction;
	else
		m_nAction = QAbstractSlider_SliderSkipAction;

}

void CLogWidget::OnHeaderContextMenu(QPoint pt)
{

	QHeaderView *pHeader = ui->m_pTreeView->header();

	pt = pHeader->mapToGlobal(pt);

	QMenu *pMenu = new QMenu();
	for(int iColumn = 0; iColumn < GetLogModel()->columnCount(); iColumn++)
	{
		QAction *pAction = pMenu->addAction(GetLogModel()->headerData(iColumn, Qt::Horizontal).toString());
		pAction->setCheckable(true);
		pAction->setChecked(!pHeader->isSectionHidden(iColumn));
		pAction->setData(iColumn);
	}
	pMenu->addSeparator();
	pMenu->addAction("Reset")->setData(-1);

	QAction *pAction = pMenu->exec(pt);
	if(pAction)
	{
		int iSection = pAction->data().toInt();
		if(iSection == -1)
		{
			for(int iColumn = 0; iColumn < GetLogModel()->columnCount(); iColumn++)
				pHeader->showSection(iColumn);
			pHeader->resizeSections(QHeaderView::ResizeToContents);
			return;
		}
		else if(pHeader->isSectionHidden(iSection))
			pHeader->showSection(iSection);
		else
			pHeader->hideSection(iSection);
	}

	for(int iColumn = 0; iColumn < GetLogModel()->columnCount(); iColumn++)
	{
		if(!pHeader->isSectionHidden(iColumn))
			return;
	}

	for(int iColumn = 0; iColumn < GetLogModel()->columnCount(); iColumn++)
		pHeader->showSection(iColumn);

}

void CLogWidget::OnHeaderSectionDoubleClicked(int logicalIndex)
{

	QHeaderView *pHeader = ui->m_pTreeView->header();
	if(logicalIndex == pHeader->count() - 1)
	{
		int nSizeHint = pHeader->sizeHintForColumn(logicalIndex);
		pHeader->resizeSection(logicalIndex, nSizeHint);
	}
	ui->m_pTreeView->resizeColumnToContents(logicalIndex);

}

void CLogWidget::OnHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{

	emit OnHeaderSectionResized(logicalIndex, newSize);

}

void CLogWidget::OnItemContextMenuClicked(QPoint pt)
{

	pt = ui->m_pTreeView->mapToGlobal(pt);

	emit OnItemContextMenu(pt);

}

void CLogWidget::OnItemClicked(const QModelIndex &index)
{

	ILogFile *pLogFile = GetLogFile();

	int nCurrentRow = index.row();
	qint64 nSelected = pLogFile->GetFrameOffset(nCurrentRow);

	m_nSelected = nSelected;

	LogMessage("OnItemClicked(row:%d, record:%d, readed:%d, sel:%d)", nCurrentRow, pLogFile->GetRecord(), pLogFile->GetReaded(), (int)m_nSelected);

}

void CLogWidget::OnItemDoubleClicked(const QModelIndex &index)
{

	if(QApplication::queryKeyboardModifiers() & Qt::AltModifier)
	{
		emit OnItemAltAction(true);
		return;
	}

	if(!m_pFiltered)
	{
		OnItemAltAction(false);
		return;
	}

	ILogFile *pLogFile = GetLogFile();
	bool fVOffset = pLogFile->IsFilter(true);
	qint64 nROffset = pLogFile->GetFrameOffset(index.row(), false);
	qint64 nVOffset = fVOffset ? pLogFile->GetFrameOffset(index.row(), true) : -1;
	m_pFiltered->SelectItem(nROffset, nVOffset);

}

void CLogWidget::OnNavigate(int eType)
{

	LogMessage("OnNavigate: %d", eType);

	ILogFile *pLogFile = GetLogFile();

	QModelIndex pCurrent = ui->m_pTreeView->currentIndex();

	QScrollBar *pScroll = ui->m_pTreeView->verticalScrollBar();

	int nOffset = pScroll->value();
	int nCurrent = pCurrent.row() - pLogFile->GetRecord();
	int nLastPage = pLogFile->GetSize() - pScroll->pageStep();

	LogMessage("\trow:%d, crow:%d, record:%d, readed:%d, last:%d", pCurrent.row(), nCurrent, pLogFile->GetRecord(), pLogFile->GetReaded(), nLastPage);
	
	if(eType == CListView::ENavTypeLineDown)
		nCurrent = nCurrent >= pLogFile->GetReaded() - 2 ? (nOffset = pLogFile->MoveOn(1)) + nCurrent : pCurrent.row() + 1;
	else if(eType == CListView::ENavTypeLineUp)
		nCurrent = !nCurrent ? (nOffset = pLogFile->MoveOn(-1)) : pCurrent.row() - 1;
	else if(eType == CListView::ENavTypePageDown)
		nCurrent = (nOffset = pLogFile->MoveOn(pScroll->pageStep())) + nCurrent;
	else if(eType == CListView::ENavTypePageUp)
		nCurrent = (nOffset = pLogFile->MoveOn(-pScroll->pageStep())) + nCurrent;
	else if(eType == CListView::ENavTypeEnd)
		nCurrent = (nOffset = pLogFile->MoveTo(nLastPage)) + nCurrent;
	else if(eType == CListView::ENavTypeHome)
		nCurrent = (nOffset = pLogFile->MoveAt(0)) + nCurrent;

	LogMessage("\toff:%d, crow:%d, record:%d, readed:%d", nOffset, nCurrent, pLogFile->GetRecord(), pLogFile->GetReaded());

	if(nOffset != pScroll->value())
		pScroll->setValue(nOffset);

	int nSelected = pLogFile->GetFrameOffset(nCurrent);
	if(!nSelected && nCurrent)
		return;

	m_nSelected = nSelected;
	LogMessage("\tselected:%d", m_nSelected);

	QModelIndex pIndex = m_pLogModel->index(nCurrent, 0);
	ui->m_pTreeView->selectionModel()->setCurrentIndex(pIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows | QItemSelectionModel::Select);
	ui->m_pTreeView->Repaint();

}

void CLogWidget::OnSearchTriggered()
{

	QString sFilter = ui->m_pComboSearch->lineEdit()->text();

	if(ApplyFilter(sFilter))
	{
		QStringList &pList = CAppConfig::Instance().GetFilterSearch(m_nViewID);
		for(int iFilter = 0; iFilter < pList.length(); iFilter++)
		{
			if(pList[iFilter] == sFilter)
			{
				pList.removeAt(iFilter);
				ui->m_pComboSearch->removeItem(iFilter);
				break;
			}
		}
		CAppConfig::Instance().GetFilterSearch(m_nViewID).push_front(sFilter);
		ui->m_pComboSearch->insertItem(0, sFilter);
		ui->m_pComboSearch->setCurrentIndex(0);
		if(CAppConfig::IsMobile)
			ui->m_pTreeView->setFocus();
	}

}

