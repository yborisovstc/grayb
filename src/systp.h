#ifndef __GRAYB_SYSTP_H
#define __GRAYB_SYSTP_H

#include "vertp.h"
#include "msyst.h"

/**
 * @brief System agent, PIA design
 */
class Systp: public Vertp, public MSyst
{
    public:
	static const char* Type() { return "Systp";};
	static string PEType();
	Systp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	bool AreCpsCompatible(MUnit* aNode1, const string& aCp1, MUnit* aNode2, const string& aCp2);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MOwner
	virtual void OnCompDeleting(MUnit& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MUnit& aComp, const string& aContName = string(), TBool aModif = EFalse);
	// From Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MSyst
	MVertp::TPairsEr GetPairsForCp(const string& aCp) override;
	TCpsEr   GetCpsForPair(MVertp* aPair) override;
	TIfRange GetIfiCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx = TICacheRCtx()) override;
    protected:
	void UpdateIfiForConnPoint(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	void UpdateIfiForExtender(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	void UpdateIfiForSocket(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	bool GetExtended(MUnit* aNode, const string& aExt, string& aInt, bool& aInv);
};

#endif
