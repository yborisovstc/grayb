
#ifndef __GRAYB_PROP_H
#define __GRAYB_PROP_H

#include "mprop.h"
#include "elem.h"

// Property
class Prop: public Elem, public MProp
{
    public:
	static const char* Type() { return "Prop";};
	static string PEType();
	Prop(const string& aName, Elem* aMan, MEnv* aEnv);
	Prop(Elem* aMan, MEnv* aEnv);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MProp
	virtual const string& Value() const;
	// From MElem
	virtual void GetCont(string& aCont); 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue); 
    protected:
	string iValue;
};

// Desctiption. Just prop introduced for describing of something
class Description: public Prop
{
    public:
	static const char* Type() { return "Description";};
	static string PEType();
	Description(const string& aName, Elem* aMan, MEnv* aEnv);
	Description(Elem* aMan, MEnv* aEnv);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

#endif
