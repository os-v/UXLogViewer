//
//  @file LogFileRaw.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogFileRaw.h"
#include <QProgressDialog>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "LogTheme.h"
#include "LogFilter.h"

CLogFileRaw::CLogFileRaw()
{

	m_sFile = "";
	m_pTheme = &CLogTheme::Instance();

	m_nFrame = 0;
	m_nSSize = 0;
	m_nFSize = 0;
	m_nFOffset = 0;
	m_nRecord = 0;
	m_pFrame = 0;
	m_pOffset = 0;
	m_nReaded = 0;

	m_pFileSave = 0;

	m_fTerminate = false;

	m_pProgress = 0;
	m_nProgressDone = 0;

}

CLogFileRaw::~CLogFileRaw()
{

	Destroy();

}

bool CLogFileRaw::Create(QString sPath, int nFrame, CLogTheme *pLogTheme)
{

	m_pFile.Create(sPath, 1024);
	if(!m_pFile.IsOpened())
	{
		Reset();
		return false;
	}

	m_sFile = sPath;
	m_nFrame = nFrame;
	m_nSSize = DEFAULT_ROWSCOUNT;
	m_nFSize = 0;
	m_nFOffset = -1;
	m_nRecord = -1;
	m_nReaded = 0;
	m_fTerminate = false;
	m_pTheme = pLogTheme;

	SetFrameSize(nFrame);

	ProcessData(false);

	return true;
}

void CLogFileRaw::Destroy()
{

	DestroyFrame();

	SafeDelete(m_pFileSave);

	m_pFile.Destroy();

}

void CLogFileRaw::ProcessData(bool fSave)
{

	QProgressDialog pProgress(QApplication::activeWindow());
	//pProgress.setWindowIcon(QApplication::activeWindow()->windowIcon());
	m_pProgress = &pProgress;
	//m_pProgress->setWindowModality(Qt::WindowModal);
	m_pProgress->setModal(true);
	m_pProgress->setLabelText(fSave ? "Saving file data ..." : "Caching file data ...");
	m_pProgress->setRange(0, 100);
	m_pProgress->setValue(0);

	connect(this, SIGNAL(ProgressUpdate(int)), this, SLOT(OnProgressUpdate(int)), Qt::BlockingQueuedConnection);

	start();

	m_pProgress->exec();

	m_pProgress->close();

	disconnect();

	m_fTerminate = true;

	QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	wait();

}

void CLogFileRaw::UpdateTheme(CLogTheme *pTheme)
{

	m_pTheme = pTheme;

	if(m_pFrame)
		SetFrameSize(m_nFrame);

	ReadFrame();

}

void CLogFileRaw::SaveFile(QString sFile)
{

	m_fTerminate = false;

	m_pFileSave = new QFile(sFile);
	if(m_pFileSave->open(QIODevice::WriteOnly | QIODevice::Truncate))
		ProcessData(true);

	SafeDelete(m_pFileSave);

}

void CLogFileRaw::Reset()
{

	Destroy();

	m_nSSize = 0;

	m_nReaded = 0;

}

void CLogFileRaw::EnumRecords(FOnEnumRecordsCallback OnEnumRecordsCallback, void *pContext)
{

	QFile *pFile = m_pFile.GetFileObject();
	if(!pFile)
		return;

	qint64 nPos = pFile->pos();

	pFile->seek(0);

	qint64 nFSize = pFile->size();

	CTextFile pStream;
	pStream.Attach(pFile);
	for(; !pStream.IsEOF(); )
	{
		pStream.ReadNextLine();
		QString sLine = pStream.GetLineText();
		qint64 nOffset = pStream.GetLineOffset();
		int nProgress = (int)(nOffset * 100 / nFSize);
		if(!OnEnumRecordsCallback(pContext, nOffset, -1, nProgress, sLine))
			break;
	}
	pStream.Detach();

	pFile->seek(nPos);

}

