#ifndef __GRAYB_MRCUST_H
#define __GRAYB_MRCUST_H

#include "plat.h"

class Elem;

// Interface of visual representation customization helper
class MRCust
{
    public:
	// Direction 
	enum TDir {EVert, EHoriz};
    public:
	static const char* Type() { return "MRCust";};
	Elem* GetNextComp(const Elem* aPrevComp, TDir aDir = EVert) = 0;
	Elem* GetPrevComp(const Elem* aNextComp, TDir aDir = EVert) = 0;
	void MoveComp(Elem* aComp, TDir aDir = EVert, const Elem* aPrevComp) = 0;
	void InsertComp(Elem* aComp, TDir aDir = EVert, const Elem* aPrevComp) = 0;
};

#endif
