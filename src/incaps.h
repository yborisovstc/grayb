#ifndef __GRAYB_INCAPS_H
#define __GRAYB_INCAPS_H

#include "syst.h"

class Incaps: public Syst, public MACompsObserver
{
    public:
	static const char* Type() { return "IncapsAgent";};
	Incaps(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual void HandleCompChanged(Elem& aContext, Elem& aComp);
};

#endif
