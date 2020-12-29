//
//  @file LogFilter.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 23.04.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "LogFilter.h"
#include "LogTheme.h"

#define ISDEC(c)		(c >= '0' && c <= '9')
#define ISHEX(c)		(c >= '0' && c <= '9' || c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F')
#define ISALPHA(c)		(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
#define ISWHITE(c)		(c == ' ' || c == '\t' || c == '\r' || c == '\n')

const char *CLogFilter::m_pOperator[] = { "", "(", ")", ".", ",", "&&", "||", "=#", "=~", "=^", "=$", "!#", "!~", "!^", "!$", "==", "!=", ">=", "<=", ">", "<", "!" };

CLogFilter::CLogFilter()
{

	m_pField = 0;

	m_pExpression = 0;

	m_pTheme = 0;

}

CLogFilter::~CLogFilter()
{

	if(m_pExpression)
	{
		m_pExpression->Destroy();
		m_pExpression = 0;
	}

}

bool CLogFilter::Load(QString sFilter, CLogTheme *pTheme)
{

	m_sError = "";

	m_pTheme = pTheme;

	if(m_pField)
		delete m_pField;

	m_pField = new QString*[m_pTheme->GetColCount()];
	for(int iField = 0; iField < m_pTheme->GetColCount(); iField++)
		m_pField[iField] = 0;

	QVector<int> pToken;
	QStringList pIdent;

	if(!Parse(sFilter, pToken, pIdent) || !pToken.size())
		return false;

	if(m_pExpression)
		m_pExpression->Destroy();

	m_pExpression = Build(pToken, pIdent, 0, 0, false);

	return m_pExpression != 0;
}

bool CLogFilter::Check(QString *pRecord)
{

	for(int iField = 0; iField < m_pTheme->GetColCount(); iField++)
		m_pField[iField] = &pRecord[iField];

	return m_pExpression->Evaluate();
}

bool CLogFilter::Parse(QString &sFilter, QVector<int> &pToken, QStringList &pIdent)
{

	ushort pStrOpen[]  = { '"', '\'', 171, 8216, 8217, 8218, 8220, 8221 };
	ushort pStrClose[] = { '"', '\'', 187, 8217, 8217, 8216, 8221, 8221 };

	for(const ushort *lpText = sFilter.utf16(); *lpText; lpText++)
	{

		ushort cCurr = *lpText;
		ushort cNext = *(lpText + 1);
		int eToken = TOKEN_INVALID;
		QString sToken = "";

		if(ISWHITE(cCurr))
			continue;

		int iStrChar = -1;
		if(cCurr == '0' && (cNext == 'x' || cNext == 'X'))
		{
			lpText++, lpText++;
			for(; *lpText && ISHEX(*lpText); lpText++)
				sToken += *lpText;
			eToken = TOKEN_HEX;
			if(*lpText) lpText--;
		}
		else if(ISDEC(cCurr))
		{
			for(; *lpText && ISDEC(*lpText); lpText++)
				sToken += *lpText;
			eToken = TOKEN_DEC;
			if(*lpText) lpText--;
		}
		else if(ISALPHA(cCurr))
		{
			int iItem = 0;
			for(; *lpText && ISALPHA(*lpText); lpText++)
				sToken += QChar(*lpText);
			for(iItem = 0; eToken == TOKEN_INVALID && iItem < m_pTheme->GetColCount() && sToken.compare(m_pTheme->FieldName[iItem], Qt::CaseInsensitive); iItem++);
			eToken = eToken == TOKEN_INVALID && iItem != m_pTheme->GetColCount() ? TOKEN_IDENTIFIER + iItem : eToken;
			if(*lpText) lpText--;
		}
		else if((iStrChar = ArrayFindIndex(pStrOpen, ARRAY_SIZE(pStrOpen), cCurr)) != -1)
		{
			lpText++;
			for(; *lpText && *lpText != pStrClose[iStrChar]; lpText++)
				sToken += QChar(*lpText);
			eToken = *lpText ? TOKEN_STRING : TOKEN_INVALID;
		}
		else if((iStrChar = StrArrayFindIndex(m_pOperator, 1, ARRAY_SIZE(m_pOperator), lpText)) != -1)
			eToken = TOKEN_OPERATOR + iStrChar;

		if(eToken >= TOKEN_OPERATOR + OP_BOOL_AND && eToken <= TOKEN_OPERATOR + OP_BOOL_LEE)
			lpText++;

		pToken.push_back(eToken);
		pIdent.push_back(sToken);

		if(!*lpText)
			break;

	}

	return true;
}

