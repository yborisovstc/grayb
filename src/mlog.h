#ifndef __GRAYB_MLOG_H
#define __GRAYB_MLOG_H

class Elem;

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
	virtual void Flush() = 0;
};

#endif
