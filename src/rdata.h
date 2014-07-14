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
	DtBase(const DtBase& d): mValid(d.mValid) {};
	virtual ~DtBase();
	static int ParseSigPars(const string& aCont, string& aSig);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const DtBase& aData, const string& aTypeSig);
	TBool Set(const DtBase& d) { return mValid != d.mValid, mValid = d.mValid; };
	void ToString(string& aString) const;
	TBool FromString(const string& aString);
    public:
	virtual string GetTypeSig() const { return "?";};
	virtual void DataToString(stringstream& aStream) const {aStream << "?";};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) {};
    public:
	TBool mValid;
};

template<class T> class Sdata: public DtBase
{
    public:
	Sdata(): DtBase() {};
	Sdata(const Sdata& d): DtBase(d) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return DtBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Sdata<T>& aData) { return DtBase::IsDataFit(aData, TypeSig());};
	TBool Set(const Sdata& d);
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void DataToString(stringstream& aStream) const { aStream << mData;};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes);
    public:
	T mData;
};

template<class T> TBool Sdata<T>::Set(const Sdata& d) 
{ 
    TBool res = DtBase::Set(d);
    if (mData != d.mData) {
	mData = d.mData;
	res = ETrue;
    }
   return res;
};

template<class T> TBool Sdata<T>::DataFromString(istringstream& aStream, TBool& aRes)
{
    TBool changed = EFalse;
    T sdata;
    aStream >> sdata;
    if (aRes = !aStream.fail() && mData != sdata) { mData = sdata; changed = ETrue; }
    return changed;
}



// Matrix
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
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << "?";};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes) {};
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
	T GetElem(int r, int c) { int i = Ind(r,c); return (i == -1) ? T(0) : mData.at(i);};
	TBool operator==(const Mtr<T>& b) { 
	    return this->mValid == b.mValid && this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	TBool operator!=(const Mtr<T>& b) {return !this->operator==(b);};
	virtual string GetTypeSig() const { return TypeSig();};
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << mData.at(aInd);};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes);
    public:
	vector<T> mData;
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


#endif
