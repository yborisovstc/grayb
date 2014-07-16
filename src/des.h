#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include "elem.h"
#include "mdata.h"
#include "mdes.h"
#include "func.h"

// Transition function agent base. 
class ATrBase: public Elem, public MACompsObserver
{
    public:
	static const char* Type() { return "ATrBase";};
	static string PEType();
	ATrBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
};

// Agent base of Int transition function
class ATrInt: public ATrBase, public MDIntGet
{
    public:
	static const char* Type() { return "ATrInt";};
	static string PEType();
	ATrInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
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
	ATrIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrIncInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Subtraction of Int data"
class ATrSubInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrSubInt";};
	static string PEType();
	ATrSubInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrSubInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Multiplying of Int data"
class ATrMplInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrMplInt";};
	static string PEType();
	ATrMplInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrMplInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Agent function "Deviding of Int data"
class ATrDivInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrDivInt";};
	static string PEType();
	ATrDivInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrDivInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};


// Agent base of Var transition function
class ATrVar: public ATrBase, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "ATrVar";};
	static string PEType();
	ATrVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDVarGet
	virtual Elem* VarGetBase();
	virtual string VarGetIfid() const;
	virtual void *DoGetDObj(const char *aName);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
	virtual void OnFuncContentChanged();
	virtual void LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...);
	// From Elem
	virtual void GetCont(string& aCont); 
    protected:
	virtual void Init(const string& aIfaceName) {};
	virtual string GetInpUri(TInt aId);
    protected:
	Func* mFunc;
};


// Agent function "Addition of Var data"
class ATrAddVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrAddVar";};
	static string PEType();
	ATrAddVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrAddVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Multiplication of Var data"
class ATrMplVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrMplVar";};
	static string PEType();
	ATrMplVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrMplVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Division of Var data"
class ATrDivVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrDivVar";};
	static string PEType();
	ATrDivVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrDivVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Switch controlled by var data"
class ATrSwitchVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrSwitchVar";};
	static string PEType();
	ATrSwitchVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrSwitchVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Switch controlled by var data"
class ATrAtVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrAtVar";};
	static string PEType();
	ATrAtVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrAtVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Composing of vector, var data"
class ATrCpsVectVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrCpsVectVar";};
	static string PEType();
	ATrCpsVectVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrCpsVectVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
};

// Agent function "Boolean comparition of Var data"
class ATrBcmpVar: public ATrVar
{
    public:
	static const char* Type() { return "ATrBcmpVar";};
	static string PEType();
	ATrBcmpVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrBcmpVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDVarGet
	virtual string VarGetIfid() const;
	// From ATrVar
	virtual void Init(const string& aIfaceName);
	virtual string GetInpUri(TInt aId);
    protected:
	FBcmpBase::TFType GetFType();
};


// State base agent
class StateAgent: public Elem, public MDesSyncable, public MDesObserver
{
    public:
	static const char* Type() { return "StateAgent";};
	static string PEType();
	StateAgent(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	StateAgent(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Elem
	//virtual void OnCompChanged(Elem& aComp);
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
	// Iface provider
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
    private:
	TBool iActive;
	TBool iUpdated;
};

// DES base agent
class ADes: public Elem, public MDesSyncable, public MDesObserver
{
    public:
	static const char* Type() { return "ADes";};
	static string PEType();
	ADes(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ADes(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Elem
	//virtual void OnCompChanged(Elem& aComp);
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
    private:
	TBool iActive;
	TBool iUpdated;
};


#endif
