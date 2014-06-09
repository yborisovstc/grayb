
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
	RqContext ctx(this);
	TIfRange rg = eout->GetIfi(MDataObserver::Type(), &ctx);
	for (IfIter it = rg.first; it != rg.second; it++) {
	    MDataObserver* obsr = (MDataObserver*) (*it);
	    obsr->OnDataChanged();
	}
	/*
	MVert* mvout = eout->GetObj(mvout);
	MVert* mpair = *(mvout->Pairs().begin());
	if (mpair != NULL) {
	    MDataObserver* obsr = mpair->EBase()->GetObj(obsr);
	    if (obsr != NULL) {
		obsr->OnDataChanged();
	    }
	}
	*/
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
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
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
	InsertIfCache(aName, aCtx, this, res);
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
	InsertIfCache(aName, aCtx, this, res);
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
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else if (strcmp(aName, MDVarSet::Type()) == 0) {
	res = (MDVarSet*) this;
    }
    else {
	res = DataBase::DoGetObj(aName, aIncUpHier);
    }
    if (res == NULL) {
	if (mData == NULL) {
	    Init(aName);
	}
	if (mData != NULL) {
	    res = mData->DoGetObj(aName, aIncUpHier);
	}
    }
    return res;
}

string DVar::VarGetIfid() const
{
    return mData != NULL ? mData->IfaceGetId() : string();
}

Elem* DVar::VarGetBase() 
{
    return this;
}

Elem* DVar::VarSetBase() 
{
    return this;
}

DVar::~DVar()
{
    if (mData != NULL) {
	delete mData;
    }
}

TBool DVar::Init(const string& aString)
{
    if (mData != NULL) {
	delete mData;
	mData == NULL;
    }
    if ((mData = HInt::Create(this, aString)) != NULL);
    else if ((mData = HFloat::Create(this, aString)) != NULL);
    else if ((mData = HVFloat::Create(this, aString)) != NULL);
    else if ((mData = HBool::Create(this, aString)) != NULL);
    if (mData != NULL) {
	mData->FromString(aString);
    }
    return mData != NULL;
}

TBool DVar::FromString(const string& aData) 
{
    TBool res = EFalse;
    if (mData == NULL) {
	res = Init(aData);
    }
    if (mData != NULL) {
	res = mData->FromString(aData);
	if (!res) {
	    Init(aData);
	}
    }
    if (res) {
	NotifyUpdate();
    }
    return res;
}

bool DVar::ToString(string& aData) 
{
    TBool res = EFalse;
    if (mData == NULL) {
	res = Init(aData);
    }
    if (mData != NULL) {
	mData->ToString(aData);
    }
    return res;

}

TBool DVar::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    if (aCp->Name() == "inp" || aCp->Name() == "Inp") {
	// Check input change
	if (mData != NULL) {
	    Update();
	}
    }
    else if (aCp->Name() == "out") {
	NotifyUpdate();
    }
    return res;
}

TBool DVar::Update()
{
    TBool res = EFalse;
    string old_value;
    mData->ToString(old_value);
    Elem* inp = GetInp();
    if (inp != NULL) {
	RqContext ctx(this);
	MDVarGet* vget = (MDVarGet*) inp->GetSIfi(MDVarGet::Type(), &ctx);
	if (vget != NULL) {
	    Elem* eget = vget->VarGetBase();
	    if (eget != NULL) {
		res = mData->Set(eget);
		if (res && IsLogeventUpdate()) {
		    string new_value;
		    mData->ToString(new_value);
		    Logger()->Write(MLogRec::EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
		}
	    }
	}
    }
    return res;
}

Elem* DVar::GetInp()
{
    Elem* einp = GetNode("../../Capsule/inp");
    if (einp == NULL) {
	einp = GetNode("../../Capsule/Inp");
    }
    return einp;
}

TBool DVar::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    DataBase::HandleCompChanged(aContext, aComp);
}

// Bool data
void *DVar::HBool::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDBoolGet::Type()) == 0) res = (MDBoolGet*) this;
    return res;
}

DVar::HBase* DVar::HBool::Create(DVar* aHost, const string& aString)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'B') {
	res = new HBool(aHost);
    }
    return res;
}

TBool DVar::HBool::FromString(const string& aString)
{
    TBool res = EFalse;
    if (aString.at(0) == 'B') {
	istringstream ss(aString.substr(2));
	ss >> std::boolalpha >> res;
	Set(res);
    }
    return ETrue;
}

void DVar::HBool::ToString(string& aString)
{
    stringstream ss;
    ss << "B " << std::boolalpha << mData;
    aString = ss.str();
}

void DVar::HBool::Set(TBool aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.UpdateProp();
	mHost.NotifyUpdate();
    }
}

