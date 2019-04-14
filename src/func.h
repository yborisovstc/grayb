#ifndef __GRAYB_FUNC_H
#define __GRAYB_FUNC_H

#include "elem.h"
#include "mdata.h"

class FuncBase: public Elem, public MACompsObserver, public MDataObserver
{
    public:
	static const char* Type() { return "FuncBase";};
	static string PEType();
	FuncBase(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
	// From MDataObserver
	virtual void OnDataChanged();
    protected:
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	void NotifyUpdate();
};

// Agent base of Int function
class AFunInt: public FuncBase, public MDIntGet, public MAgent
{
    public:
	static const char* Type() { return "AFunInt";};
	static string PEType();
	AFunInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
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
	AIncInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Agent of Int function result
class AFunIntRes: public AFunInt
{
    public:
	static const char* Type() { return "AFunIntRes";};
	static string PEType();
	AFunIntRes(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
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
	AAddInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Agent function "Count of data matched a criteria"
class ACountCritInt: public AFunInt
{
    public:
	static const char* Type() { return "ACountCritInt";};
	static string PEType();
	ACountCritInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From FuncBase
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	// From MDataObserver
	virtual void OnDataChanged();
};

// Function agent base without result caching
// It implements elem content methods in purpose of monitoring the value from client side
class AFunc: public Elem, public MACompsObserver, public MDataObserver, public MAgent
{

    public:
	static const char* Type() { return "AFunc";};
	static string PEType();
	AFunc(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
	// From MDataObserver
	virtual void OnDataChanged();
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    protected:
	void NotifyUpdate();
	TBool IsLogeventUpdate();
	inline MUnit* Host();
    public:
	static const string KDiagErr;
};

inline MUnit* AFunc::Host() { return (iMan == NULL) ? NULL : iMan->GetMan(); };

class AFuncInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFuncInt";};
	static string PEType();
	AFuncInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual TInt GetValue();
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MUnit
	virtual TBool GetCont(string& aValue, const string& aName=string()) const; 
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
    protected:
	TInt mData;
};

class AFAddInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFAddInt";};
	static string PEType();
	AFAddInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
};

class AFSubInt: public AFuncInt
{
    public:
	static const char* Type() { return "AFSubInt";};
	static string PEType();
	AFSubInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual TInt GetValue();
};

// Restriction of value from top and bottom
class AFLimInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFLimInt";};
	static string PEType();
	AFLimInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
};

// Division
class AFDivInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFDivInt";};
	static string PEType();
	AFDivInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
};



// Inputs to vector conversion
class AFIntToVect: public AFunc, public MVIntGet
{
    public:
	static const char* Type() { return "AFIntToVect";};
	static string PEType();
	AFIntToVect(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
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
		MIface* Call(const string& aSpec, string& aRes) override { return NULL;}
		TInt Value() override { return iHost->iSample;};
		MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
		string MDIntGet_Mid() const override { return iHost->Elem::MElem_Mid();}
		AFConvInt* iHost;
	};
    public:
	static const char* Type() { return "AFConvInt";};
	static string PEType();
	AFConvInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	virtual TInt GetValue();
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
	AFuncm(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
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
	AFuncmAdd(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
};

// Agent for Greater Than Int functin
class AFGTInt: public AFunc, public MDBoolGet
{
    public:
	static const char* Type() { return "AFGTInt";};
	static string PEType();
	AFGTInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDBoolGet
	virtual TBool Value();
};

// Agent for Bool to Int functin
class AFBoolToInt: public AFunc, public MDIntGet
{
    public:
	static const char* Type() { return "AFBoolToInt";};
	static string PEType();
	AFBoolToInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
};

// Executive part of function 
class Func: public Base {
    public:
	enum {
	    EInp1 = 0, EInp2, EInp3, EInp4
	};
    public:
	class Host {
	    friend class Elem;
	    public: 
	    virtual TInt GetInpCpsCount() const = 0;
	    MDVarGet* GetInp(TInt aInpId, TBool aOpt = EFalse);
	    virtual Unit::TIfRange GetInps(TInt aId, TBool aOpt = EFalse) = 0;
	    virtual void OnFuncContentChanged() = 0;
	    virtual void LogWrite(TLogRecCtg aCtg, const char* aFmt,...) = 0;
	    virtual string GetInpUri(TInt aId) const = 0;
	    virtual Unit* GetAgent() = 0;
	};
    public:
    Func(Host& aHost): Base(), mHost(aHost) {};
    virtual string IfaceGetId() const = 0;
    virtual void GetResult(string& aResult) const { aResult = "<?>";};
    virtual string GetInpExpType(TInt aId) const { return "<?>";};
    virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
    virtual TInt GetContCount() const {return 1;};
    MLogRec* Logger() {return mHost.GetAgent()->Logger();};
    Host& mHost;
    map<MDVarGet*, string> mInps;
};

// Agent of function of variable type
class AFunVar: public AFunc, public MDVarGet, public Func::Host
{
    public:
	static const char* Type() { return "AFunVar";};
	static string PEType();
	AFunVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	void Construct();
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
	// From Func::Host
	virtual TIfRange GetInps(TInt aId, TBool aOpt = EFalse);
	virtual void OnFuncContentChanged();
	virtual void LogWrite(TLogRecCtg aCtg, const char* aFmt,...);
	virtual Elem* GetAgent() {return this;};
	virtual TInt GetInpCpsCount() const {return -1;};
	// From MUnit
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const; 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TInt GetContCount(const string& aName = string()) const;
	virtual string GetContComp(const string& aOwnerName, TInt aInd) const;
	virtual string GetAssociatedData(const string& aUri) const;
    protected:
	virtual void Init(const string& aIfaceName) {};
	virtual string GetInpUri(TInt aId) const;
    protected:
	void GetContInpTypes(string& aCont) const;
	void GetContInp(TInt aInd, string& aName, string& aCont) const;
	TInt GetInpsCount() const;
    protected:
	Func* mFunc;
	string mIfaceReq;
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
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDIntGet::Type();};
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return mHost.GetAgent()->MUnit::Mid();}
};

class FAddFloat: public FAddBase, public MDFloatGet {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddFloat(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult) const;
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TInt GetContCount() const;
	virtual string GetInpExpType(TInt aId) const { return "float";};
	static string DataToString(float aData);
	float mRes;
};

template <class T> class FAddData: public FAddBase, public MDataGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddData(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDataGet<T>::Type();};
	virtual void DataGet(T& aData);
	virtual void GetResult(string& aResult) const;
	T mRes;
};

