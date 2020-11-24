#ifndef __GRAYB_RMUTDATA_H
#define __GRAYB_RMUTDATA_H

#include "plat.h"
#include "chromo.h"
#include "rdata.h"

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


#endif // __GRAYB_RMUTDATA_H

