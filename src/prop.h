
#ifndef __GRAYB_PROP_H
#define __GRAYB_PROP_H

#include "mprop.h"
#include "elem.h"

// Property
class Prop: public Elem, public MProp
{
    public:
	static const char* Type() { return "Prop";};
	Prop(const string& aName, Elem* aMan, MEnv* aEnv);
    public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MProp
	virtual const string& Value() const;
	// From MElem
	virtual TBool ChangeCont(const string& aVal); 
    protected:
	string iValue;
};

#endif
