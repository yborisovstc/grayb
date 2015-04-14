#ifndef __GRAYB_EDGE_H
#define __GRAYB_EDGE_H

#include "medge.h"
#include "elem.h"

// Edge base
// TODO [YB] Added ConnectP1,2 to do partial connections of edge. So SetPoints
// are not required anymore. Connection processing has been upgraded for Vert.
// To rework connection processing for syst .... Remove Set* methods.
class Edge: public Elem, public MEdge
{
    public:
	class IterImplEdge: public IterImplBase
	{
	    friend class Elem;
	    public:
	    enum TIter {
		EP1, EP2, EEnd
	    };
	    static const char* Type() { return "IterImplEdge";};
	    IterImplEdge(Elem& aElem, GUri::TElem aId, TBool aToEnd = EFalse);
	    IterImplEdge(Elem& aElem);
	    IterImplEdge(const IterImplEdge& aImpl);
	    virtual void Set(const IterImplBase& aImpl);
	    virtual void PostIncr();
	    virtual TBool IsEqual(const IterImplBase& aImpl) const;
	    virtual TBool IsCompatible(const IterImplBase& aImpl) const;
	    virtual Elem*  GetElem();
	    virtual void* DoGetObj(const char *aName);
	    virtual const void* DoGetObj(const char *aName) const;
	    public:
	    TIter iPIter;
	};

    public:
	static const char* Type() { return "Edge";};
	static string PEType();
	Edge(const string& aName, Elem* aMan, MEnv* aEnv);
	Edge(Elem* aMan, MEnv* aEnv);
	virtual ~Edge();
    public:
	const string& Point1u();
	const string& Point2u();
	const string& Pointu(Elem* aCp);
	Elem* Point1p();
	Elem* Point2p();
	Elem* Point1r();
	Elem* Point2r();
	Elem* Point1rc();
	Elem* Point2rc();
	Elem* Pointr(Elem* aCp);
	MVert* Point1v();
	MVert* Point2v();
	MVert* Pointv(Elem* aCp);
	TBool Connect(Elem* aCp);
	void Disconnect(Elem* aCp);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEdge
	virtual Base* EBase();
	virtual const Base* EBase() const;
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Pair(const MVert* aPoint);
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	virtual MVert* Ref1() const; // Proposed refs point1
	virtual MVert* Ref2() const; // Proposed refs point2
	// From MCompsObserver
	virtual TBool OnCompChanged(Elem& aComp);
	// From Elem
	//virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	//virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
    protected:
	// Just one-way relation to vert. It does't mean the full point to point relation is established.
	MVert* iPoint1;
	MVert* iPoint2;
};

#endif
