
#include "mvert.h"
#include "mdata.h"
#include "func.h"

FuncBase::FuncBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
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
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AIncInt::SetRes(TInt aData)
{
    if (mData != aData) {
	mData = aData;
    }
}

TBool AIncInt::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    if (aComp.EType() == "ConnPoint") {
	MVert* vert = aComp.GetObj(vert);
	if (vert != NULL) {
	    set<MVert*>& pairs = vert->Pairs();
	    if (!pairs.empty()) {
		MVert* pair = *pairs.begin();
		MDIntGet* dget = pair->EBase()->GetObj(dget);
		if (dget != NULL) {
		    SetRes(mData + 1);
		    res = ETrue;
		}
	    }
	}

    }
    return res;
}

