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
	Edge(const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual ~Edge();
    public:
	const string& Point1u();
	const string& Point2u();
	const string& Pointu(MUnit* aCp);
	MUnit* Point1p();
	MUnit* Point2p();
	MUnit* Point1r();
	MUnit* Point2r();
	MUnit* Point1rc();
	MUnit* Point2rc();
	MUnit* Pointr(MUnit* aCp);
	MVert* Pointv(MUnit* aCp);
	MVert* Point1v();
	MVert* Point2v();
	TBool Connect(MUnit* aCp);
	void Disconnect(MUnit* aCp);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
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
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
	// From Elem
	virtual void SetRemoved(TBool aModif);
	// From MIface
	virtual MIface* MEdge_Call(const string& aSpec, string& aRes);
	string MEdge_Mid() const override { return Elem::MElem_Mid();}
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
	Aedge(const string& aName, MUnit* aMan, MEnv* aEnv);
	virtual ~Aedge();
    public:
	const string& Point1u() const;
	const string& Point2u() const;
	// From Base
	virtual MIface *DoGetObj(const char *aName);
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
	virtual MIface* MEdge_Call(const string& aSpec, string& aRes);
	string MEdge_Mid() const override { return Elem::MElem_Mid();}
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
