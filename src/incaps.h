#ifndef __GRAYB_INCAPS_H
#define __GRAYB_INCAPS_H

#include "syst.h"

class Incaps: public Elem, public MACompsObserver, public MAgent
{
    public:
	static const char* Type() { return "IncapsAgent";};
	static string PEType();
	Incaps(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Incaps(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MElem& aContext, MElem& aComp, const string& aContName = string());
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    protected:
	TBool IsPtOk(MElem& aContext, MElem* aPt);
};

#endif
