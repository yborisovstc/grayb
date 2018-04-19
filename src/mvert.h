#ifndef __GRAYB_MVERT_H
#define __GRAYB_MVERT_H

#include "plat.h"
#include "melem.h"
#include "ifu.h"
#include <set>

using namespace std;

class Base;
class MEdge;

// TODO [YB] To we need compat checker. Isn't is better to include this to MVert?
// TODO [YB] To reconsider mechanism of compat checking. Current ony is ugly
// Compatibility checker. Behaves both as checking and checked parts.
// TODO [YB] It is iface of Connectable actually, to rename
class MCompatChecker: public MIface
{
    public:
	enum TDir { ERegular, EInp, EOut };
    public:
	static const char* Type() { return "MCompatChecker";};
	// Checking
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse) = 0;
	// Checked
	// In case if checked is extender, to get extended part
	virtual MElem* GetExtd() = 0;
	// Direction
	virtual TDir GetDir() const = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    public:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
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
	// TODO [YB] We introduced local iface resolver here. It is restricted for some ifaces only
	// The idea was that earlier the EBase() methos was used that return Base - not safe solution
	// But in fact it's same, current resolver allows MElem (i.e. Base). To consider safer solution
	// Currently MElem and MCompatChecker are allowe by resolver, MElem is used for getting Uri for logging
	// Possible to use Uid() instead and introduce MCompatChecker getter
	virtual MIface* MVert_DoGetObj(const char *aName) = 0;
	template <class T> T* GetObj(T* aInst) {return aInst = dynamic_cast<T*>(MVert_DoGetObj(aInst->Type()));};
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

// Simple Connection point
// Connection point gets info of agent role, i.e. the ifaces the agent provides and requires.
class MConnPoint: public MIface
{
    public:
	static const char* Type() { return "MConnPoint";};
	virtual TBool IsProvided(const string& aIfName) const = 0;
	virtual TBool IsRequired(const string& aIfName) const = 0;
	virtual string Provided() const = 0;
	virtual string Required() const = 0;
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

// Connection point with multiple ifaces (isn't used at the moment)
// Connection point gets info of agent role, i.e. the ifaces the agent provides and requires.
class MConnPointm: public MIface
{
    public:
	static const char* Type() { return "MConnPointm";};
	virtual TBool IsProvided(const string& aIfName) const = 0;
	virtual TBool IsRequired(const string& aIfName) const = 0;
	virtual void GetProvided(set<string>& aIfNames) const = 0;
	virtual void GetRequired(set<string>& aIfNames) const = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
};

// Socket

class MSocket: public MIface
{
    public:
	static const char* Type() { return "MSocket";};
	virtual TInt PinsCount() const = 0;
	virtual MElem* GetPin(TInt aInd) = 0;
	// Get pin existing in context
	virtual MElem* GetPin(const TICacheRCtx& aCtx) = 0;
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
