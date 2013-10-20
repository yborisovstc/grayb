#ifndef __GRAYB_EDGE_H
#define __GRAYB_EDGE_H

#include "medge.h"
#include "elem.h"

// Edge base
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
	virtual ~Edge();
    public:
	const string& Point1u();
	const string& Point2u();
	void SetPoints(MVert* aPoint1, MVert* aPoint2);
	void SetPoint1(MVert* aPoint);
	void SetPoint2(MVert* aPoint);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEdge
	virtual Base* EBase();
	virtual const Base* EBase() const;
	virtual TBool Connect();
	virtual void Disconnect();
	virtual MVert* Pair(const MVert* aPoint);
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	// From Elem
	virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
    protected:
	static void Init();
    protected:
	// Sign of inited
	static bool iInit;
	MVert* iPoint1;
	MVert* iPoint2;
};

#endif
