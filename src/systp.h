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
	// From MOwner
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
};

#endif
