//
//  @file LogFileRaw.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGFILERAW_H
#define LOGFILERAW_H

#include <QThread>
#include <QVector>
#include "TextFile.h"
#include "LogFile.h"

class QProgressDialog;

class CLogTheme;

class CLogFileRaw : public QThread, public ILogFile
{

	Q_OBJECT

public:

	CLogFileRaw();
	~CLogFileRaw();

	bool Create(QString sPath, int nFrame, CLogTheme *pLogTheme);
	void Destroy();

	void UpdateTheme(CLogTheme *pTheme);

	void SaveFile(QString sFile);

	void Reset();

	bool IsFilter(bool fParent) {
		return false;
	}

	bool IsFiltered() {
		return false;
	}

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
		return iLine < m_nRecord || iLine - m_nRecord >= m_nReaded ? m_nFSize : m_pOffset[iLine - m_nRecord];
	}

	CLogTheme *GetTheme() {
		return m_pTheme;
	}

protected:

	QString m_sFile;
	CLogTheme *m_pTheme;

	int m_nFrame;
	int m_nSSize;
	qint64 m_nFSize;
	qint64 m_nFOffset;
	int m_nRecord;
	QString **m_pFrame;
	qint64 *m_pOffset;
	int m_nReaded;

	CTextFile m_pFile;
	QIODevice *m_pFileSave;

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
		return ((CLogFileRaw*)pContext)->OnEnumRecordsCallback(nROffset, nVOffset, nPercent, sMessage);
	}

signals:

	void ProgressUpdate(int nProgress);

private slots:

	void OnProgressUpdate(int nProgress);

};

#endif
