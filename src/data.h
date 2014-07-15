#ifndef __GRAYB_DATA_H
#define __GRAYB_DATA_H

#include "elem.h"
#include "mdata.h"

class DataBase: public Elem, public MACompsObserver, public MUpdatable, public MDataObserver
{
    public:
	static const char* Type() { return "DataBase";};
	static string PEType();
	DataBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	DataBase(Elem* aMan = NULL, MEnv* aEnv = NULL);
    protected:
	virtual TBool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
	// From MUpdatable
	virtual TBool Update();
    protected:
	void UpdateProp();
	void NotifyUpdate();
	TBool IsLogeventUpdate();
};

// Data is representing some static model
//
// Data of integer
class DInt: public DataBase, public MDInt, public MDIntGet, public MDIntSet
{
    public:
	static const char* Type() { return "DInt";};
	static string PEType();
	DInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	DInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDInt
	virtual TInt Data() const;
	virtual void Set(TInt aData);
	// From MDIntGet
	virtual TInt Value();
	// From MDIntSet
	virtual void SetValue(TInt aData);
	// From Data
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	// From MUpdatable
	virtual TBool Update();
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
    protected:
	TInt mData;
};

// Data of integer. Doesn't support nofification of change
class DNInt: public DInt
{
    public:
	static const char* Type() { return "DNInt";};
	static string PEType();
	DNInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	DNInt(Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MDInt
	virtual void Set(TInt aData);
	// From Elem
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
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
		HBase(DVar* aHost): Base(string()), mHost(*aHost) {};
		virtual TBool FromString(const string& aString) = 0;
		virtual void ToString(string& aString) = 0;
		virtual TBool Set(Elem* aInp) = 0;
		virtual string IfaceGetId() const = 0;
		virtual TBool IsValid() const { return ETrue;};
		virtual TBool IsSigOK() const { return EFalse;};
		DVar& mHost;
	};
	class HBool: public HBase, public MDBoolGet {
	    public:
		HBool(DVar* aHost): HBase(aHost), mData(EFalse) {};
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MDBoolGet::Type();};
		virtual void Set(TBool aData);
		virtual TBool Set(Elem* aInp);
		virtual TBool Value();
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		TBool mData;
	};
	class HInt: public HBase, public MDInt, public MDIntGet, public MDIntSet {
	    public:
		HInt(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MDIntGet::Type();};
		virtual TInt Data() const;
		virtual void Set(TInt aData);
		virtual TBool Set(Elem* aInp);
		virtual TInt Value();
		virtual void SetValue(TInt aData);
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		TInt mData;
	};
	class HFloat: public HBase, public MDFloat, public MDFloatGet, public MDFloatSet {
	    public:
		HFloat(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MDFloatGet::Type();};
		virtual float Data() const;
		virtual void Set(float aData);
		virtual TBool Set(Elem* aInp);
		virtual float Value();
		virtual void SetValue(float aData);
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		float mData;
	};
	template <class T> class HData: public HBase, public MDataGet<T> {
	    public:
		HData(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MVectGet<T>::Type();};
		// From MDataGet
		virtual void DataGet(T& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(Elem* aInp);
		T mData;
		static string mId;
	};
	template <class T> class HVect: public HBase, public MVectGet<T> {
	    public:
		HVect(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MVectGet<T>::Type();};
		// From MVFloatGet
		virtual void VectGet(Vect<T>& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(Elem* aInp);
		virtual TBool IsValid() const { return mValid;};
		Vect<T> mData;
		static string mId;
		TBool mValid;
	};
	template <class T> class HMtrd: public HBase, public MMtrdGet<T> {
	    public:
		HMtrd(DVar* aHost);
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
		// From MMtrdGet
		virtual void MtrdGet(Mtrd<T>& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(Elem* aInp);
		Mtrd<T> mData;
		static string mId;
	};
	// Matrix
	template <class T> class HMtr: public HBase, public MMtrGet<T> {
	    public:
		HMtr(DVar* aHost, const string& aCont);
		HMtr(DVar* aHost, const Mtr<T>& aData);
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MMtrdGet<T>::Type();};
		// From MMtrdGet
		virtual void MtrGet(Mtr<T>& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(Elem* aInp);
		virtual TBool IsValid() const { return mData.mValid;};
	    protected:
		Mtr<T> mData;
	};

	// Generic data
	template <class T> class HDt: public HBase, public MDtGet<T> {
	    public:
		HDt(DVar* aHost, const string& aCont);
		HDt(DVar* aHost, const T& aData);
		static HBase* Create(DVar* aHost, const string& aString, Elem* aInp = NULL);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual string IfaceGetId() const { return MDtGet<T>::Type();};
		// From MMtrdGet
		virtual void DtGet(T& aData);
		// From HBase
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		virtual TBool Set(Elem* aInp);
		virtual TBool IsValid() const { return mData.mValid;};
		virtual TBool IsSigOK() const { return mData.mSigTypeOK;};
	    protected:
		T mData;
	};
    public:
	static const char* Type() { return "DVar";};
	static string PEType();
	DVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	DVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~DVar();
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MUpdatable
	virtual TBool Update();
	// From DataBase
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	// From MDVarGet
	virtual Elem* VarGetBase();
	virtual string VarGetIfid() const;
	// From MDVarSet
	virtual Elem* VarSetBase();
    protected:
	Elem* GetInp();
	TBool Init(const string& aString, Elem* aInp = NULL);
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
	ConfDVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	ConfDVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~ConfDVar();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MConfirmable
	virtual TBool Confirm();
    protected:
	HBase* mDataPrep;
	HBase* mDataConf;
};
*/

#endif
