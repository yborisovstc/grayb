
#include "mvert.h"
#include "vert.h"
#include "prop.h"
#include "mdata.h"
#include "func.h"

FuncBase::FuncBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *FuncBase::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else if (strcmp(aName, MDataObserver::Type()) == 0) {
	res = (MDataObserver*) this;
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
	    res = HandleIoChanged(aContext, cp);
	}
    }
    return res;
}

void FuncBase::OnDataChanged()
{
}

void FuncBase::NotifyUpdate()
{
    Elem* eout = GetNode("../../Elem:Capsule/ConnPoint:out");
    if (eout != NULL) {
	MVert* mvout = eout->GetObj(mvout);
	MVert* mpair = *(mvout->Pairs().begin());
	if (mpair != NULL) {
	    MDataObserver* obsr = mpair->EBase()->GetObj(obsr);
	    if (obsr != NULL) {
		obsr->OnDataChanged();
	    }
	}
    }
}

// Agent base of Int function
AFunInt::AFunInt(const string& aName, Elem* aMan, MEnv* aEnv): FuncBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type());
    SetParent(Type());
}

void *AFunInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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

void AFunInt::SetRes(TInt aData)
{
    if (mData != aData) {
	Logger()->WriteFormat("AFunInt [%s] updated [%d <- %d]", Name().c_str(), mData, aData);
	mData = aData;
	NotifyUpdate();
    }
}

TInt AFunInt::Value()
{
    return mData;
}

MDIntGet* AFunInt::GetInp(const string& aInpName)
{
    MDIntGet* res = NULL;
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:" + aInpName);
    if (einp != NULL) {
	Vert* vert = einp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    res = pair->EBase()->GetObj(res);
	}
	// Attempt to use the iface getting rule basing on vert pairs was denied
	// res = einp->GetObj(res);
    }
    return res;
}


AIncInt::AIncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *AIncInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TBool AIncInt::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    // Checking input change
    if (aCp->Name() == "inp") {
	/*
	Vert* vert = aCp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    Vert* vpair = pair->EBase()->GetObj(vpair);
	    MDIntGet* dget = vpair->GetObj(dget);
	    if (dget != NULL) {
		TInt val = dget->Value();
		SetRes(val + 1);
		res = ETrue;
	    }
	}
	*/
	MDIntGet* dget = aCp->GetObj(dget);
	if (dget != NULL) {
	    TInt val = dget->Value();
	    SetRes(val + 1);
	    res = ETrue;
	}
    }
    return res;
}

void AIncInt::OnDataChanged()
{
    MDIntGet* mget = GetInp("inp");
    TInt val = mget->Value();
    SetRes(val + 1);
}

// Agent of Int function result
AFunIntRes::AFunIntRes(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *AFunIntRes::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = FuncBase::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TBool AFunIntRes::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    Vert* vert = aCp->GetObj(vert);
    MVert* pair = *(vert->Pairs().begin());
    if (pair != NULL) {
	Vert* vpair = pair->EBase()->GetObj(vpair);
	if (aCp->Name() == "inp") {
	    // Check input change
	    MDIntGet* dget = vpair->GetObj(dget);
	    if (dget != NULL) {
		TInt val = dget->Value();
		SetRes(val);
		UpdateOutp();
		res = ETrue;
	    }
	}
	else if (aCp->Name() == "out") {
	    UpdateOutp();
	}
    }
    return res;
}

void AFunIntRes::UpdateOutp()
{
    Elem* out = GetNode("../../Elem:Capsule/ConnPoint:out");
    if (out != NULL) {
	Vert* vert = out->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    Vert* vpair = pair->EBase()->GetObj(vpair);
	    MDIntSet* dset = vpair->GetObj(dset);
	    if (dset != NULL) {
		dset->SetValue(mData);
	    }
	}
    }
}

void AFunIntRes::OnDataChanged()
{
    MDIntGet* mget = GetInp("inp");
    TInt val = mget->Value();
    SetRes(val);
    UpdateOutp();
}
