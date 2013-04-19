#include "prop.h"


Prop::Prop(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
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

TBool Prop::ChangeCont(const string& aVal)
{
    TBool res = ETrue;
    if (aVal != iValue) {
	iValue = aVal;
	iMan->OnCompChanged(*this);
    }
    return res;
}

