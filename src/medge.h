#ifndef __GRAYB_MEDGE_H
#define __GRAYB_MEDGE_H

#include "plat.h"

class MVert;
class Base;

// Element of native graph hier
class MEdge
{
    public:
	static const char* Type() { return "MEdge";};
	virtual Base* EBase()= 0;
	virtual const Base* EBase() const = 0;
	virtual TBool Connect() = 0;
	virtual TBool ConnectP1(MVert* aPoint) = 0;
	virtual TBool ConnectP2(MVert* aPoint) = 0;
	// TODO [YB] Do we need disconnect from point? To clarify the concept of connection, ref uc_015
	virtual void Disconnect(MVert* aPoint) = 0;
	virtual void Disconnect() = 0;
	virtual MVert* Pair(const MVert* aPoint) = 0;
	virtual MVert* Point1() const = 0;
	virtual MVert* Point2() const = 0;
};

#endif
