#include "log.h"

GLogRec::GLogRec(const string& aName, const string& aLogFileName): Base(aName), iLogFileName(aLogFileName)
{
};

void *GLogRec::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

void GLogRec::WriteRecord(const char* aText)
{
}

void GLogRec::WriteFormat(const char* aFmt,...)
{
}

void GLogRec::Flush()
{
}

