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

// Variant data. Supports multiple data types. The actual data type is configured on init phase
// This agents shows the technic of dynamic negotiation of interface paremeter.
// This initial ifase [MDVar*] doesn't specify the type of data, it is established on movement phase 
class DVar:  public DataBase, public MDVar, public MDVarGet, public MDVarSet
{
    public:
	// Data handler base
	class HBase: public Base {
	    public:
		HBase(DVar* aHost): Base(), mHost(*aHost) {};
		virtual TBool FromString(const string& aString) = 0;
		virtual void ToString(string& aString) = 0;
		virtual TBool Set(MDVarGet* aInp) = 0;
		virtual string IfaceGetId() const = 0;
		virtual TBool IsValid() const { return ETrue;};
		virtual TBool IsSigOK() const { return EFalse;};
		DVar& mHost;
	};
	class HBool: public HBase, public MDBoolGet {
	    public:
		HBool(DVar* aHost): HBase(aHost), mData(EFalse) {};
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MDBoolGet::Type();};
		virtual void Set(TBool aData);
		virtual TBool Set(MDVarGet* aInp);
		virtual TBool Value();
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		TBool mData;
	};
	class HInt: public HBase, public MDInt, public MDIntGet, public MDIntSet {
	    public:
		HInt(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MDIntGet::Type();};
		virtual TInt Data() const;
		virtual void Set(TInt aData);
		virtual TBool Set(MDVarGet* aInp);
		// From MDIntGet
		TInt Value() override;
		MIface* MDIntGet_Call(const string& aSpec, string& aRes) override { return NULL;}
		string MDIntGet_Mid() const override { return mHost.Elem::MElem_Mid();}
		// From MDIntSet
		virtual void SetValue(TInt aData);
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		TInt mData;
	};
	class HFloat: public HBase, public MDFloat, public MDFloatGet, public MDFloatSet {
	    public:
		HFloat(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MDFloatGet::Type();};
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
		HData(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MVectGet<T>::Type();};
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
		HVect(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MVectGet<T>::Type();};
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
		HMtrd(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
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
		HMtr(DVar* aHost, const string& aCont);
		HMtr(DVar* aHost, const Mtr<T>& aData);
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
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
	template <class T> class HDt: public HBase, public MDtGet<T> {
	    public:
		HDt(DVar* aHost, const string& aCont);
		HDt(DVar* aHost, const T& aData);
		static HBase* Create(DVar* aHost, const string& aString, MDVarGet* aInp = NULL);
		virtual MIface *DoGetObj(const char *aName);
		virtual string IfaceGetId() const { return MDtGet<T>::Type();};
		// From MMtrdGet
		virtual void DtGet(T& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(MDVarGet* aInp);
		virtual TBool IsValid() const { return mData.mValid;};
		virtual TBool IsSigOK() const { return mData.mSigTypeOK;};
	    protected:
		T mData;
	};
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
	virtual Elem* VarSetBase();
	// From MUnit
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const; 
    protected:
	MUnit* GetInp();
	TBool Init(const string& aString, MDVarGet* aInp = NULL);
    protected:
	HBase* mData;
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
