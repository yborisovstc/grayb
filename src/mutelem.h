#ifndef __GRAYB_MUTELEM_H
#define __GRAYB_MUTELEM_H

#include "mmuta.h"
#include "elem.h"

class Chromo;

// Element of native graph hier - mutable
class MutElem: public Elem, public MMutable 
{
    public:
	static const char* Type() { return "MutElem";};
	MutElem(const string &aName = string());
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
    protected:
	void MutAddNode(const ChromoNode& aSpec);
    protected:
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
};


#endif
