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
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	//virtual set<MVert*>& Pairs();
	virtual MIface* MVert_DoGetObj(const char *aName);
	// From Elem
	virtual TBool OnCompChanged(MElem& aComp);
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual void SetRemoved();
	// Iface cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
    protected:
	// Cached pairs
	set<MVert*> iPairs;
};

#endif
