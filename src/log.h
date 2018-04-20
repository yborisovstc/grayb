#ifndef __GRAYB_LOG_H
#define __GRAYB_LOG_H

#include "base.h"
#include "mlog.h"

    // Log recorder interface
    class GLogRec: public Base, public MLogRec
    {
	public:
	    static const char* Type() { return "GLogRec";};
	    GLogRec(const string& aLogFileName);
	    virtual ~GLogRec();
	public:
	    // From Base
	    virtual MIface *DoGetObj(const char *aName) override { return NULL;}
	    // From MLogRec
	    virtual void WriteFormat(const char* aFmt,...);
	    virtual void Write(TLogRecCtg aCtg, const MElem* aNode, const char* aFmt,...);
	    virtual void Write(TLogRecCtg aCtg, MElem* aNode, const ChromoNode& aMut, const char* aFmt,...);
	    virtual void Flush();
	    virtual void SetContextMutId(TInt aMutId = -1);
	    virtual TBool AddLogObserver(MLogObserver* aObs);
	    virtual void RemoveLogObserver(MLogObserver* aObs);
	    virtual TInt GetStat(TLogRecCtg aCtg) const;
	    virtual void Write(const TLog& aRec);
	    // From MIface
	    virtual MIface* Call(const string& aSpec, string& aRes);
	    virtual string Uid() const { return Mid() + "%" + Type();};
	    string Mid() const override { return string();}
	protected:
	    void WriteRecord(const char* aText);
	    void WriteRecord(const string& aText);
	protected:
	    FILE* iLogFile;
	    string iLogFileName;
	    TBool iLogFileValid;
	    MLogObserver* iObs;
	    TInt mCtxMutId;
	    TInt mStat[ECtg_Max];
	public:
	    static const TInt KLogRecBufSize;
    };

#endif
