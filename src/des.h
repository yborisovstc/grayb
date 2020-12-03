#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include <functional> 

#include "elem.h"
#include "mdata.h"
#include "mdes.h"
#include "mlauncher.h"
#include "func.h"
#include "vert.h"
#include "syst.h"
#include "rmutdata.h"

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
	// TODO YB Not used actually. Remove ?
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
    protected:
	void AddInput(const string& aName);
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
	virtual string MDVarGet_Mid() const override {return GetUri(NULL, ETrue);}
	// From Func::Host
	virtual TIfRange GetInps(TInt aId, TBool aOpt = EFalse) override;
	virtual void OnFuncContentChanged() override;
	virtual void LogWrite(TLogRecCtg aCtg, const char* aFmt,...) override;
	virtual Unit* GetAgent() override {return this;}
	virtual TInt GetInpCpsCount() const {return 0;}
	// From Elem
	virtual TBool GetCont(string& aCont, const string& aName=string()) const; 
    protected:
	virtual void Init(const string& aIfaceName);
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


/** @brief Transition agent "Min of Var data"
 * */
class ATrcMinVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcMinVar";};
	static string PEType();
	ATrcMinVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 1;}
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

/** @brief Agent function "Append"
 * */
class ATrcApndVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcApndVar";};
	static string PEType();
	ATrcApndVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 2;}
};

/** @brief Agent function "Number to string"
 * */
class ATrcNtosVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcNtosVar";};
	static string PEType();
	ATrcNtosVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 1;}
};

/** @brief Agent function "Compare"
 * */
class ATrcCmpVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcCmpVar";};
	static string PEType();
	ATrcCmpVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	FCmpBase::TFType GetFType();
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 2;}
};

/** @brief Agent function "Switcher"
 * */
class ATrcSwitchBool: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcSwitchBool";};
	static string PEType();
	ATrcSwitchBool(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrcVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From MDVarGet
	virtual void *DoGetDObj(const char *aName);
};

/** @brief Transition agent "Getting container size"
 * */
class ATrcSizeVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcSizeVar";};
	static string PEType();
	ATrcSizeVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 1;}
	// From MDVarGet
	virtual string VarGetIfid() override;
};


/** @brief Agent function "Getting component"
 * */
class ATrcAtVar: public ATrcVar
{
    public:
	static const char* Type() { return "ATrcAtVar";};
	static string PEType();
	ATrcAtVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName) override;
	virtual string GetInpUri(TInt aId) const override;
	// From Func::Host
	virtual TInt GetInpCpsCount() const override {return 2;}
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
// TODO YB to base the agent on Unit, little chance to mutate it
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


/** @brief DES adapter base
 * Internal "access points" are used to create required topology instead of
 * using "true" DES with transitions. This is for optimization purpose.
 * */
