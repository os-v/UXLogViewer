//
//  @file LogFileFlt.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 23.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogFileFlt.h"
#include <QProgressDialog>
#include <QApplication>
#include <QFile>
#include <QBuffer>
#include <QTextStream>
#include <QDebug>
#include "LogTheme.h"
#include "LogFilter.h"
#include "LogFile.h"

CLogFileFlt::CLogFileFlt()
{

	m_pLogFile = 0;
	m_pTheme = &CLogTheme::Instance();
	m_pFile = 0;
	m_pFileV = 0;
	m_pFileSave = 0;
	m_fSave = false;

	m_nFrame = 0;
	m_nSSize = 0;
	m_nFSize = 0;
	m_nFOffset = 0;
	m_nRecord = 0;
	m_pFrame = 0;
	m_pOffset = 0;
	m_pVOffset = 0;
	m_nReaded = 0;

	m_fTerminate = false;

	m_nProgressDone = 0;
	m_pProgress = 0;

}

CLogFileFlt::~CLogFileFlt()
{

	Destroy();

}

bool CLogFileFlt::Create(ILogFile *pLogFile, QString sQuery, int nFrame, bool fMemFile)
{

	Destroy();

	m_pLogFile = pLogFile;
	m_pTheme = m_pLogFile->GetTheme();
	m_pFile = fMemFile ? (QIODevice*)new QBuffer() : (QIODevice*)new QTemporaryFile();
	if(!m_pTheme || /*!fMemFile && */!((QTemporaryFile*)m_pFile)->open() || !sQuery.isEmpty() && !m_pFilter.Load(sQuery, m_pTheme))
		return false;

	if(pLogFile->IsFilter(false))
	{
		m_pFileV = fMemFile ? (QIODevice*)new QBuffer() : (QIODevice*)new QTemporaryFile();
		if(!fMemFile)
			((QTemporaryFile*)m_pFileV)->open();
	}

	m_nFrame = nFrame;
	m_nSSize = DEFAULT_ROWSCOUNT;
	m_nFSize = 0;
	m_nFOffset = 0;
	m_nRecord = 0;
	m_nReaded = 0;
	m_fTerminate = false;

	SetFrameSize(nFrame);

	if(sQuery.isEmpty())
		m_nSSize = 0;
	else
		ProcessData(false);

	return true;//m_nSSize > 0;
}

void CLogFileFlt::Destroy()
{

	DestroyFrame();

	SafeDelete(m_pFile);
	SafeDelete(m_pFileV);
	SafeDelete(m_pFileSave);

	m_pLogFile = 0;

	m_nSSize = 0;

}

void CLogFileFlt::ProcessData(bool fSave)
{

	QProgressDialog pProgress(QApplication::activeWindow());
	//pProgress.setWindowIcon(QApplication::activeWindow()->windowIcon());
	m_pProgress = &pProgress;
	//m_pProgress->setWindowModality(Qt::WindowModal);
	m_pProgress->setModal(true);
	m_pProgress->setLabelText(fSave ? "Saving data ..." : "Filtering data ...");
	m_pProgress->setRange(0, 100);
	m_pProgress->setValue(0);

	connect(this, SIGNAL(ProgressUpdate(int)), this, SLOT(OnProgressUpdate(int)), Qt::QueuedConnection);

	m_fTerminate = false;
	
	start();

	m_pProgress->exec();

	disconnect();

	m_fTerminate = true;

	QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	wait();

	m_pProgress->close();

}

void CLogFileFlt::UpdateTheme(CLogTheme *pTheme)
{

	m_pTheme = pTheme;

	if(m_pFrame)
		SetFrameSize(m_nFrame);

}

void CLogFileFlt::InsertRecord(qint64 nOffset)
{

	if(!qobject_cast<QBuffer*>(m_pFile))
		return;

	QByteArray &pBuffer = ((QBuffer*)m_pFile)->buffer();

	QInsertValue(pBuffer, nOffset, true);
	//pBuffer.append((char*)&nOffset, sizeof(nOffset));

	m_nFSize = m_pFile->size() / sizeof(qint64);

	m_nSSize = m_nFSize > DEFAULT_ROWSCOUNT ? DEFAULT_ROWSCOUNT : (int)m_nFSize;

}

