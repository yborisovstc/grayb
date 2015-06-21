#ifndef __GRAYB_MMUTA_H
#define __GRAYB_MMUTA_H

#include "plat.h"

class ChromoNode;

// Mutable element of graph
class MMutable
{
    public:
	//virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse) = 0;
	virtual void DoMutation(const ChromoNode& aMutSpec, const ChromoNode& aSelMut, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode) =  0;
};

#endif
