#ifndef __GRAYB_RCUST_H
#define __GRAYB_RCUST_H

#include "mrcust.h"
#include "elem.h"


// Agent of visual representation customizing helper
class RCust: public Elem, public MRCust, public MACompsObserver
{
    public:
	typedef multimap<TNMKey, MEdge*> TEdgesMap;

    public:
	static const char* Type() { return "RCust";};
	static string PEType();
	RCust(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	RCust(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~RCust();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp) = 0;
	// From MRCust
	Elem* GetNextComp(const Elem* aPrevComp, TDir aDir = EVert);
	Elem* GetPrevComp(const Elem* aNextComp, TDir aDir = EVert);
	void MoveComp(Elem* aComp, TDir aDir = EVert, const Elem* aPrevComp);
	void InsertComp(Elem* aComp, TDir aDir = EVert, const Elem* aPrevComp);
	// From Elem
	virtual void OnCompDeleting(Elem& aComp, TBool aSoft = ETrue);
	virtual void OnCompAdding(Elem& aComp);
	virtual void DoOnCompChanged(Elem& aComp);
};

#endif
