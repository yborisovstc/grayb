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
	Vert(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Vert(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vert();
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual TBool Connect(MEdge* aEdge);
	virtual void Disconnect(MVert* aPair);
	virtual void Disconnect(MEdge* aEdge);
	virtual set<MVert*>& Pairs();
	virtual Base* EBase();
	// From Elem
	virtual TBool OnCompChanged(MElem& aComp);
	virtual void SetRemoved();
	// Iface cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
    protected:
	void Disconnect();
	void RemoveFromMap(MEdge* aEdge, const TNMKey& aKey);
    protected:
	set<MVert*> iPairs;
	TEdgesMap iMEdges;
};

#endif