#define NEXT_TOKEN(msg)						iToken++; if((eNext = (iToken < (int)pToken.size() ? pToken[iToken] : TOKEN_INVALID)) == TOKEN_INVALID) { m_sError = msg; break; };
#define ERROR_BREAK(Cond, ErrorMessage)		if(Cond) { m_sError = ErrorMessage; break; }

CLogFilter::CExprBase *CLogFilter::Build(QVector<int> &pToken, QStringList &pIdent, int nLevel, int *pIndex, bool fUnary)
{

	m_sError = "";

	QList<CExprBase*> pExprList;

	int lToken = 0;
	int &iToken = pIndex ? *pIndex : lToken;

	CExprBlock *pExpr = new CExprBlock();

	for(; iToken < (int)pToken.size(); iToken++)
	{
		int eCurr = pToken[iToken], eNext = -1;
		QString sCurr = pIdent[iToken];
		bool fOpNot = eCurr == TOKEN_OPERATOR + OP_BOOL_NOT;
		if(eCurr >= TOKEN_IDENTIFIER && !fUnary)
		{
			NEXT_TOKEN("Invalid expression after: " + sCurr);
			int eOpCode = eNext - TOKEN_OPERATOR;
			ERROR_BREAK(!(eOpCode >= OP_REGEXP && eOpCode <= OP_BOOL_LE), "Invalid logic operator: " + QString::number(eOpCode));
			int eFunc = eOpCode - OP_REGEXP + FN_REGEXP;
			NEXT_TOKEN("Invalid identifier after: " + sCurr);
			ERROR_BREAK(!(eNext >= TOKEN_DEC && eNext <= TOKEN_STRING), "Invalid identifier: " + QString::number(eOpCode));
			CExprValue *pArg1 = new CExprValue(eCurr, sCurr, GetFieldByToken(eCurr));
			CExprValue *pArg2 = new CExprValue(eNext, pIdent[iToken], GetFieldByToken(eNext));
			CExprFunc *pFunc = new CExprFunc(TOKEN_FUNCTION + eFunc, pArg1, pArg2);
			pExpr->Add(pFunc);
		}
		else if(eCurr == TOKEN_OPERATOR + OP_LBRACE || fOpNot)
		{
			iToken++;
			CLogFilter::CExprBase *pInnerExpr = Build(pToken, pIdent, nLevel + (fOpNot ? 0 : 1), &iToken, fOpNot);
			if (!pInnerExpr)
				break;
			pExpr->Add(fOpNot ? new CExprFunc(TOKEN_FUNCTION + FN_NOT, pInnerExpr, pInnerExpr) : pInnerExpr);
		}
		else if(eCurr == TOKEN_OPERATOR + OP_RBRACE)
		{
			ERROR_BREAK(!nLevel, "Invalid operator ) ");
			break;
		}
		else if(eCurr > TOKEN_OPERATOR && eCurr < TOKEN_IDENTIFIER && !fUnary)
		{
			int eOpCode = eCurr - TOKEN_OPERATOR;
			ERROR_BREAK(!(eOpCode >= OP_BOOL_AND && eOpCode <= OP_BOOL_OR), "Invalid logic operator: " + QString::number(eOpCode));
			CExprBase *pLogic = new CExprBase(eCurr, "");
			pExpr->Add(pLogic);
		}
		else
		{
			m_sError = "Invalid token: " + QString::number(eCurr) + " [" + sCurr + "]";
			break;
		}
		if (fUnary)
			return pExpr;
	}

	if(!m_sError.isEmpty() || !pExpr->IsValid())
	{
		pExpr->Destroy();
		pExpr = 0;
	}

	return pExpr;
}

QString **CLogFilter::GetFieldByToken(int iToken)
{

	iToken -= TOKEN_IDENTIFIER;
	if(iToken < 0 || iToken >= m_pTheme->GetColCount())
		return 0;

	return &m_pField[iToken];
}

