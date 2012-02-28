#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"

class ChromoNode;

// Mutable element of graph
class MMutable
{
    public:
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime) = 0;
};

#endif
