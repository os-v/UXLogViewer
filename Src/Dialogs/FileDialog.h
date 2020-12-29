//
//  @file FileDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 02.05.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QDialog>

namespace Ui {
	class CFileDialog;
}

class QFileSystemModel;

class CFileDialog : public QDialog
{

	Q_OBJECT

public:

	explicit CFileDialog(bool fSave, QString sPath, bool fStaticFolder, QWidget *parent = nullptr);
	~CFileDialog();

	static QString PromptFileName(QWidget *pParent, bool fSave, QString sName, QString sPath, bool fStaticFolder = false);

	QString GetPath() {
		return m_sPath;
	}

private:

	Ui::CFileDialog *ui;

	QString m_sPath;
	
	bool m_fStaticFolder;

	QFileSystemModel *m_pModel;

	virtual void accept();

private slots:

	void OnButtonUpClicked();
	void OnItemClicked(const QModelIndex &pItem);
	void OnItemDoubleClicked(const QModelIndex &pItem);

};

#endif
