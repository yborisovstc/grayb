#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"

// Agent function "Increment of Int data"
class AIncInt: public Elem
{
    public:
	static const char* Type() { return "AIncInt";};
	AIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
};

#endif
