#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"
#include "mdata.h"

class FuncBase: public Elem, public MACompsObserver, public MDataObserver
{
    public:
	static const char* Type() { return "FuncBase";};
	FuncBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp) = 0;
	void NotifyUpdate();
};

// Agent base of Int function
class AFunInt: public FuncBase, public MDIntGet
{
    public:
	static const char* Type() { return "AFunInt";};
	AFunInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
    protected:
	void SetRes(TInt aData);
	MDIntGet* GetInp(const string& aInpName);
    protected:
	TInt mData;
};

// Agent function "Increment of Int data"
class AIncInt: public AFunInt
{
    public:
	static const char* Type() { return "AIncInt";};
	AIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Agent of Int function result
class AFunIntRes: public AFunInt
{
    public:
	static const char* Type() { return "AFunIntRes";};
	AFunIntRes(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	void UpdateOutp();
};

// Agent function "Addition of Int data"
class AAddInt: public AFunInt
{
    public:
	static const char* Type() { return "AAddInt";};
	AAddInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};


#endif
