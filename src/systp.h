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
	Systp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	bool AreCpsCompatible(const GUri& aCp1, const GUri& aCp2);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MOwner
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	// From Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MSyst
	MVertp::TPairsEr GetPairsForCp(const string& aCp) override;
	TCpsEr   GetCpsForPair(MVertp* aPair) override;
	TIfRange GetIfiForCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx = TICacheRCtx()) override;
    protected:
	void UpdateIfiForConnPoint(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	void UpdateIfiForExtender(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	bool GetExtended(MElem* aNode, const string& aExt, string& aInt, bool& aInv);
};

#endif