template <class T> class FAddVect: public FAddBase, public MVectGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddVect(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MVectGet<T>::Type();};
	virtual void VectGet(Vect<T>& aData);
	virtual void GetResult(string& aResult) const;
	Vect<T> mRes;
};

template <class T> class FAddMtrd: public FAddBase, public MMtrdGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddMtrd(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual void MtrdGet(Mtrd<T>& aData);
};

template <class T> class FAddMtr: public FAddBase, public MMtrGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddMtr(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrGet<T>::Type();};
	virtual void MtrGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const;
	Mtr<T> mRes;
};

// Addintion, generic data
template <class T> class FAddDt: public FAddBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FAddDt(Host& aHost): FAddBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	virtual string GetInpExpType(TInt aId) const;
	T mRes;
};

class AFAddVar: public AFunVar
{
    public:
	static const char* Type() { return "AFAddVar";};
	static string PEType();
	AFAddVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const;
    protected:
	virtual void Init(const string& aIfaceName);
	static const string KContVal_About;
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
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult) const;
	float mRes;
};

//   Simple data
template <class T> class FMplDt: public FMplBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMplDt(Host& aHost): FMplBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TInt GetContCount() const {return 2;};
	T mRes;
};

class AFMplVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplVar";};
	static string PEType();
	AFMplVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 1;};
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
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual void VectGet(Vect<T>& aData);
	virtual void GetResult(string& aResult) const;
	Vect<T> mRes;
};

// Multiplication, matrix
template <class T> class FMplMtr: public FMplncBase, public MMtrGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aIfaceName);
	FMplMtr(Host& aHost): FMplncBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual void MtrGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const;
	Mtr<T> mRes;
};

// Multiplication, generic data
template <class T> class FMplncDt: public FMplncBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aIfaceName);
	FMplncDt(Host& aHost): FMplncBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	virtual string GetInpExpType(TInt aId) const;
	T mRes;
};

// Multiplication, scalar data to matrix
template <class T> class FMplncScMtr: public FMplncBase, public MDtGet<Mtr<T> > {
    public:
	static Func* Create(Host* aHost, const string& aIfaceName);
	FMplncScMtr(Host& aHost): FMplncBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<Mtr<T> >::Type();};
	virtual void DtGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const;
	Mtr<T> mRes;
};


class AFMplncVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplncVar";};
	static string PEType();
	AFMplncVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 2;};
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
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual void MtrdGet(Mtrd<T>& aData);
	virtual void GetResult(string& aResult) const;
	Mtrd<T> mRes;
};

