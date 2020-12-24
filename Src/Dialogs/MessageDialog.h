//
//  @file MessageDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 02.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>

namespace Ui {
	class CMessageDialog;
}

class CMessageDialog : public QDialog
{
	Q_OBJECT
public:

	explicit CMessageDialog(QString sMessage, QWidget *parent = nullptr);
	~CMessageDialog();

private:

	Ui::CMessageDialog *ui;

};

#endif
