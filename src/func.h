#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"
#include "mdata.h"

class FuncBase: public Elem, public MACompsObserver, public MDataObserver
{
    public:
	static const char* Type() { return "FuncBase";};
	static string PEType();
	FuncBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	FuncBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	void NotifyUpdate();
};

// Agent base of Int function
class AFunInt: public FuncBase, public MDIntGet
{
    public:
	static const char* Type() { return "AFunInt";};
	static string PEType();
	AFunInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFunInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
    protected:
	void SetRes(TInt aData);
	MDIntGet* GetInp(const string& aInpName);
    protected:
	TInt mData;
};

// Agent function "Increment of Int data"
class AIncInt: public AFunInt
{
    public:
	static const char* Type() { return "AIncInt";};
	static string PEType();
	AIncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AIncInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Agent of Int function result
class AFunIntRes: public AFunInt
{
    public:
	static const char* Type() { return "AFunIntRes";};
	static string PEType();
	AFunIntRes(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFunIntRes(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	void UpdateOutp();
};

// Agent function "Addition of Int data"
class AAddInt: public AFunInt
{
    public:
	static const char* Type() { return "AAddInt";};
	static string PEType();
	AAddInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AAddInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Agent function "Count of data matched a criteria"
class ACountCritInt: public AFunInt
{
    public:
	static const char* Type() { return "ACountCritInt";};
	static string PEType();
	ACountCritInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Function agent base without result caching
// It implements elem content methods in purpose of monitoring the value from client side
class AFunc: public Elem, public MACompsObserver, public MDataObserver
{

    public:
	static const char* Type() { return "AFunc";};
	static string PEType();
	AFunc(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFunc(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	void NotifyUpdate();
	TBool IsLogeventUpdate();
	inline Elem* Host();
};

inline Elem* AFunc::Host() { return iMan->GetMan();};

class AFuncInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFuncInt";};
	static string PEType();
	AFuncInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFuncInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual TInt GetValue();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MElem
	virtual void GetCont(string& aCont); 
	// From MDIntGet
	virtual TInt Value();
    protected:
	TInt mData;
};

class AFAddInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFAddInt";};
	static string PEType();
	AFAddInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFAddInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

class AFSubInt: public AFuncInt
{
    public:
	static const char* Type() { return "AFSubInt";};
	static string PEType();
	AFSubInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFSubInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual TInt GetValue();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

// Restriction of value from top and bottom
class AFLimInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFLimInt";};
	static string PEType();
	AFLimInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFLimInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Division
class AFDivInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFDivInt";};
	static string PEType();
	AFDivInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFDivInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};



// Inputs to vector conversion
class AFIntToVect: public AFunc, public MVIntGet
{
    public:
	static const char* Type() { return "AFIntToVect";};
	static string PEType();
	AFIntToVect(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual vector<TInt> Value();
};

// Convolution of inputs. Used external working function.
class AFConvInt: public AFuncInt
{
    public:
	class Sample: public MDIntGet {
	    friend class AFConvInt;
	    public:
		Sample(): iHost(NULL) {};
		virtual TInt Value() { return iHost->iSample;};
		AFConvInt* iHost;
	};
    public:
	static const char* Type() { return "AFConvInt";};
	static string PEType();
	AFConvInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFConvInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt GetValue();
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
    private:
	Sample iSampleHolder;
	TInt iSample;
};

// Function agent base without result caching and with multitype support
// TODO [YB] !! Not for use. Doesn't really support dynamic configuration as was ailmed
class AFuncm: public Elem, public MACompsObserver, public MDataObserver, public MDVarGet
{
    public:
	// Executive part of function 
	class Exec
	{
	    friend class AFuncm;
	    public:
	    Exec(AFuncm& aHost): mHost(aHost) {};
	    Exec(const string& aIName, AFuncm& aHost): mHost(aHost) { mHost.mExecs.insert(pair<string, Exec*>(aIName, this));};
	    AFuncm& mHost;
	};
    public:
	static const char* Type() { return "AFuncm";};
	static string PEType();
	AFuncm(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	void NotifyUpdate();
    protected:
	map<string, Exec*> mExecs;
};


class AFuncmAdd: public AFuncm
{
    public:
	class ExcInt: public Exec, public MDIntGet {
	    ExcInt(AFuncm& aHost): Exec(MDIntGet::Type(), aHost) {};
	    virtual TInt Value();
	};
    public:
	static const char* Type() { return "AFuncAdd";};
	static string PEType();
	AFuncmAdd(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

// Agent for Greater Than Int functin
class AFGTInt: public AFunc, public MDBoolGet
{
    public:
	static const char* Type() { return "AFGTInt";};
	static string PEType();
	AFGTInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFGTInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDBoolGet
	virtual TBool Value();
};

// Agent for Bool to Int functin
class AFBoolToInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFBoolToInt";};
	static string PEType();
	AFBoolToInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFBoolToInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Executive part of function 
class Func: public Base {
    public:
	class Host {
	    public: 
		virtual Elem::TIfRange GetInps(TInt aId) = 0;
	};
    public:
    Func(Host& aHost): Base(string()), mHost(aHost) {};
    Host& mHost;
};

// Agent of function of variable type
class AFunVar: public AFunc, public MDVarGet
{
    public:
	static const char* Type() { return "AFunVar";};
	static string PEType();
	AFunVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFunVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDVarGet
	virtual Elem* VarGetBase();
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
    protected:
	virtual void Init(const string& aIfaceName) {};
    protected:
	Func* mFunc;
};

class FAddBase: public Func {
    public:
	enum {
	    EInp, EInpN
	};
	FAddBase(Host& aHost): Func(aHost) {};
	static const string mIdInp;
};

class FAddInt: public FAddBase, public MDIntGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddInt(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual TInt Value();
};

class FAddFloat: public FAddBase, public MDFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddFloat(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual float Value();
};

class FAddVFloat: public FAddBase, public MVFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddVFloat(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual void VFloatGet(VFloat& aData);
};

class AFAddVar: public AFunVar, public Func::Host
{
    public:
	static const char* Type() { return "AFAddVar";};
	static string PEType();
	AFAddVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFAddVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};


#endif
