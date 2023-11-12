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

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
	//QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);

	QApplication pApp(argc, argv);

	QApplication::setWindowIcon(QIcon(":/Resources/LogView.svg"));

	CAppConfig::Instance().Load();

	ScaleSet(0);

	FontInit();

	CLogTheme::Instance().UpdateThemes(CAppConfig::Instance().ThemeDefs);
	CLogTheme::Instance().Select(CAppConfig::Instance().ThemeSelected);

	pApp.setAutoSipEnabled(false);

	QInputMethod* input = QGuiApplication::inputMethod();
	input->setVisible(false);

	CMainWindow *pWindow = new CMainWindow();
	pWindow->show();

	int nResult = pApp.exec();

	delete pWindow;

	FontDel();

	CAppConfig::Instance().Save();

	return nResult;
}
