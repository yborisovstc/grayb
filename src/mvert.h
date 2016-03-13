#ifndef __GRAYB_MVERT_H
#define __GRAYB_MVERT_H

#include "plat.h"
#include "melem.h"
#include "ifu.h"
#include <set>

using namespace std;

class Base;
class MEdge;
class Elem;

// TODO [YB] To we need compat checker. Isn't is better to include this to MVert?
// TODO [YB] To reconsider mechanism of compat checking. Current ony is ugly
// Compatibility checker. Behaves both as checking and checked parts.
// TODO [YB] It is iface of Connectable actually, to rename
class MCompatChecker
{
    public:
	enum TDir { ERegular, EInp, EOut };
    public:
	static const char* Type() { return "MCompatChecker";};
	// Checking
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse) = 0;
	// Checked
	// In case if checked is extender, to get extended part
	virtual Elem* GetExtd() = 0;
	// Direction
	virtual TDir GetDir() const = 0;
	// Getting associated point
//	virtual vector<Elem*> GetAssoc(RqContext* aCtx) = 0;
};

// Graph vertex interface
class MVert: public MIface
{
    public:
	static const char* Type() { return "MVert";};
	virtual TBool Connect(MVert* aPair) = 0;
	virtual void Disconnect(MVert* aPair) = 0;
	virtual TInt PairsCount() const = 0;
	virtual MVert* GetPair(TInt aInd) const = 0;
	virtual TBool IsPair(const MVert* aPair) const = 0;
	//virtual set<MVert*>& Pairs() = 0;
	virtual void *MVert_DoGetObj(const char *aName) = 0;
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(MVert_DoGetObj(aInst->Type()));};
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
