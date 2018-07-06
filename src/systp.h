#ifndef __GRAYB_SYSTP_H
#define __GRAYB_SYSTP_H

#include "vertp.h"

/**
 * @brief System agent, PIA design
 */
class Systp: public Vertp
{
    public:
	static const char* Type() { return "Systp";};
	static string PEType();
	Systp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Systp(MElem* aMan = NULL, MEnv* aEnv = NULL);
	bool AreCpsCompatible(const GUri& aCp1, const GUri& aCp2);
	// From MOwner
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	// From Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
    protected:
	void UpdateIfiForConnPoint(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId);
	bool GetExtended(MElem* aNode, const string& aExt, string& aInt, bool& aInv);
};

#endif
