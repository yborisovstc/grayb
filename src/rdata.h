#ifndef __GRAYB_RDATA_H
#define __GRAYB_RDATA_H

#include "plat.h"
#include <set>
#include <vector>
#include <string>
#include <sstream>

// Row data
//

using namespace std;

class Base;

// Scalar data

class DtBase 
{
    public:
	DtBase(): mValid(EFalse) {};
	DtBase(const DtBase& d): mValid(d.mValid), mSigTypeOK(EFalse) {};
	virtual ~DtBase();
	static int ParseSigPars(const string& aCont, string& aSig);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const DtBase& aData, const string& aTypeSig);
	//TBool Set(const DtBase& d) { return mValid != d.mValid, mValid = d.mValid; };
	void ToString(string& aString) const;
	TBool FromString(const string& aString);
	virtual TBool operator==(const DtBase& b) { return mValid == b.mValid;};
	TBool operator!=(const DtBase& b) {return !this->operator==(b);};
    public:
	virtual string GetTypeSig() const { return "?";};
	virtual void DataToString(stringstream& aStream) const {aStream << "?";};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) {};
    public:
	TBool mValid;
	// Invalidity reason: sigtype is invalis
	TBool mSigTypeOK;
};

template<class T> class Sdata: public DtBase
{
    public:
	Sdata(): DtBase() {};
	Sdata(const Sdata& d): DtBase(d) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Sdata<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	static Sdata<T>* Construct(const string& aSrep) {Sdata<T>* res = NULL; if (IsSrepFit(aSrep)) { res = new Sdata<T>(); } else ;return res;};
	//TBool Set(const Sdata& d);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const { aStream << mData;};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes);
	virtual TBool operator==(const DtBase& sb) { const Sdata<T>& b = dynamic_cast<const Sdata<T>& >(sb); return &b != NULL && DtBase::operator==(b) && mData == b.mData;};
	Sdata<T>& operator+=(const Sdata<T>& b) { mData += b.mData; return *this;};
	//TBool operator!=(const Sdata<T>& b) {return !this->operator==(b);};
	TBool Set(const T& aData) { TBool res = aData != mData; mData = aData; mValid = ETrue; return res; };
	Sdata<T>& operator=(const Sdata<T>& b) { mData = b.mData; mValid = b.mValid; mSigTypeOK = b.mSigTypeOK; return *this;};
    public:
	T mData;
};

template<class T> TBool Sdata<T>::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = EFalse;
    T sdata;
    aStream >> sdata;
    if ((aRes = !aStream.fail()) && mData != sdata) { mData = sdata; mValid = ETrue; changed = ETrue; }
    return changed;
}



// Matrix
// Operations required the result to provide the hints: type or dim required values.
// If there is no hint for particular par, the par can be redifined by the operaton
class MtrBase: public DtBase
{ 
    public: 
	// Type of matrix
	enum TMtrType {
	    EMt_Unknown,
	    EMt_Regular,
	    EMt_Diagonal,
	    EMt_Utriang, // Upper-triangular
	    EMt_Btriang, // Bottom-triangular
	};
	// Dimension
	typedef pair<int, int> TMtrDim;
    public: 
	MtrBase(): DtBase(), mType(EMt_Unknown), mDim(TMtrDim(0, 0)) {};
	MtrBase(const MtrBase& aMtr): DtBase(aMtr), mType(aMtr.mType), mDim(aMtr.mDim) {};
	void ToString(string& aString) const;
	TBool FromString(const string& aString);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const MtrBase& aData, const string& aTypeSig);
	static int ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim);
	int Ind(int r, int c) const;
	int IntSize() const;
	MtrBase& operator+=(const MtrBase& b);
	MtrBase& Mpl(const MtrBase& a, const MtrBase& b);
	MtrBase& Invm(const MtrBase& a);
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << "?";};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes) {};
	virtual void AddElem(const MtrBase& aB, TInt aRI, TInt aCI) {};
	virtual void MplElems(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac, const MtrBase& b, TInt br, TInt bc) {};
	virtual void MplRtoC(TInt r, TInt c, const MtrBase& a, const MtrBase& b) {};
	virtual void InvmElem(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac) {};
	virtual void SetIntSize(TInt aSize) {};
    public:
	TMtrType mType;
	TMtrDim mDim;
};

template<class T> class Mtr: public MtrBase
{ 
    public:
	Mtr(): MtrBase() {};
	Mtr(const Mtr<T>& aMtr): MtrBase(aMtr), mData(aMtr.mData) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return MtrBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Mtr<T>& aData) { return MtrBase::IsDataFit(aData, TypeSig());};
	T GetElem(int r, int c) const { int i = Ind(r,c); return (i == -1) ? T(0) : mData.at(i);};
	T& Elem(int r, int c) { int i = Ind(r,c); __ASSERT(i >= 0); return mData.at(i);};
	virtual TBool operator==(const DtBase& sb) { 
	    const Mtr<T>& b = dynamic_cast<const Mtr<T>& >(sb); return &b != NULL && DtBase::operator==(b) && this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	//TBool operator==(const Mtr<T>& b) { 
	 //   return this->mValid == b.mValid && this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	//TBool operator!=(const Mtr<T>& b) {return !this->operator==(b);};
	template <class TA> void CastDown(const Mtr<TA>& a);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << mData.at(aInd);};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes);
	virtual void AddElem(const MtrBase& b, TInt r, TInt c);
	virtual void MplElems(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac, const MtrBase& b, TInt br, TInt bc);
	virtual void MplRtoC(TInt r, TInt c, const MtrBase& a, const MtrBase& b);
	virtual void InvmElem(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac) { T e = ((Mtr<T>&) a).GetElem(ar, ac); Elem(r, c) = 1/e;};
	virtual void SetIntSize(TInt aSize) { if (mData.size() != aSize) mData.resize(aSize);};
    public:
	vector<T> mData;
};

