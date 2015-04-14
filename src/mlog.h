#ifndef __GRAYB_MLOG_H
#define __GRAYB_MLOG_H

#include <plat.h>
#include <string>

class Elem;
class MLogObserver;
class ChromoNode;

// Log recorder interface
class MLogRec
{
    public:
	// Category of log record
	enum TLogRecCtg {
	    EErr = 0, 
	    EWarn,
	    EInfo,
	    EDbg, 
	    ECtg_Max
	};
    public:
	virtual void WriteFormat(const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, Elem* aNode, const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, Elem* aNode, const ChromoNode& aMut, const char* aFmt,...) = 0;
	virtual void SetContextMutId(TInt aMutId = -1) = 0;
	virtual void Flush() = 0;
	virtual TBool AddLogObserver(MLogObserver* aObs) = 0;
	virtual void RemoveLogObserver(MLogObserver* aObs) = 0;
};

// Log observer
class MLogObserver
{
    public:
	virtual void AddObservable(MLogRec* aObservable) = 0;
	virtual void RemoveObservable(MLogRec* aObservable) = 0;
	virtual void OnLogAdded(long aTimeStamp, MLogRec::TLogRecCtg aCtg, Elem* aNode, const std::string& aContent, TInt aMutId = 0) = 0;
	virtual void OnLogRecDeleting(MLogRec* aLogRec) = 0;
};

#endif
