#ifndef __GRAYB_LOG_H
#define __GRAYB_LOG_H

#include "base.h"
#include "mlog.h"

// Log recorder interface
class GLogRec: public Base, public MLogRec
{
    public:
	static const char* Type() { return "GLogRec";};
	GLogRec(const string& aName, const string& aLogFileName);
	virtual ~GLogRec();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MLogRec
	virtual void WriteRecord(const char* aText);
	virtual void WriteFormat(const char* aFmt,...);
	virtual void Flush();
    protected:
	FILE* iLogFile;
	string iLogFileName;
	TBool iLogFileValid;
};

#endif
