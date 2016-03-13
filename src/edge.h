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
	MVert* Point1v();
	MVert* Point2v();
	MVert* Pointv(MElem* aCp);
	TBool Connect(MElem* aCp);
	void Disconnect(MElem* aCp);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MEdge
	virtual string EdgeName() const;
	virtual string EdgeUri() const;
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	virtual MVert* Ref1() const; // Proposed refs point1
	virtual MVert* Ref2() const; // Proposed refs point2
	// From MOwner
	virtual TBool OnCompChanged(MElem& aComp);
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

#endif
