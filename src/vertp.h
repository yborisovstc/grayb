#ifndef __GRAYB_VERTP_H
#define __GRAYB_VERTP_H

#include "mvertp.h"
#include "elem.h"
#include <set>

/**
 * @brief Vertex agent, PIA design
 */
class Vertp: public Elem, public MVertp
{
    public:
	static const char* Type() { return "Vertp";};
	static string PEType();
	Vertp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Vertp();
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MVertp
	virtual TBool Connect(MVertp* aPair, const string& aCp = string());
	virtual void Disconnect(MVertp* aPair);
	virtual TInt PairsCount() const;
	virtual MVertp* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVertp* aPair) const;
	virtual MIface* MVertp_DoGetObj(const char *aName);
	virtual void DumpCps() const;
	// From Elem
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	// From MIface
	virtual MIface* MVertp_Call(const string& aSpec, string& aRes);
	string MVertp_Mid() const override { return Elem::MElem_Mid();}
    protected:
	void RemovePairFromReg(MVertp* aPair);
    public:
	static const string KContent_Edges;
	static const string KContent_Connpoints;
	static const string KContent_P1;
	static const string KContent_P2;
	static const string KContent_CP1;
	static const string KContent_CP2;
    protected:
	TPairToCpReg mPairToCpReg;
	TCpToPairReg mCpToPairReg;
};

#endif
