#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "elem.h"
#include <set>

class Edge;

class Vert: public Elem, public MVert
{
    public:
	typedef multimap<TCkey, MEdge*> TEdgesMap;
    public:
	class IterImplVert: public IterImplBase
	{
	    friend class Elem;
	    public:
	    static const char* Type() { return "IterImplVert";};
	    IterImplVert(Elem& aElem, GUri::TElem aId, TBool aToEnd = EFalse);
	    IterImplVert(Elem& aElem);
	    IterImplVert(const IterImplVert& aImpl);
	    virtual void Set(const IterImplBase& aImpl);
	    virtual void PostIncr();
	    virtual TBool IsEqual(const IterImplBase& aImpl) const;
	    virtual TBool IsCompatible(const IterImplBase& aImpl) const;
	    virtual Elem*  GetElem();
	    virtual void* DoGetObj(const char *aName);
	    virtual const void* DoGetObj(const char *aName) const;
	    public:
	    TEdgesMap::iterator iEIter; // Edges iter
	    pair<TEdgesMap::iterator, TEdgesMap::iterator> iEIterRange;
	};

    public:
	static const char* Type() { return "Vert";};
	Vert(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
	virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void DoOnCompChanged(Elem& aComp);
    protected:
	void RemoveFromMap(MEdge* aEdge, const TCkey& aKey);
    protected:
	set<MVert*> iPairs;
	multimap<TCkey, MEdge*> iMEdges;
};

#endif
