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
#include <QMenu>
#include <QDebug>
#include "../Utils.h"

CMessageDialog::CMessageDialog(QString sMessage, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CMessageDialog)
{

	ui->setupUi(this);

	ui->m_pEditMessage->setPlainText(sMessage);

	FontSet(this, true);

	ui->m_pEditMessage->setFont(*FontGet(true));

	connect(ui->m_pEditMessage, SIGNAL(selectionChanged()), this, SLOT(OnSelectionChanged()));

}

CMessageDialog::~CMessageDialog()
{

	delete ui;

}

void CMessageDialog::OnSelectionChanged()
{

	if(QApplication::mouseButtons() != Qt::NoButton)
		return;

	QTextCursor pTextCursor = ui->m_pEditMessage->textCursor();
	if(pTextCursor.selectedText().isEmpty())
		return;

	QPoint ptPos = QCursor::pos();
	QMenu *pMenu = ui->m_pEditMessage->createStandardContextMenu();
	pMenu->exec(ptPos);
	delete pMenu;

}