void CLogFileFlt::SaveFile(QString sFile)
{

	m_pFileSave = new QFile(sFile);
	if(m_pFileSave->open(QIODevice::WriteOnly | QIODevice::Truncate))
		ProcessData(true);

	delete m_pFileSave;
	m_pFileSave = 0;

}

void CLogFileFlt::Reset()
{

	//Destroy();

	m_nSSize = 0;

	m_nReaded = 0;

	m_pLogFile = 0;

}

bool SEnumAllRecordsCallback(void *pContext, qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage)
{
	QStringList *pRecords = (QStringList*)pContext;
	pRecords->append(sMessage);
	return true;
}

void CLogFileFlt::EnumRecords(QStringList &pRecords)
{
	EnumRecords(SEnumAllRecordsCallback, (void*)&pRecords);
}

void CLogFileFlt::EnumRecords(FOnEnumRecordsCallback OnEnumRecordsCallback, void *pContext)
{

	if(!m_pFile)
		return;

	qint64 nPos = m_pFile->pos();

	qint64 nRecords = m_pFile->size() / sizeof(qint64), nRecord = 0;

	m_pFile->seek(0);

	qint64 pOffset[256];
	QString pRecords[256];
	QString *pRecordsPtr = pRecords;
	for(qint64 nReaded = 0, nVOffset = 0; (nReaded = m_pFile->read((char*)&pOffset[0], sizeof(pOffset))) > 0; )
	{
		nReaded /= sizeof(qint64);
		m_pLogFile->PollRecords(pOffset, &pRecordsPtr, 0, nReaded);
		for(int index = 0; index < nReaded; index++, nRecord++, nVOffset += sizeof(qint64))
			OnEnumRecordsCallback(pContext, pOffset[index], nVOffset, (int)(nRecord * 100 / nRecords), pRecords[index]);
	}

	m_pFile->seek(nPos);

}

int CLogFileFlt::PollRecords(qint64 *pOffset, QString **pFrame, CLogTheme *pTheme, int nCount)
{
	return m_pLogFile->PollRecords(pOffset, pFrame, pTheme, nCount);
}

void CLogFileFlt::SetFrameSize(int nFrame)
{

	DestroyFrame();

	CLogTheme *pTheme = m_pTheme;

	m_pFrame = new QString*[nFrame + 1];
	for(int iRecord = 0; iRecord < nFrame + 1; iRecord++)
		m_pFrame[iRecord] = new QString[pTheme->GetColCount()];

	if(m_nFrame == nFrame && m_pOffset)
		return;

	m_nFrame = nFrame;

	m_pOffset = new qint64[nFrame + 1];
	memset(m_pOffset, 0, (nFrame + 1) * sizeof(qint64));
	m_pVOffset = new qint64[nFrame + 1];
	memset(m_pVOffset, 0, (nFrame + 1) * sizeof(qint64));

}

void CLogFileFlt::DestroyFrame()
{

	if(m_pFrame)
	{
		for(int iRecord = 0; iRecord < m_nFrame + 1; iRecord++)
			delete[] m_pFrame[iRecord];
		delete m_pFrame;
		m_pFrame = 0;
		m_nFrame = 0;
		//m_nRecord = 0;
	}

	SafeDelete(m_pOffset);
	SafeDelete(m_pVOffset);

}

int CLogFileFlt::FindRecord(qint64 nOffset)
{

	for(int iRecord = 0; iRecord < m_nReaded; iRecord++)
	{
		if(m_pOffset[iRecord] == nOffset)
			return m_nRecord + iRecord;
	}

	return -1;
}

int CLogFileFlt::MoveAt(qint64 nFOffset)
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
		MoveOn(-m_nFrame);
	}

	return m_nRecord;
}

int CLogFileFlt::MoveTo(int nOffset)
{

	if(m_nFSize == 0)
		return 0;

	m_nFOffset = (m_nFSize - m_nFrame);
	m_nFOffset *= nOffset;
	m_nFOffset /= (m_nSSize - m_nFrame ? m_nSSize - m_nFrame : 1);

	if(m_nFOffset + m_nFrame > m_nFSize)
		m_nFOffset = m_nFSize - m_nFrame < 0 ? 0 : m_nFSize - m_nFrame;

	m_nRecord = nOffset;

	ReadFrame();

	return m_nRecord;
}

