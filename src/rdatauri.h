#ifndef __GRAYB_RDATAURI_H
#define __GRAYB_RDATAURI_H

#include "plat.h"
#include "guri.h"
#include "rdata.h"


/** @brief Uri data
 * Wraps Guri to aline with HDt
 * */
class DGuri: public DtBase
{
    public:
	DGuri(): DtBase() {};
	DGuri(const DGuri& aSrc): DtBase(aSrc), mData(aSrc.mData) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const DGuri& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) override;
	virtual void DataToString(stringstream& aStream) const override;
	virtual TBool operator==(const MDtBase& b) override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DGuri&>(b).mData);}
	TBool operator>(const DGuri& b) const { return mData > b.mData;};
	TBool operator>=(const DGuri& b) const { return mData >= b.mData;};
	TBool operator<(const DGuri& b) const { return mData < b.mData;};
	TBool operator<=(const DGuri& b) const { return mData <= b.mData;};
	//virtual TBool IsCompatible(const DtBase& aSrc) override;
    public:
	GUri mData;
};

#endif

