#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "elem.h"
#include <set>

class Edge;

class Vert: public Elem, public MVert
{
    public:
	typedef multimap<TNMKey, MEdge*> TEdgesMap;

    public:
	static const char* Type() { return "Vert";};
	static string PEType();
	Vert(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Vert(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vert();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual TBool Connect(MEdge* aEdge);
	virtual void Disconnect(MVert* aPair);
	virtual void Disconnect(MEdge* aEdge);
	virtual set<MVert*>& Pairs();
	virtual Base* EBase();
	// From Elem
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void DoOnCompChanged(Elem& aComp);
	// Iface cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
    protected:
	void RemoveFromMap(MEdge* aEdge, const TNMKey& aKey);
    protected:
	set<MVert*> iPairs;
	// TODO [YB] Do we need map with ECkey? There is no edges type variation at the moment.
	TEdgesMap iMEdges;
};

#endif
