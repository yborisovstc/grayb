#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "mconn.h"
#include "vert.h"

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert, public MCompatChecker
{
    public:
	static const char* Type() { return "ConnPointBase";};
	ConnPointBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompatChecker
	virtual TBool IsCompatible(Elem* aPair);
};

// Extention agent. Redirects request for iface to internal CP of extention.
class ExtenderAgent: public Elem, public MCompatChecker
{
    public:
	static const char* Type() { return "ExtenderAgent";};
	ExtenderAgent(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompatChecker
	virtual TBool IsCompatible(Elem* aPair);
};

// Socket agent: redirects iface requests to pins
class ASocket: public Elem
{
    public:
	static const char* Type() { return "ASocket";};
	ASocket(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};


class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	Syst(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompsObserver
	virtual void DoOnCompChanged(Elem& aComp);
};

#endif
