#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 
#include <sstream>
#include "ifu.h" 

char Ifu::KRinvSep = ',';
char Ifu::KEsc = '\\';
string Ifu::KArraySep = ";";
char Ifu::KUidSep = '%';

Ifu::Ifu()
{
}

Ifu::Ifu(const string& aPars)
{
    size_t par_end = 0;
    size_t par_beg = 0;
    do {
	par_end = aPars.find_first_of(KRinvSep, par_beg); 
	string name = aPars.substr(par_beg, (par_end == string::npos) ? string::npos : par_end - par_beg);
	par_beg = par_end + 1;
	par_end = aPars.find_first_of(KRinvSep, par_beg); 
	string num = aPars.substr(par_beg, (par_end == string::npos) ? string::npos : par_end - par_beg);
	par_beg = par_end + 1;
	stringstream ss(num);
	TInt numv;
	ss >> numv;
	RegMethod(name, numv);
    } while (par_end != string::npos);
}

string Ifu::CombineIcSpec(const string& aName, const string& aSig)
{
    return aName + KRinvSep + aSig;
}

string Ifu::CombineIcSpec(const string& aName, const string& aSig, const string& aArg)
{
    return aName + KRinvSep + aSig + KRinvSep + EscCtrl(aArg, KRinvSep);
}

void Ifu::AddIcSpecArg(string& aSpec, const string& aArg)
{
    aSpec += KRinvSep + EscCtrl(aArg, KRinvSep);
}

void Ifu::AddIcSpecArg(string& aSpec, TBool aArg)
{
    aSpec += KRinvSep + EscCtrl(FromBool(aArg), KRinvSep);
}

void Ifu::ParseIcSpec(const string& aSpec, string& aName, string& aSig, vector<string>& aArgs) 
{
    // Separate method name
    size_t mname_beg = 0;
    size_t mname_end = aSpec.find_first_of(KRinvSep, mname_beg); 
    if (mname_end == mname_beg) throw (runtime_error("Missing method name"));
    aName = aSpec.substr(mname_beg, (mname_end == string::npos) ? string::npos : mname_end - mname_beg);
    if (mname_end != string::npos) {
	// Separate signature
	size_t sig_beg = mname_end + 1;
	size_t sig_end = aSpec.find_first_of(KRinvSep, sig_beg); 
	aSig = aSpec.substr(sig_beg, (sig_end == string::npos) ? string::npos : sig_end - sig_beg);
	if (sig_end != string::npos) {
	    // Separate arguments
	    size_t arg_end = sig_end;
	    size_t arg_beg = arg_end + 1;
	    do {
		arg_beg = arg_end + 1;
		size_t arg_mid = arg_beg;
		// Find first non-escaped separator
		do {
		    arg_end = aSpec.find_first_of(KRinvSep, arg_mid); 
		    arg_mid = arg_end + 1;
		} while(arg_end != string::npos && aSpec.at(arg_end - 1) == KEsc);
		string arg = aSpec.substr(arg_beg, (arg_end == string::npos) ? string::npos : arg_end - arg_beg);
		aArgs.push_back(DeEscCtrl(arg, KRinvSep));
	    } while (arg_end != string::npos);
	}
    }
}

size_t Ifu::FindFirstCtrl(const string& aString, const char aCtrl, size_t aPos)
{
    size_t pos_beg = aPos;
    size_t pos;
    do {
	pos = aString.find_first_of(aCtrl, pos_beg); 
	pos_beg = pos + 1;
    } while(pos != string::npos && aString.at(pos - 1) == KEsc);
    return pos;
}

TBool Ifu::ToBool(const string& aString)
{
    TBool res = EFalse;
    if (aString == "false") res = EFalse;
    else if (aString == "true") res = ETrue;
    else throw (runtime_error("Incorrect boolean value: " + aString));
    return res;
}

string Ifu::FromBool(TBool aBool)
{
    return aBool ? "true" : "false";
}

TInt Ifu::ToInt(const string& aString)
{
    TInt res = 0;
    stringstream ss(aString);
    ss >> res;
    return res;
}

string Ifu::FromInt(TInt aInt)
{
    stringstream ss;
    ss << aInt;
    return ss.str();
}

void Ifu::RegMethod(const string& aName, TInt aArgsNum)
{
    __ASSERT(mMpars.count(aName) == 0);
    mMpars.insert(pair<string, TInt>(aName, aArgsNum));
}

TBool Ifu::CheckMpars(const string& aName, TInt aArgsNum) const
{
    TBool res = mMpars.at(aName) == aArgsNum;
    return res;
}

TBool Ifu::CheckMname(const string& aName) const
{
    TBool res = mMpars.count(aName) > 0;
    return res;
}

void Ifu::ToStringArray(const string& aString, vector<string>& aRes)
{
    size_t end = 0;
    size_t beg = end + 1;
    do {
	beg = end + 1;
	size_t mid = beg;
	// Find first non-escaped separator
	do {
	    end = aString.find_first_of(KArraySep, mid); 
	    mid = end + 1;
	} while (end != string::npos && aString.at(end - 1) == KEsc);
	string elem = aString.substr(beg, (end == string::npos) ? string::npos : end - beg);
	aRes.push_back(elem);
    } while (end != string::npos);
}

void Ifu::ParseUid(const string& aUid, string& aOid, string& aType)
{
    size_t oid_beg = 0, oid_end = 0;
    oid_end = aUid.find_first_of(KUidSep, oid_beg); 
    aOid = aUid.substr(oid_beg, (oid_end == string::npos) ? string::npos : oid_end - oid_beg);
    if (oid_end != string::npos) {
	size_t type_beg = oid_end + 1;
	aType = aUid.substr(type_beg);
    }
}

void Ifu::CombineUid(const string& aOid, const string& aType, string& aUid)
{
    aUid = aOid + KUidSep + aType;
}

TBool Ifu::IsSimpleIid(const string& aIid)
{
    size_t sep = aIid.find_first_of(KUidSep); 
    return (sep == string::npos);
}

string Ifu::EscCtrl(const string& aInp, char aCtrl)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	if (cc == aCtrl) {
	    res.push_back(KEsc);
	}
	res.push_back(cc);
    }
    return res;
}

string Ifu::DeEscCtrl(const string& aInp, char aCtrl)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	const char cn = ((it + 1) != aInp.end()) ? *(it +1) : 0x0;
	if (cc == KEsc && cn == aCtrl) {
	    continue;
	}
	res.push_back(cc);
    }
    return res;
}
