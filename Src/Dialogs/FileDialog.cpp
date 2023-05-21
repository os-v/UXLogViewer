//
//  @file FileDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 02.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "FileDialog.h"
#include "ui_FileDialog.h"
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QScroller>
#include <QMessageBox>
#include "../Utils.h"
#include "../AppConfig.h"

class CFileSortFilterProxyModel : public QSortFilterProxyModel
{
public:
	CFileSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}
protected:
	virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const
	{
		if (sortColumn() == 0)
		{
			QFileSystemModel *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
			bool asc = sortOrder() == Qt::AscendingOrder ? true : false;
			QFileInfo leftFileInfo  = fsm->fileInfo(left);
			QFileInfo rightFileInfo = fsm->fileInfo(right);
			if (sourceModel()->data(left).toString() == "..")
				return asc;
			if (sourceModel()->data(right).toString() == "..")
				return !asc;
			if (!leftFileInfo.isDir() && rightFileInfo.isDir())
				return !asc;
			if (leftFileInfo.isDir() && !rightFileInfo.isDir())
				return asc;
		}
		return QSortFilterProxyModel::lessThan(left, right);
	}
};

CFileDialog::CFileDialog(bool fSave, QString sPath, bool fStaticFolder, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CFileDialog)
{

	ui->setupUi(this);

	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);

	ui->m_pEditName->setVisible(fSave);

	m_fStaticFolder = fStaticFolder;
	
	if(sPath.isEmpty())
		sPath = QDir::currentPath();

	m_pModel = new QFileSystemModel();
	m_pModel->setResolveSymlinks(false);
	m_pModel->setFilter(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllDirs | QDir::Filter::AccessMask | QDir::Filter::AllEntries);
	m_pModel->setRootPath(sPath);
	CFileSortFilterProxyModel *pProxy = new CFileSortFilterProxyModel(this);
	pProxy->setDynamicSortFilter(true);
	pProxy->setFilterKeyColumn(0);
	pProxy->setSourceModel(m_pModel);
	pProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	pProxy->sort(0);
	ui->m_pListView->setModel(pProxy);
	ui->m_pListView->setRootIndex(pProxy->mapFromSource(m_pModel->index(sPath)));
	ui->m_pEditPath->setText(sPath);

	connect(ui->m_pListView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnItemClicked(const QModelIndex&)));
	connect(ui->m_pListView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(OnItemDoubleClicked(const QModelIndex&)));
	connect(ui->m_pButtonUp, SIGNAL(clicked()), this, SLOT(OnButtonUpClicked()));

	//QScroller::grabGesture(ui->m_pListView->viewport(), QScroller::LeftMouseButtonGesture);
	//grabGesture(Qt::TapAndHoldGesture);

	ui->m_pListView->setFont(ui->m_pEditPath->font());

	if(fStaticFolder)
	{
		ui->m_pEditPath->setEnabled(false);
		ui->m_pButtonUp->setEnabled(false);
	}
	
	FontSet(this, true);

}

CFileDialog::~CFileDialog()
{

	delete ui;

}

QString CFileDialog::PromptFileName(QWidget *pParent, bool fSave, QString sName, QString sPath, bool fStaticFolder)
{

	CFileDialog pDlg(fSave, sPath, fStaticFolder, pParent);
	//pDlg.setWindowModality(Qt::WindowModal);
	//pDlg.open();
	//pDlg.show();
	//pDlg.showFullScreen();
	pDlg.setWindowTitle(fSave ? "Save File" : "Open File");
	if(CAppConfig::IsMobile)
		pDlg.showMaximized();

	int eResult = pDlg.exec();
	if(eResult != QDialog::Accepted)
		return "";

	return pDlg.GetPath();
}

void CFileDialog::accept()
{

	if(ui->m_pEditName->text().isEmpty())
		return;

	m_sPath = QDir(ui->m_pEditPath->text()).filePath(ui->m_pEditName->text());
	ui->m_pEditName->setText("");

	if(QFileInfo(m_sPath).isDir())
	{
		QSortFilterProxyModel *pProxy = (QSortFilterProxyModel*)ui->m_pListView->model();
		ui->m_pListView->setRootIndex(pProxy->mapFromSource(m_pModel->index(m_sPath)));
		ui->m_pEditPath->setText(m_sPath);
		return;
	}

	if(ui->m_pEditName->isVisible() && QFileInfo::exists(m_sPath) &&
		QMessageBox::warning(this, "Warning!", "Selected file already exists. Do you want to overwrite it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
		return;

	QDialog::accept();

}

void CFileDialog::OnButtonUpClicked()
{

	QSortFilterProxyModel *pProxy = (QSortFilterProxyModel*)ui->m_pListView->model();
	QModelIndex pItem = ui->m_pListView->rootIndex();
	QString sPath = m_pModel->filePath(pProxy->mapToSource(pItem));
	int iSlashIndex = sPath.lastIndexOf('/');
	if(iSlashIndex != -1)
	{
		if(iSlashIndex == 0 && sPath.length() > 1)
			sPath = "/";
		else if(iSlashIndex == 2 && sPath[1] == ':' && iSlashIndex == sPath.length() - 1)
			sPath = "";
		else
			sPath = sPath.left(iSlashIndex);
		m_pModel->setRootPath(sPath);
		ui->m_pEditPath->setText(sPath);
		ui->m_pListView->setRootIndex(pProxy->mapFromSource(m_pModel->index(sPath)));
	}

}

void CFileDialog::OnItemClicked(const QModelIndex &pItemList)
{

	QSortFilterProxyModel *pProxy = (QSortFilterProxyModel*)ui->m_pListView->model();
	QModelIndex pItem = pProxy->mapToSource(pItemList);

	QString sPath = m_pModel->fileName(pItem);
	ui->m_pEditName->setText(sPath);

}

void CFileDialog::OnItemDoubleClicked(const QModelIndex &pItemList)
{

	QSortFilterProxyModel *pProxy = (QSortFilterProxyModel*)ui->m_pListView->model();
	QModelIndex pItem = pProxy->mapToSource(pItemList);
	if(m_pModel->isDir(pItem))
	{
		QString sPath = m_pModel->filePath(pItem);
		ui->m_pListView->setRootIndex(pProxy->mapFromSource(pItem));
		ui->m_pEditPath->setText(sPath);
	}
	else
		accept();

}

