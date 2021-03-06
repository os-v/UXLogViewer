//
//  @file Utils.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "Utils.h"
#include <sys/stat.h>
#include <time.h>
#include <QThread>
#include <QtWidgets>
#include <QFont>
#include <QScreen>
#include "AppConfig.h"

bool GLogEnabled = false;
QString GLogMessages;

float GScaleFactor = 1.0;

QFont *GFontMain = 0;
QFont *GFontFixed = 0;

bool LogIsEnabled()
{
	return GLogEnabled;
}

void LogEnable(bool fEnable)
{
	GLogEnabled = fEnable;
	if(!fEnable)
		GLogMessages = "";
}

QString &LogMessages()
{
	return GLogMessages;
}

void LogMessage(const char *lpFmt, ...)
{
	if(!GLogEnabled)
		return;
	va_list args;
	va_start(args, lpFmt);
	QString sMessage = QString::vasprintf(lpFmt, args);
	va_end(args);
	QString sTime = StrFormat("%d", (int)(QDateTime::currentMSecsSinceEpoch() - QDateTime(QDate::currentDate().startOfDay()).toMSecsSinceEpoch()));
	GLogMessages += sTime + ": " + sMessage + "\n";
	qDebug() << sMessage;
}

void SleepMS(unsigned long nValue)
{

	QThread::msleep(nValue);

}

void ScaleSet(float fValue)
{
	if(fValue == 0)
	{
		float nDPIX = QGuiApplication::primaryScreen()->logicalDotsPerInch();
		if(nDPIX <= 96)
			nDPIX = 100;
		if(nDPIX == 120)
			nDPIX = 125;
		fValue = nDPIX / 100;
	}
	GScaleFactor = fValue;
}

float ScaleGet()
{
	return GScaleFactor;
}

float ScaleUI(int nValue)
{
	return nValue * GScaleFactor;
}

void FontInit()
{
	QString sFontMain = QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/Resources/Fonts/Roboto-Light.ttf")).at(0);
	QString sFontFixed = QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(":/Resources/Fonts/Courier-New.ttf")).at(0);
	FontSet(
		new QFont(sFontMain, ScaleUI(CAppConfig::Instance().FontSizeMain), QFont::Normal),
		new QFont(sFontFixed, ScaleUI(CAppConfig::Instance().FontSizeFixed), QFont::Normal)
	);
}

void FontDel()
{
	SafeDelete(GFontMain);
	SafeDelete(GFontFixed);
}

void FontSet(QFont *pFont, QFont *pFixed)
{
	SafeDelete(GFontMain);
	SafeDelete(GFontFixed);
	GFontMain = pFont;
	GFontFixed = pFixed;
}

QFont *FontGet(bool fFixed)
{
	return fFixed ? GFontFixed : GFontMain;
}

void FontSet(QWidget *pWidget, bool fRecursive, QFont *pFont)
{
	if(!pFont)
		pFont = GFontMain;
	pWidget->setFont(*pFont);
	const QObjectList &pObjectList = pWidget->children();
	for(int iObject = 0; iObject < pObjectList.count(); iObject++)
	{
		QWidget *pChild = qobject_cast<QWidget*>(pObjectList[iObject]);
		if(pChild)
		{
			pChild->setFont(*pFont);
			if(fRecursive)
				FontSet(pChild, fRecursive, pFont);
		}
	}
}

int StrArrayFindIndex(const char **pArray, int nStart, int nCount, const ushort *sValueText)
{
	for(int iItem = nStart; iItem < nCount; iItem++)
	{
		const char *pItemPtr = pArray[iItem];
		const ushort *pValuePtr = sValueText;
		for(; *pItemPtr && *pValuePtr && *pItemPtr == *pValuePtr; pItemPtr++, pValuePtr++);
		if(!*pItemPtr)
			return iItem;
	}
	return -1;
}
