#include <stdio.h>
#include "log.h"

const TInt KLogRecBufSize = 400;

GLogRec::GLogRec(const string& aName, const string& aLogFileName): NBase(aName), iLogFileName(aLogFileName), iLogFileValid(EFalse)
{
    remove(iLogFileName.c_str()); 
    iLogFile = fopen(iLogFileName.c_str(), "w+");
    if(iLogFile)
    {
	iLogFileValid=ETrue;
	fputs("----------New Log----------\015\012", iLogFile);
	fflush(iLogFile);
    }
    else
	iLogFileValid= EFalse;
};

GLogRec::~GLogRec()
{
    if (iLogFileValid) 
	fclose(iLogFile);
}

/*
void *GLogRec::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx) 
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}
*/

void GLogRec::WriteRecord(const char* aText)
{
    if (iLogFile)
    {
	fputs(aText, iLogFile);
	fputs("\015\012", iLogFile);
	fflush(iLogFile);
    }
}

void GLogRec::WriteFormat(const char* aFmt,...)
{
    char buf[KLogRecBufSize] = "";
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf, aFmt, list);
    TInt len = strlen(buf);
    WriteRecord(buf);
}

void GLogRec::Flush()
{
    if (iLogFile)
	fflush(iLogFile);
}