int CLogFileRaw::PollRecords(qint64 *pOffset, QString **pFrame, CLogTheme *pTheme, int nCount)
{

	QFile *pFile = m_pFile.GetFileObject();
	if(!pFile)
		return 0;

	qint64 nPos = pFile->pos();

	pFile->seek(0);

	int nReaded = 0;

	int nColCount = pTheme ? pTheme->GetColCount() : 0;

	CTextFile pStream;
	pStream.Attach(pFile, 256);
	for(nReaded = 0; nReaded < nCount && !pStream.IsEOF(); nReaded++)
	{
		pStream.SetOffset(pOffset[nReaded]);
		pStream.ReadNextLine();
		QString sMessage = pStream.GetLineText();
		if(pTheme)
		{
			QString *pArgs = pFrame[nReaded];
			if(!pTheme->Parse(sMessage, pArgs))
			{
				for(int iArg = 0; iArg < nColCount; iArg++)
					pArgs[iArg] = "";
				pArgs[nColCount - 1] = sMessage;
			}
		}
		else
			(*pFrame)[nReaded] = sMessage;
	}
	pStream.Detach();

	pFile->seek(nPos);

	return nReaded;
}

void CLogFileRaw::SetFrameSize(int nFrame)
{

	DestroyFrame();

	m_pFrame = new QString*[nFrame + 1];
	for(int iRecord = 0; iRecord < nFrame + 1; iRecord++)
		m_pFrame[iRecord] = new QString[m_pTheme->GetColCount()];

	if(m_nFrame == nFrame && m_pOffset)
		return;

	m_nFrame = nFrame;

	m_pOffset = new qint64[nFrame + 1];
	for(int iRecord = 0; iRecord < nFrame + 1; iRecord++)
		m_pOffset[iRecord] = -1;

}

void CLogFileRaw::DestroyFrame()
{

	if(m_pFrame)
	{
		for(int iRecord = 0; iRecord < m_nFrame + 1; iRecord++)
			delete[] m_pFrame[iRecord];
		delete m_pFrame;
		m_pFrame = 0;
		m_nFrame = 0;
	}

	SafeDelete(m_pOffset);

}

int CLogFileRaw::FindRecord(qint64 nOffset)
{

	if(!m_pOffset)
		return -1;

	for(int iRecord = 0; iRecord < m_nReaded; iRecord++)
	{
		if(m_pOffset[iRecord] == nOffset)
			return m_nRecord + iRecord;
	}

	return nOffset == m_nFSize ? m_nSSize - 1 : -1;
}

int CLogFileRaw::MoveAt(qint64 nFOffset)
{

	m_nFOffset = nFOffset;

	qint64 nOffset = m_nFOffset;
	nOffset *= (m_nSSize - m_nFrame);
	nOffset /= m_nFSize;

	m_nRecord = (int)nOffset;

	ReadFrame();

	if(m_nReaded < m_nFrame)
	{
		m_nFOffset = m_nFSize;
		MoveOn(-m_nFrame + 1);
		m_nRecord = m_nSSize - m_nFrame;
	}

	return m_nRecord;
}

int CLogFileRaw::MoveTo(int nOffset)
{

	if(m_nSSize - m_nFrame == 0 || !m_pFile.IsOpened())
		return m_nRecord;

	qint64 nFOffset = m_nFSize;
	nFOffset *= nOffset;
	nFOffset /= (m_nSSize - m_nFrame);
	//if(nFOffset == m_nFOffset)
		//return false;

	m_nFOffset = nFOffset;

	m_nRecord = nOffset;

	m_pFile.SetOffset(nFOffset);
	if(nFOffset == m_nFSize)
	{
		MoveOn(-m_nFrame + 1);
		m_nRecord = nOffset;
		return m_nRecord;
	}

	if(nFOffset)
		m_pFile.ReadNextLine();

	m_nFOffset = m_pFile.GetOffset();

	ReadFrame();

	if(m_nReaded < m_nFrame)
	{
		m_nFOffset = m_nFSize;
		MoveOn(-m_nFrame + 1);
		m_nRecord = m_nSSize - m_nFrame;
	}

	return m_nRecord;
}

