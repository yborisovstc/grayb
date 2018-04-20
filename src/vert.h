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
	virtual MIface* DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	//virtual set<MVert*>& Pairs();
	virtual MIface* MVert_DoGetObj(const char *aName);
	// From Elem
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return Elem::Mid();}
    protected:
	void Disconnect();
    protected:
	// Cached pairs
	set<MVert*> iPairs;
};

#endif
