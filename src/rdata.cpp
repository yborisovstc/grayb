
#include <stdlib.h>
#include <stdio.h>
#include "rdata.h"


DtBase::~DtBase()
{
}

// Scalar data
template<> const char* Sdata<int>::TypeSig() { return  "SI";};

template<> const char* Sdata<float>::TypeSig() { return  "SF";};


int DtBase::ParseSigPars(const string& aCont, string& aSig)
{
    size_t res = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	res = sigp_e;
	string sigp = aCont.substr(beg, sigp_e);
	int sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
    }
    return res;
}

TBool DtBase::IsSrepFit(const string& aString, const string& aTypeSig)
{
    string sig;
    ParseSigPars(aString, sig);
    return  sig == aTypeSig;
}

TBool DtBase::IsDataFit(const DtBase& aData, const string& aTypeSig)
{
    return  aData.mValid && aData.GetTypeSig() == aTypeSig;
}

void DtBase::ToString(string& aString) const
{
    stringstream ss;
    ss << GetTypeSig() << " ";
    if (!mValid) {
	ss << "<ERR>";
    }
    else {
	DataToString(ss);
    }
    aString = ss.str();
}

TBool DtBase::FromString(const string& aString)
{
    TBool res = ETrue;
    TBool changed = EFalse;
    string sig;
    int end = ParseSigPars(aString, sig);
    if (sig == GetTypeSig()) {
	mSigTypeOK = ETrue;
	string ss;
	if (end != string::npos) {
	    int beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	}
	if (!ss.empty()) {
	    istringstream sstr(ss);
	    changed |= DataFromString(sstr, res);
	}
	else {
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
	mSigTypeOK = EFalse;
    }
    if (mValid != res) { mValid = res; changed = ETrue; }
    return changed;
}

// Matrix, base

template<> const char* Mtr<float>::TypeSig() { return  "MF";};


int MtrBase::Ind(int r, int c) const
{
    int res = -1;
    if (mType == EMt_Regular) res = mDim.second*r + c;
    else if (mType == EMt_Diagonal) res = (r == c) ? r : -1;
    return res;
}

int MtrBase::ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim)
{
    size_t res = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	res = sigp_e;
	string sigp = aCont.substr(beg, sigp_e);
	int sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    beg = sig_e + 1;
	    int par_e = sigp.find_first_of(',', beg);
	    string par = sigp.substr(beg, par_e - beg);
	    if (par.length() == 1) {
		enum {ENone, EVect, EStr} opt = ENone;
		char ts = par.at(0);
		switch (ts) {
		    case 'R': aType = MtrBase::EMt_Regular; break;
		    case 'V': aType = MtrBase::EMt_Regular; opt = EVect; break;
		    case 'S': aType = MtrBase::EMt_Regular; opt = EStr; break;
		    case 'D': aType = MtrBase::EMt_Diagonal; break;
		    case 'U': aType = MtrBase::EMt_Utriang; break;
		    case 'B': aType = MtrBase::EMt_Btriang; break;
		    default: ;
		}
		beg = par_e + 1;
		par_e = sigp.find_first_of(',', beg);
		par = sigp.substr(beg, par_e - beg);
		if (!par.empty()) {
		    int dp1 = 0, dp2 = 0;
		    istringstream sstr(par);
		    sstr >> dp1;
		    if (dp1 != 0) {
			if (par_e != string::npos) {
			    beg = par_e + 1;
			    par = sigp.substr(beg);
			    istringstream sstr(par);
			    sstr >> dp2;
			}
			aDim.first = dp1; aDim.second = dp2; 
			if (dp2 == 0) {
			    aDim.second = dp1;
			    if (opt == EVect) {
				aDim.first = dp1; aDim.second = 1;
			    }
			    else if (opt == EStr) {
				aDim.first = 1; aDim.second = dp1;
			    }
			}
			else if (opt == EVect | opt == EStr || aType == MtrBase::EMt_Diagonal) {
			    aDim.first = 0; aDim.second = 0; aType = MtrBase::EMt_Unknown;
			}
		    }
		}
	    }
	}
    }
    return res;
}

int MtrBase::IntSize() const
{
    int res = mDim.first * mDim.second;
    if (mType == EMt_Diagonal) {
	res = mDim.first;
    } 
    return res;
}

