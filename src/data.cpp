
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

DataBase::DataBase(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

TBool DataBase::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    if (aComp.Name() == "Value" && aComp.EType() == "Prop") {
	MProp* prop = aComp.GetObj(prop);
	if (prop == NULL) {
	    Logger()->Write(MLogRec::EErr, this, "Missing MProp iface in property [%s]", aComp.Name().c_str());
	}
	else {
	    string curr;
	    ToString(curr);
	    if (IsLogeventUpdate()) {
		Logger()->Write(MLogRec::EInfo, this, "Updated [%s <- %s]", prop->Value().c_str(), curr.c_str());
	    }
	    FromString(prop->Value());
	}
	res = ETrue;
    }
    else {
	Elem* caps = aContext.GetNode("Capsule");
	if (caps != NULL) {
	    Elem* cp = caps->GetCompOwning("ConnPoint", &aComp);
	    if (cp != NULL) {
		res = HandleIoChanged(aContext, cp);
	    }
	}
    }
    return res;
}

TBool DataBase::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    return EFalse;
}

void DataBase::NotifyUpdate()
{
    Elem* eout = GetNode("../../Capsule/out");
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
    Elem* eprop = GetNode("../../Value");
    if (eprop != NULL) {
	string res;
	ToString(res);
	eprop->ChangeCont(res);
    }
}
	
TBool DataBase::IsLogeventUpdate() 
{
    Elem* node = GetNode("../../Logspec/Update");
    return node != NULL;
}

void DataBase::OnDataChanged()
{
    // Update data
    Update();
}

TBool DataBase::Update()
{
    return EFalse;
}

TBool DataBase::FromString(const string& aData) 
{
    return EFalse;
}

bool DataBase::ToString(string& aData) 
{
    return EFalse;
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

DInt::DInt(Elem* aMan, MEnv* aEnv): DataBase(Type(), aMan, aEnv), mData(0)
{
    SetEType(DataBase::PEType());
    SetParent(DataBase::PEType());
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
    Elem* einp = GetNode("../../Capsule/inp");
    if (einp == NULL) {
	einp = GetNode("../../Capsule/Inp");
    }
    if (einp != NULL) {
	Vert* vert = einp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    inp = pair->EBase()->GetObj(inp);
	    if (inp != NULL) {
		TInt idata = inp->Value();
		if (IsLogeventUpdate()) {
		    Logger()->Write(MLogRec::EInfo, this, "Updated [%d <- %d]", idata, mData);
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

DNInt::DNInt(Elem* aMan, MEnv* aEnv): DInt(Type(), aMan, aEnv)
{
    SetEType(DInt::PEType());
    SetParent(DInt::PEType());
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



// Variant data
string DVar::PEType()
{
    return DataBase::PEType() + GUri::KParentSep + Type();
}

DVar::DVar(const string& aName, Elem* aMan, MEnv* aEnv): DataBase(aName, aMan, aEnv), mData(NULL)
{
    SetEType(Type(), DataBase::PEType());
    SetParent(Type());
}

DVar::DVar(Elem* aMan, MEnv* aEnv): DataBase(Type(), aMan, aEnv), mData(NULL)
{
    SetEType(DataBase::PEType());
    SetParent(DataBase::PEType());
}

void *DVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVar::Type()) == 0) {
	res = (MDVar*) this;
    }
    else {
	res = DataBase::DoGetObj(aName, aIncUpHier);
    }
    if (res == NULL && mData != NULL) {
	res = mData->DoGetObj(aName, aIncUpHier);
    }
    return res;
}

DVar::~DVar()
{
    if (mData != NULL) {
	delete mData;
    }
}

TBool DVar::ChangeCont(const string& aVal, TBool aRtOnly)
{
    TBool res = ETrue;
    if (aVal != mContent) {
	mContent = aVal;
	if (mData == NULL) {
	    res = Init(mContent);
	}
	if (res)  {
	    mData->FromString(mContent);
	}
	if (aRtOnly) {
	    iMan->OnContentChanged(*this);
	} else {
	    iMan->OnCompChanged(*this);
	}
    }
    return res;
}

void DVar::GetCont(string& aCont)
{
    aCont = mContent;
}

TBool DVar::Init(const string& aString)
{
    if ((mData = HInt::Create(this, aString)) != NULL);
    return mData != NULL;
}

TBool DVar::FromString(const string& aData) 
{
    TBool res = EFalse;
    res = mData->FromString(aData);
    if (res) {
	NotifyUpdate();
    }
    return res;
}

bool DVar::ToString(string& aData) 
{
}

TBool DVar::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    if (aCp->Name() == "inp") {
	// Check input change
	mData->Set(aCp);
    }
    else if (aCp->Name() == "out") {
	UpdateProp();
	NotifyUpdate();
    }
    return res;
}

TBool DVar::Update()
{
    TBool res = EFalse;
    string old_value;
    mData->ToString(old_value);
    res = mData->Update();
    if (res && IsLogeventUpdate()) {
	string new_value;
	mData->ToString(new_value);
	Logger()->Write(MLogRec::EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
    }
    return res;
}

// Int data
void *DVar::HInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDInt::Type()) == 0) res = (MDInt*) this;
    else if (strcmp(aName, MDIntGet::Type()) == 0) res = (MDIntGet*) this;
    else if (strcmp(aName, MDIntSet::Type()) == 0)  res = (MDIntSet*) this;
    return res;
}

DVar::HBase* DVar::HInt::Create(DVar* aHost, const string& aString)
{
    HBase* res = NULL;
    if (aString.at(0) == 'I') {
	res = new HInt(aHost);
    }
    return res;
}

TBool DVar::HInt::FromString(const string& aString)
{
    TInt data;
    sscanf(aString.c_str(), "%d", &data);
    Set(data);
}

void DVar::HInt::ToString(string& aString)
{
    stringstream ss;
    ss << mData;
    aString = ss.str();
}

TInt DVar::HInt::Data() const
{
    return mData;
}

void DVar::HInt::Set(TInt aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.UpdateProp();
    }
}

void DVar::HInt::Set(Elem* aInp)
{
    MDIntGet* dget = (MDIntGet*) aInp->GetSIfi(MDIntGet::Type());
    if (dget != NULL) {
	TInt val = dget->Value();
	Set(val);
    }
}

TInt DVar::HInt::Value()
{
    return mData;
}

void DVar::HInt::SetValue(TInt aData)
{
    Set(aData);
}

TBool DVar::HInt::Update()
{
    TBool res = EFalse;
    Elem* einp = mHost.GetNode("../../Capsule/inp");
    if (einp == NULL) {
	einp = mHost.GetNode("../../Capsule/Inp");
    }
    if (einp != NULL) {
	MDIntGet* inp = (MDIntGet*) einp->GetSIfi(MDIntGet::Type());
	if (inp != NULL) {
	    TInt idata = inp->Value();
	    Set(idata);
	    res = ETrue;
	}
    }
    return res;
}
