//
//  @file ThemeDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 17.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "ThemeDialog.h"
#include "ui_ThemeDialog.h"
#include "../LogUI/LogTheme.h"
#include "../AppConfig.h"

CThemeDialog::CThemeDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CThemeDialog)
{

	ThemeEdited = false;

	ui->setupUi(this);

	connect(ui->m_pComboTheme, SIGNAL(currentIndexChanged(int)), this, SLOT(OnThemeChanged(int)));
	connect(ui->m_pButtonAdd, SIGNAL(clicked()), this, SLOT(OnButtonAdd()));
	connect(ui->m_pButtonDel, SIGNAL(clicked()), this, SLOT(OnButtonDel()));

	connect(ui->m_pEditTheme, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));

	m_pThemesDefs = CLogTheme::Instance().GetThemesDefs();

	for(int iTheme = 0; iTheme < m_pThemesDefs.length(); iTheme++)
		ui->m_pComboTheme->addItem(CLogTheme::UnpackThemeName(m_pThemesDefs[iTheme]), iTheme);

	ui->m_pComboTheme->setCurrentIndex(CLogTheme::Instance().GetSelected());

	FontSet(this, true);

}

CThemeDialog::~CThemeDialog()
{

	delete ui;

}

void CThemeDialog::accept()
{

	CAppConfig &pConfig = CAppConfig::Instance();

	QComboBox *pComboTheme = ui->m_pComboTheme;

	int iSelected = pComboTheme->currentIndex();
	if(iSelected >= CLogTheme::Instance().GetThemesDefaultCount())
	{

		QString sPlainText = ui->m_pEditTheme->document()->toPlainText();

		sPlainText.remove("\r");

		m_pThemesDefs[iSelected] = sPlainText;

		QVariantList pFilterHeader[FILTER_COUNT];
		for(int iTheme = 0; iTheme < pComboTheme->count(); iTheme++)
		{
			int iRealIndex = pComboTheme->itemData(iTheme).toInt();
			for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
				pFilterHeader[iFilter].push_back(iRealIndex == -1 ? QByteArray() : pConfig.GetFilterHeader(iFilter, iRealIndex));
		}

		for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
			pConfig.FilterHeader[iFilter] = pFilterHeader[iFilter];

	}

	CLogTheme::UpdateThemes(m_pThemesDefs);

	CLogTheme::Instance().Select(iSelected);

	pConfig.ThemeSelected = iSelected;

	pConfig.Save();

	QDialog::accept();

}

void CThemeDialog::OnTextChanged()
{

	QString sPlainText = ui->m_pEditTheme->document()->toPlainText();

	QString sThemeName = CLogTheme::UnpackThemeName(sPlainText);

	int iSelected = ui->m_pComboTheme->currentIndex();

	ui->m_pComboTheme->setItemText(iSelected, sThemeName);

}

void CThemeDialog::OnButtonAdd()
{

	QString sThemeName = "NEW THEME";

	QString sCurTheme = ui->m_pComboTheme->currentText();

	ui->m_pComboTheme->addItem(sThemeName, -1);

	QString sPlainText = ui->m_pEditTheme->document()->toPlainText();

	int iNamePos = sPlainText.indexOf(sCurTheme);
	if(iNamePos != -1)
		sPlainText.replace(iNamePos, sCurTheme.length(), sThemeName);

	m_pThemesDefs.append(sPlainText);

	ui->m_pComboTheme->setCurrentIndex(m_pThemesDefs.length() - 1);

	QTextCursor cursor = ui->m_pEditTheme->textCursor();
	cursor.setPosition(sThemeName.length(), QTextCursor::KeepAnchor);
	ui->m_pEditTheme->setTextCursor(cursor);

}

void CThemeDialog::OnButtonDel()
{

	int iSelected = ui->m_pComboTheme->currentIndex();
	if(iSelected < CLogTheme::Instance().GetThemesDefaultCount())
		return;

	m_pThemesDefs.removeAt(iSelected);

	ui->m_pComboTheme->removeItem(iSelected);

}

void CThemeDialog::OnThemeChanged(int index)
{

	ui->m_pEditTheme->document()->setPlainText(m_pThemesDefs[index]);

	ui->m_pEditTheme->setReadOnly(index < CLogTheme::Instance().GetThemesDefaultCount());

	ui->m_pEditTheme->setFocus();

	ui->m_pEditTheme->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);

}

