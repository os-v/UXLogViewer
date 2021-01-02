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
#include "../AppConfig.h"
#include <QScreen>

CAboutDialog::CAboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CAboutDialog)
{

	ui->setupUi(this);

	//setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

	QString sMessage = StrFormat("<html><body>UXLogViewer v%s<br><br>%s<br><br><a href='%s'>%s</a></body></html>", PRODUCT_SVERSION, PRODUCT_LEGALCOPYRIGHT, PRODUCT_COMPANYNAME, PRODUCT_COMPANYNAME);
	ui->m_pLabelText->setText(sMessage);

	FontSet(this, true);

	//if(CAppConfig::Instance().IsMobile)
		//layout()->removeItem(ui->m_pVerticalSpacerDesktop);
	//else
		//layout()->removeItem(ui->m_pVerticalSpacerMobile);

}

CAboutDialog::~CAboutDialog()
{

	delete ui;

}
