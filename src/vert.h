#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "base.h"

class Vert: public Base, public MVert
{
    public:
	static const char* Type() { return "Vert";};
	Vert(const string& aName = string());
	// From Base
	virtual void *DoGetObj(const char *aName);
};

#endif
