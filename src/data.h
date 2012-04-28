#ifndef __GRAYB_DATA_H
#define __GRAYB_DATA_H

#include "elem.h"
#include "mdata.h"

class DataBase: public Elem, public MACompsObserver
{
    public:
	static const char* Type() { return "DataBase";};
	DataBase(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
    protected:
	virtual bool FromString(const string& aData) = 0; 
	virtual bool ToString(string& aData) = 0; 
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
    protected:
	void UpdateProp();
};

// Data is representing some static model
//
// Data of integer
class DInt: public DataBase, public MDInt, public MDIntGet, public MDIntSet
{
    public:
	static const char* Type() { return "DInt";};
	DInt(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
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
    private:
	TInt mData;
};

#endif
