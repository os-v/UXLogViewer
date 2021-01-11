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

CSettingsDialog::CSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CSettingsDialog)
{

	ui->setupUi(this);

	CAppConfig &pConfig = CAppConfig::Instance();

	ui->m_pCheckFileOpenUI->setChecked(pConfig.FileOpenUI);
	ui->m_pCheckFileSaveUI->setChecked(pConfig.FileSaveUI);

	for(int iFontSize = 8; iFontSize <= 32; iFontSize++)
	{
		ui->m_pFontUI->addItem(StrFormat("UI font size %d", iFontSize), iFontSize);
		ui->m_pFontLog->addItem(StrFormat("Log font size %d", iFontSize), iFontSize);
	}

	ui->m_pFontUI->setCurrentIndex(pConfig.FontSizeMain - 8);
	ui->m_pFontLog->setCurrentIndex(pConfig.FontSizeFixed - 8);

	FontSet(this, true);

	if(CAppConfig::Instance().IsMobile)
		ui->m_pWidgetSpacingDesktop->setVisible(false);
	else
		ui->m_pWidgetSpacingMobile->setVisible(false);

}

CSettingsDialog::~CSettingsDialog()
{

	delete ui;

}

void CSettingsDialog::accept()
{

	CAppConfig &pConfig = CAppConfig::Instance();

	pConfig.FileOpenUI = ui->m_pCheckFileOpenUI->isChecked();
	pConfig.FileSaveUI = ui->m_pCheckFileSaveUI->isChecked();
	pConfig.FontSizeMain = ui->m_pFontUI->currentIndex() + 8;
	pConfig.FontSizeFixed = ui->m_pFontLog->currentIndex() + 8;

	pConfig.Save();

	QDialog::accept();

}

