#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "elem.h"
#include <set>

class Vert: public Elem, public MVert
{
    public:
	static const char* Type() { return "Vert";};
	Vert(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vert();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual TBool Connect(MEdge* aEdge);
	virtual void Disconnect(MVert* aPair);
	virtual void Disconnect(MEdge* aEdge);
	virtual set<MVert*>& Pairs();
	virtual Base* EBase();
	// From Elem
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void DoOnCompChanged(Elem& aComp);
    protected:
	set<MVert*> iPairs;
	multimap<TCkey, MEdge*> iEdges;
};

#endif
