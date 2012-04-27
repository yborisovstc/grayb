#ifndef __GRAYB_MDES_H
#define __GRAYB_MDES_H

#include "plat.h"
#include <string>

using namespace std;

class MDesActivable
{
    public:
	static const char* Type() { return "MDesActivable";};
	virtual const void SetActive() = 0;
};

#endif
