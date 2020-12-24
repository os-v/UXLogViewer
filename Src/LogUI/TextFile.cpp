//
//  @file TextFile.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "TextFile.h"
#include <QFile>
#include <QTextStream>
#include "../Utils.h"

CTextFile::CTextFile()
{

	m_pFile = 0;

	m_nBufferSize = 0;
	m_nBufferOffset = 0;
	m_nBufferPtr = 0;

	m_nLineOffset = 0;
	m_sLineText = "";

}

CTextFile::~CTextFile()
{

	Destroy();

}

void CTextFile::Attach(QFile *pFile, int nFrame)
{

	m_pFile = pFile;

	m_pBuffer.resize(nFrame);

	m_nBufferPtr = 0;
	m_nBufferSize = 0;

}

void CTextFile::Detach()
{

	m_pFile = 0;

	m_nBufferSize = 0;
	m_nBufferOffset = 0;
	m_nBufferPtr = 0;

	m_nLineOffset = 0;
	m_sLineText = "";

}

bool CTextFile::Create(QString sPath, int nFrame)
{

	m_pFile = new QFile();
	m_pFile->setFileName(sPath);
	if(!m_pFile->open(QIODevice::ReadOnly))
	{
		SafeDelete(m_pFile);
		return false;
	}

	m_pBuffer.resize(nFrame);

	m_nBufferPtr = 0;
	m_nBufferSize = 0;

	return true;
}

void CTextFile::Destroy()
{

	SafeDelete(m_pFile);	

}

bool CTextFile::ReadNextLine(bool fSkipCR)
{

	QByteArray pString;

	m_nLineOffset = m_nBufferOffset + m_nBufferPtr;
	m_sLineText = "";

	for( ; ; )
	{

		if(m_nBufferPtr >= m_nBufferSize)
		{
			m_nBufferOffset = m_pFile->pos();
			m_nBufferSize = m_pFile->read(m_pBuffer.data(), m_pBuffer.size());
			m_nBufferPtr = 0;
			if(!m_nBufferSize)
				break;
		}

		char *pCharStart = m_pBuffer.data() + m_nBufferPtr;
		char *pCharEnd = m_pBuffer.data() + m_nBufferSize;
		char *pCharPtr = pCharStart;
		for( ; pCharPtr < pCharEnd && *pCharPtr != '\n'; pCharPtr++);

		m_nBufferPtr =  pCharPtr - m_pBuffer.data();

		pString.append(pCharStart, pCharPtr - pCharStart);

		if(pCharPtr != pCharEnd)
		{
			m_nBufferPtr++;
			break;
		}

	}

	m_sLineText = fSkipCR && pString.length() && pString[pString.length() - 1] == '\r' ? pString.remove(pString.length() - 1, 1) : pString;

	return m_sLineText.size();
}

bool CTextFile::ReadPrevLine(bool fSkipCR)
{

	QByteArray pString;

	m_nLineOffset = m_nBufferOffset + m_nBufferPtr;
	m_sLineText = "";

	for( ; ; )
	{

		if(m_nBufferPtr <= 0)
		{
			if(!m_nBufferOffset)
				return false;
			if(m_nBufferOffset >= m_pBuffer.size())
			{
				m_nBufferSize = m_pBuffer.size();
				m_nBufferOffset = m_nBufferOffset - m_pBuffer.size();
			}
			else
			{
				m_nBufferSize = m_nBufferOffset;
				m_nBufferOffset = 0;
			}
			m_pFile->seek(m_nBufferOffset);
			m_pFile->read(m_pBuffer.data(), m_nBufferSize);
			m_pFile->seek(m_nBufferOffset);
			m_nBufferPtr = m_nBufferSize;
			if(!m_nBufferSize)
				break;
		}

		char *pCharStart = m_pBuffer.data() + m_nBufferPtr;
		char *pCharEnd = m_pBuffer.data();
		char *pCharPtr = pCharStart;
		for( ; pCharPtr > pCharEnd && *(pCharPtr - 1) != '\n'; pCharPtr--);

		pString.insert(0, pCharPtr, pCharStart - pCharPtr);

		if(*(pCharPtr - 1) == '\n')
			pCharPtr--;
		m_nBufferPtr =  pCharPtr - m_pBuffer.data();

		if(pCharPtr != pCharEnd)
			break;

	}

	m_sLineText = fSkipCR && pString.length() && pString[pString.length() - 1] == '\r' ? pString.remove(pString.length() - 1, 1) : pString;

	return m_sLineText.size();
}

