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
	virtual bool FromString(const string& aData); 
	virtual bool ToString(string& aData); 
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


#endif
