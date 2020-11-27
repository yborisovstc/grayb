#ifndef __GRAYB_RDATA_H
#define __GRAYB_RDATA_H

#include "plat.h"
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

// Row data
//

using namespace std;


/** @brief Data base interface
 * */
class MDtBase
{
    public:
	static const char* Type() { return "MDtBase";};
    public:
	virtual void ToString(string& aString) const = 0;
	virtual TBool FromString(const string& aString) = 0;
	virtual TBool IsValid() const { return EFalse;}
	virtual TBool operator==(const MDtBase& b) {return EFalse;}
	TBool operator!=(const MDtBase& b) {return !this->operator==(b);}
    public:
	virtual string GetTypeSig() const { return "?";}
	virtual void DataToString(stringstream& aStream) const {aStream << "?";}
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) { return EFalse;}
	virtual TBool IsCompatible(const MDtBase& b) {return ETrue;}
    public:
	static const char mKTypeToDataSep;
};


// Scalar data

class DtBase : public MDtBase
{
    public:
	DtBase(): mValid(EFalse) {};
	DtBase(const DtBase& d): mValid(d.mValid), mSigTypeOK(EFalse) {};
	virtual ~DtBase();
	string ToString() const { string res; ToString(res); return res; }
	static int ParseSigPars(const string& aCont, string& aSig);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const DtBase& aData, const string& aTypeSig);
	virtual void ToString(string& aString) const override;
	virtual TBool FromString(const string& aString) override;
	virtual TBool operator==(const MDtBase& b) override { return mValid == b.IsValid();};
	virtual TBool IsValid() const override { return mValid;}
    public:
	virtual DtBase* Clone() {return NULL;}
	virtual void SetMplncArg1Hint(const DtBase& res, const DtBase& arg2) {};
	virtual void SetMplncArg2Hint(const DtBase& res, const DtBase& arg1) {};
    public:
	TBool mValid;
	// Invalidity reason: sigtype is invalis
	TBool mSigTypeOK;
};

template<class T> class Sdata: public DtBase
{
    public:
	Sdata(): DtBase() {};
	Sdata(const T& aRdata): Sdata() { mData = aRdata; mValid = ETrue;}
	Sdata(const Sdata& d): DtBase(d), mData(d.mData) {};
	template <class TA> Sdata(const Sdata<TA>& b): DtBase(b), mData(static_cast<T>(b.mData)) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Sdata<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	static Sdata<T>* Construct(const string& aSrep) {Sdata<T>* res = NULL; if (IsSrepFit(aSrep)) { res = new Sdata<T>(); } else ;return res;};
	//TBool Set(const Sdata& d);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const override { aStream << std::boolalpha << mData;};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) override;
	virtual DtBase* Clone() {return new Sdata<T>(*this);};
	virtual TBool operator==(const MDtBase& sb) override
	{ const Sdata<T>& b = dynamic_cast<const Sdata<T>& >(sb); return &b != NULL && DtBase::operator==(b) && mData == b.mData;};
	TBool operator>(const Sdata<T>& b) const { return mData > b.mData;};
	TBool operator>=(const Sdata<T>& b) const { return mData >= b.mData;};
	TBool operator<(const Sdata<T>& b) const { return mData < b.mData;};
	TBool operator<=(const Sdata<T>& b) const { return mData <= b.mData;};
	Sdata<T>& Invm(const Sdata<T>& b) { mData = 1 / b.mData; return *this;};
	Sdata<T>& operator=(const Sdata<T>& b) { this->DtBase::operator=(b); mData = b.mData; return *this;};
	//Sdata<T>& operator=(const Sdata<T>& b) { mData = b.mData; mValid = b.mValid; mSigTypeOK = b.mSigTypeOK; return *this;};
	Sdata<T>& operator+=(const Sdata<T>& b) { mData += b.mData; return *this;};
	Sdata<T>& operator-=(const Sdata<T>& b) { mData -= b.mData; return *this;};
	Sdata<T>& operator*=(const Sdata<T>& b) { mData *= b.mData; return *this;};
	Sdata<T>& operator!() { mData = !mData; return *this;};
	//TBool operator!=(const Sdata<T>& b) {return !this->operator==(b);};
	TBool Set(const T& aData) { TBool res = aData != mData; mData = aData; mValid = ETrue; return res; };
    protected:
	void InpFromString(istringstream& aStream, T& aRes);
    public:
	T mData;
};

template<class T> TBool Sdata<T>::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = EFalse;
    T sdata;
    InpFromString(aStream, sdata);
