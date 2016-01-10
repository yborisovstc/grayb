#ifndef __GRAYB_BASE_H
#define __GRAYB_BASE_H

#include <stdarg.h>
#include <string.h>
#include "plat.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>

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
	virtual ~Base() {};
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(DoGetObj(aInst->Type()));};
	void* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual void *DoGetObj(const char *aName) = 0;
};


#endif
