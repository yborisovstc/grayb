
#include <stdlib.h>
#include <stdio.h>
#include "rdata.h"


DtBase::~DtBase()
{
}

// Scalar data
template<> const char* Sdata<int>::TypeSig() { return  "SI";};
template<> const char* Sdata<float>::TypeSig() { return  "SF";};
template<> const char* Sdata<bool>::TypeSig() { return  "SB";};
template<> const char* Sdata<string>::TypeSig() { return  "SS";};


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

template<> const char* Mtr<int>::TypeSig() { return  "MI";};
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
	if (mType == EMt_Unknown) 
	{ 
	    if (a.mType == EMt_Diagonal && b.mType == EMt_Diagonal) {
		mType = EMt_Diagonal; 
	    }
	    else {
		mType = EMt_Regular;
	    }
	}
	if (mDim.first == 0) {
	    mDim.first = a.mDim.first;
	}
	if (mDim.second == 0) {
	    mDim.second = b.mDim.second;
	}
	SetIntSize(IntSize());
	if (a.mDim.second == b.mDim.first && a.mDim.first == mDim.first && b.mDim.second == mDim.second) {
	    mType = EMt_Regular;
	    if (a.mType == EMt_Diagonal) {
		mType = b.mType;
	    }
	    else if (b.mType == EMt_Diagonal) {
		mType = a.mType;
	    }
	    if (mType == EMt_Diagonal) {
		for (TInt r = 0; r < mDim.first; r++) {
		    MplElems(r, r, a, r, r, b, r, r);
		}
	    }
	    else {
		for (TInt r = 0; r < mDim.first; r++) {
		    for (TInt c = 0; c < mDim.second; c++) {
			if (a.mType == EMt_Diagonal) {
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
		mValid = ETrue;
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


MtrBase& MtrBase::Invm(const MtrBase& a)
{
    if (a.mType == EMt_Diagonal) {
	// Only diag mtr is supported for now
	if ((mDim.first == 0 || mDim.first == a.mDim.first) && (mDim.second == 0 || mDim.second == a.mDim.second)) {
	    mDim = a.mDim;
	    mType = EMt_Diagonal;
	    SetIntSize(IntSize());
	    for (TInt r = 0; r < mDim.first; r++) {
		InvmElem(r, r, a, r, r);
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



#if 0
// Tuple, unnamed components
const char* RTuple::TypeSig() { return  "TPU";};

RTuple::~RTuple()
{
    for (tComps::iterator it = mData.begin(); it != mData.end(); it++) {
	DtBase* elem = *it;
	delete elem;
    }
    mData.clear();
}

TBool RTuple::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

TBool RTuple::IsDataFit(const RTuple& aData)
{
    return  aData.mValid && aData.GetTypeSig() == TypeSig();
}

int RTuple::ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes)
{
    size_t res = string::npos;
    size_t end = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	string sigp = aCont.substr(beg, sigp_e);
	res = sigp_e;
	size_t sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    aCTypes.clear();
	    end = sig_e;
	    do {
		beg = end + 1;
		end = sigp.find(',', beg);
		string ss = sigp.substr(beg, end - beg);
		aCTypes.push_back(ss);
	    } while (end != string::npos);
	}
    }
    return res;
}

TBool RTuple::IsCTypesFit(const tCTypes& aCt) const
{
    TBool res = ETrue;
    if (mData.size() == aCt.size()) {
	for (TInt i = 0; i < mData.size(); i++) {
	    DtBase* comp = mData.at(i);
	    if (comp->GetTypeSig() != aCt.at(i)) {
		res = EFalse; break;
	    };
	}
    }
    else {
	res = EFalse;
    }
    return res;
}

TBool RTuple::FromString(const string& aString)
{
    TBool res = ETrue;
    TBool changed = EFalse;
    string ss;
    string sig;
    tCTypes ctypes;
    int beg = 0, end = 0;
    end = ParseSigPars(aString, sig, ctypes);
    if (sig == GetTypeSig()) {
	if (!IsCTypesFit(ctypes)) { 
	    Init(ctypes);
	    changed = ETrue; 
	}
	string ss;
	int cnt = 0;
	do {
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		DtBase* comp = mData.at(cnt);
		istringstream sstr(ss);
		changed |= comp->DataFromString(sstr, res);
	    }
	    cnt++;
	} while (end != string::npos && res && cnt < mData.size());
	if (cnt != mData.size() || end != string::npos) {
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
    }
    if (mValid != res) { mValid = res; changed = ETrue; }
    return changed;
}

void RTuple::Init(const tCTypes& aCt)
{
    mData.clear();
    for (tCTypes::const_iterator it = aCt.begin(); it != aCt.end(); it++) {
	const string& sr = *it;
	DtBase* comp = NULL;
	if ((comp = Sdata<int>::Construct(sr)) != NULL);
	else if ((comp = Sdata<float>::Construct(sr)) != NULL);
	if (comp != NULL) {
	    mData.push_back(comp);
	}
    }
}
#endif 

// Tuple, named components
const char* NTuple::TypeSig() { return  "TPL";};

NTuple::~NTuple()
{
    for (tComps::iterator it = mData.begin(); it != mData.end(); it++) {
	DtBase* elem = it->second;
	delete elem;
    }
    mData.clear();
}

TBool NTuple::IsSrepFit(const string& aString)
{
    return DtBase::IsSrepFit(aString, TypeSig());
}

TBool NTuple::IsDataFit(const NTuple& aData)
{
    return  aData.mValid && aData.GetTypeSig() == TypeSig();
}

int NTuple::ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes)
{
    size_t res = string::npos;
    size_t end = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	string sigp = aCont.substr(beg, sigp_e);
	res = sigp_e;
	size_t sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    aCTypes.clear();
	    end = sig_e;
	    do {
		beg = end + 1;
		end = sigp.find(',', beg);
		string compsig = sigp.substr(beg, end - beg);
		size_t cs1 = compsig.find(':');
		if (cs1 != string::npos) {
		    string cs_type = compsig.substr(0, cs1);
		    string cs_name = compsig.substr(cs1 + 1);
		    aCTypes.push_back(tCompSig(cs_type, cs_name));
		}
		else {
		    break;
		}
	    } while (end != string::npos);
	}
    }
    return res;
}

TBool NTuple::IsCTypesFit(const tCTypes& aCt) const
{
    TBool res = ETrue;
    if (mData.size() == aCt.size()) {
	for (TInt i = 0; i < mData.size(); i++) {
	    tComp cp = mData.at(i);
	    DtBase* comp = cp.second;
	    const tCompSig& cpsig = aCt.at(i);
	    if (cp.first != cpsig.second || comp->GetTypeSig() != cpsig.first) {
		res = EFalse; break;
	    };
	}
    }
    else {
	res = EFalse;
    }
    return res;
}

TBool NTuple::FromString(const string& aString)
{
    TBool res = ETrue;
    TBool changed = EFalse;
    string ss;
    string sig;
    tCTypes ctypes;
    int beg = 0, end = 0;
    end = ParseSigPars(aString, sig, ctypes);
    if (sig == GetTypeSig()) {
	if (!IsCTypesFit(ctypes)) { 
	    Init(ctypes);
	    changed = ETrue; 
	}
	string ss;
	int cnt = 0;
	do {
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		tComp& cp =  mData.at(cnt);
		DtBase* comp = cp.second;
		istringstream sstr(ss);
		changed |= comp->DataFromString(sstr, res);
	    }
	    cnt++;
	} while (end != string::npos && res && cnt < mData.size());
	if (cnt != mData.size() || end != string::npos) {
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
    }
    if (mValid != res) { mValid = res; changed = ETrue; }
    return changed;
}

void NTuple::Init(const tCTypes& aCt)
{
    mData.clear();
    for (tCTypes::const_iterator it = aCt.begin(); it != aCt.end(); it++) {
	const tCompSig& ctype = *it;
	const string& sr = ctype.first;
	const string& name = ctype.second;
	DtBase* comp = NULL;
	if ((comp = Sdata<int>::Construct(sr)) != NULL);
	else if ((comp = Sdata<float>::Construct(sr)) != NULL);
	if (comp != NULL) {
	    mData.push_back(tComp(name, comp));
	}
    }
}

DtBase* NTuple::GetElem(const string& aName)
{
    DtBase* res = NULL;
    for (tComps::iterator it = mData.begin(); it != mData.end(); it++) {
	tComp& cp = *it;
	if (cp.first == aName) {
	    res = cp.second;
	    break;
	}
    }
    return res;
}

TBool NTuple::operator==(const DtBase& sb) 
{ 
    const NTuple& b = dynamic_cast<const NTuple&>(sb);
    TBool res = ETrue;
    if (&b != NULL && DtBase::operator==(b) || mData.size() != b.mData.size()) {
	res = EFalse;
    }
    else {
    }
    return res;
};

NTuple& NTuple::operator=(const NTuple& b)
{
    if (&b == this) return *this;
    mValid = b.mValid;
    mSigTypeOK = b.mSigTypeOK;
    mData.clear();
    for (tComps::const_iterator it = b.mData.begin(); it != b.mData.end(); it++) {
	const tComp& bcomp = *it;
	const string& name = bcomp.first;
	DtBase* comp = bcomp.second->Clone();
	if (comp != NULL) {
	    mData.push_back(tComp(name, comp));
	}
    }
    return *this;
}

