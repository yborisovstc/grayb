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
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp) = 0;
};

// Agent base of Int function
class AFunInt: public FuncBase, public MDIntGet
{
    public:
	static const char* Type() { return "AFunInt";};
	AFunInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MDIntGet
	virtual TInt Value();
    protected:
	void SetRes(TInt aData);
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
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
};

// Agent of Int function result
class AFunIntRes: public AFunInt
{
    public:
	static const char* Type() { return "AFunIntRes";};
	AFunIntRes(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
    protected:
	void UpdateOutp(Elem& aContext);
};

#endif
