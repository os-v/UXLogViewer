//
//  @file LogFileFlt.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 23.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGFILEFLT_H
#define LOGFILEFLT_H

#include <QThread>
#include <QVector>
#include <QBuffer>
#include <QTemporaryFile>
#include "TextFile.h"
#include "LogFilter.h"
#include "LogFile.h"

class QProgressDialog;

class CLogFile;
class CLogTheme;

class CLogFileFlt : public QThread, public ILogFile
{

	Q_OBJECT

public:

	CLogFileFlt();
	~CLogFileFlt();

	bool Create(ILogFile *pLogFile, QString sQuery, int nFrame, bool fMemFile = false);
	void Destroy();

	void UpdateTheme(CLogTheme *pTheme);

	void InsertRecord(qint64 nOffset);

	void SaveFile(QString sFile);

	void Reset();

	bool IsFilter(bool fParent) {
		return fParent ? (m_pLogFile && m_pLogFile->IsFilter(false)) : true;
	}

	bool IsFiltered() {
		return m_pLogFile != 0;
	}

	void EnumRecords(QStringList &pRecords);
	void EnumRecords(FOnEnumRecordsCallback OnEnumRecordsCallback, void *pContext);
	int PollRecords(qint64 *pOffset, QString **pFrame, CLogTheme *pTheme, int nCount);

	int FindRecord(qint64 nOffset);

	int MoveAt(qint64 nOffset);
	int MoveTo(int nOffset);
	int MoveOn(int nRecords);
	void ReadFrame();

	bool IsEOF() {
		return m_nFOffset == m_nFSize;
	}
	qint64 GetOffset() {
		return m_nFOffset;
	}
	int GetRecord() {
		return m_nRecord;
	}
	int GetSize() {
		return m_nSSize;
	}
	int GetReaded() {
		return m_nReaded;
	}

	void SetFrameSize(int nFrame);
	int GetFrameSize() {
		return m_nFrame;
	}

	QString *GetFrameLine(int iLine) {
		return iLine < m_nRecord || iLine - m_nRecord >= m_nReaded ? 0 : m_pFrame[iLine - m_nRecord];
	}

	qint64 GetFrameOffset(int iLine, bool fVOffset = false) {
		return iLine < m_nRecord || iLine - m_nRecord >= m_nReaded ? 0 : (fVOffset ? GetFrameVOffset(iLine - m_nRecord) : m_pOffset[iLine - m_nRecord]);
	}

	qint64 GetFrameVOffset(int iLine);

	CLogTheme *GetTheme() {
		return m_pTheme;
	}

	QBuffer *GetBuffer() {
		return qobject_cast<QBuffer*>(m_pFile);
	}

protected:

	CLogFilter m_pFilter;
	ILogFile *m_pLogFile;
	CLogTheme *m_pTheme;
	QIODevice *m_pFile;
	QIODevice *m_pFileV;
	QIODevice *m_pFileSave;
	bool m_fSave;

	int m_nFrame;
	int m_nSSize;
	qint64 m_nFSize;
	qint64 m_nFOffset;
	int m_nRecord;
	QString **m_pFrame;
	qint64 *m_pOffset;
	qint64 *m_pVOffset;
	int m_nReaded;

	volatile bool m_fTerminate;

	int m_nProgressDone;
	QProgressDialog *m_pProgress;

	void DestroyFrame();

	void ProcessData(bool fSave);

	void DoLoad();
	void DoSave();

	virtual void run();

	bool OnEnumRecordsCallback(qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage);
	static bool SOnEnumRecordsCallback(void *pContext, qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage) {
		return ((CLogFileFlt*)pContext)->OnEnumRecordsCallback(nROffset, nVOffset, nPercent, sMessage);
	}

signals:

	void ProgressUpdate(int nProgress);

private slots:

	void OnProgressUpdate(int nProgress);

};

#endif
