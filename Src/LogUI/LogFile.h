//
//  @file LogFile.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 23.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGFILE_H
#define LOGFILE_H

#include <QThread>

#define interface struct

class CLogTheme;

#define DEFAULT_ROWSCOUNT		10000

interface ILogFile
{

	virtual void SaveFile(QString sFile) = 0;

	virtual void Reset() = 0;

	virtual bool IsFilter(bool fParent) = 0;
	virtual bool IsFiltered() = 0;

	typedef bool (*FOnEnumRecordsCallback)(void *pContext, qint64 nROffset, qint64 nVOffset, int nPercent, QString &sMessage);
	virtual void EnumRecords(FOnEnumRecordsCallback OnEnumRecordsCallback, void *pContext) = 0;
	virtual int PollRecords(qint64 *pOffset, QString **pFrame, CLogTheme *pTheme, int nCount) = 0;

	virtual int FindRecord(qint64 nOffset) = 0;

	virtual int MoveAt(qint64 nOffset) = 0;
	virtual int MoveTo(int nOffset) = 0;
	virtual int MoveOn(int nRecords) = 0;
	virtual void ReadFrame() = 0;

	virtual bool IsEOF() = 0;
	virtual qint64 GetOffset() = 0;
	virtual int GetRecord() = 0;
	virtual int GetSize() = 0;
	virtual int GetReaded() = 0;

	virtual void SetFrameSize(int nFrame) = 0;
	virtual int GetFrameSize() = 0;

	virtual QString *GetFrameLine(int iLine) = 0;
	virtual qint64 GetFrameOffset(int iLine, bool fVOffset = false) = 0;
	virtual CLogTheme *GetTheme() = 0;

};

#endif
