
#include <stdlib.h>
#include <stdio.h>
#include "mprop.h"
#include "mvert.h"
#include "data.h"

DataBase::DataBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

TBool DataBase::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    if (aComp.Name() == "Value" && aComp.EType() == "Prop") {
	MProp* prop = aComp.GetObj(prop);
	if (prop == NULL) {
	    Logger()->WriteFormat("ERROR: [%s:%s] missing MProp iface in property [%s]", EType().c_str(), Name().c_str(), aComp.Name().c_str());
	}
	else {
	    FromString(prop->Value());
	    Logger()->WriteFormat("[%s:%s] value changed to [%s]", EType().c_str(), Name().c_str(), prop->Value().c_str());
	}
	res = ETrue;
    }
    return res;
}

void DataBase::NotifyUpdate()
{
    Elem* eout = GetNode("../../Elem:Capsule/ConnPoint:out");
    if (eout != NULL) {
	MVert* mvout = eout->GetObj(mvout);
	MVert* mpair = *(mvout->Pairs().begin());
	if (mpair != NULL) {
	    MDataObserver* obsr = mpair->EBase()->GetObj(obsr);
	    obsr->OnDataChanged();
	}
    }
}

void DataBase::UpdateProp()
{
    Elem* eprop = GetNode("../../Prop:Value");
    if (eprop != NULL) {
	string res;
	ToString(res);
	eprop->ChangeCont(res);
    }
}
	

DInt::DInt(const string& aName, Elem* aMan, MEnv* aEnv): DataBase(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *DInt::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else if (strcmp(aName, MDIntSet::Type()) == 0) {
	res = (MDIntSet*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt DInt::Data() const
{
    return mData;
}

void DInt::Set(TInt aData)
{
    if (mData != aData) {
	mData = aData;
	UpdateProp();
	NotifyUpdate();
    }
}

bool DInt::FromString(const string& aData)
{
    TInt data;
    sscanf(aData.c_str(), "%d", &data);
    Set(data);
}

bool DInt::ToString(string& aData)
{
    char buf[20];
    memset(buf, 0, 20);
    sprintf(buf, "%d", mData);
    aData.append(buf);
}

TInt DInt::Value()
{
    return mData;
}

void DInt::SetValue(TInt aData)
{
    Set(aData);
}

