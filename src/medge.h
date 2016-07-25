#ifndef __GRAYB_MEDGE_H
#define __GRAYB_MEDGE_H

#include "plat.h"
#include <string>
#include "ifu.h"
#include "miface.h"

class MVert;

using namespace std;

// Element of native graph hier
class MEdge: public MIface
{
    public:
	static const char* Type() { return "MEdge";};
	virtual TBool ConnectP1(MVert* aPoint) = 0;
	virtual TBool ConnectP2(MVert* aPoint) = 0;
	// TODO [YB] Do we need disconnect from point? To clarify the concept of connection, ref uc_015
	virtual void Disconnect(MVert* aPoint) = 0;
	virtual void Disconnect() = 0;
	virtual MVert* Point1() const = 0;
	virtual MVert* Point2() const = 0;
	virtual MVert* Ref1() const = 0;
	virtual MVert* Ref2() const = 0;
	virtual void SetPoint1(const string& aRef) = 0;
	virtual void SetPoint2(const string& aRef) = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

#endif
