//
//  @file TextFile.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 12.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <QFile>

class CTextFile
{
public:

	CTextFile();
	~CTextFile();

	void Attach(QFile *pFile, int nFrame = 64 * 1024);
	void Detach();

	bool Create(QString sPath, int nFrame = 64 * 1024);
	void Destroy();

	void Update();

	bool ReadNextLine(bool fSkipCR = true, int nLineLimit = 0);
	bool ReadPrevLine(bool fSkipCR = true, int nLineLimit = 0);

	qint64 GetFrameOffset() {
		return m_nBufferOffset;
	}

	bool SetOffset(qint64 nOffset) {

		if(!m_pFile->seek(nOffset))
			return false;

		m_nBufferSize = 0;
		m_nBufferOffset = nOffset;
		m_nBufferPtr = 0;

		return true;
	}

	bool IsOpened() {
		return m_pFile != 0;
	}

	qint64 GetSize() {
		return m_nFileSize;
	}

	qint64 GetOffset() {
		return m_nBufferOffset + m_nBufferPtr;
	}

	bool IsBOF() {
		return m_pFile->pos() == 0 && m_nBufferPtr <= 0;
	}

	bool IsEOF() {
		return m_pFile->pos() == m_nFileSize && m_nBufferPtr >= m_nBufferSize;
	}

	qint64 GetLineOffset() {
		return m_nLineOffset;
	}

	QString &GetLineText() {
		return m_sLineText;
	}

	QFile *GetFileObject() {
		return m_pFile;
	}

protected:

	QFile *m_pFile;

	qint64 m_nFileSize;

	int m_nBufferSize;
	qint64 m_nBufferOffset;
	int m_nBufferPtr;
	QByteArray m_pBuffer;

	qint64 m_nLineOffset;
	QString m_sLineText;

};

#endif
