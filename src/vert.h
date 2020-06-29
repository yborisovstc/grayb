#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "elem.h"
#include <set>

class Edge;

/**
 * @brief Vertex agent
 */
class Vert: public Elem, public MVert
{
    public:
	static const char* Type() { return "Vert";};
	static string PEType();
	Vert(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vert();
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	virtual TBool IsLinked(const MVert* aPair, TBool aDirect = EFalse) const override;
	//virtual set<MVert*>& Pairs();
	virtual MIface* MVert_DoGetObj(const char *aName);
	// From Elem
	virtual TBool OnCompChanged(MUnit& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual void OnCompDeleting(MUnit& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	// From MIface
	virtual MIface* MVert_Call(const string& aSpec, string& aRes);
	virtual string MVert_Mid() const { return Elem::MElem_Mid();}
    protected:
	void Disconnect();
    protected:
	// Cached pairs
	set<MVert*> iPairs;
};

/**
 * @brief Vertex unit
 * It doesn't support mutation so is compact enough for connection purpose
 */
class Vertu: public Unit, public MVert
{
    public:
	static const char* Type() { return "Vertu";};
	static string PEType();
	Vertu(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vertu();
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	virtual TBool IsLinked(const MVert* aPair, TBool aDirect = EFalse) const override;
	//virtual set<MVert*>& Pairs();
	virtual MIface* MVert_DoGetObj(const char *aName);
	// From Elem
	virtual TBool OnCompChanged(MUnit& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual void OnCompDeleting(MUnit& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	// From MIface
	virtual MIface* MVert_Call(const string& aSpec, string& aRes);
	string Mid() const override { return Unit::Mid();}
	virtual string MVert_Mid() const { return Unit::Mid();}
    protected:
	void Disconnect();
    protected:
	// Cached pairs
	set<MVert*> iPairs;
};


#endif