TBool DVar::HBool::Set(Elem* aInp)
{
    TBool res = EFalse;
    MDBoolGet* dget = (MDBoolGet*) aInp->GetObj(dget);
    if (dget != NULL) {
	TBool val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

TBool DVar::HBool::Value()
{
    return mData;
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
    if (!aString.empty() && aString.at(0) == 'I') {
	res = new HInt(aHost);
    }
    return res;
}

TBool DVar::HInt::FromString(const string& aString)
{
    TBool res = EFalse;
    TInt data;
    if (aString.at(0) == 'I' && aString.at(1) == ' ') {
	sscanf(aString.c_str(), "I %d", &data);
	Set(data);
	res = ETrue;
    }
    return res;
}

void DVar::HInt::ToString(string& aString)
{
    stringstream ss;
    ss << "I " << mData;
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
	mHost.NotifyUpdate();
    }
}

TBool DVar::HInt::Set(Elem* aInp)
{
    TBool res = EFalse;
    MDIntGet* dget = (MDIntGet*) aInp->GetObj(dget);
    if (dget != NULL) {
	TInt val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

TInt DVar::HInt::Value()
{
    return mData;
}

void DVar::HInt::SetValue(TInt aData)
{
    Set(aData);
}

// Float data
void *DVar::HFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDFloat::Type()) == 0) res = (MDFloat*) this;
    else if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    else if (strcmp(aName, MDFloatSet::Type()) == 0)  res = (MDFloatSet*) this;
    return res;
}

DVar::HBase* DVar::HFloat::Create(DVar* aHost, const string& aString)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'F') {
	res = new HFloat(aHost);
    }
    return res;
}

TBool DVar::HFloat::FromString(const string& aString)
{
    TBool res = EFalse;
    float data;
    if (aString.at(0) == 'F') {
	istringstream ss(aString.substr(2));
	ss >> data;
	Set(data);
	res = ETrue;
    }
    return res;
}

void DVar::HFloat::ToString(string& aString)
{
    stringstream ss;
    ss << "F " << mData;
    aString = ss.str();
}

float DVar::HFloat::Data() const
{
    return mData;
}

void DVar::HFloat::Set(float aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.UpdateProp();
	mHost.NotifyUpdate();
    }
}

TBool DVar::HFloat::Set(Elem* aInp)
{
    TBool res = EFalse;
    MDFloatGet* dget = (MDFloatGet*) aInp->GetObj(dget);
    if (dget != NULL) {
	float val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

float DVar::HFloat::Value()
{
    return mData;
}

void DVar::HFloat::SetValue(float aData)
{
    Set(aData);
}

// Float vector

string DVar::HVFloat::mId = "VF";

void *DVar::HVFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVFloatGet::Type()) == 0) res = (MVFloatGet*) this;
    return res;
}

DVar::HBase* DVar::HVFloat::Create(DVar* aHost, const string& aString)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'V' && aString.at(1) == 'F') {
	res = new HVFloat(aHost);
    }
    return res;
}

TBool DVar::HVFloat::FromString(const string& aString)
{
    TBool res = ETrue;
    int res1 = 0;
    string ss;
    int beg = 0, end = 0;
    end = aString.find(' ');
    ss = aString.substr(beg, end);
    if (ss == mId) {
	mData.clear();
	TBool fin = EFalse;
	do {
	    float data;
	    beg = end + 1;
	    end = aString.find(' ', beg);
	    ss = aString.substr(beg, end - beg);
	    if (!ss.empty()) {
		istringstream sstr(ss);
		sstr >> data;
		ios_base::iostate state = sstr.rdstate();
		if (!sstr.fail()) {
		    mData.push_back(data);
		}
		else {
		    res = EFalse;
		}
	    }
	    if (end == string::npos) {
		fin = ETrue;
	    }
	} while (!fin);
    }
    else {
	res = EFalse;
    }
    return res;
}

void DVar::HVFloat::ToString(string& aString)
{
    stringstream ss;
    ss << "VF";
    for (VFloat::iterator it = mData.begin(); it != mData.end(); it++) {
	float data = *it;
	ss << " " << data;
    }
    aString = ss.str();
}

TBool DVar::HVFloat::Set(Elem* aInp)
{
    TBool res = EFalse;
    MVFloatGet* dget = (MVFloatGet*) aInp->GetObj(dget);
    if (dget != NULL) {
	dget->VFloatGet(mData);
	mHost.UpdateProp();
	mHost.NotifyUpdate();
	res = ETrue;
    }
    return res;
}

void DVar::HVFloat::VFloatGet(VFloat& aData)
{
    aData = mData;
}

