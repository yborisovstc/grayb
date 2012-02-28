#ifndef __GRAYB_EDGE_H
#define __GRAYB_EDGE_H

#include "medge.h"
#include "elem.h"

// Edge base
class Edge: public Elem, public MEdge
{
    public:
	static const char* Type() { return "Edge";};
	Edge(const string& aName, Elem* aMan, MEnv* aEnv);
    public:
	const string& Point1u();
	const string& Point2u();
	void SetPoints(MVert* aPoint1, MVert* aPoint2);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MEdge
	virtual TBool Connect();
	virtual void Disconnect();
	virtual MVert* Pair(const MVert* aPoint);
	// From Elem
    protected:
	static void Init();
    protected:
	// Sign of inited
	static bool iInit;
	MVert* iPoint1;
	MVert* iPoint2;
};

#endif
