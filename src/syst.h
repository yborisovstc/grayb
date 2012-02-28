#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "vert.h"

class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	Syst(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From Elem
	virtual Elem* Clone(const string& aName, Elem* aMan, MEnv* aEnv) const;
	virtual void OnCompChanged(Elem& aComp);
};

#endif
