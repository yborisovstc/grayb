#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "vert.h"

// Caplule
class ACapsule: public Elem
{
    public:
	static const char* Type() { return "ACapsule";};
	static string PEType();
	ACapsule(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ACapsule(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MOwner
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string());
};

// Introducint iface extender to avoid clashing MIface methods
class MCompatChecker_Imd: public MCompatChecker
{
    virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes) = 0;
    virtual string MCompatChecker_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MCompatChecker_Call(aSpec, aRes);};
    virtual string Mid() const { return MCompatChecker_Mid();};
};

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ConnPointBase";};
	static string PEType();
	ConnPointBase(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBase(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
//	virtual Elem* GetAssoc(RqContext* aCtx);
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	//virtual MIface* Call(const string& aSpec, string& aRes);
	//virtual string Mid() const;
};

// Base of mutli-content ConnPoint 
class ConnPointMc: public Vert, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ConnPointMc";};
	static string PEType();
	ConnPointMc(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointMc(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
    protected:
	string GetProvided() const;
	string GetRequired() const;
    public:
	static const string KContName_Provided;
	static const string KContName_Required;
	static const string KContDir;
	static const string KContDir_Val_Regular;
	static const string KContDir_Val_Inp;
	static const string KContDir_Val_Out;
};

	inline string ConnPointMc::GetProvided() const { return GetContent(KContName_Provided);};
	inline string ConnPointMc::GetRequired() const { return GetContent(KContName_Required);};

// Input ConnPoint base
class ConnPointBaseInp: public ConnPointBase
{
    public:
	static const char* Type() { return "ConnPointBaseInp";};
	static string PEType();
	ConnPointBaseInp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBaseInp(MElem* aMan = NULL, MEnv* aEnv = NULL);
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
	ConnPointBaseOut(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ConnPointBaseOut(MElem* aMan = NULL, MEnv* aEnv = NULL);
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
	ExtenderAgent(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgent(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
};

// Input Extender agent
class ExtenderAgentInp: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentInp";};
	static string PEType();
	ExtenderAgentInp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgentInp(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// Output Extender agent
class ExtenderAgentOut: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentOut";};
	static string PEType();
	ExtenderAgentOut(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ExtenderAgentOut(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// Extention agent (multicontent). Redirects request for iface to internal CP of extention.
class AExtender: public Elem, public MCompatChecker
{
    public:
	static const char* Type() { return "AExtender";};
	static string PEType();
	AExtender(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	AExtender(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
};


// Socket agent: redirects iface requests to pins
class ASocket: public Elem, public MCompatChecker
{
    public:
	static const char* Type() { return "ASocket";};
	static string PEType();
	ASocket(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ASocket(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// Get pin existing in context
	MElem* GetPin(const RqContext* aCtx);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
};

// Input Socket agent
class ASocketInp: public ASocket
{
    public:
	static const char* Type() { return "ASocketInp";};
	static string PEType();
	ASocketInp(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ASocketInp(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// Output Socket agent
class ASocketOut: public ASocket
{
    public:
	static const char* Type() { return "ASocketOut";};
	static string PEType();
	ASocketOut(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	ASocketOut(MElem* aMan = NULL, MEnv* aEnv = NULL);
	virtual void *DoGetObj(const char *aName);
	virtual TDir GetDir() const;
};

// System: relates to others via proxies - ConnPoints, that specialized relation with roles
class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	static string PEType();
	Syst(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Syst(MElem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MOwner
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string());
    protected:
	TBool IsPtOk(MElem* aPt);
};

#endif
