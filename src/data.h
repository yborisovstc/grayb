#ifndef __GRAYB_DATA_H
#define __GRAYB_DATA_H

#include "elem.h"
#include "mdata.h"

class DataBase: public Elem, public MACompsObserver, public MUpdatable, public MDataObserver, public MAgent
{
    public:
	static const char* Type() { return "DataBase";};
	static string PEType();
	DataBase(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
    protected:
	TBool FromString(const string& aType, const string& aData); 
	virtual TBool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	bool ToString(string& aType, string& aData);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
	// From MDataObserver
	virtual void OnDataChanged();
	// From MUpdatable
	virtual TBool Update();
	// From MIfaceProv
	virtual MIface* MAgent_DoGetIface(const string& aUid);
    protected:
	void UpdateProp();
	void NotifyUpdate();
	TBool IsLogeventUpdate();
	inline MUnit* Context();
	static const string KCont_Value;
};

inline MUnit* DataBase::Context() { return iMan;};

// Data is representing some static model
//
// Data of integer
class DInt: public DataBase, public MDInt, public MDIntSet, public MDIntGet
{
    /*
    private:
	class DIntGet: public MDIntGet { public: DInt& mHost; DIntGet(DInt& aHost): mHost(aHost) {}
	    MIface* Call(const string& aSpec, string& aRes) override { return mHost.MDIntGet_Call(aSpec, aRes);}
	    string Mid() const override { return mHost.Mid();}
	    TInt Value() override { return mHost.Value();}
	};
	*/
    public:
	static const char* Type() { return "DInt";};
	static string PEType();
	DInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MDInt
	virtual TInt Data() const;
	virtual void Set(TInt aData);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override;
	string MDIntGet_Mid() const override { return Elem::MElem_Mid();}
	// From MDIntSet
	virtual void SetValue(TInt aData);
	// From Data
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	// From MUpdatable
	virtual TBool Update();
	// From MIfaceProv
	virtual MIface* MAgent_DoGetIface(const string& aUid);
    protected:
	//DIntGet mDIntGet;
	TInt mData;
};

// Data of integer. Doesn't support nofification of change
class DNInt: public DInt
{
    public:
	static const char* Type() { return "DNInt";};
	static string PEType();
	DNInt(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	// From MDInt
	virtual void Set(TInt aData);
};



/** @brief Interface of data host
 * */
class DHost
{
    public:
	virtual void HUpdateProp() = 0;
	virtual void HNotifyUpdate() = 0;
};

// Data handler base
class HBase: public Base {
    public:
	HBase(DHost* aHost): Base(), mHost(*aHost) {};
	virtual TBool FromString(const string& aString) = 0;
	virtual void ToString(string& aString) = 0;
	virtual TBool Set(MDVarGet* aInp) = 0;
	//!< Gets Id of data getter iface
	virtual string IfaceGetId() const = 0;
	//!< Gets Id of data setter iface
	virtual string IfaceSGetId() const = 0;
	virtual TBool IsValid() const { return ETrue;};
	virtual TBool IsSigOK() const { return EFalse;};
	DHost& mHost;
};

class HBool: public HBase, public MDBoolGet {
    public:
	HBool(DHost* aHost): HBase(aHost), mData(EFalse) {};
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDBoolGet::Type();};
	virtual string IfaceSGetId() const { return string();}
	virtual void Set(TBool aData);
	virtual TBool Set(MDVarGet* aInp);
	virtual TBool Value();
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	TBool mData;
};

class HInt: public HBase, public MDInt, public MDIntGet, public MDIntSet {
    public:
	HInt(DHost* aHost): HBase(aHost), mData(0) {};
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDIntGet::Type();};
	virtual string IfaceSGetId() const { return MDIntSet::Type();};
	virtual TInt Data() const;
	virtual void Set(TInt aData);
	virtual TBool Set(MDVarGet* aInp);
	// From MDIntGet
	TInt Value() override;
	MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
	string MDIntGet_Mid() const override { return string();}
	// From MDIntSet
	virtual void SetValue(TInt aData);
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	TInt mData;
};

class HFloat: public HBase, public MDFloat, public MDFloatGet, public MDFloatSet {
    public:
	HFloat(DHost* aHost): HBase(aHost), mData(0) {};
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDFloatGet::Type();};
	virtual string IfaceSGetId() const { return MDFloatSet::Type();};
	virtual float Data() const;
	virtual void Set(float aData);
	virtual TBool Set(MDVarGet* aInp);
	virtual float Value();
	virtual void SetValue(float aData);
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	float mData;
};

template <class T> class HData: public HBase, public MDataGet<T> {
    public:
	HData(DHost* aHost);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MVectGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MDataGet
	virtual void DataGet(T& aData);
	// From HBase
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual TBool Set(MDVarGet* aInp);
	T mData;
	static string mId;
};

