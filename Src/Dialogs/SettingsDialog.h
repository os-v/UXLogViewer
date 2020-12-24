//
//  @file SettingsDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 11.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
	class CSettingsDialog;
}

class CSettingsDialog : public QDialog
{

	Q_OBJECT

public:

	explicit CSettingsDialog(QWidget *parent = nullptr);
	~CSettingsDialog();

private:

	Ui::CSettingsDialog *ui;

	virtual void accept();

};

#endif
