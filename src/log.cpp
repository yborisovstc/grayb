#include <stdio.h>
#include "log.h"
#include "elem.h"

const TInt KLogRecBufSize = 400;

const char* CtgText[MLogRec::ECtg_Max] = {"ERR", "WARN", "INFO", "DBG"};
const char* KColSep = "; ";

GLogRec::GLogRec(const string& aName, const string& aLogFileName): Base(aName), iLogFileName(aLogFileName), iLogFileValid(EFalse),
    iObs(NULL), mCtxMutId(-1)
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
    if (iObs != NULL) {
	iObs->OnLogRecDeleting(this);
    }
    if (iLogFileValid) 
	fclose(iLogFile);
}

void *GLogRec::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx) 
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

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
    va_end(list);
}

void GLogRec::Write(TLogRecCtg aCtg, Elem* aNode, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss;
    ss << CtgText[aCtg] << KColSep;
    TInt mutid = mCtxMutId;
    if (mutid != -1) {
	ss << mutid << KColSep;
    }
    else {
	ss << KColSep;
    }
    if (aNode != NULL) {
	GUri fullpath;
	aNode->GetUri(fullpath);
	ss << fullpath.GetUri(ETrue) << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    if (iObs != NULL) {
	iObs->OnLogAdded(aCtg, aNode, buf1, mutid);
    }
    va_end(list);
}

void GLogRec::Write(TLogRecCtg aCtg, Elem* aNode, const ChromoNode& aMut, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss;
    ss << CtgText[aCtg] << KColSep;
    TInt lineid = aMut.LineId();
    ss << lineid << KColSep;
    if (aNode != NULL) {
	GUri fullpath;
	aNode->GetUri(fullpath);
	ss << fullpath.GetUri(ETrue) << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    if (iObs != NULL) {
	iObs->OnLogAdded(aCtg, aNode, buf1, lineid);
    }
    va_end(list);
}

void GLogRec::Flush()
{
    if (iLogFile)
	fflush(iLogFile);
}

TBool GLogRec::AddLogObserver(MLogObserver* aObs)
{
    TBool res = EFalse;
    if (aObs != NULL && iObs == NULL) {
	iObs = aObs;
	iObs->AddObservable(this);
	res = ETrue;
    }
    return res;
}

void GLogRec::RemoveLogObserver(MLogObserver* aObs)
{
    __ASSERT(aObs == iObs);
    iObs->RemoveObservable(this);
    iObs = NULL;
}

void GLogRec::SetContextMutId(TInt aMutId)
{
    if (mCtxMutId != aMutId) {
	mCtxMutId = aMutId;
    }
}