template <class T> template<class TA>  void Mtr<T>::CastDown(const Mtr<TA>& a) 
{
    if (mType == EMt_Unknown) { mType = a.mType; }
    if (mDim.first == 0) mDim.first = a.mDim.first;
    if (mDim.second == 0) mDim.second = a.mDim.second;
    SetIntSize(IntSize());
    if (mType == a.mType && mDim == a.mDim && mData.size() == a.mData.size()) {
	for (TInt m = 0; m < mData.size(); m++) {
	    mData.at(m) = (T) a.mData.at(m);
	}
    }
    else mValid = EFalse;
};

template<class T> void Mtr<T>::MplRtoC(TInt r, TInt c, const MtrBase& a, const MtrBase& b)
{
    for (TInt m = 0; m < a.mDim.second; m++) {
	Elem(r, c) += ((Mtr<T>&) a).GetElem(r, m) * ((Mtr<T>&) b).GetElem(m, c);
    }
}

template<class T> void Mtr<T>::MplElems(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac, const MtrBase& b, TInt br, TInt bc)
{
    //    if (Ind(r,c) == -1 || a.Ind(ar,ac) == -1 || b.Ind(br,bc) == -1) { mValid = EFalse; return;} ; 
    Elem(r, c) = ((Mtr<T>&) a).GetElem(ar, ac) * ((Mtr<T>&) b).GetElem(br, bc);
}

template<class T> void Mtr<T>::AddElem(const MtrBase& b, TInt r, TInt c) 
{ 
    if (Ind(r,c) == -1) {
	mValid = EFalse; return;} ; 
    Elem(r, c) += ((Mtr<T>&) b).GetElem(r, c);
};

template<class T> TBool Mtr<T>::ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes)
{
    TBool changed = EFalse;
    T sdata;
    aStream >> sdata;
    if (aRes = !aStream.fail()) {
	if (aInd >= mData.size()) { mData.resize(IntSize()); changed = ETrue; }
	T& data = mData.at(aInd);
	if (data != sdata) { data = sdata; changed = ETrue; }
    }
    return changed;
}


#if 0
// Tuple, with unnamed components, adds the components on run-time
class RTuple: public DtBase
{ 
    public:
	// Components
	typedef vector<DtBase*> tComps;
	// Types signature of components
	typedef vector<string> tCTypes;
    public:
	RTuple(): DtBase() {};
	RTuple(const RTuple& aTuple): DtBase(aTuple) {};
	virtual ~RTuple();
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const RTuple& aData);
	static int ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes);
	void Init(const tCTypes& aCt);
	TBool FromString(const string& aString);
	DtBase* GetElem(int ind) const { __ASSERT(ind >= 0 && ind < mData.size()); return mData.at(ind);};
	TBool operator==(const RTuple& b) { return this->mValid == b.mValid  && this->mData == b.mData;};
	TBool operator!=(const RTuple& b) {return !this->operator==(b);};
	virtual string GetTypeSig() const { return TypeSig();};
    protected:
	TBool IsCTypesFit(const tCTypes& aCt) const;
    public:
	tComps mData;
};
#endif

// Tuple, with named components, adds the components on run-time
// Full_signature := TypeSig , ',' ,  Paremeters ;
// Paremeters := CompSig , {"," , CompSig} ;
// CompSig := CompTypeSig , ':' , CompName ;
class NTuple: public DtBase
{ 
    public:
	// Components: Name, Data
	typedef pair<string, DtBase*> tComp;
	typedef vector<tComp> tComps;
	// Signature of component: Typesig, Name
	typedef pair<string, string> tCompSig;
	// Signature of components
	typedef vector<tCompSig> tCTypes;
    public:
	NTuple(): DtBase() {};
	NTuple(const NTuple& aTuple): DtBase(aTuple) {};
	virtual ~NTuple();
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const NTuple& aData);
	static int ParseSigPars(const string& aCont, string& aSig, tCTypes& aCTypes);
	void Init(const tCTypes& aCt);
	TBool FromString(const string& aString);
	DtBase* GetElem(const string& aName);
	virtual TBool operator==(const DtBase& sb);
	NTuple& operator=(const NTuple& b);
	//TBool operator==(const NTuple& b);
	//TBool operator!=(const NTuple& b) {return !this->operator==(b);};
	virtual string GetTypeSig() const { return TypeSig();};
    protected:
	TBool IsCTypesFit(const tCTypes& aCt) const;
    public:
	tComps mData;
};


#endif
