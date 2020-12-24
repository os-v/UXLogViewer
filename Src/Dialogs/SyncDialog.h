//
//  @file SyncDialog.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 21.04.20.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef SYNCDIALOG_H
#define SYNCDIALOG_H

#include <QDialog>
#include <QUrlQuery>
#include <QNetworkAccessManager>

namespace Ui {
	class CSyncDialog;
}

class CSyncDialog : public QDialog
{

	Q_OBJECT

public:

	explicit CSyncDialog(QWidget *parent = nullptr);
	~CSyncDialog();

	bool IsUpdated() {
		return m_fUpdated;
	}

private:

	Ui::CSyncDialog *ui;

	volatile bool m_fResponse;

	bool m_fUpdated;

	virtual void accept();

	QString EncryptPassword(QString sUsername, QString sPassword);
	bool SendRequest(QString sURL, QJsonDocument &pRequestData, QJsonDocument &pResponseData, QString &sError);

private slots:

	void OnRequestFinished(QNetworkReply *reply);

};

#endif
