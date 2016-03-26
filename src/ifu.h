#ifndef __GRAYB_IFU_H
#define __GRAYB_IFU_H

#include "plat.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

// Interface support utility
class Ifu
{
    public:
	Ifu();
	Ifu(const string& aPars);
	// Parsing of Iface call invocation spec
	static void ParseIcSpec(const string& aSpec, string& aName, string& aSig, vector<string>& aArgs);
	static TBool ToBool(const string& aString);
	static string FromBool(TBool aBool);
	static TInt ToInt(const string& aString);
	static string FromInt(TInt aInt);
	static void ToStringArray(const string& aString, vector<string>& aRes);
	void RegMethod(const string& aName, TInt aArgsNum);
	TBool CheckMname(const string& aName) const;
	TBool CheckMpars(const string& aName, TInt aArgsNum) const;
    public:
	static string KRinvSep;
	static char KRinvEscape;
	static string KArraySep;
    protected:
	// Methods parameters
	map<string, TInt> mMpars;

};


#endif
