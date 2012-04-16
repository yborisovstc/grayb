#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"

class FuncBase: public Elem, public MACompsObserver
{
    public:
	static const char* Type() { return "FuncBase";};
	FuncBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
    protected:
	virtual TBool HandleIoChanged(MVert* aCp) = 0;
};

// Agent function "Increment of Int data"
class AIncInt: public FuncBase, public MDIntGet
{
    public:
	static const char* Type() { return "AIncInt";};
	AIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From FuncBase
	virtual TBool HandleIoChanged(MVert* aCp);
	// From MDIntGet
	virtual TInt Value();
    protected:
	void SetRes(TInt aData);
    private:
	TInt mData;
};

#endif
