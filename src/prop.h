
#ifndef __GRAYB_PROP_H
#define __GRAYB_PROP_H

#include "mprop.h"
#include "elem.h"

class MProp_Imd: public MProp
{
    virtual MIface* MProp_Call(const string& aSpec, string& aRes) = 0;
    virtual string MProp_Mid() const = 0;
    // From MIface
    virtual MIface* Call(const string& aSpec, string& aRes) { return MProp_Call(aSpec, aRes);};
    virtual string Mid() const { return MProp_Mid();};
};

// Property
class Prop: public Elem, public MProp_Imd
{
    public:
	static const char* Type() { return "Prop";};
	static string PEType();
	Prop(const string& aName, MElem* aMan, MEnv* aEnv);
	Prop(MElem* aMan, MEnv* aEnv);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MProp
	virtual const string& Value() const;
	// From MElem
	virtual string GetContent(const string& aName=string()) const; 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	// From MProp_Imd
	virtual MIface* MProp_Call(const string& aSpec, string& aRes);
	virtual string MProp_Mid() const;
    protected:
	string iValue;
};

// Desctiption. Just prop introduced for describing of something
class Description: public Prop
{
    public:
	static const char* Type() { return "Description";};
	static string PEType();
	Description(const string& aName, MElem* aMan, MEnv* aEnv);
	Description(MElem* aMan, MEnv* aEnv);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
};

#endif
