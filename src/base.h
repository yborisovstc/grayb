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

#if 0
// Interface provider. uc_008
class MIfaceProv
{
    public:
	class IterImplBase
    {
	public:
	IterImplBase() {};
	virtual ~IterImplBase() {};
	IterImplBase(const IterImplBase& aImpl);
	virtual void Set(const IterImplBase& aImpl);
	virtual void PostIncr();
	virtual TBool IsEqual(const IterImplBase& aImpl) const;
	virtual void*  Get();
    };

	class Iterator: public iterator<input_iterator_tag, void*>
    {
	public:
	Iterator(): iImpl(NULL) {};
	Iterator(IterImplBase* aImpl): iImpl(aImpl) {};
	~Iterator() { delete iImpl;};
	Iterator(const Iterator& aIt) { iImpl = new IterImplBase(*(aIt.iImpl));};
	Iterator& operator=(const Iterator& aIt) { iImpl->Set(*(aIt.iImpl)); return *this;};
	Iterator& operator++() { iImpl->PostIncr(); return *this;};
	Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; };
	TBool operator==(const Iterator& aIt) { return iImpl->IsEqual((*aIt.iImpl));};
	TBool operator!=(const Iterator& aIt) { return !operator==(aIt);};
	void*  operator*() { return iImpl->Get();};
	public:
	IterImplBase* iImpl;
    };

	typedef pair<Iterator, Iterator> TIfRange;

    public:
	virtual TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL) = 0;
};
#endif 

// Base supporting iface provider
class Base
{
    public:
	Base(const string& aName) {};
	virtual ~Base() {};
	const string& Name() const { return iName;}
	template <class T> T* GetObj(T* aInst) {RqContext ctx(this); return aInst = static_cast<T*>(DoGetObj(aInst->Type(), ETrue, &ctx));};
	void* GetObj(const char *aType) {return DoGetObj(aType); };
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL) = 0;
    protected:
	string iName;
};

#endif
