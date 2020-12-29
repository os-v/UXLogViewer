//
//  @file SyncDialog.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 21.04.20.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "SyncDialog.h"
#include "ui_SyncDialog.h"
#include <QVariantMap>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCryptographicHash>
#include <QPushButton>
#include "Utils.h"
#include "AppConfig.h"
#include "../LogUI/LogTheme.h"

CSyncDialog::CSyncDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CSyncDialog)
{

	ui->setupUi(this);

	m_fResponse = false;
	m_fUpdated = false;

	FontSet(this, true);

	ui->m_pEditUsername->setText(CAppConfig::Instance().ServiceUser);

	ui->m_pRadioTypeReplace->setChecked(true);
	ui->m_pFrameType->setEnabled(false);

	if(CAppConfig::Instance().IsMobile)
		ui->m_pWidgetSpacingDesktop->setVisible(false);
	else
		ui->m_pWidgetSpacingMobile->setVisible(false);

	ui->m_pButtonsDialog->button(QDialogButtonBox::Ok)->setText("Sync");
	ui->m_pButtonsDialog->button(QDialogButtonBox::Cancel)->setText("Close");

}

CSyncDialog::~CSyncDialog()
{

	delete ui;

}

void CSyncDialog::accept()
{

	CAppConfig &pConfig = CAppConfig::Instance();

	bool fToServer = ui->m_pRadioDirToServer->isChecked();
	bool fTypeAppend = ui->m_pRadioTypeAppend->isChecked();

	pConfig.ServiceUser = ui->m_pEditUsername->text();

	QVariantMap pRequestMap;
	pRequestMap.insert("username", ui->m_pEditUsername->text());
	pRequestMap.insert("password", EncryptPassword(ui->m_pEditUsername->text(), ui->m_pEditPassword->text()));
	if(fToServer)
	{
		QString s;
		pRequestMap.insert("append", fTypeAppend ? "true" : "false");
		pRequestMap.insert("themes", CLogTheme::Instance().ExportDefs());
		pRequestMap.insert("active", pConfig.ThemeSelected);
	}

	QJsonDocument pRequestData = QJsonDocument::fromVariant(pRequestMap), pResponseData;

	QString sError;
	if(SendRequest(pConfig.ServiceURL, pRequestData, pResponseData, sError))
	{
		QJsonObject pResponseObject = pResponseData.object();
		if(pResponseObject.contains("error"))
			sError = pResponseObject["error"].toString();
		else if(!pResponseObject.contains("themes") || !pResponseObject.contains("active"))
			sError = "Invalid response";
		else if(!fToServer)
		{
			QString pThemeDefs = pResponseObject["themes"].toVariant().toString();
			if(pThemeDefs.length())
			{
				CLogTheme::Instance().ImportDefs(pThemeDefs);
				pConfig.ThemeDefs = CLogTheme::Instance().GetThemesDefs();
				pConfig.ThemeSelected = pResponseObject["active"].toString().toInt();
				pConfig.Save();
				CLogTheme::Instance().Select(pConfig.ThemeSelected);
				m_fUpdated = true;
			}
		}
	}

	if(!sError.isEmpty())
		QMessageBox::critical(this, "Error!", "Error: " + sError, QMessageBox::Ok);
	else
		QDialog::accept();

}

QString CSyncDialog::EncryptPassword(QString sUsername, QString sPassword)
{

	if(sPassword.isEmpty())
		return "";

	QByteArray pUsername = sUsername.toLatin1();
	QByteArray pPassword = sPassword.toLatin1();

	QByteArray pUserPass = pPassword;

	QCryptographicHash pHash(QCryptographicHash::Md5);

	pHash.addData(pUserPass);

	QString sResult = pHash.result().toHex();

	return sResult.toUpper();
}

bool CSyncDialog::SendRequest(QString sURL, QJsonDocument &pRequestData, QJsonDocument &pResponseData, QString &sError)
{

	QProgressDialog pProgress(this);
	pProgress.setModal(true);
	pProgress.setWindowTitle("Sync");
	pProgress.setLabelText("Requesting server ...");
	pProgress.setRange(0, 100);
	pProgress.setValue(0);
	pProgress.show();
	QCoreApplication::instance()->processEvents(QEventLoop::AllEvents, 100);

	QUrl pURL = QUrl::fromUserInput(sURL);
	QNetworkRequest pRequest(pURL);
	pRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	m_fResponse = false;

	QNetworkAccessManager pAccessManager(this);
	QNetworkReply *pReply = pAccessManager.post(pRequest, pRequestData.toJson());

	QObject::connect(&pAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnRequestFinished(QNetworkReply*)));

	for(int nProgress = 0; !m_fResponse; nProgress++, SleepMS(10))
	{
		QCoreApplication::instance()->processEvents(QEventLoop::ExcludeUserInputEvents, 10);
		pProgress.setValue(nProgress / 100);
	}

	bool fResult = pReply->error() == QNetworkReply::NoError;
	if(!fResult)
		sError = StrFormat("%d [%s]", pReply->error(), pReply->errorString().toLatin1().data());
	else
		pResponseData = QJsonDocument::fromJson(pReply->readAll());

	return fResult;
}

void CSyncDialog::OnRequestFinished(QNetworkReply *reply)
{

	m_fResponse = true;

}

