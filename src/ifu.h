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
	static string CombineIcSpec(const string& aName, const string& aSig);
	static string CombineIcSpec(const string& aName, const string& aSig, const string& aArg);
	static void AddIcSpecArg(string& aSpec, const string& aArg);
	static void AddIcSpecArg(string& aSpec, TBool aArg);
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
	static void ParseUid(const string& aUid, string& aOid, string& aType);
	static void CombineUid(const string& aOid, const string& aType, string& aUid);
	static TBool IsSimpleIid(const string& aIid);
	static string EscCtrl(const string& aInp, char aCtrl);
	static string DeEscCtrl(const string& aInp, char aCtrl);
	static size_t FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos); 
	static size_t FindFirstCtrl(const string& aString, const string& aCtrls, size_t aPos); 
	// Find first occurance of symbol out of delimiters. Delims can be simple and nested
//	static size_t FindFirstOutOfDelims(const string& aString, const string& aSymbols, char aSimpleDelim,
//		const string& aNestedDelims, size_t aBeg); 
	static size_t FindEndNestedDelim(const string& aString, char LeftDelim, char RightDelim);
    public:
	static char KRinvSep;
	static char KEsc;
	static string KArraySep;
	static char KUidSep;
    protected:
	// Methods parameters
	map<string, TInt> mMpars;

};


#endif
