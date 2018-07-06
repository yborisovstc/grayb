#ifndef __GRAYB_MVERTP_H
#define __GRAYB_MVERTP_H

#include "miface.h"
#include "plat.h"
#include "melem.h"
#include "ifu.h"

/** \brief Graph vertex interface, connecting property based
 *
 */
class MVertp: public MIface
{
    public:
	static const char* Type() { return "MVertp";};
	/** @biref Connecting pair
	 * @param aCp - connection point Id
	 */
	virtual TBool Connect(MVertp* aPair, const string& aCp = string()) = 0;
	virtual void Disconnect(MVertp* aPair) = 0;
	virtual TInt PairsCount() const = 0;
	virtual MVertp* GetPair(TInt aInd) const = 0;
	virtual TBool IsPair(const MVertp* aPair) const = 0;
	// TODO [YB] We introduced local iface resolver here. It is restricted for some ifaces only
	// The idea was that earlier the EBase() methos was used that return Base - not safe solution
	// But in fact it's same, current resolver allows MElem (i.e. Base). To consider safer solution
	// Currently MElem and MCompatChecker are allowe by resolver, MElem is used for getting Uri for logging
	// Possible to use Uid() instead and introduce MCompatChecker getter
	virtual MIface* MVertp_DoGetObj(const char *aName) = 0;
	template <class T> T* GetObj(T* aInst) {return aInst = dynamic_cast<T*>(MVertp_DoGetObj(aInst->Type()));};
	virtual void DumpCps() const = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
	virtual MIface* MVertp_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MVertp_Mid() const = 0;
	MIface* Call(const string& aSpec, string& aRes) override {return MVertp_Call(aSpec, aRes);}
	string Mid() const override { return MVertp_Mid();}
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

#endif
