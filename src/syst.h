#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "mconn.h"
#include "vert.h"

// Caplule
class ACapsule: public Elem
{
    public:
	static const char* Type() { return "ACapsule";};
	static string PEType();
	ACapsule(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ACapsule(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MOwner
	virtual TBool OnCompChanged(MElem& aComp);
};


// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert, public MCompatChecker
{
    public:
	static const char* Type() { return "ConnPointBase";};
	static string PEType();
	ConnPointBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
	virtual TDir GetDir() const;
	virtual void GetMajorIdep(TMDep& aDep);
//	virtual Elem* GetAssoc(RqContext* aCtx);
};

// Input ConnPoint base
class ConnPointBaseInp: public ConnPointBase
{
    public:
	static const char* Type() { return "ConnPointBaseInp";};
	static string PEType();
	ConnPointBaseInp(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBaseInp(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TDir GetDir() const;
};

// Output ConnPoint base
class ConnPointBaseOut: public ConnPointBase
{
    public:
	static const char* Type() { return "ConnPointBaseOut";};
	static string PEType();
	ConnPointBaseOut(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBaseOut(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TDir GetDir() const;
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
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
	virtual TDir GetDir() const;
	virtual void GetMajorIdep(TMDep& aDep);
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
};

// Input Extender agent
class ExtenderAgentInp: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentInp";};
	static string PEType();
	ExtenderAgentInp(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgentInp(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// Output Extender agent
class ExtenderAgentOut: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentOut";};
	static string PEType();
	ExtenderAgentOut(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgentOut(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
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
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual Elem* GetExtd();
	virtual TDir GetDir() const;
	virtual void GetMajorIdep(TMDep& aDep);
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
};

// Input Socket agent
class ASocketInp: public ASocket
{
    public:
	static const char* Type() { return "ASocketInp";};
	static string PEType();
	ASocketInp(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ASocketInp(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// Output Socket agent
class ASocketOut: public ASocket
{
    public:
	static const char* Type() { return "ASocketOut";};
	static string PEType();
	ASocketOut(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ASocketOut(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
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
	virtual void *DoGetObj(const char *aName);
	// From MOwner
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual TBool OnCompChanged(MElem& aComp);
	// Gets major dep for referenced node, ref ds_indp_mutord_impl
	virtual void GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef);
    protected:
	TBool IsPtOk(Elem* aPt);
};

#endif
