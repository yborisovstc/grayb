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

template<class T> class Sdata 
{
    public:
	TBool mValid;
	T mData;
};

// Matrix
class MtrBase 
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
	MtrBase(): mType(EMt_Unknown), mDim(TMtrDim(0, 0)), mValid(false) {};
	MtrBase(const MtrBase& aMtr): mType(aMtr.mType), mDim(aMtr.mDim), mValid(aMtr.mValid) {};
	virtual ~MtrBase();
	//void ToString(string& aString, const string& aTypeSig) const;
	void ToString(string& aString) const;
	//TBool FromString(const string& aString, const string& aTypeSig, int& aPos);
	TBool FromString(const string& aString);
	static TBool IsSrepFit(const string& aString, const string& aTypeSig);
	static TBool IsDataFit(const MtrBase& aData, const string& aTypeSig);
	static int ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim);
	int Ind(int r, int c) const;
	int IntSize() const;
	virtual string GetTypeSig() const { return "?";};
	virtual void ElemToString(TInt aInd, stringstream& aStream) const { aStream << "?";};
	virtual TBool ElemFromString(TInt aInd, istringstream& aStream, TBool& aRes) {};
    public:
	TMtrType mType;
	TMtrDim mDim;
	bool mValid;
};

template<class T> class Mtr: public MtrBase
{ 
    public:
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString) { return MtrBase::IsSrepFit(aString, TypeSig());};
	static TBool IsDataFit(const Mtr<T>& aData) { return MtrBase::IsDataFit(aData, TypeSig());};
	Mtr(): MtrBase() {};
	Mtr(const Mtr<T>& aMtr): MtrBase(aMtr), mData(aMtr.mData) {};
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
