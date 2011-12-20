#ifndef __GRAYB_MLOG_H
#define __GRAYB_MLOG_H

// Log recorder interface
class MLogRec
{
    public:
	virtual void WriteRecord(const char* aText) = 0;
	virtual void WriteFormat(const char* aFmt,...) = 0;
	virtual void Flush() = 0;
};

#endif
