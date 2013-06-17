#ifndef __GRAYB_BASE_H
#define __GRAYB_BASE_H

#include <stdarg.h>
#include <string.h>
#include "plat.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

class Base;

// Request connext
class RqContext
{
    public:
	RqContext(Base* aRequestor, const RqContext* aRequestorCtx = NULL): iRequestor(aRequestor), iRequestorCtx(aRequestorCtx) {};
	TBool IsInContext(const Base* aElem) const {
	    return (iRequestor == NULL) ? EFalse: ((aElem == iRequestor) ? ETrue : 
		    (iRequestorCtx == NULL ? EFalse : iRequestorCtx->IsInContext(aElem))); }
	Base* Requestor() const { return iRequestor;}
	const RqContext* Ctx() const { return iRequestorCtx;}
    private:
	Base* iRequestor;
	// Requestor context
	const RqContext* iRequestorCtx;
};

// Interface provider. uc_008
class MIfaceProv
{
    public:
	// Iface cache mapping key: iface name, requestor
	typedef pair<string, Base*> TICMapKey;
	// Iface cache
	typedef multimap<TICMapKey, void*> TICache;
	// Iface cache const iterator
	typedef multimap<TICMapKey, void*>::const_iterator TICacheCIter;
	// Iface cache range
	typedef pair<TICacheCIter, TICacheCIter> TICacheRange;
    public:
	virtual TICacheRange GetIfi(const char *aName, const RqContext* aCtx = NULL) = 0;
};

// Base named
class NBase
{
    public:
	NBase(const string& aName): iName(aName) {};
	virtual ~NBase() {};
	const string& Name() const { return iName;}
    protected:
	string iName;
};
;

// Base supporting iface provider
class Base: public NBase, public MIfaceProv
{
    public:
	Base(const string& aName): NBase(aName) {};
	virtual ~Base() {};
	const string& Name() const { return iName;}
	template <class T> T* GetObj(T* aInst) {RqContext ctx(this); return aInst = static_cast<T*>(DoGetObj(aInst->Type(), ETrue, &ctx));};
	void* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL) = 0;
	// Form MIfaceProv
	virtual TICacheRange GetIfi(const char *aName, const RqContext* aCtx = NULL) = 0;
};

#endif
