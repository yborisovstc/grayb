#ifndef __GRAYB_SYST_H
#define __GRAYB_SYST_H

#include "vert.h"

// Caplule
class ACapsule: public Elem
{
    public:
	static const char* Type() { return "ACapsule";};
	static string PEType();
	ACapsule(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MOwner
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual TBool OnChanged(const MUnit* aComp);
};

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr
class ConnPointBase: public Vert, public MConnPoint_Imd, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ConnPointBase";};
	static string PEType();
	ConnPointBase(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
	// From MConnPoint MIface
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes);
	virtual string MConnPoint_Mid() const;
};

// Base of mutli-content ConnPoint 
class ConnPointMc: public Vert, public MConnPoint_Imd, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ConnPointMc";};
	static string PEType();
	ConnPointMc(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MUnit
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
	// From MConnPoint MIface
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes);
	virtual string MConnPoint_Mid() const;
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

/** @brief Base of mutli-content ConnPoint unit
 * */
class ConnPointMcu: public Vertu, public MConnPoint_Imd, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ConnPointMcu";};
	static string PEType();
	ConnPointMcu(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// Iface provider
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From Base
	virtual MIface *DoGetObj(const char *aName) override;
	// From MUnit
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
	// From MConnPoint MIface
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes);
	virtual string MConnPoint_Mid() const;
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

	inline string ConnPointMcu::GetProvided() const { return GetContent(KContName_Provided);};
	inline string ConnPointMcu::GetRequired() const { return GetContent(KContName_Required);};

// Input ConnPoint base
class ConnPointBaseInp: public ConnPointBase
{
    public:
	static const char* Type() { return "ConnPointBaseInp";};
	static string PEType();
	ConnPointBaseInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MCompatChecker
	virtual TDir GetDir() const;
};

// Output ConnPoint base
class ConnPointBaseOut: public ConnPointBase
{
    public:
	static const char* Type() { return "ConnPointBaseOut";};
	static string PEType();
	ConnPointBaseOut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MCompatChecker
	virtual TDir GetDir() const;
};

// Extention agent. Redirects request for iface to internal CP of extention.
class ExtenderAgent: public Elem, public MAgent
{
    public:
	class CompatChecker: public MCompatChecker { public:
	    ExtenderAgent& mHost;
	    CompatChecker(ExtenderAgent& aHost): mHost(aHost) {}
	    // From MCompatChecker MIface
	    virtual MIface* Call(const string& aSpec, string& aRes) override { return mHost.MCompatChecker_Call(aSpec, aRes);}
	    virtual string Mid() const override { return mHost.MUnit::Mid();}
	    virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse) override { return mHost.IsCompatible(aPair, aExt);}
	    virtual MUnit* GetExtd() override { return mHost.GetExtd();}
	    virtual TDir GetDir() const override {return mHost.GetDir();}

	};
    public:
	static const char* Type() { return "ExtenderAgent";};
	static string PEType();
	ExtenderAgent(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual MCompatChecker::TDir GetDir() const;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From Elem
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MIfaceProv
	virtual MIface* MAgent_DoGetIface(const string& aUid);
    protected:
	CompatChecker mCompatChecker;
	inline MUnit* Host() const { return iMan;}
};

// Input Extender agent
class ExtenderAgentInp: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentInp";};
	static string PEType();
	ExtenderAgentInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual MCompatChecker::TDir GetDir() const;
};

// Output Extender agent
class ExtenderAgentOut: public ExtenderAgent
{
    public:
	static const char* Type() { return "ExtenderAgentOut";};
	static string PEType();
	ExtenderAgentOut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual MCompatChecker::TDir GetDir() const;
};

// Extention agent (multicontent). Redirects request for iface to internal CP of extention.
class AExtender: public Elem, public MCompatChecker, public MAgent
{
    public:
	static const char* Type() { return "AExtender";};
	static string PEType();
	AExtender(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From Elem
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    protected:
	inline MUnit* Host() const { return iMan;}
};

/**
 * @brief Extention agent (monolitic, multicontent). Redirects request for iface to internal CP of extention.
 */
class AExtd: public Vert, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "AExtd";};
	static string PEType();
	AExtd(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From Vert
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
    protected:
	inline MUnit* Host() const { return iMan;}
};

/**
 * @brief Extention agent (monolitic, multicontent, input). Redirects request for iface to internal CP of extention.
 */
