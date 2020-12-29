//
//  @file AboutDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 07.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include "../ProductInfo.h"
#include "../Utils.h"
#include <QDesktopWidget>
#include <QScreen>

CAboutDialog::CAboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CAboutDialog)
{

	ui->setupUi(this);

	//setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

	QString sText = StrFormat("dpix:%d, dpiy:%d, dpi:%f", qApp->desktop()->logicalDpiX(), qApp->desktop()->logicalDpiY(), QApplication::primaryScreen()->logicalDotsPerInch());
	ui->m_pLabelText->setText(sText);
	QString sMessage = StrFormat("UXLogViewer v%s\n\n%s\n\n%s", PRODUCT_SVERSION, PRODUCT_LEGALCOPYRIGHT, PRODUCT_COMPANYNAME);
	ui->m_pLabelText->setText(sMessage);

	FontSet(this, true);

}

CAboutDialog::~CAboutDialog()
{

	delete ui;

}