template <class T> class FMplinvMtr: public FMplinvBase, public MMtrGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMplinvMtr(Host& aHost): FMplinvBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrGet<T>::Type();};
	virtual void MtrGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const;
	Mtr<T> mRes;
};

// Inversion, generic data
template <class T> class FMplinvDt: public FMplinvBase, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aString);
	FMplinvDt(Host& aHost): FMplinvBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	T mRes;
};

class AFMplinvVar: public AFunVar
{
    public:
	static const char* Type() { return "AFMplinvVar";};
	static string PEType();
	AFMplinvVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 1;};
    protected:
	virtual void Init(const string& aIfaceName);
};

// Casting, variable

class AFCastVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCastVar";};
	static string PEType();
	AFCastVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 1;};
    protected:
	virtual void Init(const string& aIfaceName);
};

// Casting, generic data
template <class T, class TA> class FCastDt: public Func, public MDtGet<T> {
    public:
	static Func* Create(Host* aHost, const string& aTypeId, const string& aArtTypeId);
	FCastDt(Host& aHost): Func(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual void DtGet(T& aData);
	virtual void GetResult(string& aResult) const;
	T mRes;
};



// Composing of diag matrix, variable


// Composing of diag matrix, from vector
template <class T> class FCpsMtrdVect: public Func, public MDtGet<Mtr<T> > {
    public:
	static Func* Create(Host* aHost, const string& aString, MDVarGet* aInp1Var);
	FCpsMtrdVect(Host& aHost): Func(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<Mtr<T> >::Type();};
	virtual void DtGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const {mRes.ToString(aResult);};
	virtual string GetInpExpType(TInt aId) const;
    protected:
	Mtr<T> mRes;
};

class AFCpsMtrdVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCpsMtrdVar";};
	static string PEType();
	AFCpsMtrdVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 1;};
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
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual float Value();
	virtual void GetResult(string& aResult) const;
	float mRes;
};

class AFDivVar: public AFunVar
{
    public:
	static const char* Type() { return "AFDivVar";};
	static string PEType();
	AFDivVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 2;};
    protected:
	virtual void Init(const string& aIfaceName);
};


class FBcmpBase: public Func {
    public:
	enum { EInp_1 = EInp1, EInp_2, EInp_Ftype };
	enum TFType {ELt, ELe, EEq, EGe, EGt};
	FBcmpBase(Host& aHost, TFType aFType): Func(aHost), mFType(aFType) {};
	virtual void GetResult(string& aResult) const;
    protected:
	TFType mFType;
	TBool mRes;
};

// Boolean comparition, variable data
class FBcmpFloat: public FBcmpBase, public MDBoolGet {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, MDVarGet* aInp1Var, MDVarGet* aInp2Var, TFType aFType);
	FBcmpFloat(Host& aHost, TFType aFType): FBcmpBase(aHost, aFType) {};
	virtual MIface *DoGetObj(const char *aName);
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
	AFBcmpVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 3;};
    protected:
	virtual void Init(const string& aIfaceName);
	FBcmpBase::TFType GetFType();
};

// Comparition, variable data

class FCmpBase: public Func, public MDtGet<Sdata<bool> >
{
    public:
	enum TFType {ELt, ELe, EEq, EGe, EGt};
    public:
	FCmpBase(Host& aHost, TFType aFType): Func(aHost), mFType(aFType) {};
	virtual ~FCmpBase();
	virtual string IfaceGetId() const;
	virtual void GetResult(string& aResult) const;
	virtual MIface *DoGetObj(const char *aName);
	virtual void DtGet(Sdata<bool>& aData);
    protected:
	TFType mFType;
	Sdata<bool> mRes;
};

template <class T> class FCmp: public FCmpBase
{
    public:
	static Func* Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, TFType aFType);
	FCmp(Host& aHost, TFType aFType): FCmpBase(aHost, aFType) {};
	virtual void DtGet(Sdata<bool>& aData);
};


class AFCmpVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCmpVar";};
	static string PEType();
	AFCmpVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	FCmpBase::TFType GetFType();
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 2;};
    protected:
	virtual void Init(const string& aIfaceName);
};

// Getting component of container
class AFAtVar: public AFunVar
{
    public:
	static const char* Type() { return "AFAtVar";};
	static string PEType();
	AFAtVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 2;};
    protected:
	virtual void Init(const string& aIfaceName);
};

class FAtBase: public Func 
{
    public:
	FAtBase(Host& aHost): Func(aHost) {};
};

