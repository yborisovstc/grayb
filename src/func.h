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
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp) = 0;
	void NotifyUpdate();
};

// Agent base of Int function
class AFunInt: public FuncBase, public MDIntGet
{
    public:
	static const char* Type() { return "AFunInt";};
	static string PEType();
	AFunInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
class AFunc: public Elem, public MACompsObserver, public MDataObserver
{

    public:
	static const char* Type() { return "AFunc";};
	static string PEType();
	AFunc(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	void NotifyUpdate();
	TBool IsLogeventUpdate();
};

class AFAddInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFAddInt";};
	static string PEType();
	AFAddInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

class AFSubInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFSubInt";};
	static string PEType();
	AFSubInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};

// Restriction of value from top and bottom
class AFLimInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFLimInt";};
	static string PEType();
	AFLimInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
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
class AFConvInt: public AFunc, public MDIntGet
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
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
    private:
	Sample iSampleHolder;
	TInt iSample;
};

// Function agent base without result caching and with multitype support
// TODO [YB] !! Not for use. Doesn't really support dynamic configuration as was ailmed
class AFuncm: public Elem, public MACompsObserver, public MDataObserver
{
    public:
	// Executive part of function 
	class Exec
	{
	    friend class AFunc;
	    public:
	    Exec(const string& aIName, AFuncm& aHost): iHost(aHost) { iHost.iExecs.insert(pair<string, void*>(aIName, this));};
	    public:
	    AFuncm& iHost;
	};
    public:
	static const char* Type() { return "AFunc";};
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
	map<string, void*> iExecs;
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
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDIntGet
	virtual TInt Value();
};




#endif
