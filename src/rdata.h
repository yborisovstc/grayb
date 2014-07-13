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
	void ToString(string& aString, const string& aTypeSig) const;
	static int ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim);
	int Ind(int r, int c) const;
    public:
	TMtrType mType;
	TMtrDim mDim;
	bool mValid;
};

template<class T> struct Mtr: public MtrBase
{ 
    public:
	static const char* TypeSig();
	vector<T> mData;
	Mtr(): MtrBase() {};
	Mtr(const Mtr<T>& aMtr): MtrBase(aMtr), mData(aMtr.mData) {};
	void ToString(string& aString) const;
	T GetElem(int r, int c) { int i = Ind(r,c); return (i == -1) ? T(0) : mData.at(i);};
	TBool operator==(const Mtr<T>& b) { return this->mValid == b.mValid && this->mType == b.mType && this->mDim == b.mDim && this->mData == b.mData;};
	TBool operator!=(const Mtr<T>& b) {return !this->operator==(b);};
};


template<class T> inline void Mtr<T>::ToString(string& aString) const
{
    MtrBase::ToString(aString, TypeSig());
    stringstream ss;
    if (mValid) {
	for (typename vector<T>::const_iterator it = mData.begin(); it != mData.end(); it++) {
	    ss << " " << (*it);
	}
    }
    aString += ss.str();
}


#endif
