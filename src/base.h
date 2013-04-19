#ifndef __GRAYB_BASE_H
#define __GRAYB_BASE_H

#include <stdarg.h>
#include <string.h>
#include "plat.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

// Base for all classes
class Base
{
    public:
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

    public:
	Base(const string& aName): iName(aName) {};
	virtual ~Base() {};
	const string& Name() const { return iName;}
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(DoGetObj(aInst->Type())); };
	// TODO [YB] To implement getting ifaces list
	void* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL) = 0;
    protected:
	string iName;
};

#endif
