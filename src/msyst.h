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
};

#endif


