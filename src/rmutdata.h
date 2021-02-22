#ifndef __GRAYB_RMUTDATA_H
#define __GRAYB_RMUTDATA_H

#include "plat.h"
#include "chromo.h"
#include "rdata.h"
#include "chromo2.h"

/** @brief Mutation data
 * Wraps TMut to aline with HDt
 * */
class DMut: public DtBase
{
    public:
	DMut(): DtBase() {};
	DMut(const DMut& aSrc): DtBase(aSrc), mData(aSrc.mData) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const DMut& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) override;
	virtual void DataToString(stringstream& aStream) const override;
	virtual TBool operator==(const MDtBase& b) override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DMut&>(b).mData);}
	//virtual TBool IsCompatible(const DtBase& aSrc) override;
    public:
	TMut mData;
};


/** @brief Chromo2 segment data
 * Wraps Chromo to aline with HDt
 * */
class DChr2: public DtBase
{
    public:
	using TData = Chromo2;
    public:
	DChr2(): DtBase() {};
	DChr2(const DChr2& aSrc): DtBase(aSrc), mData(aSrc.mData) {};
	static const char* TypeSig();
	static TBool IsSrepFit(const string& aString);
	static TBool IsDataFit(const DChr2& aData);
    public:
	// From DtBase
	virtual string GetTypeSig() const override { return TypeSig();};
	virtual TBool DataFromString(istringstream& aStream, TBool& aRes) override;
	virtual void DataToString(stringstream& aStream) const override;
	virtual TBool operator==(const MDtBase& b) override {
	    return this->DtBase::operator==(b) && (mData == dynamic_cast<const DChr2&>(b).mData);}
    public:
	Chromo2 mData;
};


#endif // __GRAYB_RMUTDATA_H

