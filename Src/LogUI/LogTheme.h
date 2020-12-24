//
//  @file LogTheme.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 17.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGTHEME_H
#define LOGTHEME_H

#include "../Utils.h"

class CLogTheme
{
public:

	QString ThemeName;
	QString ThemeFormat;
	QStringList FieldName;
	CIntArray FieldType;
	CIntArray FieldSize;

	static CLogTheme &Instance() {
		static CLogTheme pInstance;
		return pInstance;
	}

	static QStringList GetThemesDefs();
	static int GetThemesDefaultCount();
	static void UpdateThemes(const QStringList &pThemeDefs);
	static QString PackTheme(QString sThemeName, QString sThemeFormat, QStringList pFieldName, CIntArray pFieldType, CIntArray pFieldMinLen);
	static void UnpackTheme(QString sThemeDef, QString &sThemeName, QString &sThemeFormat, QStringList &pFieldName, CIntArray &pFieldType, CIntArray &pFieldSize, QStringList &pFieldTokens);
	static QString UnpackThemeName(QString sThemeDef);
	static QString ExportDefs();
	static void ImportDefs(const QString &pData);

	CLogTheme();
	~CLogTheme();

	bool Select(int iIndex);

	bool Parse(QString &sMessage, QString *pArgs);

	int GetSelected() {
		return m_nThemeIndex;
	}

	int GetColCount() {
		return FieldName.size();
	}

private:

	enum { THEME_DEFAULT = 0, THEME_SIMPLE, THEME_CUSTOM };
	enum { TID_NAME = 0, TID_FORMAT, TID_COUNT };
	enum { FID_NAME = 0, FID_TYPE, FID_SIZE, FID_COUNT };

	static QStringList m_pThemeDefs;

	int m_nThemeIndex;
	QStringList m_pFieldTokens;

	static QStringList InitThemeDefs();

};

#endif
