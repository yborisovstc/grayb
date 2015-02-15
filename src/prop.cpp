#include "prop.h"


string Prop::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Prop::Prop(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

Prop::Prop(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

void *Prop::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
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
	    res = iMan->OnContentChanged(*this);
	} else {
	    res = iMan->OnCompChanged(*this);
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
    SetEType(Type(), Prop::PEType());
    SetParent(Type());
}

Description::Description(Elem* aMan, MEnv* aEnv): Prop(Type(), aMan, aEnv)
{
    SetEType(Prop::PEType());
    SetParent(Prop::PEType());
}

void *Description::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MProp::Type()) == 0) {
	res = (MProp*) this;
    }
    else {
	res = Prop::DoGetObj(aName, aIncUpHier, aCtx);
    }
    return res;
}


