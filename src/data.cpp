
#include <stdlib.h>
#include <stdio.h>
#include "mprop.h"
#include "mvert.h"
#include "data.h"
#include "vert.h"


string DataBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

DataBase::DataBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

TBool DataBase::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    if (aComp.Name() == "Value" && aComp.EType() == "Prop") {
	MProp* prop = aComp.GetObj(prop);
	TBool lupd = IsLogeventUpdate();
	GUri fullpath;
	if (lupd) {
	    Elem* host = iMan->GetMan();
	    host->GetUri(fullpath);
	}
	if (prop == NULL) {
	    Logger()->WriteFormat("ERROR: [%s:%s] missing MProp iface in property [%s]", EType().c_str(), Name().c_str(), aComp.Name().c_str());
	}
	else {
	    string curr;
	    ToString(curr);
	    if (lupd) {
		Logger()->WriteFormat("[%s]: Updated [%s <- %s]", fullpath.GetUri(ETrue).c_str(), curr.c_str(), prop->Value().c_str());
	    }
	    FromString(prop->Value());
	}
	res = ETrue;
    }
    return res;
}

void DataBase::NotifyUpdate()
{
    Elem* eout = GetNode("../../Elem:Capsule/ConnPoint:out");
    // TODO [YB] Scheme of getting iface should be enough to get MDataObserver directly from eout. Seems the chunk below is redundant.
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

void *DataBase::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MUpdatable::Type()) == 0) {
	res = (MUpdatable*) this;
    }
    else if (strcmp(aName, MDataObserver::Type()) == 0) {
	res = (MDataObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
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
	
TBool DataBase::IsLogeventUpdate() 
{
    Elem* node = GetNode("../../Elem:Logspec/DataLogevent:Update");
    return node != NULL;
}

void DataBase::OnDataChanged()
{
    // Update data
    Update();
}


string DInt::PEType()
{
    return DataBase::PEType() + GUri::KParentSep + Type();
}

DInt::DInt(const string& aName, Elem* aMan, MEnv* aEnv): DataBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type(), DataBase::PEType());
    SetParent(Type());
}

void *DInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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
	res = DataBase::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void DInt::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else if (strcmp(aName.c_str(), MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else if (strcmp(aName.c_str(), MDIntSet::Type()) == 0) {
	res = (MDIntSet*) this;
    }
    else {
	res = DataBase::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
    }
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

TBool DInt::Update()
{
    TBool res = EFalse;
    MDIntGet* inp = NULL;
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:inp");
    if (einp == NULL) {
	einp = GetNode("../../Elem:Capsule/ConnPoint:Inp");
    }
    if (einp != NULL) {
	Vert* vert = einp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    inp = pair->EBase()->GetObj(inp);
	    if (inp != NULL) {
		TInt idata = inp->Value();
		if (IsLogeventUpdate()) {
		    Elem* host = iMan->GetMan();
		    GUri fullpath;
		    host->GetUri(fullpath);
		    Logger()->WriteFormat("[%s]: Updated [%d <- %d]", fullpath.GetUri(ETrue).c_str(), mData, idata);
		}
		Set(idata);
		res = ETrue;
	    }
	}
    }
    // Attempt to use the iface getting rule basing on vert pairs was denied
    /*
       Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:Inp");
       if (einp != NULL) {
       MDIntGet* inp = einp->GetObj(inp);
       if (inp != NULL) {
       mData = inp->Value();
       }
       }
       */
    return res;
}



// Data of integer. Doesn't support nofification of change

string DNInt::PEType()
{
    return DInt::PEType() + GUri::KParentSep + Type();
}

DNInt::DNInt(const string& aName, Elem* aMan, MEnv* aEnv): DInt(aName, aMan, aEnv)
{
    SetEType(Type(), DInt::PEType());
    SetParent(Type());
}

void *DNInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = DInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void DNInt::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else {
	res = DInt::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
    }
}
void DNInt::Set(TInt aData)
{
    if (mData != aData) {
	mData = aData;
	UpdateProp();
    }
}

