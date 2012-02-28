#ifndef __GRAYB_MPROP_H
#define __GRAYB_MPROP_H

#include "plat.h"
#include <string>

using namespace std;

// Interface of property
class MProp
{
    public:
	static const char* Type() { return "MProp";};
	virtual const string& Value() const = 0;
};

#endif
