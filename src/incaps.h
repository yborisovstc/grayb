#ifndef __GRAYB_INCAPS_H
#define __GRAYB_INCAPS_H

#include "syst.h"

class Incaps: public Syst
{
    public:
	static const char* Type() { return "Incaps";};
	Incaps(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From Elem
	virtual Elem* Clone(const string& aName, Elem* aMan, MEnv* aEnv) const;
	virtual void OnCompChanged(Elem& aComp);
};

#endif
