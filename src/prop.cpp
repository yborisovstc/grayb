#include "prop.h"


string Prop::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Prop::Prop(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

Prop::Prop(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *Prop::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

const string& Prop::Value() const
{
    return iValue;
}

TBool Prop::IsContChangeable(const string& aName) const
{
    return ETrue;
}

TBool Prop::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    //if (aVal != iValue) {
    if (true) {
	iValue = aVal;
	if (aRtOnly) {
	    iMan->OnContentChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

void Prop::GetCont(string& aCont, const string& aName)
{
    aCont = iValue;
}



string Description::PEType()
{
    return Prop::PEType() + GUri::KParentSep + Type();
}

Description::Description(const string& aName, Elem* aMan, MEnv* aEnv): Prop(aName, aMan, aEnv)
{
    SetParent(Type());
}

Description::Description(Elem* aMan, MEnv* aEnv): Prop(Type(), aMan, aEnv)
{
    SetParent(Prop::PEType());
}

void *Description::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
    } else {
	res = Prop::DoGetObj(aName);
    }
    return res;
}


