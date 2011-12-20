#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"

class ChromoNode;

// Mutable element of graph
class MMutable
{
    public:
	virtual TBool DoMutation(const ChromoNode& aCromo) = 0;
};

#endif
