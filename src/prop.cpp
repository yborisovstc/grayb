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

TBool Prop::ChangeCont(const string& aVal, TBool aRtOnly)
{
    TBool res = ETrue;
    if (aVal != iValue) {
	iValue = aVal;
	if (aRtOnly) {
	    iMan->OnContentChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

void Prop::GetCont(string& aCont)
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