int CLogFileRaw::MoveOn(int nRecords)
{

	if(!m_nFSize || !m_pFile.IsOpened())
		return 0;

	if(nRecords > 0 && m_pFile.IsEOF())
	{
		m_nFOffset = m_nFSize;
		MoveOn(-m_nFrame + 1);
		m_nRecord = m_nSSize - m_nFrame;
		return m_nRecord;
	}

	bool fLastPage = m_nFOffset == m_nFSize;
	m_pFile.SetOffset(m_nFOffset);

	for(int iSkip = 0; iSkip < nRecords && !m_pFile.IsEOF(); iSkip++)
		m_pFile.ReadNextLine();

	for(int iSkip = nRecords - (nRecords < 0 ? 1 : 0); iSkip < 0 && !m_pFile.IsBOF(); iSkip++)
		m_pFile.ReadPrevLine();

	m_nFOffset = m_pFile.GetOffset();
	if(m_nFOffset && nRecords < 0)
		m_nFOffset++;
	qint64 nOffset = m_nFOffset;
	nOffset *= (m_nSSize - m_nFrame);
	nOffset /= m_nFSize;

	if(!nOffset && m_nFOffset > 0)
		nOffset++;

	m_nRecord = (int)nOffset;

	ReadFrame();

	if(m_nReaded < m_nFrame && !fLastPage)
	{
		m_nFOffset = m_nFSize;
		MoveOn(-m_nFrame + 1);
		m_nRecord = m_nSSize - m_nFrame;
	}

	return m_nRecord;
}

void CLogFileRaw::ReadFrame()
{

	if(!m_pFile.IsOpened())
		return;

	m_pFile.SetOffset(m_nFOffset);

	int nColCount = m_pTheme->GetColCount();
	for(m_nReaded = 0; m_nReaded < m_nFrame + 1 && !m_pFile.IsEOF(); m_nReaded++)
	{
		m_pFile.ReadNextLine();
		m_pOffset[m_nReaded] = m_pFile.GetLineOffset();
		QString *pArgs = m_pFrame[m_nReaded];
		if(!m_pTheme->Parse(m_pFile.GetLineText(), pArgs))
		{
			for(int iArg = 0; iArg < nColCount; iArg++)
				pArgs[iArg] = "";
			pArgs[nColCount - 1] = m_pFile.GetLineText();
		}
	}

}

void CLogFileRaw::DoLoad()
{

	emit ProgressUpdate(0);

	QFile *pFile = m_pFile.GetFileObject();
	m_nFSize = pFile->size();
	if(!m_nFSize)
		m_nSSize = 0;

	pFile->seek(0);

	emit ProgressUpdate(100);

	emit ProgressUpdate(-1);

}

void CLogFileRaw::DoSave()
{

	emit ProgressUpdate(0);

	EnumRecords(CLogFileRaw::SOnEnumRecordsCallback, this);

	emit ProgressUpdate(100);

	emit ProgressUpdate(-1);

}

void CLogFileRaw::run()
{

	if (!m_pFileSave)
		DoLoad();
	else
		DoSave();

}

void CLogFileRaw::OnProgressUpdate(int nProgress)
{

	m_pProgress->setValue(nProgress);

	if(nProgress == -1)
		m_pProgress->close();

}

bool CLogFileRaw::OnEnumRecordsCallback(qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage)
{

	if(nPercent != m_nProgressDone)
	{
		m_nProgressDone = nPercent;
		emit ProgressUpdate(m_nProgressDone);
	}

	m_pFileSave->write((sMessage + "\r\n").toLatin1().data());

	return !m_fTerminate;
}

