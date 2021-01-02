//
//  @file LogFilter.h
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 23.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#ifndef LOGFILTER_H
#define LOGFILTER_H

#include "../Utils.h"
#include <QRegularExpression>

class CLogTheme;

class CLogFilter
{
public:

	CLogFilter();
	~CLogFilter();

	bool Load(QString sFilter, CLogTheme *pTheme, bool fStrict);

	bool Check(QString *pRecord);

	QString GetError() {
		return m_sError;
	}

protected:

	enum { TOKEN_INVALID = 0, TOKEN_BLOCK, TOKEN_DEC, TOKEN_HEX, TOKEN_STRING, TOKEN_FUNCTION = 100, TOKEN_OPERATOR = 200, TOKEN_IDENTIFIER = 300 };

	enum { OP_INVALID = 0, OP_LBRACE, OP_RBRACE, OP_DOT, OP_COMA, OP_BOOL_AND, OP_BOOL_OR, OP_REGEXP, OP_CONTS, OP_STARTS, OP_ENDS, OP_NREGEXP, OP_NCONTS, OP_NSTARTS, OP_NENDS, OP_BOOL_EQ, OP_BOOL_NEQ, OP_BOOL_GTE, OP_BOOL_LEE, OP_BOOL_GT, OP_BOOL_LE, OP_BOOL_NOT, OP_COUNT };
	enum { FN_INVALID = 0, FN_REGEXP, FN_CONTS, FN_STARTS, FN_ENDS, FN_NREGEXP, FN_NCONTS, FN_NSTART, FN_NEND, FN_EQ, FN_NEQ, FN_GTE, FN_LEE, FN_GT, FN_LE, FN_NOT, FN_COUNT };

	class CExprBase
	{
	public:
		int m_eType;
		QString m_sIdent;
		CExprBase(int eType, QString sIdent) {
			m_eType = eType;
			m_sIdent = sIdent;
		}
		virtual ~CExprBase() {
		}
		int Type() {
			return m_eType;
		}
		virtual void Destroy() {
			delete this;
		}
		virtual bool Evaluate() {
			return false;
		}
	};

	class CExprBlock : public CExprBase
	{
	public:
		QVector<CExprBase*> m_pExprList;
		CExprBlock() : CExprBase(TOKEN_BLOCK, "") {
		}
		~CExprBlock() {
		}
		void Add(CExprBase *pExpr) {
			m_pExprList.push_back(pExpr);
		}
		bool IsValid() {
			return m_pExprList.size() > 0;
		}
		virtual void Destroy() {
			for(int iExpr = 0; iExpr < m_pExprList.size(); iExpr++)
				m_pExprList[iExpr]->Destroy();
			m_pExprList.clear();
			CExprBase::Destroy();
		}
		virtual bool Evaluate() {
			if(!m_pExprList.size())
				return false;
			CExprBase *pExpr = m_pExprList[0];
			bool fResult = pExpr->Evaluate();
			for(int iExpr = 1; iExpr < m_pExprList.size(); iExpr++)
			{
				pExpr = m_pExprList[iExpr];
				iExpr++;
				if(pExpr->Type() == TOKEN_OPERATOR + OP_BOOL_AND)
					fResult = fResult && m_pExprList[iExpr]->Evaluate();
				else if(pExpr->Type() == TOKEN_OPERATOR + OP_BOOL_OR)
					fResult = fResult || m_pExprList[iExpr]->Evaluate();
			}
			return fResult;
		}
	};

	class CExprValue : public CExprBase
	{
	public:
		QString **m_pValue;
		CExprValue(int eType, QString sIdent, QString **pValue) : CExprBase(eType, sIdent) {
			m_pValue = pValue;
		}
		virtual void Destroy() {
			CExprBase::Destroy();
		}
		virtual QString &Value() {
			return m_pValue ? **m_pValue : m_sIdent;
		}
		virtual bool Evaluate() {
			return false;
		}
	};

	class CExprFunc : public CExprBase
	{
	public:
		CExprBase *m_pArg1;
		CExprBase *m_pArg2;
		QRegularExpression m_pRegExp;
		CExprFunc(int eType, CExprBase *pArg1, CExprBase *pArg2) : CExprBase(eType, "") {
			m_pArg1 = pArg1;
			m_pArg2 = pArg2;
			if(eType == TOKEN_FUNCTION + FN_REGEXP || eType == TOKEN_FUNCTION + FN_NREGEXP)
				m_pRegExp = QRegularExpression(((CExprValue*)m_pArg2)->Value(), QRegularExpression::CaseInsensitiveOption);
		}
		virtual void Destroy() {
			m_pArg1->Destroy();
			if(m_pArg1 != m_pArg2)
				m_pArg2->Destroy();
			CExprBase::Destroy();
		}
		virtual bool Evaluate() {
			if(m_eType == TOKEN_FUNCTION + FN_NOT)
				return !m_pArg1->Evaluate();
			QString &sValue1 = ((CExprValue*)m_pArg1)->Value();
			QString &sValue2 = ((CExprValue*)m_pArg2)->Value();
			switch (m_eType - TOKEN_FUNCTION)
			{
				case FN_REGEXP:
					return m_pRegExp.match(sValue1).hasMatch();
				case FN_NREGEXP:
					return !m_pRegExp.match(sValue1).hasMatch();
				case FN_CONTS:
					return sValue1.contains(sValue2, Qt::CaseInsensitive) == true;
				case FN_NCONTS:
					return sValue1.contains(sValue2, Qt::CaseInsensitive) == false;
				case FN_STARTS:
					return sValue1.startsWith(sValue2, Qt::CaseInsensitive) == true;
				case FN_NSTART:
					return sValue1.startsWith(sValue2, Qt::CaseInsensitive) == false;
				case FN_ENDS:
					return sValue1.endsWith(sValue2, Qt::CaseInsensitive) == true;
				case FN_NEND:
					return sValue1.endsWith(sValue2, Qt::CaseInsensitive) == false;
				case FN_EQ:
					return sValue1.compare(sValue2, Qt::CaseInsensitive) == 0;
				case FN_NEQ:
					return sValue1.compare(sValue2, Qt::CaseInsensitive) != 0;
				case FN_GTE:
					return sValue1 >= sValue2;
				case FN_LEE:
					return sValue1 <= sValue2;
				case FN_GT:
					return sValue1 > sValue2;
				case FN_LE:
					return sValue1 < sValue2;
			}
			return false;
		}
	};

	static const char *m_pOperator[];

	QString **m_pField;

	QString m_sError;
	QString m_sFilter;
	CExprBase *m_pExpression;

	CLogTheme *m_pTheme;

	bool Parse(QString &sFilter, QVector<int> &pToken, QStringList &pIdent);
	CExprBase *Build(QVector<int> &pToken, QStringList &pIdent, int nLevel, int *pIndex, bool fUnary);
	QString **GetFieldByToken(int iToken);

};

#endif
