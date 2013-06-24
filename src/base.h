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

// Base supporting iface provider
class Base
{
    public:
	Base(const string& aName): iName(aName) {};
	virtual ~Base() {};
	const string& Name() const { return iName;}
	template <class T> T* GetObj(T* aInst) {RqContext ctx(this); return aInst = static_cast<T*>(DoGetObj(aInst->Type(), ETrue, &ctx));};
	void* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL) = 0;
    protected:
	string iName;
};

#endif
