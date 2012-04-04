#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include "incaps.h"

class State: public Incaps
{
    public:
	static const char* Type() { return "State";};
	State(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From Elem
	virtual void OnCompChanged(Elem& aComp);
};

#endif
