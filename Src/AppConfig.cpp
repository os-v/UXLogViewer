//
//  @file AppConfig.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "AppConfig.h"
#include <QSettings>
#include <QApplication>
#include <QFileInfo>
#include <QVector>
#include <QDebug>

#define GROUP_COMMON			"Common"

#define KEY_RECENTFOLDER		"RecentFolder"
#define KEY_SERVICEUSER			"ServiceUser"
#define KEY_SERVICEURL			"ServiceURL"

#define KEY_DOCKFLOAT			"DockFloat"
#define KEY_FILEOPENUI			"FileOpenUI"
#define KEY_FILESAVEUI			"FileSaveUI"
#define KEY_STRICTFILTER		"StrictFilter"

#define KEY_FONTSIZEMAIN		"FontSizeMain"
#define KEY_FONTSIZEFIXED		"FontSizeFixed"

#define KEY_MAINSTATE			"MainState"
#define KEY_MAINGEOMETRY		"MainGeometry"
#define KEY_MAINHEADER			"MainHeader"
#define KEY_MAINFILTER			"MainFilter"

#define KEY_FILTERSTATE			"FilterState"
#define KEY_FILTERGEOMETRY		"FilterGeometry"
#define KEY_FILTERHEADER		"FilterHeader"
#define KEY_FILTERSEARCH		"FilterSearch"

#define KEY_THEMESELECTED		"ThemeSelected"
#define KEY_THEMEDEFS			"ThemeDefs"
#define KEY_THEMEHEADERS		"ThemeHeaders"

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	bool CAppConfig::IsMobile = true;
#else
	bool CAppConfig::IsMobile = false;
#endif

CAppConfig::CAppConfig()
{

	m_pSettings = 0;

	RecentFolder = "";
	ServiceUser = "";
	ServiceURL = "https://lv.os-v.pw/Sync.php";

	DockFloat = false;

	FileOpenUI = false;
	FileSaveUI = false;
	StrictFilter = false;

#ifdef _WIN32
	FontSizeMain = 8;
	FontSizeFixed = 9;
#else
	FontSizeMain = 14;
	FontSizeFixed = 14;
#endif

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
		FilterState[iFilter] = iFilter == FILTER_MAIN_A;

	ThemeSelected = 0;

}

CAppConfig::~CAppConfig()
{

	delete m_pSettings;

}

void CAppConfig::Load(QString sPath)
{

	if(sPath == "")
	{
		QFileInfo pFileInfo(QApplication::applicationFilePath());
		sPath = pFileInfo.absolutePath() + "/" + pFileInfo.baseName() + ".conf";
	}

	m_sPath = sPath;

	if(m_pSettings)
		delete m_pSettings;

	m_pSettings = IsMobile ? new QSettings() : new QSettings(sPath, QSettings::IniFormat);

	m_pSettings->beginGroup(GROUP_COMMON);

	RecentFolder = m_pSettings->value(KEY_RECENTFOLDER).toString();
	ServiceUser = m_pSettings->value(KEY_SERVICEUSER, ServiceUser).toString();
	ServiceURL = m_pSettings->value(KEY_SERVICEURL, ServiceURL).toString();

	DockFloat = m_pSettings->value(KEY_FILEOPENUI, DockFloat).toBool();

	FileOpenUI = m_pSettings->value(KEY_FILEOPENUI, FileOpenUI).toBool();
	FileSaveUI = m_pSettings->value(KEY_FILESAVEUI, FileSaveUI).toBool();
	StrictFilter = m_pSettings->value(KEY_STRICTFILTER, FileSaveUI).toBool();

	FontSizeMain = m_pSettings->value(KEY_FONTSIZEMAIN, FontSizeMain).toInt();
	FontSizeFixed = m_pSettings->value(KEY_FONTSIZEFIXED, FontSizeFixed).toInt();

	MainState = m_pSettings->value(KEY_MAINSTATE).toByteArray();
	MainGeometry = m_pSettings->value(KEY_MAINGEOMETRY).toByteArray();
	MainHeader = m_pSettings->value(KEY_MAINHEADER).toList();
	MainFilter = m_pSettings->value(KEY_MAINFILTER).toStringList();

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		QString sFilterID = QString::asprintf("%d", iFilter);
		FilterState[iFilter] = m_pSettings->value(KEY_FILTERSTATE + sFilterID, FilterState[iFilter]).toBool();
		FilterGeometry[iFilter] = m_pSettings->value(KEY_FILTERGEOMETRY + sFilterID).toByteArray();
		FilterHeader[iFilter] = m_pSettings->value(KEY_FILTERHEADER + sFilterID).toList();
		FilterSearch[iFilter] = m_pSettings->value(KEY_FILTERSEARCH + sFilterID).toStringList();
	}

	ThemeSelected = m_pSettings->value(KEY_THEMESELECTED).toInt();
	ThemeDefs = m_pSettings->value(KEY_THEMEDEFS).toStringList();
	//ThemeHeaders = m_pSettings->value(KEY_THEMEHEADERS).toList();

	m_pSettings->endGroup();

}

void CAppConfig::Save()
{

	m_pSettings->beginGroup(GROUP_COMMON);

	m_pSettings->setValue(KEY_RECENTFOLDER, RecentFolder);
	m_pSettings->setValue(KEY_SERVICEUSER, ServiceUser);
	m_pSettings->setValue(KEY_SERVICEURL, ServiceURL);

	m_pSettings->setValue(KEY_FILEOPENUI, FileOpenUI);
	m_pSettings->setValue(KEY_FILESAVEUI, FileSaveUI);
	m_pSettings->setValue(KEY_STRICTFILTER, StrictFilter);

	m_pSettings->setValue(KEY_FONTSIZEMAIN, FontSizeMain);
	m_pSettings->setValue(KEY_FONTSIZEFIXED, FontSizeFixed);

	m_pSettings->setValue(KEY_MAINSTATE, MainState);
	m_pSettings->setValue(KEY_MAINGEOMETRY, MainGeometry);
	m_pSettings->setValue(KEY_MAINHEADER, MainHeader);
	m_pSettings->setValue(KEY_MAINFILTER, MainFilter);

	for(int iFilter = 0; iFilter < FILTER_COUNT; iFilter++)
	{
		QString sFilterID = QString::asprintf("%d", iFilter);
		m_pSettings->setValue(KEY_FILTERSTATE + sFilterID, FilterState[iFilter]);
		m_pSettings->setValue(KEY_FILTERGEOMETRY + sFilterID, FilterGeometry[iFilter]);
		m_pSettings->setValue(KEY_FILTERHEADER + sFilterID, FilterHeader[iFilter]);
		m_pSettings->setValue(KEY_FILTERSEARCH + sFilterID, FilterSearch[iFilter]);
	}

	m_pSettings->setValue(KEY_THEMESELECTED, ThemeSelected);
	m_pSettings->setValue(KEY_THEMEDEFS, ThemeDefs);
	//m_pSettings->setValue(KEY_THEMEHEADERS, ThemeHeaders);

	m_pSettings->endGroup();

	m_pSettings->sync();

}

