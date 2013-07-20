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
	ATrBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

// Agent base of Int transition function
class ATrInt: public ATrBase, public MDIntGet
{
    public:
	static const char* Type() { return "ATrInt";};
	ATrInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
    protected:
	MDIntGet* GetInp(const string& aInpName);
    protected:
	TInt mData;
};

// Agent function "Increment of Int data"
class ATrIncInt: public ATrInt
{
    public:
	static const char* Type() { return "ATrIncInt";};
	ATrIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
	StateAgent(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
	ADes(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
