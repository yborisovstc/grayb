#ifndef __GRAYB_VERTP_H
#define __GRAYB_VERTP_H

#include "mvert.h"
#include "elem.h"
#include <set>

/**
 * @brief Vertex agent, PIA design
 */
class Vertp: public Elem, public MVert
{
    public:
	// Pairs registry: {pair pointer, context (URI) id}
	typedef pair<MVert*, string> TPairsRegElem;
	typedef map<MVert*, string> TPairsReg;

    public:
	static const char* Type() { return "Vertp";};
	static string PEType();
	Vertp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Vertp(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vertp();
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	virtual MIface* MVert_DoGetObj(const char *aName);
	// From Elem
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return Elem::Mid();}
	string MVert_Mid() const override { return Elem::Mid();}
    protected:
	TPairsReg mPairs;
	static const string& KContent_Edges;
	static const string& KContent_Connpoints;
	static const string& KContent_P1;
	static const string& KContent_P2;
	static const string& KContent_CP1;
	static const string& KContent_CP2;
};

#endif