class AExtdInp: public AExtd
{
    public:
	static const char* Type() { return "AExtdInp";};
	static string PEType();
	AExtdInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

/**
 * @brief Extention agent (monolitic, multicontent, output). Redirects request for iface to internal CP of extention.
 */
class AExtdOut: public AExtd
{
    public:
	static const char* Type() { return "AExtdOut";};
	static string PEType();
	AExtdOut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};


/**
 * @brief Extention agent, monolitic, multicontent, unit. Redirects request for iface to internal CP of extention.
 */
class AExtdu: public Vertu, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "AExtdu";};
	static string PEType();
	AExtdu(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	// From Vert
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	// From MCompatChecker MIface
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
    protected:
	inline MUnit* Host() const { return iMan;}
};

/**
 * @brief Extention agent, monolitic, multicontent, unit, input. Redirects request for iface to internal CP of extention.
 */
class AExtduInp: public AExtdu
{
    public:
	static const char* Type() { return "AExtduInp";};
	static string PEType();
	AExtduInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

/**
 * @brief Extention agent, monolitic, multicontent, unit, output. Redirects request for iface to internal CP of extention.
 */
class AExtduOut: public AExtdu
{
    public:
	static const char* Type() { return "AExtduOut";};
	static string PEType();
	AExtduOut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};


// Iface stub to avoid clashing MIface methods
class MSocket_Imd: public MSocket
{
    virtual MIface* MSocket_Call(const string& aSpec, string& aRes) = 0;
    virtual string MSocket_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MSocket_Call(aSpec, aRes);};
    virtual string Mid() const { return MSocket_Mid();};
};

// Socket agent: redirects iface requests to pins
class ASocket: public Elem, public MCompatChecker_Imd, public MSocket_Imd, public MAgent
{
    public:
	static const char* Type() { return "ASocket";};
	static string PEType();
	ASocket(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From Elem
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MSocket
	virtual TInt PinsCount() const;
	virtual MUnit* GetPin(TInt aInd);
	MUnit* GetPin(const TICacheRCtx& aCtx) override;
	virtual MIface* MSocket_Call(const string& aSpec, string& aRes);
	virtual string MSocket_Mid() const;
	// From MAgent
	virtual MIface* MAgent_DoGetIface(const string& aUid);
    protected:
	inline MUnit* Host() const;
};

MUnit* ASocket::Host() const { return (iMan == NULL) ? NULL : iMan->GetMan(); };

// Input Socket agent
class ASocketInp: public ASocket
{
    public:
	static const char* Type() { return "ASocketInp";};
	static string PEType();
	ASocketInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual TDir GetDir() const;
};

// Output Socket agent
class ASocketOut: public ASocket
{
    public:
	static const char* Type() { return "ASocketOut";};
	static string PEType();
	ASocketOut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual TDir GetDir() const;
};

// Socket agent, multicontent: redirects iface requests to pins
class ASocketMc: public ASocket
{
    public:
	static const char* Type() { return "ASocketMc";};
	static string PEType();
	ASocketMc(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MCompatChecker
	virtual TDir GetDir() const;
};

// Socket agent, multicontent: redirects iface requests to pins, monolitic
class ASocketMcm: public Vert, public MCompatChecker_Imd, public MSocket_Imd
{
    public:
	static const char* Type() { return "ASocketMcm";};
	static string PEType();
	ASocketMcm(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From Elem
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MSocket
	virtual TInt PinsCount() const;
	virtual MUnit* GetPin(TInt aInd);
	MUnit* GetPin(const TICacheRCtx& aCtx) override;
	virtual MIface* MSocket_Call(const string& aSpec, string& aRes);
	virtual string MSocket_Mid() const;
};

// Socket agent, input, multicontent: redirects iface requests to pins, monolitic
class ASocketInpMcm: public ASocketMcm
{
    public:
	static const char* Type() { return "ASocketInpMcm";};
	static string PEType();
	ASocketInpMcm(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

// Socket agent, input, multicontent: redirects iface requests to pins, monolitic
class ASocketOutMcm: public ASocketMcm
{
    public:
	static const char* Type() { return "ASocketOutMcm";};
	static string PEType();
	ASocketOutMcm(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

// System: relates to others via proxies - ConnPoints, that specialized relation with roles
class Syst: public Vert
{
    public:
	static const char* Type() { return "Syst";};
	static string PEType();
	Syst(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MOwner
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
	// From Elem
	virtual void DoSpecificMut(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx) override;
    protected:
	void Connect(const string& argP, const string& argQ, const TNs& aNs);
	void Disconnect(const string& argP, const string& argQ, const TNs& aNs);
    protected:
	TBool IsPtOk(MUnit* aPt);
};

#endif