//    aStream >> std::boolalpha >> sdata;
    if (aRes = !aStream.fail()) {
	mValid = ETrue; if (mData != sdata) { mData = sdata; changed = ETrue; }
    }
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
	MtrBase(TMtrType aType, TMtrDim aDim): DtBase(), mType(aType), mDim(aDim) {};
	MtrBase(const MtrBase& aMtr): DtBase(aMtr), mType(aMtr.mType), mDim(aMtr.mDim) {};
	virtual void ToString(string& aString) const;
	TBool FromString(const string& aString);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const MtrBase& aData, const string& aTypeSig);
	static int ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim);
	int Ind(int r, int c) const;
	int IntSize() const;
	MtrBase& operator=(const MtrBase& b) { this->DtBase::operator=(b); mType = b.mType; mDim = b.mDim; return *this;};
	MtrBase& operator+=(const MtrBase& b);
	MtrBase& operator-=(const MtrBase& b);
	MtrBase& Mpl(const MtrBase& a, const MtrBase& b);
	MtrBase& Mpl(const void* b);
	MtrBase& Invm(const MtrBase& a);
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << "?";};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes) { return EFalse;}
	virtual void AddElem(const MtrBase& aB, TInt aRI, TInt aCI) {};
	virtual void SubElem(const MtrBase& aB, TInt aRI, TInt aCI) {};
	virtual void MplElems(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac, const MtrBase& b, TInt br, TInt bc) {};
	virtual void MplElem(TInt r, TInt c, const void* b) {};
	virtual void MplRtoC(TInt r, TInt c, const MtrBase& a, const MtrBase& b) {};
	virtual void InvmElem(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac) {};
	virtual void SetIntSize(TInt aSize) {};
	virtual void SetMplncArg1Hint(const DtBase& res, const DtBase& arg2);
	virtual void SetMplncArg2Hint(const DtBase& res, const DtBase& arg1);
    public:
	TMtrType mType;
	TMtrDim mDim;
};

template<class T> class Mtr: public MtrBase
{ 
    public:
	Mtr(): MtrBase() {};
	Mtr(TMtrType aType, TMtrDim aDim): MtrBase(aType, aDim) {};
	Mtr(const Mtr<T>& aMtr): MtrBase(aMtr), mData(aMtr.mData) {};
	template <class TA> Mtr(const Mtr<TA>& aMtr);
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return MtrBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Mtr<T>& aData) { return MtrBase::IsDataFit(aData, TypeSig());};
	T GetElem(int r, int c) const { int i = Ind(r,c); return (i == -1) ? T(0) : mData.at(i);};
	T& Elem(int r, int c) { int i = Ind(r,c); __ASSERT(i >= 0 && i < mData.size()); return mData.at(i);};
	virtual TBool operator==(const MDtBase& sb) override { 
	    const Mtr<T>& b = dynamic_cast<const Mtr<T>& >(sb); return &b != NULL && DtBase::operator==(b) &&
		this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	//TBool operator==(const Mtr<T>& b) { 
	 //   return this->mValid == b.mValid && this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	//TBool operator!=(const Mtr<T>& b) {return !this->operator==(b);};
	Mtr<T>& operator=(const Mtr<T>& b) { this->MtrBase::operator=(b); mData = b.mData; return *this;};
	template <class TA> void CastDown(const Mtr<TA>& a);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << mData.at(aInd);};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes);
	virtual void AddElem(const MtrBase& b, TInt r, TInt c);
	virtual void SubElem(const MtrBase& b, TInt r, TInt c);
	virtual void MplElems(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac, const MtrBase& b, TInt br, TInt bc);
	virtual void MplElem(TInt r, TInt c, const void* b);
	virtual void MplRtoC(TInt r, TInt c, const MtrBase& a, const MtrBase& b);
	virtual void InvmElem(TInt r, TInt c, const MtrBase& a, TInt ar, TInt ac) { T e = ((Mtr<T>&) a).GetElem(ar, ac); T res = 1/e; Elem(r, c) = res;
	    if (std::isinf(res)) mValid = EFalse;};
	virtual void SetIntSize(TInt aSize) { if (mData.size() != aSize) mData.resize(aSize);};
    public:
	vector<T> mData;
};

template <class T> template <class TA> Mtr<T>::Mtr(const Mtr<TA>& aMtr): MtrBase(aMtr) {
    CastDown(aMtr);
}

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

template<class T> void Mtr<T>::MplElem(TInt r, TInt c, const void* b)
{
    Elem(r, c) *= *((const T*) b);
}

template<class T> void Mtr<T>::AddElem(const MtrBase& b, TInt r, TInt c) 
{ 
    if (Ind(r,c) == -1) {
	mValid = EFalse; return;} ; 
    Elem(r, c) += ((Mtr<T>&) b).GetElem(r, c);
};

