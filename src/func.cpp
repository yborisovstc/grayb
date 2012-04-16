
#include "mvert.h"
#include "mdata.h"
#include "func.h"

FuncBase::FuncBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *FuncBase::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TBool FuncBase::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    Elem* caps = aContext.GetNode("Elem:Capsule");
    if (caps != NULL) {
	Elem* cp = caps->GetCompOwning("ConnPoint", &aComp);
	if (cp != NULL) {
	    MVert* vert = cp->GetObj(vert);
	    if (vert != NULL) {
		set<MVert*>& pairs = vert->Pairs();
		if (!pairs.empty()) {
		    MVert* pair = *pairs.begin();
		    res = HandleIoChanged(pair);
		}
	    }
	}
    }
    return res;
}

AIncInt::AIncInt(const string& aName, Elem* aMan, MEnv* aEnv): FuncBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type());
    SetParent(Type());
}

void *AIncInt::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = FuncBase::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AIncInt::SetRes(TInt aData)
{
    if (mData != aData) {
	mData = aData;
    }
}

TBool AIncInt::HandleIoChanged(MVert* aCp)
{
    TBool res = EFalse;
    MDIntGet* dget = aCp->EBase()->GetObj(dget);
    if (dget != NULL) {
	TInt val = dget->Value();
	SetRes(val + 1);
	res = ETrue;
    }
    return res;
}

TInt AIncInt::Value()
{
    return mData;
}