// 	Getting component of container: matrix-vector 
template <class T> class FAtMVect: public FAtBase, public MDtGet<Sdata<T> > {
    public:
	static Func* Create(Host* aHost, const string& aOutIid, const string& aInp1Id);
	FAtMVect(Host& aHost): FAtBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<Sdata<T> >::Type();};
	virtual void DtGet(Sdata<T>& aData);
	virtual void GetResult(string& aResult) const {mRes.ToString(aResult);};
	virtual string GetInpExpType(TInt aId) const;
    protected:
	Sdata<T> mRes;
};

// 	Getting component of container: named tuple
class FAtNTuple: public FAtBase  {
    public:
	class IfProxyBase: public Base {
	    public:
	    IfProxyBase(FAtNTuple* aHost): Base(), mHost(aHost) {};
	    virtual ~IfProxyBase();
	    virtual void GetIfaceId(string& aId) const = 0;
	    FAtNTuple* mHost;
	};
	template <class T> class IfProxy: public IfProxyBase, public MDtGet<T> {
	    public:
	    IfProxy(FAtNTuple* aHost): IfProxyBase(aHost) {};
	    virtual void GetIfaceId(string& aId) const { aId = MDtGet<T>::Type();};
	    virtual MIface *DoGetObj(const char *aName);
	    virtual void DtGet(T& aData);
	};
    public:
	static Func* Create(Host* aHost, const string& aInpId, const string& aInpIndId);
	FAtNTuple(Host& aHost): FAtBase(aHost), mRes(NULL), mIfProxy(NULL) {};
	void Init();
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const;
	virtual void GetResult(string& aResult) const {if (mRes != NULL) mRes->ToString(aResult);};
	virtual string GetInpExpType(TInt aId) const;
	void GetField();
    protected:
	DtBase* mRes;
	IfProxyBase* mIfProxy;
};

// Composing vector from components

class AFCpsVectVar: public AFunVar
{
    public:
	static const char* Type() { return "AFCpsVectVar";};
	static string PEType();
	AFCpsVectVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	TInt GetInd(TInt aInpId);
    protected:
	virtual void Init(const string& aIfaceName);
};

// Composing vector from components: float
template <class T> class FCpsVect: public Func, public MDtGet<Mtr<T> > {
    public:
	static Func* Create(Host* aHost, const string& aOutIid);
	FCpsVect(Host& aHost): Func(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<Mtr<T> >::Type();};
	virtual void DtGet(Mtr<T>& aData);
	virtual void GetResult(string& aResult) const {mRes.ToString(aResult);};
	virtual string GetInpExpType(TInt aId) const;
    protected:
	Mtr<T> mRes;
};

// Case - commutation of inputs
class AFSwitchVar: public AFunVar
{
    public:
	static const char* Type() { return "AFSwitchVar";};
	static string PEType();
	AFSwitchVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From AFunVar
	virtual void *DoGetDObj(const char *aName);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 3;};
    protected:
	virtual void Init(const string& aIfaceName);
};

class FSwithcBase: public Func 
{
    public:
	enum { EInp_Sel = Func::EInp1, EInp_1 };
	FSwithcBase(Host& aHost): Func(aHost) {};
};

// 	Boolean case, selector should be of MDBoolGet or MDtGet<Sdata<bool> >
class FSwitchBool: public FSwithcBase, public MDVarGet
{
    public:
	static Func* Create(Func::Host* aHost, const string& aOutIid, const string& aInp1Id);
	FSwitchBool(Func::Host& aHost): FSwithcBase(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	    virtual string IfaceGetId() const { return MDVarGet::Type();};
	MDVarGet* GetCase() const;
	virtual string GetInpExpType(TInt aId) const;
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
    private:
	TBool GetCtrl() const;
};

// Boolean negation
class AFBoolNegVar: public AFunVar
{
    public:
	static const char* Type() { return "AFBoolNegVar";};
	static string PEType();
	AFBoolNegVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Func::Host
	virtual string GetInpUri(TInt aId) const;
	virtual TInt GetInpCpsCount() const {return 1;};
    protected:
	virtual void Init(const string& aIfaceName);
};

// 	Negation func, generic data
class FBnegDt: public Func, public MDtGet<Sdata<bool> >
{
    public:
	static Func* Create(Func::Host* aHost);
	FBnegDt(Func::Host& aHost): Func(aHost) {};
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<Sdata<bool> >::Type();};
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual void GetResult(string& aResult) const;
	virtual void DtGet(Sdata<bool>& aData);
    protected:
	Sdata<bool> mRes;
};


#endif
