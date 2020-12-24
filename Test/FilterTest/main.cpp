//
//  @file main.cpp
//  @author Sergii Oryshchenko <sergii.orishchenko@gmail.com>
//  @see https://github.com/os-v/UXLogViewer/
//
//  Created on 20.07.19.
//  Copyright 2020 Sergii Oryshchenko. All rights reserved.
//

#include "../../Src/LogUI/LogFilter.h"
#include "../../Src/LogUI/LogTheme.h"

#define CSTR(s)		s.toLatin1().data()

void Verify(const char *lpFilter, QString sRecordA, QString sRecordB, QString sRecordC, bool fRequire)
{
	CLogFilter pFilter;
	pFilter.Load(lpFilter, &CLogTheme::Instance());
	QString pRecord[] = { sRecordA, sRecordB, sRecordC };
	bool fResult = pFilter.Check(pRecord);
	printf("%s : (%s, %s, %s) -> %s [%s]\r\n", lpFilter, CSTR(sRecordA), CSTR(sRecordB), CSTR(sRecordC), fResult ? "true" : "false", fResult == fRequire ? "OK" : "FAILED");
}

int main(int argc, char *argv[])
{

	CLogTheme::Instance().Select(CLogTheme::Instance().AddTheme("Custom1\n[%Msg%]"));

	const char *lpTextA = "qwer test afsdf test1 zxcv";
	const char *lpTextB = "";
	const char *lpTextC = "";

	Verify("msg=~\"test2\"", lpTextA, "", "", false);
	Verify("msg!~\"test2\"", lpTextA, "", "", true);
	Verify("!(msg=~\"test2\")", lpTextA, "", "", true);
	Verify("!(msg!~\"test2\")", lpTextA, "", "", false);

	Verify("msg=~\"test1\"", lpTextA, "", "", true);
	Verify("msg!~\"test1\"", lpTextA, "", "", false);
	Verify("!(msg=~\"test1\")", lpTextA, "", "", false);
	Verify("!(msg!~\"test1\")", lpTextA, "", "", true);

	Verify("msg=^\"test2\"", lpTextA, "", "", false);
	Verify("msg!^\"test2\"", lpTextA, "", "", true);
	Verify("!(msg=^\"test2\")", lpTextA, "", "", true);
	Verify("!(msg!^\"test2\")", lpTextA, "", "", false);

	Verify("msg=^\"qwer\"", lpTextA, "", "", true);
	Verify("msg!^\"qwer\"", lpTextA, "", "", false);
	Verify("!(msg=^\"qwer\")", lpTextA, "", "", false);
	Verify("!(msg!^\"qwer\")", lpTextA, "", "", true);

	Verify("msg=$\"test2\"", lpTextA, "", "", false);
	Verify("msg!$\"test2\"", lpTextA, "", "", true);
	Verify("!(msg=$\"test2\")", lpTextA, "", "", true);
	Verify("!(msg!$\"test2\")", lpTextA, "", "", false);

	Verify("msg=$\"zxcv\"", lpTextA, "", "", true);
	Verify("msg!$\"zxcv\"", lpTextA, "", "", false);
	Verify("!(msg=$\"zxcv\")", lpTextA, "", "", false);
	Verify("!(msg!$\"zxcv\")", lpTextA, "", "", true);

	Verify("msg=#\"test2\"", lpTextA, "", "", false);
	Verify("msg!#\"test2\"", lpTextA, "", "", true);
	Verify("!(msg=#\"test2\")", lpTextA, "", "", true);
	Verify("!(msg!#\"test2\")", lpTextA, "", "", false);

	Verify("msg=#\"zxcv$\"", lpTextA, "", "", true);
	Verify("msg!#\"zxcv$\"", lpTextA, "", "", false);
	Verify("!(msg=#\"zxcv$\")", lpTextA, "", "", false);
	Verify("!(msg!#\"zxcv$\")", lpTextA, "", "", true);

	Verify("msg=#\"^qwer\"", lpTextA, "", "", true);
	Verify("msg!#\"^qwer\"", lpTextA, "", "", false);
	Verify("!(msg=#\"^qwer\")", lpTextA, "", "", false);
	Verify("!(msg!#\"^qwer\")", lpTextA, "", "", true);

	lpTextA = "12345";

	Verify("msg==12345", lpTextA, "", "", true);
	Verify("msg==12346", lpTextA, "", "", false);
	Verify("msg!=12345", lpTextA, "", "", false);
	Verify("msg!=12346", lpTextA, "", "", true);
	Verify("msg>=12344", lpTextA, "", "", true);
	Verify("msg>=12345", lpTextA, "", "", true);
	Verify("msg>=12346", lpTextA, "", "", false);
	Verify("msg<=12346", lpTextA, "", "", true);
	Verify("msg<=12345", lpTextA, "", "", true);
	Verify("msg<=12344", lpTextA, "", "", false);
	Verify("msg>12344", lpTextA, "", "", true);
	Verify("msg>12345", lpTextA, "", "", false);
	Verify("msg<12346", lpTextA, "", "", true);
	Verify("msg<12345", lpTextA, "", "", false);

	lpTextA = "1234";
	lpTextB = "5678";
	lpTextC = "20.05.30 13:40:27";

	CLogTheme::Instance().Select(CLogTheme::Instance().AddTheme("Custom2\n[%PID%] [%TID%] [%TIME%]"));

	Verify("pid==1234&&tid==5678", lpTextA, lpTextB, lpTextC, true);
	Verify("pid!=1234&&tid==5678", lpTextA, lpTextB, lpTextC, false);
	Verify("pid!=1235&&tid!=5679", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1235&&tid!=5679", lpTextA, lpTextB, lpTextC, false);
	Verify("pid==1234&&tid!=5679", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1234&&tid!=5678", lpTextA, lpTextB, lpTextC, false);

	Verify("pid==1234||tid==5678", lpTextA, lpTextB, lpTextC, true);
	Verify("pid!=1234||tid==5678", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1234||tid!=5678", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1235||tid!=5678", lpTextA, lpTextB, lpTextC, false);
	Verify("pid!=1234||tid==5679", lpTextA, lpTextB, lpTextC, false);
	Verify("pid!=1235||tid==5679", lpTextA, lpTextB, lpTextC, true);

	Verify("pid!=1234||tid==5678&&tid!=5679", lpTextA, lpTextB, lpTextC, true);
	Verify("pid!=1234||tid==5678&&tid==5679", lpTextA, lpTextB, lpTextC, false);
	Verify("pid!=1234||tid==5678&&tid==5679||pid==1234", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1234&&(tid==5679||tid==5678&&pid==1234)", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1234&&(tid==5679||tid==5678&&pid==1235)", lpTextA, lpTextB, lpTextC, false);
	Verify("pid==1234&&(tid==5679||tid==5678&&pid==1235||pid==1234)", lpTextA, lpTextB, lpTextC, true);
	Verify("pid==1234&&(tid==5679||tid==5678&&pid==1235||(pid>1233&&pid<1235))", lpTextA, lpTextB, lpTextC, true);
	Verify("pid!=1234&&(tid==5679||tid==5678&&pid==1235||(pid>1233&&pid<1235))", lpTextA, lpTextB, lpTextC, false);
	Verify("pid!=1235&&(tid==5679||tid==5678&&pid==1235||(pid>1233&&pid<1235))", lpTextA, lpTextB, lpTextC, true);

	Verify("time==\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, true);
	Verify("time!=\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, false);
	Verify("time>=\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, true);
	Verify("time<=\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, true);
	Verify("time>\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, false);
	Verify("time<\"20.05.30 13:40:27\"", lpTextA, lpTextB, lpTextC, false);
	Verify("time>\"20.05.30 13:40:25\"", lpTextA, lpTextB, lpTextC, true);
	Verify("time<\"20.05.30 13:40:29\"", lpTextA, lpTextB, lpTextC, true);

	return 0;
}

