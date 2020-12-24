//
//  @file LogTheme.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 17.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogTheme.h"

#define TOKEN_SYM			'\n'

QStringList CLogTheme::m_pThemeDefs = CLogTheme::InitThemeDefs();

QStringList CLogTheme::GetThemesDefs()
{
	return m_pThemeDefs;
}

int CLogTheme::GetThemesDefaultCount()
{
	return THEME_CUSTOM;
}

void CLogTheme::UpdateThemes(const QStringList &pThemeDefs)
{

	m_pThemeDefs = CLogTheme::InitThemeDefs();

	for(int iTheme = GetThemesDefaultCount(); iTheme < pThemeDefs.length(); iTheme++)
		m_pThemeDefs.append(pThemeDefs[iTheme]);

}

QString CLogTheme::PackTheme(QString sThemeName, QString sThemeFormat, QStringList pFieldName, CIntArray pFieldType, CIntArray pFieldMinLen)
{
	QString sThemeDef = sThemeName + TOKEN_SYM + sThemeFormat;
	for(int iField = 0; iField < pFieldName.length(); iField++)
		sThemeDef += TOKEN_SYM + pFieldName[iField] + "," + QString::number(pFieldType[iField]) + "," + QString::number(pFieldMinLen[iField]);
	return sThemeDef;
}

void CLogTheme::UnpackTheme(QString sThemeDef, QString &sThemeName, QString &sThemeFormat, QStringList &pFieldName, CIntArray &pFieldType, CIntArray &pFieldSize, QStringList &pFieldTokens)
{

	QStringList pThemeDefs = sThemeDef.split(TOKEN_SYM);

	sThemeName = pThemeDefs[TID_NAME];
	sThemeFormat = pThemeDefs[TID_FORMAT];

	for (int iPos = 0, iFieldPos = 0, iFieldEnd = 0; iPos < sThemeFormat.length(); iPos = iFieldEnd + 1)
	{
		for (iFieldPos = sThemeFormat.indexOf("%", iPos); iFieldPos != -1 && sThemeFormat[iFieldPos + 1] == '%'; iFieldPos = sThemeFormat.indexOf("%", iFieldPos + 2));
		iFieldEnd = sThemeFormat.indexOf("%", iFieldPos + 1);
		if (iFieldPos == -1 || iFieldEnd == -1)
			break;
		if(iFieldPos + 1 != iFieldEnd)
		{
			QString sToken = sThemeFormat.mid(iPos, iFieldPos - iPos);
			QString sField = sThemeFormat.mid(iFieldPos + 1, iFieldEnd - iFieldPos - 1);
			pFieldTokens.append(sToken);
			QStringList pFieldDefs = sField.split(',');
			pFieldName.push_back(pFieldDefs[FID_NAME]);
			pFieldType.push_back(pFieldDefs.size() > FID_TYPE ? pFieldDefs[FID_TYPE].toInt() : 0);
			pFieldSize.push_back(pFieldDefs.size() > FID_SIZE ? pFieldDefs[FID_SIZE].toInt() : 0);
		}
	}

}

QString CLogTheme::UnpackThemeName(QString sThemeDef)
{

	int iNextToken = sThemeDef.indexOf(TOKEN_SYM);

	return iNextToken == -1 ? sThemeDef : sThemeDef.left(iNextToken);
}

QString CLogTheme::ExportDefs()
{
	return qCompress(m_pThemeDefs.join("\r").toUtf8()).toBase64();
}

void CLogTheme::ImportDefs(const QString &pData)
{
	UpdateThemes(QString(qUncompress(QByteArray::fromBase64(pData.toUtf8()))).split('\r'));
}

CLogTheme::CLogTheme()
{
	m_nThemeIndex = -1;
}

CLogTheme::~CLogTheme()
{
}

bool CLogTheme::Select(int iIndex)
{

	m_nThemeIndex = iIndex;

	FieldName.clear();
	FieldType.clear();
	FieldSize.clear();
	m_pFieldTokens.clear();

	if (iIndex >= m_pThemeDefs.length())
		return false;

	QStringList pThemeDefs = m_pThemeDefs[iIndex].split(TOKEN_SYM);

	UnpackTheme(m_pThemeDefs[iIndex], ThemeName, ThemeFormat, FieldName, FieldType, FieldSize, m_pFieldTokens);

	return true;
}

bool CLogTheme::Parse(QString &sMessage, QString *pArgs)
{

	if (!m_pFieldTokens.size())
		return false;

	int iPos = 0;
	if (m_pFieldTokens[0].size())
	{
		if (!sMessage.startsWith(m_pFieldTokens[0]))
			return false;
		iPos += m_pFieldTokens[0].size();
	}

	int iArgs = 0;
	for (int iToken = 1; iToken < m_pFieldTokens.length(); iToken++)
	{
		int iEnd = sMessage.indexOf(m_pFieldTokens[iToken], iPos);
		if (iEnd == -1)
				return false;
		pArgs[iArgs++] = sMessage.mid(iPos, iEnd - iPos);
		iPos = iEnd + m_pFieldTokens[iToken].length();
	}

	pArgs[iArgs++] = sMessage.mid(iPos);

	return true;
}

QStringList CLogTheme::InitThemeDefs()
{
	QStringList pList;
	pList.append("Default\n%Msg%");
	pList.append("Simple\n%Date% %Time%: [%PID%] [%TID%] [%Proc%] [%Scope%] %Msg%");
	return pList;
}
