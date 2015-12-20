#ifndef __GRAYB_INCAPS_H
#define __GRAYB_INCAPS_H

#include "syst.h"

class Incaps: public Elem, public MACompsObserver
{
    public:
	static const char* Type() { return "IncapsAgent";};
	static string PEType();
	Incaps(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Incaps(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MElem& aContext, MElem& aComp);
    protected:
	TBool IsPtOk(Elem& aContext, Elem* aPt);
};

#endif
