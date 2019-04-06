#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include "elem.h"
#include "mdata.h"
#include "mdes.h"
#include "func.h"

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
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
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
	virtual Elem* GetAgent() {return this;};
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


// Iface stub to avoid clashing MIface methods
class MDesSyncable_Imd: public MDesSyncable
{
    virtual MIface* MDesSyncable_Call(const string& aSpec, string& aRes) = 0;
    virtual string MDesSyncable_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MDesSyncable_Call(aSpec, aRes);};
    virtual string Mid() const { return MDesSyncable_Mid();};
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

// State base agent
class StateAgent: public Elem, public MDesSyncable_Imd, public MDesObserver_Imd, public MAgent
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
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
	virtual MIface* MDesObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesObserver_Mid() const;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    private:
	TBool iActive;
	TBool iUpdated;
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
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
	virtual MIface* MDesObserver_Call(const string& aSpec, string& aRes);
	virtual string MDesObserver_Mid() const;
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    private:
	TBool iActive;
	TBool iUpdated;
};


#endif
