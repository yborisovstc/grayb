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

#if 0
// Variant data. Supports multiple data types. The actual data type is configured on init phase
class DVar:  public Elem, public MACompsObserver, public MUpdatable, public MDataObserver, public MDVar, public MDVarGet, public MDVarSet
{
    public:
	// Data handler base
	class HBase: public Base {
	    public:
		HBase(DVar* aHost): Base(string()), mHost(*aHost) {};
		virtual TBool FromString(const string& aString) = 0;
		virtual void ToString(string& aString) = 0;
		virtual TBool Set(Elem* aInp) = 0;
		DVar& mHost;
	};
	class HInt: public HBase, public MDInt, public MDIntGet, public MDIntSet {
	    public:
		HInt(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual TInt Data() const;
		virtual void Set(TInt aData);
		virtual TBool Set(Elem* aInp);
		virtual TInt Value();
		virtual void SetValue(TInt aData);
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		TInt mData;
	};
    public:
	static const char* Type() { return "DVar";};
	static string PEType();
	DVar(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	DVar(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~DVar();
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MElem
	virtual void GetCont(string& aCont); 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue); 
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDataObserver
	virtual void OnDataChanged();
	// From MUpdatable
	virtual TBool Update();
	// From DataBase
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
	virtual TBool HandleIoChanged(Elem& aContext, Elem* aCp);
    protected:
	void NotifyUpdate();
	Elem* GetInp();
	TBool Init(const string& aString);
	TBool IsLogeventUpdate();
    private:
	HBase* mData;
};
#endif

// Variant data. Supports multiple data types. The actual data type is configured on init phase
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
		DVar& mHost;
	};
	class HInt: public HBase, public MDInt, public MDIntGet, public MDIntSet {
	    public:
		HInt(DVar* aHost): HBase(aHost), mData(0) {};
		static HBase* Create(DVar* aHost, const string& aString);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
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
		static HBase* Create(DVar* aHost, const string& aString);
		virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
		virtual float Data() const;
		virtual void Set(float aData);
		virtual TBool Set(Elem* aInp);
		virtual float Value();
		virtual void SetValue(float aData);
		virtual TBool FromString(const string& aString);
		virtual void ToString(string& aString);
		float mData;
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
	// From MDVarSet
	virtual Elem* VarSetBase();
    protected:
	Elem* GetInp();
	TBool Init(const string& aString);
    private:
	HBase* mData;
};

#endif
