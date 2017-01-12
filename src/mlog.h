#ifndef __GRAYB_MLOG_H
#define __GRAYB_MLOG_H

#include <plat.h>
#include "miface.h"
#include "ifu.h"
#include <string>

class MElem;
class MLogObserver;
class ChromoNode;

enum TLogRecCtg {
    EErr = 0, 
    EWarn,
    EInfo,
    EDbg, 
    ECtg_Max
};

class TLog
{
    public:
	TLog(TLogRecCtg aCtg, const MElem* aAgt);
	TLog(const string& aString);
	TLog& operator +(const string& aString);
	operator string() const;
	string TimestampS() const;
	string NodeUriS() const;
	TInt MutId() const;
	string MutIdS() const;
	string CtgS() const;
	string Content() const { return mContent;};
    protected:
	string mTimestampS;
	string mCtgS;
	string mNodeUriS;
	string mMutIdS;
	string mContent;
};


// Log recorder interface
class MLogRec: public MIface
{
    public:
	virtual void WriteFormat(const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, const MElem* aNode, const char* aFmt,...) = 0;
	virtual void Write(TLogRecCtg aCtg, MElem* aNode, const ChromoNode& aMut, const char* aFmt,...) = 0;
	virtual void SetContextMutId(TInt aMutId = -1) = 0;
	virtual void Flush() = 0;
	virtual TBool AddLogObserver(MLogObserver* aObs) = 0;
	virtual void RemoveLogObserver(MLogObserver* aObs) = 0;
	virtual TInt GetStat(TLogRecCtg aCtg) const = 0;
	virtual void Write(const TLog& aRec) = 0;
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	static EIfu mIfu; // Interface methods utility

};

// Log observer
// TODO [YB] Thre observer-observable relation here is actually of one-way. So do we need
// OnLogRecDeleting, AddObservable, RemoveObservable?
class MLogObserver
{
    public:
	virtual void AddObservable(MLogRec* aObservable) = 0;
	virtual void RemoveObservable(MLogRec* aObservable) = 0;
	virtual void OnLogAdded(long aTimeStamp, TLogRecCtg aCtg, const MElem* aNode, const std::string& aContent, TInt aMutId = 0) = 0;
	virtual void OnLogAdded(const TLog& aLog) = 0;
	virtual void OnLogRecDeleting(MLogRec* aLogRec) = 0;
};

#endif
