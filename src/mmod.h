#ifndef __GRAYB_MMOD_H
#define __GRAYB_MMOD_H

#include "plat.h"

class Elem;
class GUri;

// Importing module
class MMod
{
    public:
	static const char* Type() { return "MMod";};
	virtual Elem* Import(const GUri& aUri) = 0;
};

#endif
