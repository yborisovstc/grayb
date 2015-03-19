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
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEdge
	virtual Base* EBase();
	virtual const Base* EBase() const;
	virtual TBool Connect();
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual TBool Connect(Elem* aCp);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual void Disconnect(Elem* aCp);
	virtual MVert* Pair(const MVert* aPoint);
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	// From Elem
	//virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	//virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
    protected:
	// Sign of inited
	static bool iInit;
	// Just one-way relation to vert. It does't mean the full point to point relation is established.
	MVert* iPoint1;
	MVert* iPoint2;
};

// Edge agent using named content to keep the refs to conn points
class Aedge: public Elem, public MEdge
{
    protected:
	enum {
	    ECnt_P1 = 0,
	    ECnt_P2 = 1,
	    ECnt_Num_
	};
    public:
	static const char* Type() { return "Aedge";};
	static string PEType();
	Aedge(const string& aName, Elem* aMan, MEnv* aEnv);
	Aedge(Elem* aMan, MEnv* aEnv);
	virtual ~Aedge();
    public:
	const string& Point1u();
	const string& Point2u();
	const string& Pointu(const string& aPname);
	Elem* Point1r();
	Elem* Point2r();
	Elem* Point1rc();
	Elem* Point2rc();
	Elem* Pointr(const string& aPname);
	MVert* Point1v();
	MVert* Point2v();
	MVert* Pointv(const string& aPname);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MEdge
	virtual Base* EBase();
	virtual const Base* EBase() const;
	virtual TBool Connect();
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Pair(const MVert* aPoint);
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	// From Elem
	virtual void GetCont(string& aCont, const string& aName = string()); 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	virtual TInt GetContCount() const {return ECnt_Num_;};
    public:
	static const string& mP1ContName;
	static const string& mP2ContName;
    protected:
	static const TInt mContNum;
	string mPoint1Uri;
	string mPoint2Uri;
	MVert* iPoint1;
	MVert* iPoint2;
};
#endif