class AAdp: public Unit, public MDesSyncable_Imd, public MDesObserver_Imd, public MDesInpObserver_Imd, public MAgent, public MDVarGet
{
    public:
	/** @brief Input access point
	 * */
	class AdpIap: public MDesInpObserver_Imd {
	    public:
		using THandler = std::function<void()>;
		THandler mHandler;
	    public:
		AdpIap(MUnit& aHost, THandler aHandler): mHost(aHost), mHandler(aHandler){}
		// From MDesInpObserver
		virtual void OnInpUpdated() override;
		virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes) override { return NULL;}
		virtual string MDesInpObserver_Mid() const  override { return mHost.GetUri(NULL, ETrue);}
	    protected:
		MUnit& mHost;
	};

	/** @brief Parameter access point, using Sdata
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPap: public MDVarGet, public MDtGet<Sdata<T>> {
	    public:
		template<typename P> using THandler = std::function<void (Sdata<P>&)>;
	    public:
		THandler<T> mHandler;
	    public:
		AdpPap(MUnit& aHost, THandler<T> aHandler): mHost(aHost), mHandler(aHandler){}
		// From MDVarGet
		virtual void *DoGetDObj(const char *aName) override;
		virtual string VarGetIfid() override {return MDtGet<Sdata<T>>::Type();}
		// From MDtGet
		virtual void DtGet(Sdata<T>& aData) override { mHandler(aData);}
		virtual string MDVarGet_Mid() const {return mHost.GetUri(NULL, ETrue);}
	    protected:
		MUnit& mHost;
	};
    public:
	/** @brief Managed agent parameter access point, using generic data
	 * Acts as a binder of MDVarGet request to method of host
	 * */
	template <typename T> class AdpPapB: public MDVarGet, public MDtGet<T> {
	    public:
		template<typename P> using THandler = std::function<void (P&)>;
	    public:
		THandler<T> mHandler;
	    public:
		AdpPapB(const AdpPapB& aSrc): mHandler(aSrc.mHandler) {}
		AdpPapB(THandler<T> aHandler): mHandler(aHandler){}
		// From MDVarGet
		virtual void *DoGetDObj(const char *aName) override;
		virtual string VarGetIfid() override {return string();}
		// From MDtGet
		virtual void DtGet(T& aData) override { mHandler(aData);}
	};

	/** @brief Managed agent observer
	 * */
	template <class T>
	class AdpMagObs : public MAgentObserver {
	    public:
		AdpMagObs(T* aHost): mHost(aHost) {}
		// From MAgentObserver
		virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) override {
		    mHost->OnMagCompDeleting(aComp, aSoft, aModif);}
		virtual void OnCompAdding(const MUnit* aComp, TBool aModif = EFalse) override {
		    mHost->OnMagCompAdding(aComp, aModif);}
		virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) override {
		    return mHost->OnMagCompChanged(aComp, aContName, aModif);}
		virtual TBool OnChanged(const MUnit* aComp) override { return mHost->OnMagChanged(aComp); }
		virtual TBool OnCompRenamed(const MUnit* aComp, const string& aOldName) override {
		    return mHost->OnMagCompRenamed(aComp, aOldName); }
		virtual void OnCompMutated(const MUnit* aNode) override { return mHost->OnMagCompMutated(aNode); }
		virtual void OnError(const MUnit* aComp) override { return mHost->OnMagError(aComp); }
		// From MIface
		virtual MIface* Call(const string& aSpec, string& aRes) override { return NULL;}
	    private:
		T* mHost;
	};

    public:
	static const char* Type() { return "AAdp";};
	static string PEType();
	AAdp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~AAdp();
	// From Base
	virtual MIface* DoGetObj(const char *aName) override;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
	// From MDVarGet
	virtual void *DoGetDObj(const char *aName) override {return NULL;}
	virtual string VarGetIfid() override {return string();}
	// From MDesSyncable
	virtual void Update() override;
	virtual void Confirm() override;
	virtual TBool IsUpdated() override { return mUpdated;}
	virtual void SetUpdated() override;
	virtual void ResetUpdated() override { mUpdated = EFalse;}
	virtual TBool IsActive() override { return mActive;}
	virtual void SetActive() override;
	virtual void ResetActive() override { mActive = EFalse;}
	virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes) override { return NULL;}
	virtual string MDesSyncable_Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	virtual void DumpActive() override {}
	virtual void DumpUpdated() override {}
	// From MDesObserver
	virtual void OnActivated() override;
	virtual void OnUpdated() override;
	virtual MIface* MDesObserver_Call(const string& aSpec, string& aRes) override {return NULL;}
	virtual string MDesObserver_Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	// From MDesInpObserver
	virtual void OnInpUpdated() override;
	virtual MIface* MDesInpObserver_Call(const string& aSpec, string& aRes) override {return NULL;}
	virtual string MDesInpObserver_Mid() const override { return GetUri(iEnv->Root(), ETrue);}
    protected:
	virtual void OnMagCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual void OnMagCompAdding(const MUnit* aComp, TBool aModif = EFalse);
	virtual TBool OnMagCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual TBool OnMagChanged(const MUnit* aComp);
	virtual TBool OnMagCompRenamed(const MUnit* aComp, const string& aOldName);
	virtual void OnMagCompMutated(const MUnit* aNode);
	virtual void OnMagError(const MUnit* aComp);
	// Local
	virtual void OnMagUpdated() {}
	void NotifyInpsUpdated(MUnit* aCp);
	TBool UpdateMag(const string& aMagUri);
	void UpdateMag();
    protected:
	// From MUnit
	virtual TEhr ProcessCompChanged(const MUnit* aComp, const string& aContName) override;
	/** @brief Notifies all states inputs of update **/
	void NotifyInpsUpdated();
	inline MUnit* Host() { return iMan;}
    protected:
	TBool mActive = ETrue;
	TBool mUpdated = ETrue;
	MUnit* mMag; /*!< Managed agent */
	string mMagUri; /*!< Managed agent URI */
	AdpMagObs<AAdp> mMagObs = AdpMagObs<AAdp>(this); /*!< Managed agent observer */
};


/** @brief MUnit iface ADP agent
 * */
