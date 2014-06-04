#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "mconn.h"
#include "vert.h"

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert, public MCompatChecker
{
    public:
	static const char* Type() { return "ConnPointBase";};
	static string PEType();
	ConnPointBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompatChecker
	virtual TBool IsCompatible(Elem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
//	virtual Elem* GetAssoc(RqContext* aCtx);
};

// Extention agent. Redirects request for iface to internal CP of extention.
class ExtenderAgent: public Elem, public MCompatChecker
{
    public:
	static const char* Type() { return "ExtenderAgent";};
	static string PEType();
	ExtenderAgent(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgent(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompatChecker
	virtual TBool IsCompatible(Elem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
};

// Socket agent: redirects iface requests to pins
class ASocket: public Elem, public MCompatChecker
{
    public:
	static const char* Type() { return "ASocket";};
	static string PEType();
	ASocket(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ASocket(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// Get pin existing in context
	Elem* GetPin(const RqContext* aCtx);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompatChecker
	virtual TBool IsCompatible(Elem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
};

// System: relates to others via proxies - ConnPoints, that specialized relation with roles
class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	static string PEType();
	Syst(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Syst(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void DoOnCompChanged(Elem& aComp);
};

#endif
