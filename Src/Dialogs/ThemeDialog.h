//
//  @file ThemeDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 17.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef THEMEDIALOG_H
#define THEMEDIALOG_H

#include <QDialog>

namespace Ui {
	class CThemeDialog;
}

class CThemeDialog : public QDialog
{

	Q_OBJECT

public:

	bool ThemeEdited;

	explicit CThemeDialog(QWidget *parent = nullptr);
	~CThemeDialog();

private:

	Ui::CThemeDialog *ui;

	QStringList m_pThemesDefs;

	virtual void accept();

private slots:

	void OnTextChanged();

	void OnButtonAdd();
	void OnButtonDel();

	void OnThemeChanged(int index);

};

#endif
