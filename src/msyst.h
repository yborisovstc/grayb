#ifndef __GRAYB_MSYSTP_H
#define __GRAYB_MSYSTP_H

#include "miface.h"
#include "mvertp.h"

/** Iface of system. System exposes its behaviour via connection points (CP)
 * 
 */
class MSyst: public MIfaceStub
{
    public:
	static const char* Type() { return "MSyst";}
    public:
	virtual MVertp::TPairsEr GetPairsForCp(const string& aCp) = 0;
	virtual MVertp::TCpsEr   GetCpsForPair(MVertp* aPair) = 0;
	/** Getting interface via external connection point
	 * @param aName - name of interface
	 * @param aCp - UID of connection point
	 * @param aCtx - context of the request
	*/
	virtual TIfRange GetIfiCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx = TICacheRCtx()) = 0;
	/** Getting interface from inside of system via connection point
	 * It is used by embedded agents
	*/
	//virtual TIfRange GetIfiForCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx = TICacheRCtx()) = 0;
};

#endif


