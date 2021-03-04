#include <stdio.h>
#include "log.h"
#include "elem.h"
#include <sys/time.h>
#include <stdexcept> 

const TInt GLogRec::KLogRecBufSize = 1400;

const char* CtgText[ECtg_Max] = {"ERR", "WRN", "INF", "DBG"};
const char* KColSep = "; ";
const char KFieldSep = ';';

string TLogGetField(const string& aPack, size_t& aBeg, TBool aESep = ETrue)
{
    size_t end = aPack.find_first_of(KFieldSep, aBeg);
    __ASSERT(!aESep || end != string::npos);
    string res = aPack.substr(aBeg, end - aBeg);
    aBeg = end + 1;
    return res;
}

TLog::TLog(TLogRecCtg aCtg, const MUnit* aAgt)
{
    stringstream ss;
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000000 + tp.tv_usec;
    ss << ms;
    mTimestampS = ss.str();
    mCtgS = CtgText[aCtg];
    if (aAgt != NULL) {
	GUri uri;
	aAgt->GetUri(uri);
	mNodeUriS = uri.toString(ETrue);
    }
}

TLog::TLog(const string& aString)
{
    size_t beg = 0;
    mTimestampS = TLogGetField(aString, beg);
    mCtgS = TLogGetField(aString, beg);
    mMutIdS = TLogGetField(aString, beg);
    mNodeUriS = TLogGetField(aString, beg);
    mContent = TLogGetField(aString, beg, EFalse);
}

TLog::operator string() const
{
    return mTimestampS + KFieldSep + mCtgS + KFieldSep + mMutIdS + KFieldSep + mNodeUriS + KFieldSep + mContent;
}

TLog& TLog::operator +(const string& aString)
{
    mContent += aString;
    return *this;
}

string TLog::TimestampS() const
{
    return mTimestampS;
}

string TLog::NodeUriS() const
{
    return mNodeUriS;
}

string TLog::MutIdS() const
{
    return mMutIdS;
}

TInt TLog::MutId() const
{
    TInt res;
    istringstream ss(mMutIdS);
    ss >> res;
    return res;
}

string TLog::CtgS() const
{
    return mCtgS;
}

MLogRec::EIfu MLogRec::mIfu;

// Ifu static initialisation
MLogRec::EIfu::EIfu()
{
    RegMethod("Flush", 0);
    RegMethod("Write", 1);
}


GLogRec::GLogRec(const string& aLogFileName): Base(), iLogFileName(aLogFileName), iLogFileValid(EFalse),
    iObs(NULL), mCtxMutId(-1)
{
    for (TInt ct = 0; ct < ECtg_Max; ct++) {
	mStat[ct] = 0;
    }
    //remove(iLogFileName.c_str()); 
    rename(iLogFileName.c_str(), (iLogFileName + "~old").c_str()); 
    iLogFile = fopen(iLogFileName.c_str(), "w+");
    if(iLogFile)
    {
	iLogFileValid=ETrue;
	//fputs("----------New Log----------\015\012", iLogFile);
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

void GLogRec::WriteRecord(const char* aText)
{
    if (iLogFile)
    {
	fputs(aText, iLogFile);
	fputs("\015\012", iLogFile);
	fflush(iLogFile);
    }
}

void GLogRec::WriteRecord(const string& aText)
{
    WriteRecord(aText.c_str());
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

void GLogRec::Write(TLogRecCtg aCtg, const MUnit* aNode, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss;
    struct timeval tp;
    gettimeofday(&tp, NULL);
    //long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    long int ms = tp.tv_sec * 1000000 + tp.tv_usec;
    ss << ms << KColSep;
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
	ss << fullpath.toString(ETrue) << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    if (iObs != NULL) {
	iObs->OnLogAdded(ms, aCtg, aNode, buf1, mutid);
    }
    va_end(list);
}

void GLogRec::Write(TLogRecCtg aCtg, MUnit* aNode, const ChromoNode& aMut, const char* aFmt,...)
{
    char buf1[KLogRecBufSize] = "";
    stringstream ss;
    ss << CtgText[aCtg] << KColSep;
    TInt lineid = aMut.LineId();
    ss << lineid << KColSep;
    if (aNode != NULL) {
	GUri fullpath;
	aNode->GetUri(fullpath);
	ss << fullpath.toString(ETrue) << KColSep;
    }
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf1, aFmt, list);
    ss << buf1;
    WriteRecord(ss.str().c_str());
    mStat[aCtg]++;
    if (iObs != NULL) {
	iObs->OnLogAdded(0, aCtg, aNode, buf1, lineid);
    }
    va_end(list);
}

void GLogRec::Write(const TLog& aRec)
{
    WriteRecord(aRec);
    if (iObs != NULL) {
	iObs->OnLogAdded(aRec);
    }
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

TInt GLogRec::GetStat(TLogRecCtg aCtg) const
{
    return mStat[aCtg];
}

MIface* GLogRec::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	    throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	    throw (runtime_error("Wrong arguments number"));
    if (name == "Flush") {
	Flush();
    } else if (name == "Write") {
	TLog lrec(args.at(0));
	Write(lrec);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}
