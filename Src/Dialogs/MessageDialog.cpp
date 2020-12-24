//
//  @file MessageDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 02.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "MessageDialog.h"
#include "ui_MessageDialog.h"
#include <QXmlStreamReader>
#include "../Utils.h"

CMessageDialog::CMessageDialog(QString sMessage, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CMessageDialog)
{

	ui->setupUi(this);

	ui->m_pEditMessage->setText(sMessage);

	FontSet(this, true);

	ui->m_pEditMessage->setFont(*FontGet(true));

}

CMessageDialog::~CMessageDialog()
{

	delete ui;

}