template<class T> void Mtr<T>::SubElem(const MtrBase& b, TInt r, TInt c)
{
    if (Ind(r,c) == -1) {
	mValid = EFalse; return;} ;
    Elem(r, c) -= ((Mtr<T>&) b).GetElem(r, c);
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

// Tuple, with named components, adds the components on run-time
// Full_signature := TypeSig  ','   Paremeters ;
// Parameters := CompSig {"," CompSig} ;
// CompSig := CompTypeSig  ':' CompName ;
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
	virtual void ToString(string& aString) const;
	TBool FromString(const string& aString);
	DtBase* GetElem(const string& aName);
	virtual TBool operator==(const MDtBase& sb) override;
	NTuple& operator=(const NTuple& b);
	//TBool operator==(const NTuple& b);
	//TBool operator!=(const NTuple& b) {return !this->operator==(b);};
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const;
    protected:
	TBool IsCTypesFit(const tCTypes& aCt) const;
	void TypeParsToString(stringstream& aStream) const;
    public:
	tComps mData;
};

// Enumeration, element of ordered finite set
// Data is represented by index in the set
class Enum: public DtBase
{
    public:
	typedef vector<string> tSet;
    public:
	Enum(): DtBase() {};
	Enum(const Enum& aData): DtBase(aData) {};
	virtual ~Enum();
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const Enum& aData);
	static int ParseSigPars(const string& aCont, string& aSig, tSet& aSet);
	static int ParseSig(const string& aCont, string& aSig);
	TBool AreTypeParsFit(const tSet& aSet) const;
	virtual void ToString(string& aString) const;
	TBool FromString(const string& aString);
	void Init(const tSet& aSet);
    public:
	virtual string GetTypeSig() const { return TypeSig();};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes);
	virtual void DataToString(stringstream& aStream) const;
	virtual TBool IsCompatible(const DtBase& b);
    public:
	virtual TBool operator==(const MDtBase& sb) override;
	TBool operator>(const Enum& b) const { return mData > b.mData;};
	TBool operator>=(const Enum& b) const { return mData >= b.mData;};
	TBool operator<(const Enum& b) const { return mData < b.mData;};
	TBool operator<=(const Enum& b) const { return mData <= b.mData;};

    protected:
	void TypeParsToString(stringstream& aStream) const;
    public:
	tSet mSet;
	TInt mData;
};


/** @brief Vector base
 * */
class VectorBase : public DtBase
{
    public:
	VectorBase(): DtBase() {};
	VectorBase(TInt aSize): DtBase() {};
	VectorBase(const VectorBase& aSrc): DtBase(aSrc) {};
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << "?";};
	virtual TBool ElemFromString(TInt aIdx, istringstream& aStream, TBool& aRes) { return EFalse;}
	virtual TInt Size() const { return -1;}
	// From DtBase
	virtual void DataToString(stringstream& aStream) const override;
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes);
	virtual TBool IsCompatible(const MDtBase& b) override;
};

/** @brief Typed vector
 * */
template <typename T>
class Vector : public VectorBase
{
    public:
	Vector(): VectorBase() {}
	Vector(TInt aSize): VectorBase(aSize) {}
	Vector(const Vector& aSrc): VectorBase(aSrc) {}
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Vector<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	// From VectorBase
	virtual TInt Size() const override { return mData.size();}
	virtual void ElemToString(TInt aIdx, stringstream& aStream) const override { aStream << mData.at(aIdx);}
	virtual TBool ElemFromString(TInt aIdx, istringstream& aStream, TBool& aRes) override {
	    __ASSERT(aIdx <= mData.size());
	    T data; aStream >> data; TBool res = aIdx >= mData.size() || mData.at(aIdx) != data; 
	    if (aIdx < mData.size())
		mData.at(aIdx) = data;
	    else 
		mData.push_back(data);
	    aRes = ETrue;
	    return ETrue; // TODO YB get non trivial ret val
	}
	virtual TBool operator==(const MDtBase& b) override {
	    if (!IsCompatible(b)) return false;
	    const Vector<T>* vb = dynamic_cast<const Vector<T>*>(&b);
	    if (!vb) return false;
    	    return this->mData == vb->mData; }
	// From MDtBase
	virtual string GetTypeSig() const { return TypeSig();};
	virtual DtBase* Clone() {return new Vector<T>(*this);};
	// Local
	virtual TBool GetElem(int aInd, T& aElem) const {
	    TBool res = ETrue;
	    if (aInd < Size()) aElem = mData.at(aInd);
	    else res = EFalse;
	    return res; }
    public:
	vector<T> mData;
};

#endif
