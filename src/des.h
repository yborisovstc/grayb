#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include "elem.h"
#include "mdata.h"
#include "mdes.h"
#include "mlauncher.h"
#include "func.h"
#include "vert.h"
#include "syst.h"

// Transition function agent base. 
class ATrBase: public Elem, public MACompsObserver, public MAgent
{
    public:
	static const char* Type() { return "ATrBase";};
	static string PEType();
	ATrBase(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit* aContext, MUnit* aComp, const string& aContName = string());
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
};

// Agent base of Int transition function
class ATrInt: public ATrBase, public MDIntGet
{
    public:
	static const char* Type() { return "ATrInt";};
	static string PEType();
	ATrInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
    protected:
	MDIntGet* GetInp(const string& aInpName);
	TIfRange GetInpRg(const string& aInpName);
    protected:
	TInt mData;
};


// Agent function "Increment of Int data"
class ATrIncInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrIncInt";};
	static string PEType();
	ATrIncInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Subtraction of Int data"
class ATrSubInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrSubInt";};
	static string PEType();
	ATrSubInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Multiplying of Int data"
class ATrMplInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrMplInt";};
	static string PEType();
	ATrMplInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Deviding of Int data"
class ATrDivInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrDivInt";};
	static string PEType();
	ATrDivInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDIntGet
	virtual TInt Value();
};


// Agent base of Var transition function
class ATrVar: public ATrBase, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "ATrVar";};
	static string PEType();
	ATrVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	// From Func::Host
	virtual TIfRange GetInps(TInt aId, TBool aOpt = EFalse);
	virtual void OnFuncContentChanged();
	virtual void LogWrite(TLogRecCtg aCtg, const char* aFmt,...);
	virtual Unit* GetAgent() override {return this;};
	virtual TInt GetInpCpsCount() const {return 0;};
	// From Elem
	virtual TBool GetCont(string& aCont, const string& aName=string()) const; 
    protected:
	virtual void Init(const string& aIfaceName) {};
	virtual string GetInpUri(TInt aId) const;
    protected:
	Func* mFunc;
};


// Agent function "Addition of Var data"
class ATrAddVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrAddVar";};
	static string PEType();
	ATrAddVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 2;};
};

// Agent function "Multiplication of Var data"
class ATrMplVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrMplVar";};
	static string PEType();
	ATrMplVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 1;};
};

// Agent function "Division of Var data"
class ATrDivVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrDivVar";};
	static string PEType();
	ATrDivVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 2;};
};

// Agent function "Switch controlled by var data"
class ATrSwitchVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrSwitchVar";};
	static string PEType();
	ATrSwitchVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 2;};
};

// Agent function "Switch controlled by var data"
class ATrAtVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrAtVar";};
	static string PEType();
	ATrAtVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 2;};
};

// Agent function "Composing of vector, var data"
class ATrCpsVectVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrCpsVectVar";};
	static string PEType();
	ATrCpsVectVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 0;};
};

// Agent function "Boolean comparition of Var data"
class ATrBcmpVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrBcmpVar";};
	static string PEType();
	ATrBcmpVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDVarGet
	virtual string VarGetIfid();
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId) const;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 2;};
    protected:
	FBcmpBase::TFType GetFType();
};


/** @brief Transition function agent base with support of 'combined chain' approach
 *
 * Ref ds_mae_scc for combined chain design
 * TODO To consider another solution: to implement ConnPoint iface directly by trans
 * as it is done for AStatec. In that case out CP is not needed.
 * */ 
class ATrcBase: public Vertu, public MConnPoint_Imd, public MCompatChecker_Imd
{
    public:
	static const char* Type() { return "ATrcBase";}
	static string PEType();
	ATrcBase(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From Unit
	virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MConnPoint
	virtual TBool IsRequired(const string& aIfName) const override;
	virtual string Required() const override;
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes) override;
	virtual string MConnPoint_Mid() const override;
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse) override;
	virtual MUnit* GetExtd() override;
	virtual TDir GetDir() const override;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes) override;
	virtual string MCompatChecker_Mid() const override;
};


