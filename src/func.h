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
	enum {
	    EInp1 = 0, EInp2, EInp3, EInp4
	};
    public:
	class Host {
	    public: 
		MDVarGet* GetInp(TInt aInpId);
		virtual Elem::TIfRange GetInps(TInt aId) = 0;
		virtual void OnFuncContentChanged() = 0;
		virtual void LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...) = 0;
	};
    public:
    Func(Host& aHost): Base(string()), mHost(aHost) {};
    virtual string IfaceGetId() const = 0;
    virtual void GetResult(string& aResult) {};
    Host& mHost;
};

// Agent of function of variable type
class AFunVar: public AFunc, public MDVarGet, public Func::Host
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
	virtual string VarGetIfid() const;
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
	virtual void OnFuncContentChanged();
	virtual void LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...);
	// From Elem
	virtual void GetCont(string& aCont); 
    protected:
    protected:
	virtual void Init(const string& aIfaceName) {};
	virtual string GetInpUri(TInt aId);
    protected:
	Func* mFunc;
};

// Addintion, variable 

class FAddBase: public Func {
    public:
	enum {
	    EInp, EInpN
	};
	FAddBase(Host& aHost): Func(aHost) {};
};

class FAddInt: public FAddBase, public MDIntGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddInt(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDIntGet::Type();};
	virtual TInt Value();
};

class FAddFloat: public FAddBase, public MDFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddFloat(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult);
	float mRes;
};

template <class T> class FAddData: public FAddBase, public MDataGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddData(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDataGet<T>::Type();};
	virtual void DataGet(T& aData);
	virtual void GetResult(string& aResult);
	T mRes;
};

template <class T> class FAddVect: public FAddBase, public MVectGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddVect(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MVectGet<T>::Type();};
	virtual void VectGet(Vect<T>& aData);
	virtual void GetResult(string& aResult);
	Vect<T> mRes;
};

template <class T> class FAddMtrd: public FAddBase, public MMtrdGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddMtrd(Host& aHost): FAddBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual void MtrdGet(Mtrd<T>& aData);
};

class AFAddVar: public AFunVar
{
    public:
	static const char* Type() { return "AFAddVar";};
	static string PEType();
	AFAddVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFAddVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

// Multiplication, variable type

class FMplBase: public Func {
    public:
	enum {
	    EInp = Func::EInp1
	};
	FMplBase(Host& aHost): Func(aHost) {};
};

class FMplFloat: public FMplBase, public MDFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMplFloat(Host& aHost): FMplBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult);
	float mRes;
};

class AFMplVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplVar";};
	static string PEType();
	AFMplVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFMplVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

// Non commutative Multiplication, variable type

class FMplncBase: public Func {
    public:
	enum {
	    EInp1 = Func::EInp1, 
	    EInp2 = Func::EInp2, 
	};
	FMplncBase(Host& aHost): Func(aHost) {};
};

// Multiplication, diag matrix * vector
template <class T> class FMplMtrdVect: public FMplncBase, public MVectGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aIfaceName);
	FMplMtrdVect(Host& aHost): FMplncBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual void VectGet(Vect<T>& aData);
	virtual void GetResult(string& aResult);
	Vect<T> mRes;
};


class AFMplncVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplncVar";};
	static string PEType();
	AFMplncVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFMplncVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

// Inversion for multiplication operation, variable

class FMplinvBase: public Func {
    public:
	enum { EInp = Func::EInp1, };
	FMplinvBase(Host& aHost): Func(aHost) {};
};


template <class T> class FMplinvMtrd: public FMplinvBase, public MMtrdGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aOutIfaceName);
	FMplinvMtrd(Host& aHost): FMplinvBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual void MtrdGet(Mtrd<T>& aData);
	virtual void GetResult(string& aResult);
	Mtrd<T> mRes;
};

class AFMplinvVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplinvVar";};
	static string PEType();
	AFMplinvVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFMplinvVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};


// Composing of diag matrix, variable

// Composing of diag matrix, from vector
template <class T> class FCpsMtrdVect: public Func, public MMtrdGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString, MDVarGet* aInp1Var);
	FCpsMtrdVect(Host& aHost): Func(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual void MtrdGet(Mtrd<T>& aData);
	virtual void GetResult(string& aResult);
    protected:
	static TBool IsInpComatible(MDVarGet* aInpVar);
	Mtrd<T> mRes;
};

class AFCpsMtrdVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCpsMtrdVar";};
	static string PEType();
	AFCpsMtrdVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFCpsMtrdVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};



// Division, variable type

class FDivBase: public Func {
    public:
	enum {
	    EInp = Func::EInp1, EInp_Dvs
	};
	FDivBase(Host& aHost): Func(aHost) {};
};

class FDivFloat: public FDivBase, public MDFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FDivFloat(Host& aHost): FDivBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult);
	float mRes;
};

class AFDivVar: public AFunVar
{
    public:
	static const char* Type() { return "AFDivVar";};
	static string PEType();
	AFDivVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFDivVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};


class FBcmpBase: public Func {
    public:
	enum { EInp_1 = EInp1, EInp_2, EInp_Ftype };
	enum TFType {ELt, ELe, EEq, EGe, EGt};
	FBcmpBase(Host& aHost, TFType aFType): Func(aHost), mFType(aFType) {};
	virtual void GetResult(string& aResult);
    protected:
	TFType mFType;
	TBool mRes;
};

// Boolean comparition, variable data
class FBcmpFloat: public FBcmpBase, public MDBoolGet {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, MDVarGet* aInp1Var, MDVarGet* aInp2Var, TFType aFType);
	FBcmpFloat(Host& aHost, TFType aFType): FBcmpBase(aHost, aFType) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDBoolGet::Type();};
	virtual TBool Value();
    protected:
	float GetArg(TInt aInpId);
	static TBool IsInpComatible(MDVarGet* aInpVar);
};

class AFBcmpVar: public AFunVar
{
    public:
	static const char* Type() { return "AFBcmpVar";};
	static string PEType();
	AFBcmpVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFBcmpVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
	FBcmpBase::TFType GetFType();
};

// Comparition, variable data
class AFCmpVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCmpVar";};
	static string PEType();
	AFCmpVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFCmpVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

class FCmpFloat: public Func, public MDIntGet {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Iid, const string& aInp2Iid);
	FCmpFloat(Host& aHost): Func(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDIntGet::Type();};
	virtual TInt Value();
    protected:
	float GetArg(TInt aInpId);
};


// Getting component of container
class AFAtVar: public AFunVar
{
    public:
	static const char* Type() { return "AFAtVar";};
	static string PEType();
	AFAtVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFAtVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	TInt GetInd(TInt aInpId);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

class FAtBase: public Func 
{
    public:
	FAtBase(Host& aHost): Func(aHost) {};
	TInt GetInd();
};

// 	Getting component of container: vector 
template <class T> class FAtVect: public FAtBase, public MDataGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtVect(Host& aHost): FAtBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDataGet<T>::Type();};
	virtual void DataGet(T& aData);
    protected:
	MVectGet<T>* GetArg();
};

// Composing vector from components

class AFCpsVectVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCpsVectVar";};
	static string PEType();
	AFCpsVectVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFCpsVectVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	TInt GetInd(TInt aInpId);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

class FCpsVBase: public Func 
{
    public:
	FCpsVBase(Host& aHost): Func(aHost) {};
	const static TInt KIndMax;
};

// Composing vector from components: float
template <class T> class FCpsVect: public FCpsVBase, public MVectGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aOutIid);
	FCpsVect(Host& aHost): FCpsVBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MVectGet<T>::Type();};
	virtual void VectGet(Vect<T>& aData);
	virtual void GetResult(string& aResult);
    protected:
	MVectGet<T>* GetArg();
    protected:
	Vect<T> mData;
};

// Case - commutation of inputs
class AFSwitchVar: public AFunVar
{
    public:
	static const char* Type() { return "AFSwitchVar";};
	static string PEType();
	AFSwitchVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AFSwitchVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From Func::Host
	virtual Elem::TIfRange GetInps(TInt aId);
    protected:
	virtual void Init(const string& aIfaceName);
};

class FSwithcBase: public Func 
{
    public:
	enum { EInp_Sel = Func::EInp1, EInp_1 };
	FSwithcBase(Host& aHost): Func(aHost) {};
};

// 	Boolean case
class FSwitchBool: public FSwithcBase, public MDVarGet
{
    public:
	static Func* Create(Func::Host* aHost, const string& aOutIid, const string& aInp1Id);
	FSwitchBool(Func::Host& aHost): FSwithcBase(aHost) {};
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	MDVarGet* GetCase() const;
	// From MDVarGet
	virtual Elem* VarGetBase();
	virtual string VarGetIfid() const;
    private:
	TBool GetCtrl() const;
};




#endif
