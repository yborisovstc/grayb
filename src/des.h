#ifndef __GRAYB_DES_H
#define __GRAYB_DES_H

#include "elem.h"
#include "mdata.h"
#include "mdes.h"

// Transition function agent base. 
class ATrBase: public Elem
{
    public:
	static const char* Type() { return "ATrBase";};
	static string PEType();
	ATrBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ATrBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
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
