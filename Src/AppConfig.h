//
//  @file AppConfig.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef APPCONF_H
#define APPCONF_H

#include <QStringList>
#include <QVariantList>

enum { FILTER_RESULT = 0, FILTER_MAIN_A, FILTER_SUB_A, FILTER_MAIN_B, FILTER_SUB_B, FILTER_COUNT };

class QSettings;

class CAppConfig
{
public:

	static bool IsMobile;

	static CAppConfig &Instance() {
		static CAppConfig pInstance;
		return pInstance;
	}

	CAppConfig();
	~CAppConfig();

	void Load(QString sPath = "");
	void Save();

	bool IsLoaded() {
		return !MainState.isEmpty();
	}

	QString RecentFolder;
	QString ServiceUser;
	QString ServiceURL;

	bool DockFloat;

	bool FileOpenUI;
	bool FileSaveUI;

	int FontSizeMain;
	int FontSizeFixed;

	QByteArray MainState;
	QByteArray MainGeometry;
	QVariantList MainHeader;
	QStringList MainFilter;

	bool FilterState[FILTER_COUNT];
	QByteArray FilterGeometry[FILTER_COUNT];
	QVariantList FilterHeader[FILTER_COUNT];
	QStringList FilterSearch[FILTER_COUNT];

	int ThemeSelected;
	QStringList ThemeDefs;
	//QVariantList ThemeHeaders;

	QStringList &GetFilterSearch(int nViewID) {
		return nViewID == -1 ? MainFilter : FilterSearch[nViewID];
	}

	QByteArray GetMainHeader() {
		for( ; MainHeader.length() <= ThemeSelected; MainHeader.append(QByteArray()));
		return MainHeader[ThemeSelected].toByteArray();
	}

	QByteArray GetFilterHeader(int index, int iTheme = -1) {
		iTheme = iTheme == -1 ? ThemeSelected : iTheme;
		for( ; FilterHeader[index].length() <= iTheme; FilterHeader[index].append(QByteArray()));
		return FilterHeader[index][iTheme].toByteArray();
	}

protected:

	QString m_sPath;

	QSettings *m_pSettings;

};

#endif