template <class T> class HVect: public HBase, public MVectGet<T> {
    public:
	HVect(DHost* aHost);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MVectGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MVFloatGet
	virtual void VectGet(Vect<T>& aData);
	// From HBase
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual TBool Set(MDVarGet* aInp);
	virtual TBool IsValid() const { return mValid;};
	Vect<T> mData;
	static string mId;
	TBool mValid;
};

template <class T> class HMtrd: public HBase, public MMtrdGet<T> {
    public:
	HMtrd(DHost* aHost);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MMtrdGet
	virtual void MtrdGet(Mtrd<T>& aData);
	// From HBase
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual TBool Set(MDVarGet* aInp);
	Mtrd<T> mData;
	static string mId;
};

// Matrix
template <class T> class HMtr: public HBase, public MMtrGet<T> {
    public:
	HMtr(DHost* aHost, const string& aCont);
	HMtr(DHost* aHost, const Mtr<T>& aData);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MMtrdGet
	virtual void MtrGet(Mtr<T>& aData);
	// From HBase
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual TBool Set(MDVarGet* aInp);
	virtual TBool IsValid() const { return mData.mValid;};
    protected:
	Mtr<T> mData;
};

// Generic data
template <class T> class HDt: public HBase, public MDtGet<T>, public MDtSet<T> {
    public:
	HDt(DHost* aHost, const string& aCont);
	HDt(DHost* aHost, const T& aData);
	static HBase* Create(DHost* aHost, const string& aString, MDVarGet* aInp = NULL);
	virtual MIface *DoGetObj(const char *aName);
	virtual string IfaceGetId() const { return MDtGet<T>::Type();};
	virtual string IfaceSGetId() const { return string();}
	// From MDtGet
	virtual void DtGet(T& aData) override;
	// From MDtSet
	virtual void DtSet(const T& aData) override;
	// From HBase
	virtual TBool FromString(const string& aString);
	virtual void ToString(string& aString);
	virtual TBool Set(MDVarGet* aInp);
	virtual TBool IsValid() const { return mData.mValid;};
	virtual TBool IsSigOK() const { return mData.mSigTypeOK;};
    protected:
	T mData;
};


/** @brief Variant data with support multiple data types.
 *
 * The actual data type is configured on init phase.
 * This agents shows the technic of dynamic negotiation of interface paremeter.
 * This initial ifase [MDVar*] doesn't specify the type of data, it is established on movement phase 
 * */
// TODO MDVarSet seems not used. To remove?
class DVar:  public DataBase, public MDVar, public MDVarGet, public MDVarSet, public DHost
{
    public:
	static const char* Type() { return "DVar";};
	static string PEType();
	DVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~DVar();
	virtual TBool HandleCompChanged(MUnit& aContext, MUnit& aComp, const string& aContName = string());
	virtual TBool HandleIoChanged(MUnit& aContext, MUnit* aCp);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MUpdatable
	virtual TBool Update();
	// From DataBase
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	// From MDVarSet
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;
	// From MUnit
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const; 
	// From DHost
	virtual void HUpdateProp() override { return UpdateProp();}
	virtual void HNotifyUpdate() override {return NotifyUpdate();}
    protected:
	MUnit* GetInp();
	TBool Init(const string& aString, MDVarGet* aInp = NULL);
    protected:
	HBase* mData;
};

/** @brief Variant base data
 *
 * This data can be used in two phase data processing, like DES state
 * */
class BdVar:  public Base, public MUpdatable, public MDVar, public MDVarGet, public MDVarSet, public DHost
{
    public:
	static const char* Type() { return "BdVar";};
	BdVar(MBdVarHost *aHost);
	virtual ~BdVar();
//	TBool FromString(const string& aType, const string& aData); 
	bool ToString(string& aType, string& aData);
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	virtual TBool IsValid() const { return mData != NULL ? mData->IsValid() : EFalse;};
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MUpdatable
	virtual TBool Update();
	// From MDVarGet
	virtual string VarGetIfid();
	virtual void *DoGetDObj(const char *aName);
	// From MDVarSet
	virtual string VarGetSIfid();
	virtual MIface *DoGetSDObj(const char *aName) override;
	// From DHost
	virtual void HUpdateProp();
	virtual void HNotifyUpdate();
    protected:
	TBool Init(const string& aString, MDVarGet* aInp = NULL);
	void NotifyUpdate();
    protected:
	HBase* mData;  //<! Data
	MBdVarHost* mHost; //<! Host
};

/*
// TODO [YB] It is to implement state date in more effective manner. Not sure it is required
// Confirmable data, double buffered
class ConfDVar: public DataBase, public MDVar, public MDVarGet, public MDVarSet, public MConfirmable
{
public:
static const char* Type() { return "ConfDVar";};
static string PEType();
ConfDVar(const string& aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
ConfDVar(MUnit* aMan = NULL, MEnv* aEnv = NULL);
virtual ~ConfDVar();
// From Base
virtual MIface *DoGetObj(const char *aName);
// From MConfirmable
virtual TBool Confirm();
protected:
HBase* mDataPrep;
HBase* mDataConf;
};
*/

#endif
