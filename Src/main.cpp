//
//  @file main.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "MainWindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFontDatabase>
#include <QProxyStyle>
#include "AppConfig.h"
#include "LogUI/LogTheme.h"

int main(int argc, char *argv[])
{

	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication::setPalette(QApplication::style()->standardPalette());

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, false);
	QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

	QApplication pApp(argc, argv);

	ScaleSet(0);

	CAppConfig::Instance().Load();

	FontInit();

	QApplication::setWindowIcon(QIcon(":/Resources/LogView.png"));

	CLogTheme::Instance().UpdateThemes(CAppConfig::Instance().ThemeDefs);
	CLogTheme::Instance().Select(CAppConfig::Instance().ThemeSelected);

	CMainWindow *pWindow = new CMainWindow();
	pWindow->show();

	int nResult = pApp.exec();

	delete pWindow;

	FontDel();

	CAppConfig::Instance().Save();

	return nResult;
}
