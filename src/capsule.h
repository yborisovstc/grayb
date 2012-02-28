#ifndef __GRAYB_CAPSL_H
#define __GRAYB_CAPSL_H

#include "syst.h"

class Capsule: public Syst
{
    public:
	static const char* Type() { return "Capsule";};
	Capsule(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From Elem
	virtual void OnCompChanged(Elem& aComp);
};

#endif