// Agent base of Var transition function
class ATrcVar: public ATrcBase, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "ATrcVar";};
	static string PEType();
	ATrcVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const override;
	virtual string Provided() const override;
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	// From Func::Host
	virtual TIfRange GetInps(TInt aId, TBool aOpt = EFalse) override;
	virtual void OnFuncContentChanged() override;
	virtual void LogWrite(TLogRecCtg aCtg, const char* aFmt,...) override;
	virtual Unit* GetAgent() override {return this;}
	virtual TInt GetInpCpsCount() const {return 0;}
	// From Elem
	virtual TBool GetCont(string& aCont, const string& aName=string()) const; 
    protected:
	virtual void Init(const string& aIfaceName) {};
	virtual string GetInpUri(TInt aId) const;
	/** @brief Gets value of MDVarGet MDtGet input */
	template<typename T> bool DtGetInp(T& aData, const string& aInpName);
	/** @brief Gets value of MDVarGet MDtGet Sdata<T> input */
	template<typename T> bool DtGetSdataInp(T& aData, const string& aInpName);
    protected:
	Func* mFunc;
};

template<typename T> bool ATrcVar::DtGetInp(T& aData, const string& aInpName)
{
    bool res = false;
    MDVarGet* vg = dynamic_cast<MDVarGet*>(GetSIfi("./" + aInpName, MDVarGet::Type()));
    if (vg) {
	MDtGet<T>* dg = vg->GetDObj(dg);
	if (dg) {
	    dg->DtGet(aData);
	    res = true;
	}
    }
    return res;
}

template<typename T> bool ATrcVar::DtGetSdataInp(T& aData, const string& aInpName)
{
    Sdata<T> sd;
    bool res = DtGetInp(sd, aInpName);
    aData = sd.mData;
    return res;
}


/** @brief Agent function "Addition of Var data"
 * */
class ATrcAddVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcAddVar";};
	static string PEType();
	ATrcAddVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 2;}
};


/** @brief Agent function "Multiplication of Var data"
 * */
class ATrcMplVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcMplVar";};
	static string PEType();
	ATrcMplVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 1;};
};

/** @brief Agent function "Boolean AND"
 * */
class ATrcAndVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcAndVar";};
	static string PEType();
	ATrcAndVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 1;};
};

/** @brief Agent function "Boolean negation"
 * */
class ATrcNegVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcNegVar";};
	static string PEType();
	ATrcNegVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const {return 1;};
};

/** @brief Agent function "Max of Var data"
 * */
class ATrcMaxVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcMaxVar";};
	static string PEType();
	ATrcMaxVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 1;}
};





// Iface stub to avoid clashing MIface methods
class MDesObserver_Imd: public MDesObserver
{
    virtual MIface* MDesObserver_Call(const string& aSpec, string& aRes) = 0;
    virtual string MDesObserver_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MDesObserver_Call(aSpec, aRes);};
    virtual string Mid() const { return MDesObserver_Mid();};
};

// Iface stub to avoid clashing MIface methods
#if 0
class MDesInpObserver_Imd: public MDesInpObserver
{
    virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes) = 0;
    virtual string MDesInpObserver_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MDesInpObserver_Call(aSpec, aRes);};
    virtual string Mid() const { return MDesInpObserver_Mid();};
};
#endif

// State base agent
class StateAgent: public Elem, public MDesSyncable_Imd, public MDesInpObserver_Imd, public MAgent
{
    public:
	static const char* Type() { return "StateAgent";};
	static string PEType();
	StateAgent(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes);
	virtual string MDesSyncable_Mid() const;
	virtual void DumpActive() override {}
	virtual void DumpUpdated() override {}
	// From MDesInpObserver
	virtual void OnInpUpdated();
	virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesInpObserver_Mid() const;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    private:
	TBool iActive;
	TBool iUpdated;
};

class BdVar;

/** @brief State agent, unit, monolitic, using host unit base organs
 *
 * Ref ds_uac for unit based orgars, ds_mae for monolitic agents, ds_mae_scm for this agent
 * */
