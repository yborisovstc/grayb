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
	static const char* Type() { return "Edge";};
	static string PEType();
	Edge(const string& aName, MElem* aMan, MEnv* aEnv);
	Edge(MElem* aMan, MEnv* aEnv);
	virtual ~Edge();
    public:
	const string& Point1u();
	const string& Point2u();
	const string& Pointu(MElem* aCp);
	MElem* Point1p();
	MElem* Point2p();
	MElem* Point1r();
	MElem* Point2r();
	MElem* Point1rc();
	MElem* Point2rc();
	MElem* Pointr(MElem* aCp);
	MVert* Pointv(MElem* aCp);
	MVert* Point1v();
	MVert* Point2v();
	TBool Connect(MElem* aCp);
	void Disconnect(MElem* aCp);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MEdge
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	virtual MVert* Ref1() const; // Proposed refs point1
	virtual MVert* Ref2() const; // Proposed refs point2
	virtual void SetPoint1(const string& aRef);
	virtual void SetPoint2(const string& aRef);
	// From MOwner
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string());
	// From Elem
	virtual void SetRemoved();
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
    protected:
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
	Aedge(const string& aName, MElem* aMan, MEnv* aEnv);
	Aedge(MElem* aMan, MEnv* aEnv);
	virtual ~Aedge();
    public:
	const string& Point1u() const;
	const string& Point2u() const;
	// From Base
	virtual void *DoGetObj(const char *aName);
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
	virtual void SetPoint1(const string& aRef);
	virtual void SetPoint2(const string& aRef);
	// From Elem
	//virtual void GetCont(string& aCont, const string& aName = string()) const; 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	virtual TInt GetContCount(const string& aName=string()) const {return ECnt_Num_;};
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
    public:
	static const string& mP1ContName;
	static const string& mP2ContName;
    protected:
	string mPoint1Uri;
	string mPoint2Uri;
	MVert* iPoint1;
	MVert* iPoint2;
};

#endif
