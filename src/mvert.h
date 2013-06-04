#ifndef __GRAYB_MVERT_H
#define __GRAYB_MVERT_H

#include "plat.h"
#include <set>

using namespace std;

class Base;
class MEdge;
class Elem;

// Compatibility checker

class MCompatChecker
{
    public:
	static const char* Type() { return "MCompatChecker";};
	virtual TBool IsCompatible(Elem* aPair, TBool aExt = EFalse) = 0;
};

// Graph vertex interface
class MVert
{
    public:
	static const char* Type() { return "MVert";};
	virtual TBool Connect(MVert* aPair) = 0;
	virtual TBool Connect(MEdge* aEdge) = 0;
	virtual void Disconnect(MVert* aPair) = 0;
	virtual void Disconnect(MEdge* aEdge) = 0;
	virtual set<MVert*>& Pairs() = 0;
	virtual Base* EBase() = 0;
};

#endif
