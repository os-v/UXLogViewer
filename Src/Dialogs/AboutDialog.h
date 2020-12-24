//
//  @file AboutDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 07.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
	class CAboutDialog;
}

class CAboutDialog : public QDialog
{

	Q_OBJECT

public:

	explicit CAboutDialog(QWidget *parent = nullptr);
	~CAboutDialog();

private:

	Ui::CAboutDialog *ui;

};

#endif
