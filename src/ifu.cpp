#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 
#include <sstream>
#include "ifu.h" 

string Ifu::KRinvSep = ",";
char Ifu::KRinvEscape = '\\';

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
		} while(arg_end != string::npos && aSpec.at(arg_end - 1) == KRinvEscape);
		string arg = aSpec.substr(arg_beg, (arg_end == string::npos) ? string::npos : arg_end - arg_beg);
		aArgs.push_back(arg);
	    } while (arg_end != string::npos);
	}
    }
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