class AMunitAdp : public AAdp
{
    public:
#if 0
	/** @brief Managed agent comps names access point via MVectorGet<string> */
	class AdpPapCns: public MVectorGet<string> {
	    public:
		AdpPapCns(const AMunitAdp& aHost): mHost(aHost) {}
		// From MVectorGet
		virtual int Size() const override {return mHost.mCompNames.size();}
		virtual void GetElem(int aInd, string& aElem) const override { aElem = mHost.mCompNames.at(aInd);}
		virtual string MVectorGet_Mid() const { return mHost.GetUri(NULL, true) + "#AdpPapCns%" +
		    string(MVectorGet<string>::Type());}
		virtual MIface* MVectorGet_Call(const string& aSpec, string& aRes) override {}
	    private:
		const AMunitAdp& mHost;
	};
#endif
    public:
	using TCmpNames = Vector<string>;
    public:
	static const char* Type() { return "AMunitAdp";};
	static string PEType();
	AMunitAdp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MUnit
	virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	// From MDesSyncable
	virtual void Confirm() override;
    protected:
	void GetCompsCount(Sdata<TInt>& aData);
	void GetCompNames(TCmpNames& aData) { aData = mCompNames;}
	// From AAdp
	virtual void OnMagCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) override;
	virtual void OnMagCompAdding(const MUnit* aComp, TBool aModif = EFalse) override;
	virtual TBool OnMagCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) override;
	virtual TBool OnMagChanged(const MUnit* aComp) override;
	virtual TBool OnMagCompRenamed(const MUnit* aComp, const string& aOldName) override;
	virtual void OnMagCompMutated(const MUnit* aNode) override;
	virtual void OnMagError(const MUnit* aComp) override;
	// From AAdp
	virtual void OnMagUpdated() override;
    protected:
	// Comps count param adapter. Even if the count can be get via comp names vector we support separate param for convenience
	AdpPap<int> mApCmpCount = AdpPap<int>(*this, [this](Sdata<TInt>& aData) {GetCompsCount(aData);}); /*!< Comps count access point */
	AdpPapB<TCmpNames> mApCmpNames = AdpPapB<TCmpNames>([this](TCmpNames& aData) {GetCompNames(aData);}); /*!< Comp names access point */
//	AdpPapCns mApCmpNames = AdpPapCns(*this); /*!< Comps names access point */
    protected:
	TCmpNames mCompNames;
	TBool mCompNamesUpdated = ETrue;
};

/** @brief MElem iface ADP agent
 * */
class AMelemAdp : public AAdp
{
    public:
	static const char* Type() { return "AMelemAdp";};
	static string PEType();
	AMelemAdp(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MUnit
	virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
    protected:
	void ApplyMut();
	void OnInpMut();
	// From MDesSyncable
	virtual void Update();
    protected:
	AdpIap mIapInpMut = AdpIap(*this, [this]() {OnInpMut();}); /*!< Mut Add Widget input access point */
    protected:
	MChromo* mMagChromo; /*<! Managed agent chromo */
	TBool mInpMutChanged = ETrue;
};

/** @brief Agent functions "Mut composer" base
 * */
class ATrcMut: public ATrcBase, public MDVarGet, public MDtGet<DMut>
{
    public:
	static const char* Type() { return "ATrcMut";};
	static string PEType();
	ATrcMut(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual string GetInpUri(TInt aId) const = 0;
	template<typename T> TBool GetInp(TInt aId, T& aRes);
	// From Base
	virtual MIface* DoGetObj(const char *aName);
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	virtual string MDVarGet_Mid() const override {return GetUri(NULL, ETrue);}
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const override;
	virtual string Provided() const override;
    protected:
	DMut mRes;  /*<! Cached result */
};


/** @brief Agent function "Mut Node composer"
 * */
class ATrcMutNode: public ATrcMut
{
    public:
	enum { EInpName, EInpParent };
    public:
	static const char* Type() { return "ATrcMutNode";};
	static string PEType();
	ATrcMutNode(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(TInt aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};

/** @brief Agent function "Mut connect composer"
 * */
class ATrcMutConn: public ATrcMut
{
    public:
	enum { EInpCp1, EInpCp2 };
    public:
	static const char* Type() { return "ATrcMutConn";};
	static string PEType();
	ATrcMutConn(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From ATrcMut
	virtual string GetInpUri(TInt aId) const override;
	// From MDtGet
	virtual void DtGet(DMut& aData) override;
};




// Helpers
template <typename T> TBool GetSData(MUnit* aDvget, T& aData)
{
    TBool res = EFalse;
    MDVarGet* vget = aDvget->GetSIfit(vget);
    if (vget) {
	MDtGet<Sdata<T>>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    Sdata<T> st;
	    gsd->DtGet(st);
	    aData = st.mData;
	    res = ETrue;
	}
    }
    return res;
}

template <typename T> TBool GetGData(MUnit* aDvget, T& aData)
{
    TBool res = EFalse;
    MDVarGet* vget = aDvget->GetSIfit(vget);
    if (vget) {
	MDtGet<T>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    gsd->DtGet(aData);
	    res = ETrue;
	}
    }
    return res;
}



#endif
