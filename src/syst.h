#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "vert.h"

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert
{
    public:
	static const char* Type() { return "ConnPointBase";};
	ConnPointBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
};

class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	Syst(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompsObserver
	virtual void DoOnCompChanged(Elem& aComp);
};

#endif
