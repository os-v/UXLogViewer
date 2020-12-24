//
//  @file Utils.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 10.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef UTILS_H
#define UTILS_H

#include <QVector>
#include <QStringList>

#ifndef WIN32
	#include <strings.h>
#endif

#define CArray			QVector
#define CIntArray		CArray<int>

#define SafeDelete(x)	if(x) { delete x; x = 0; }

class QFont;
class QWidget;

#define StrFormat		QString::asprintf

void SleepMS(unsigned long nValue);

void ScaleSet(float fValue);
float ScaleGet();
float ScaleUI(int nValue);
void FontInit();
void FontDel();
void FontSet(QFont *pFont, QFont *pFixed);
QFont *FontGet(bool fFixed = false);
void FontSet(QWidget *pWidget, bool fRecursive, QFont *pFont = 0);

template<class TItem> int ArrayFindIndex(TItem *pArray, int nCount, TItem pValue)
{

	int iItem = 0;
	for(iItem = 0; iItem < nCount && pArray[iItem] != pValue; iItem++);

	if(iItem == nCount)
		return -1;

	return iItem;
}

template<class TValue> void QInsertValue(QByteArray &pBuffer, TValue nValue, bool fSkipEqual)
{

	TValue *pData = (TValue*)pBuffer.data();

	int nSize = pBuffer.size() / sizeof(TValue);
	int nLast = nSize > 0 ? nSize - 1 : 0;
	nSize /= 2;
	int iData = nSize;
	for(; nSize; nSize /= 2)
	{
		if(pData[iData] < nValue)
			iData += (nSize / 2 + (nSize % 2 ? 1 : 0));
		else if(pData[iData] > nValue)
			iData -= (nSize / 2 + (nSize % 2 ? 1 : 0));
		else if(fSkipEqual)
			return;
		else
			break;
	}

	if(pBuffer.size() && iData <= nLast && pData[iData] < nValue)
		iData++;

	if(fSkipEqual && iData <= nLast && pData[iData] == nValue)
		return;

	pBuffer.insert(iData * sizeof(TValue), (char*)&nValue, sizeof(nValue));

}

#endif