int CLogFileFlt::MoveOn(int nRecords)
{

	if(m_nFSize == 0)
		return 0;

	m_nFOffset += nRecords;
	if(m_nFOffset < 0)
		m_nFOffset = 0;
	if(m_nFOffset + m_nFrame >= m_nFSize)
	{
		m_nFOffset = m_nFSize - m_nFrame < 0 ? 0 : m_nFSize - m_nFrame;
		m_nRecord = m_nSSize - m_nFrame;
	}
	else
	{
		qint64 nRecord = m_nFOffset;
		nRecord *= (m_nSSize - m_nFrame);
		nRecord /= (m_nFSize - m_nFrame);
		m_nRecord = nRecord;
	}

	if(!m_nRecord && m_nFOffset > 0)
		m_nRecord++;

	ReadFrame();

	return m_nRecord;
}

void CLogFileFlt::ReadFrame()
{

	if(!m_pFile || !m_pLogFile)
		return;

	m_pFile->seek(m_nFOffset * sizeof(qint64));

	m_nReaded = m_pFile->read((char*)m_pOffset, (m_nFrame + 1) * sizeof(qint64));
	if(m_nReaded <= 0)
		return;

	m_nReaded /= sizeof(qint64);

	m_nReaded = m_pLogFile->PollRecords(m_pOffset, m_pFrame, m_pTheme, m_nReaded);

}

qint64 CLogFileFlt::GetFrameVOffset(int iLine)
{

	if(!m_pFileV)
		return 0;

	qint64 nVOffset = 0;
	m_pFileV->seek((m_nFOffset + iLine) * sizeof(qint64));
	m_pFileV->read((char*)&nVOffset, sizeof(nVOffset));

	return nVOffset / sizeof(qint64);
}

void CLogFileFlt::DoLoad()
{

	emit ProgressUpdate(0);

	m_nProgressDone = 0;
	m_pLogFile->EnumRecords(CLogFileFlt::SOnEnumRecordsCallback, this);

	m_nFSize = m_pFile->size() / sizeof(qint64);
	if(!m_nFSize)
		m_nSSize = 0;

	m_nSSize = m_nFSize > DEFAULT_ROWSCOUNT ? DEFAULT_ROWSCOUNT : m_nFSize;

	emit ProgressUpdate(100);

	emit ProgressUpdate(-1);

}

void CLogFileFlt::DoSave()
{

	emit ProgressUpdate(0);

	m_nProgressDone = 0;

	EnumRecords(CLogFileFlt::SOnEnumRecordsCallback, this);

	emit ProgressUpdate(100);

	emit ProgressUpdate(-1);

}

void CLogFileFlt::run()
{

	if (!m_pFileSave)
		DoLoad();
	else
		DoSave();

}

bool CLogFileFlt::OnEnumRecordsCallback(qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage)
{

	if(nPercent != m_nProgressDone)
	{
		m_nProgressDone = nPercent;
		emit ProgressUpdate(m_nProgressDone);
	}

	if(m_pFileSave)
	{
		m_pFileSave->write((sMessage + "\r\n").toLatin1().data());
		return !m_fTerminate;
	}

	QString *pArgs = m_pFrame[0];

	CLogTheme *pTheme = m_pTheme;
	if(!pTheme->Parse(sMessage, pArgs))
	{
		for(int iArg = 0; iArg < pTheme->GetColCount(); iArg++)
			pArgs[iArg] = "";
		pArgs[pTheme->GetColCount() - 1] = sMessage;
	}

	if(m_pFilter.Check(pArgs))
	{
		m_pFile->write((const char*)&nROffset, sizeof(nROffset));
		if(m_pFileV)
			m_pFileV->write((const char*)&nVOffset, sizeof(nVOffset));
	}

	return !m_fTerminate;
}

void CLogFileFlt::OnProgressUpdate(int nProgress)
{

	m_pProgress->setValue(nProgress);

	if(nProgress == -1)
		m_pProgress->close();

}

