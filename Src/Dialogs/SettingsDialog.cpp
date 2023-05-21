//
//  @file SettingsDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 11.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "../AppConfig.h"
#include "../Utils.h"
#include <limits.h>

CSettingsDialog::CSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CSettingsDialog)
{

	ui->setupUi(this);

	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	CAppConfig &pConfig = CAppConfig::Instance();

	ui->m_pComboFilterType->addItem("Use Custom Filter");
	ui->m_pComboFilterType->addItem("Use RegExp Filter");
	ui->m_pComboFilterType->addItem("Use Simple Filter");
	ui->m_pComboFilterType->addItem("Use Automatic Filter");

	for(int iFontSize = 8; iFontSize <= 32; iFontSize++)
	{
		ui->m_pFontUI->addItem(StrFormat("UI font size %d", iFontSize), iFontSize);
		ui->m_pFontLog->addItem(StrFormat("Log font size %d", iFontSize), iFontSize);
	}

	ui->m_pCheckReopenLast->setChecked(pConfig.FileReopen);
	ui->m_pCheckFileOpenUI->setChecked(pConfig.FileOpenUI);
	ui->m_pCheckFileSaveUI->setChecked(pConfig.FileSaveUI);
	ui->m_pCheckMonitorFile->setChecked(pConfig.FileMonitor);
	ui->m_pComboFilterType->setCurrentIndex(pConfig.FilterType);

	ui->m_pFontUI->setCurrentIndex(pConfig.FontSizeMain - 8);
	ui->m_pFontLog->setCurrentIndex(pConfig.FontSizeFixed - 8);
	ui->m_pEditColumnWidth->setMaximum(INT_MAX);
	ui->m_pEditColumnWidth->setValue(pConfig.ColumnWidth);
	ui->m_pEditLineLimit->setMaximum(INT_MAX);
	ui->m_pEditLineLimit->setValue(pConfig.MaxLineLength);

	FontSet(this, true);

	setFixedSize(size());

	ui->m_pWidgetSpacingDesktop->setVisible(false);
	if(!CAppConfig::Instance().IsMobile)
		ui->m_pWidgetSpacingMobile->setVisible(false);

}

CSettingsDialog::~CSettingsDialog()
{

	delete ui;

}

void CSettingsDialog::accept()
{

	CAppConfig &pConfig = CAppConfig::Instance();

	pConfig.FileReopen = ui->m_pCheckReopenLast->isChecked();
	pConfig.FileOpenUI = ui->m_pCheckFileOpenUI->isChecked();
	pConfig.FileSaveUI = ui->m_pCheckFileSaveUI->isChecked();
	pConfig.FileMonitor = ui->m_pCheckMonitorFile->isChecked();
	pConfig.FilterType = ui->m_pComboFilterType->currentIndex();
	pConfig.FontSizeMain = ui->m_pFontUI->currentIndex() + 8;
	pConfig.FontSizeFixed = ui->m_pFontLog->currentIndex() + 8;
	pConfig.ColumnWidth = ui->m_pEditColumnWidth->value();
	pConfig.MaxLineLength = ui->m_pEditLineLimit->value();

	pConfig.Save();

	QDialog::accept();

}

