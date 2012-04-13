#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"

class FuncBase: public Elem
{
    public:
	static const char* Type() { return "FuncBase";};
	FuncBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
};

// Agent function "Increment of Int data"
class AIncInt: public FuncBase, public MACompsObserver
{
    public:
	static const char* Type() { return "AIncInt";};
	AIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
    protected:
	void SetRes(TInt aData);
    private:
	TInt mData;
};

#endif