TBool MtrBase::IsSrepFit(const string& aString, const string& aTypeSig)
{
    TMtrType mtype;
    TMtrDim mdim;
    string sig;
    ParseSigPars(aString, sig, mtype, mdim);
    return  sig == aTypeSig &&  mtype != EMt_Unknown && mdim.first != 0 && mdim.second != 0;
}

TBool MtrBase::IsDataFit(const MtrBase& aData, const string& aTypeSig)
{
    return  aData.mValid && aData.GetTypeSig() == aTypeSig &&  aData.mType != EMt_Unknown && aData.mDim.first != 0 && aData.mDim.second != 0;
}

void MtrBase::ToString(string& aString) const
{
    stringstream ss;
    ss << GetTypeSig();
    char mt = 'R';
    switch (mType) {
	case MtrBase::EMt_Unknown: mt = '?'; break;
	case MtrBase::EMt_Diagonal: mt = 'D'; break;
	case MtrBase::EMt_Utriang: mt = 'U'; break;
	case MtrBase::EMt_Btriang: mt = 'B'; break;
    }
    enum {ENone, EVect, EStr} opt = ENone;
    if (mDim.first == 1) { mt = 'S'; opt = EStr;}
    else if (mDim.second == 1) { mt = 'V'; opt = EVect;};
    ss << ',' << mt;
    if (mType == MtrBase::EMt_Diagonal || opt == EVect) {
	ss << ',' << mDim.first;
    }
    else if (opt == EStr) {
	ss << ',' << mDim.second;
    }
    else {
	ss << ',' << mDim.first << ',' << mDim.second;
    }
    if (!mValid) {
	ss << " <ERROR>";
    }
    else {
	for (TInt cnt = 0; cnt < IntSize(); cnt++) {
	    ss << " ";
	    ElemToString(cnt, ss);
	}
    }
    aString = ss.str();
}

TBool MtrBase::FromString(const string& aString)
{
    TBool res = ETrue;
    TBool changed = EFalse;
    string ss;
    TMtrType mtype;
    TMtrDim mdim;
    string sig;
    int beg = 0, end = 0;
    end = ParseSigPars(aString, sig, mtype, mdim);
    if (sig == GetTypeSig() && mtype != EMt_Unknown) {
	if (mType != mtype) { mType = mtype; changed = ETrue; }
	if (mDim != mdim) { mDim = mdim; changed = ETrue; }
	string ss;
	int cnt = 0;
	do {
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		istringstream sstr(ss);
		changed |= ElemFromString(cnt++, sstr, res);
	    }
	} while (end != string::npos && res && cnt < IntSize());
	if (cnt != IntSize() || end != string::npos) {
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
    }
    if (mValid != res) { mValid = res; changed = ETrue; }
    return changed;
}

MtrBase& MtrBase::operator+=(const MtrBase& b)
{
    TBool res = ETrue;
    if (mDim == b.mDim) {
	for (TInt cntr = 0; cntr < mDim.first; cntr++) {
	    for (TInt cntc = 0; cntc < mDim.second; cntc++) {
		TInt cnt = mDim.second*cntr + cntc;
		AddElem(b, cntr, cntc);
	    }
	}
    }
    else {
	res = EFalse;
    }
    if (mValid != res) { mValid = res; }
    return *this;
}


MtrBase& MtrBase::Mpl(const MtrBase& a, const MtrBase& b) 
{
    if (a.mValid && b.mValid) {
	if (a.mDim.second == b.mDim.first && a.mDim.first == mDim.first && b.mDim.second == mDim.second) {
	    mType = EMt_Regular;
	    if (a.mType == EMt_Diagonal) {
		mType = b.mType;
	    }
	    else if (b.mType == EMt_Diagonal) {
		mType = a.mType;
	    }
	    for (TInt r = 0; r < mDim.first; r++) {
		for (TInt c = 0; c < mDim.second; c++) {
		    if (a.mType == EMt_Diagonal &&  b.mType == EMt_Diagonal) {
			MplElems(r, c, a, r, r, b, c, c);
		    }
		    else if (a.mType == EMt_Diagonal) {
			MplElems(r, c, a, r, r, b, r, c);
		    }
		    else if (b.mType == EMt_Diagonal) {
			MplElems(r, c, a, r, c, b, c, c);
		    }
		    else {
			MplRtoC(r, c, a, b);
		    }
		}
	    }
	}
	else {
	    mValid = EFalse;
	}
    }
    else {
	mValid = EFalse;
    }
}