class AState: public Vertu, public MConnPoint_Imd, public MCompatChecker_Imd, public MDesSyncable_Imd, public MDesInpObserver_Imd,
    public MAgent, public MBdVarHost
{
    public:
	static const char* Type() { return "AState";};
	static string PEType();
	AState(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes);
	virtual string MDesSyncable_Mid() const;
	virtual void DumpActive() override {}
	virtual void DumpUpdated() override {}
	// From MDesInpObserver
	virtual void OnInpUpdated();
	virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesInpObserver_Mid() const;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
	// From MConnPoint MIface
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes);
	virtual string MConnPoint_Mid() const;
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From MBdVarHost
	virtual MDVarGet* HGetInp(const Base* aRmt) override;
	virtual void HOnDataChanged(const Base* aRmt) override;

	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
    protected:
	TBool IsLogeventUpdate();
	// From MUnit
	virtual TEhr ProcessCompChanged(const MUnit* aComp, const string& aContName) override;
    private:
	TBool iActive;
	TBool iUpdated;
	BdVar* mPdata;   //<! Preparing (updating) phase data
	BdVar* mCdata;   //<! Confirming phase data
	static const string KContVal; //<! Content Value  name
};

/** @brief Connection point - input of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStatecInp: public ConnPointMcu
{
    public:
	static const char* Type() { return "CpStatecInp";};
	static string PEType();
	CpStatecInp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

/** @brief Connection point - output of combined chain state AStatec
 * Just ConnPointMcu with pre-configured prepared/required
 * */
class CpStatecOutp: public ConnPointMcu
{
    public:
	static const char* Type() { return "CpStatecOutp";};
	static string PEType();
	CpStatecOutp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};


/** @brief State agent, unit, monolitic, using host unit base organs, combined chain
 *
 * Ref ds_uac for unit based orgars, ds_mae for monolitic agents, ds_mae_scc for combined chain design
 * TODO It is not obvious solution to expose MDVarSet iface, but I don't find more proper solution ATM. To redesign?
 * */
class AStatec: public Vertu, public MConnPoint_Imd, public MCompatChecker_Imd, public MDesSyncable_Imd, public MDesInpObserver_Imd,
     public MBdVarHost, public MDVarSet
{
    public:
	static const char* Type() { return "AStatec";};
	static string PEType();
	AStatec(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From Unit
	//virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes);
	virtual string MDesSyncable_Mid() const;
	virtual void DumpActive() override {}
	virtual void DumpUpdated() override {}
	// From MDesInpObserver
	virtual void OnInpUpdated();
	virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesInpObserver_Mid() const;
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
	// From MConnPoint MIface
	virtual MIface* MConnPoint_Call(const string& aSpec, string& aRes);
	virtual string MConnPoint_Mid() const;
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse);
	virtual MUnit* GetExtd();
	virtual TDir GetDir() const;
	virtual MIface* MCompatChecker_Call(const string& aSpec, string& aRes);
	virtual string MCompatChecker_Mid() const;
	// From MBdVarHost
	virtual MDVarGet* HGetInp(const Base* aRmt) override;
	virtual void HOnDataChanged(const Base* aRmt) override;
	virtual string GetUid() { return GetUri(NULL, ETrue);}
	// From MDVarSet
	virtual string MDVarGet_Mid() const {return GetUri(NULL, ETrue);}
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;

	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
    protected:
	/** @brief Notifies dependencies of input updated */
	void NotifyInpsUpdated();
	TBool IsLogeventUpdate();
	// From MUnit
	virtual TEhr ProcessCompChanged(const MUnit* aComp, const string& aContName) override;
    private:
	TBool iActive;
	TBool iUpdated;
	BdVar* mPdata;   //<! Preparing (updating) phase data
	BdVar* mCdata;   //<! Confirming phase data
	static const string KContVal; //<! Content Value  name
};


// DES base agent
class ADes: public Elem, public MDesSyncable_Imd, public MDesObserver_Imd, public MAgent
{
    public:
	static const char* Type() { return "ADes";};
	static string PEType();
	ADes(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes);
	virtual string MDesSyncable_Mid() const;
	virtual void DumpActive() override;
	virtual void DumpUpdated() override;
	// From MDesObserver
	virtual void OnActivated();
	virtual void OnUpdated();
	virtual MIface* MDesObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesObserver_Mid() const;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    private:
	TBool iActive;
	TBool iUpdated;
};

/** @brief Launcher of DES
 * */
class ADesLauncher: public Elem, public MLauncher, public MAgent
{
    public:
	static const char* Type() { return "ADesLauncher";};
	static string PEType();
	ADesLauncher(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MLauncher
	virtual TBool Run() override;
	virtual TBool Stop() override;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
	// Local
	virtual void OnIdle();
    protected:
	TBool mStop;
};

#endif
