
#include <stdlib.h>
#include <stdio.h>
#include "mprop.h"
#include "mvert.h"
#include "data.h"
#include "vert.h"
#include "rmutdata.h"

const string DataBase::KCont_Value = "Value";

string DataBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

DataBase::DataBase(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    if (iMan != NULL) { // There is a context
	MUnit* ctx = iMan->GetMan();
	ctx->ChangeCont("", ETrue, KCont_Value);
    }
}

MIface* DataBase::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
}

// TODO [YB] Bug ds_di_wsucv: Wrong scheme of using content "Value" in DataBase
// To fix
TBool DataBase::HandleCompChanged(MUnit* aContext, MUnit* aComp, const string& aContName)
{
    TBool res = ETrue;
    if ((aComp->Name() == "Value" || aComp->Name() == "Type") && aComp->EType() == "Prop") {
	MUnit* etype = aContext->GetNode("./Type");
	MUnit* eval = aContext->GetNode("./Value");
	__ASSERT(eval != NULL);
	MProp* prop = eval->GetObj(prop);
	if (prop == NULL) {
	    Logger()->Write(EErr, this, "Missing MProp iface in property [%s]", aComp->Name().c_str());
	    res = EFalse;
	}
	else {
	    string curr;
	    ToString(curr);
	    if (IsLogeventUpdate()) {
		Logger()->Write(EInfo, this, "Updated via content [%s <- %s]", prop->Value().c_str(), curr.c_str());
	    }
	    MProp* ptype = etype == NULL ? NULL: etype->GetObj(ptype);
	    string stype = ptype == NULL ? string() : ptype->Value();
	    FromString(stype, prop->Value());
	}
    } else if (aComp == Context() && aContName == KCont_Value) { // Value - multicontent
	string val = Context()->GetContent(KCont_Value);
	if (IsLogeventUpdate()) {
	    string curr;
	    ToString(curr);
	    Logger()->Write(EInfo, this, "Updated via content [%s <- %s]", val.c_str(), curr.c_str());
	}
	FromString(string(), val);
    } else {
	MUnit* caps = aContext->GetNode("./Capsule");
	if (caps != NULL) {
	    MUnit* cp = caps->GetCompOwning("ConnPointInp", aComp);
	    if (cp == NULL) {
		cp = caps->GetCompOwning("ConnPointOut", aComp);
	    }
	    if (cp == NULL) {
		cp = caps->GetCompOwning("ConnPointInpMc", aComp);
	    }
	    if (cp == NULL) {
		cp = caps->GetCompOwning("ConnPointOutMc", aComp);
	    }
	    if (cp != NULL) {
		res = HandleIoChanged(*aComp, cp);
	    }
	}
    }
    return res;
}

TBool DataBase::HandleIoChanged(MUnit& aContext, MUnit* aCp)
{
    return ETrue;
}

void DataBase::NotifyUpdate()
{
    MUnit* eout = GetNode("./../Capsule/out");
    // TODO [YB] Scheme of getting iface should be enough to get MDataObserver directly from eout. Seems the chunk below is redundant.
    if (eout != NULL) {
	TIfRange rg = eout->GetIfi(MDataObserver::Type(), this);
	for (TIfIter it = rg.first; it != rg.second; it++) {
	    MDataObserver* obsr = (MDataObserver*) (*it);
	    obsr->OnDataChanged();
	}
    }
    // Also notify context in order to propagate the update event to observer
    if (iMan != NULL) {
	iMan->OnChanged(this);
    }
    iEnv->Observer()->OnChanged(this);
}

MIface *DataBase::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MUpdatable::Type()) == 0) {
	res = (MUpdatable*) this;
    }
    else if (strcmp(aName, MDataObserver::Type()) == 0) {
	res = (MDataObserver*) this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else if (strcmp(aName, MAgent::Type()) == 0) {
	res = dynamic_cast<MAgent*>(this);
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void DataBase::UpdateProp()
{
    MUnit* eprop = GetNode("./../Value");
    if (eprop != NULL) {
	MUnit* etype = GetNode("./../Type");
	if (etype == NULL) {
	    string res;
	    ToString(res);
	    eprop->ChangeCont(res);
	}
	else {
	    string data, type;
	    ToString(type, data);
	    etype->ChangeCont(type);
	    eprop->ChangeCont(data);
	}
    } else { // Multicontent ?
	__ASSERT(Context() != NULL);
	string val = Context()->GetContent(KCont_Value);
	string res;
	ToString(res);
	Context()->ChangeCont(res, ETrue, KCont_Value);
    }
}
	
TBool DataBase::IsLogeventUpdate() 
{
    MUnit* node = GetNode("./../Logspec/Update");
    string upd = GetMan()->GetContent("Debug.Update");
    return node != NULL || upd == "y";
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

TBool DataBase::FromString(const string& aType, const string& aData)
{
    TBool res = EFalse;
    if (aType.empty()) {
	res = FromString(aData);
    } else {
	res = FromString(aType + string(1, DtBase::mKTypeToDataSep) + aData);
    }
    return res;
}

TBool DataBase::FromString(const string& aData) 
{
    return EFalse;
}

bool DataBase::ToString(string& aData) 
{
    return EFalse;
}

bool DataBase::ToString(string& aType, string& aData)
{
    string full;
    ToString(full);
    int end = full.find(DtBase::mKTypeToDataSep);
    aType = full.substr(0, end);
    aData = full.substr(end + 1, string::npos);
    return ETrue; // TODO YB to form return value
}

/*
TBool DataBase::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = Elem::ChangeCont(aVal, aRtOnly, aName);
    if (aName == KCont_Value && res) {
	string curr;
	ToString(curr);
	if (IsLogeventUpdate()) {
	    Logger()->Write(EInfo, this, "Updated [%s <- %s]", aVal.c_str(), curr.c_str());
	}
	FromString(aVal);
    }
    return res;
}
*/

string DInt::PEType()
{
    return DataBase::PEType() + GUri::KParentSep + Type();
}

DInt::DInt(const string& aName, MUnit* aMan, MEnv* aEnv): DataBase(aName, aMan, aEnv)/*, mDIntGet(*this)*/, mData(0)
{
    SetCrAttr(PEType(), aName);
}

MIface* DInt::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MDIntGet::Type())
	res = dynamic_cast<MDIntGet*>(this);
    else 
	res = DataBase::MAgent_DoGetIface(aName);
    return res;
}

MIface *DInt::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDInt::Type()) == 0) {
	res = (MDInt*) this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = dynamic_cast<MDIntGet*>(this);
    }
    else if (strcmp(aName, MDIntSet::Type()) == 0) {
	res = (MDIntSet*) this;
    }
    else {
	res = DataBase::DoGetObj(aName);
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
	// TODO if set on Value property change, then UpdateProp causes same property change again. To avoid
	UpdateProp();
	NotifyUpdate();
    }
}

bool DInt::FromString(const string& aData)
{
    TInt data;
    sscanf(aData.c_str(), "%d", &data);
    Set(data);
    return ETrue; // TODO YB get non trivial ret val
}

bool DInt::ToString(string& aData)
{
    char buf[20];
    memset(buf, 0, 20);
    sprintf(buf, "%d", mData);
    aData.append(buf);
    return ETrue; // TODO YB get non trivial ret val
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
    MUnit* einp = GetNode("./../Capsule/inp");
    if (einp == NULL) {
	einp = GetNode("./../Capsule/Inp");
    }
    if (einp != NULL) {
	MVert* vert = einp->GetObj(vert);
	MVert* pair = vert->GetPair(0);
	if (pair != NULL) {
	    MUnit* inpe = pair->GetObj(inpe);
	    inp = (MDIntGet*) inpe->GetSIfi(MDIntGet::Type(), this);
	    if (inp != NULL) {
		TInt idata = inp->Value();
		if (IsLogeventUpdate()) {
		    Logger()->Write(EInfo, this, "Updated [%d <- %d]", idata, mData);
		}
		Set(idata);
		res = ETrue;
	    }
	}
    }
    return res;
}

MIface* DInt::MDIntGet_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MCompatChecker::mIfu.CheckMname(name);
    if (!name_ok)
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MCompatChecker::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "Value") {
	TInt rr = Value();
	aRes = Ifu::FromInt(rr);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}


// Data of integer. Doesn't support nofification of change

string DNInt::PEType()
{
    return DInt::PEType() + GUri::KParentSep + Type();
}

DNInt::DNInt(const string& aName, MUnit* aMan, MEnv* aEnv): DInt(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

DVar::DVar(const string& aName, MUnit* aMan, MEnv* aEnv): DataBase(aName, aMan, aEnv), mData(NULL)
{
    SetCrAttr(PEType(), aName);
}

MIface *DVar::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDVar::Type()) == 0) {
	res = (MDVar*) this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else if (strcmp(aName, MDVarSet::Type()) == 0) {
	res = (MDVarSet*) this;
    }
    else {
	res = DataBase::DoGetObj(aName);
    }
    return res;
}

string DVar::VarGetIfid()
{
    return mData != NULL ? mData->IfaceGetId() : string();
}

string DVar::VarGetSIfid()
{
    return mData != NULL ? mData->IfaceSGetId() : string();
}

void *DVar::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (mData == NULL) {
	Init(aName);
    }
    if (mData != NULL) {
	res = mData->DoGetObj(aName);
    }
    return res;
}

MIface *DVar::DoGetSDObj(const char *aName)
{
    MIface* res = NULL;
    if (mData != NULL) {
	res = mData->DoGetObj(aName);
    }
    return res;
}

DVar::~DVar()
{
    if (mData != NULL) {
	delete mData;
    }
}

TBool DVar::Init(const string& aString, MDVarGet* aInpv)
{
    TBool res = EFalse;
    if (mData != NULL) {
	delete mData;
	mData == NULL;
    }
    if ((mData = HInt::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HFloat::Create(this, aString, aInpv)) != NULL);
    // else if ((mData = HMtr<float>::Create(this, aString, aInp)) != NULL);
    else if ((mData = HDt<Sdata <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <bool> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <string> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<NTuple>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Enum>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<DMut>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<string>>::Create(this, aString, aInpv)) != NULL);
    //else if ((mData = HVect<float>::Create(this, aString, aInpv)) != NULL);
    //else if ((mData = HMtrd<float>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HBool::Create(this, aString, aInpv)) != NULL);
    /*
       if (mData != NULL && !aString.empty()) {
       res = mData->FromString(aString);
       }
       */
    res = mData != NULL;
    return res;
}

TBool DVar::FromString(const string& aData) 
{
    TBool res = EFalse;
    if (mData == NULL) {
	res = Init(aData);
    }
    if (mData != NULL) {
	res = mData->FromString(aData);
	if (!res && !mData->IsSigOK()) {
	    // Signature get's not fit, reinit
	    Init(aData);
	    res = ETrue;
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

TBool DVar::HandleIoChanged(MUnit& aContext, MUnit* aCp)
{
    TBool res = ETrue;
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
    ToString(old_value);
    MUnit* inp = GetInp();
    if (inp != NULL) {
	MDVarGet* vget = (MDVarGet*) inp->GetSIfi(MDVarGet::Type(), this);
	if (vget != NULL) {
	    if (mData == NULL) {
		Init(string(), vget);
	    }
	    if (mData != NULL) {
		res = mData->Set(vget);
		/* UpdateProp and NotifyUpdate already done in Set()
		   if (res) {
		   UpdateProp();
		   NotifyUpdate();
		   if (IsLogeventUpdate()) {
		   string new_value;
		   ToString(new_value);
		   Logger()->Write(EInfo, this, "Updated2 [%s <- %s]", new_value.c_str(), old_value.c_str());
		   }
		   }
		   */
		if (res && IsLogeventUpdate()) {
		    string new_value;
		    ToString(new_value);
		    Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
		}

	    }
	}
    }
    return res;
}

MUnit* DVar::GetInp()
{
    MUnit* einp = GetNode("./../Capsule/inp");
    if (einp == NULL) {
	einp = GetNode("./../Capsule/Inp");
    }
    return einp;
}

TBool DVar::HandleCompChanged(MUnit* aContext, MUnit* aComp, const string& aContName)
{
    return DataBase::HandleCompChanged(aContext, aComp, aContName);
}

string DVar::GetContent(const string& aName, TBool aFull) const
{
    string res;
    if (aName == "") {
	((DVar*) this)->ToString(res);
    } else { 
	res = Elem::GetContent(aName, aFull);
    }
    return res;
}



// Bool data
MIface *HBool::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDBoolGet::Type()) == 0) res = (MDBoolGet*) this;
    return res;
}

HBase* HBool::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'B') {
	res = new HBool(aHost);
    }
    if (res == NULL && aInp != NULL) {
	MDBoolGet* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HBool(aHost);
	}
    }
    return res;
}

TBool HBool::FromString(const string& aString)
{
    TBool res = EFalse;
    if (aString.at(0) == 'B') {
	istringstream ss(aString.substr(2));
	TBool data;
	ss >> std::boolalpha >> data;
	Set(data);
	res = ETrue;
    }
    return res;
}

void HBool::ToString(string& aString)
{
    stringstream ss;
    ss << "B " << std::boolalpha << mData;
    aString = ss.str();
}

void HBool::Set(TBool aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
    }
}

TBool HBool::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MDBoolGet* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	TBool val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

TBool HBool::Value()
{
    return mData;
}


// Int data
MIface *HInt::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDInt::Type()) == 0) res = (MDInt*) this;
    else if (strcmp(aName, MDIntGet::Type()) == 0) res = (MDIntGet*) this;
    else if (strcmp(aName, MDIntSet::Type()) == 0)  res = (MDIntSet*) this;
    return res;
}

HBase* HInt::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HInt* res = NULL;
    if (!aString.empty() && aString.at(0) == 'I') {
	res = new HInt(aHost);
	if (res != NULL && aString.at(1) == ' ') {
	    // Set up the value
	    TInt data = 0;
	    sscanf(aString.c_str(), "I %d", &data);
	    res->Set(data);
	}
    }
    if (res == NULL && aInp != NULL) {
	// TODO [YB] Is this case used at all? To check up and remove if not
	MDIntGet* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HInt(aHost);
	}
    }
    return res;
}

TBool HInt::FromString(const string& aString)
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

void HInt::ToString(string& aString)
{
    stringstream ss;
    ss << "I " << mData;
    aString = ss.str();
}

TInt HInt::Data() const
{
    return mData;
}

void HInt::Set(TInt aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
    }
}

TBool HInt::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MDIntGet* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	TInt val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

TInt HInt::Value()
{
    return mData;
}

void HInt::SetValue(TInt aData)
{
    Set(aData);
}

// Float data
MIface *HFloat::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDFloat::Type()) == 0) res = (MDFloat*) this;
    else if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    else if (strcmp(aName, MDFloatSet::Type()) == 0)  res = (MDFloatSet*) this;
    return res;
}

HBase* HFloat::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'F') {
	res = new HFloat(aHost);
    }
    if (res == NULL && aInp != NULL) {
	MDFloatGet* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HFloat(aHost);
	}
    }
    return res;
}

TBool HFloat::FromString(const string& aString)
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

void HFloat::ToString(string& aString)
{
    stringstream ss;
    ss << "F " << mData;
    aString = ss.str();
}

float HFloat::Data() const
{
    return mData;
}

void HFloat::Set(float aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
    }
}

TBool HFloat::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MDFloatGet* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	float val = dget->Value();
	Set(val);
	res = ETrue;
    }
    return res;
}

float HFloat::Value()
{
    return mData;
}

void HFloat::SetValue(float aData)
{
    Set(aData);
}

// Scalar data
template<> const char* MDataGet<float>::Type() { return "MDataGet_float";};

template<> const char* MDataGet<float>::TypeSig() { return "DF";};

template<> string HData<float>::mId = "DF";

template<class T> MIface *HData<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDataGet<T>::Type()) == 0) res = (MDataGet<T>*) this;
    return res;
}

template<class T> HBase* HData<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty()) {
	int sig_e = aString.find_first_of(' ');
	string sig = aString.substr(0, sig_e);
	if (sig == mId) {
	    res = new HData<T>(aHost);
	}
    }
    if (res == NULL && aInp != NULL) {
	MVectGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HData<T>(aHost);
	}
    }
    return res;
}

template<class T> TBool HData<T>::FromString(const string& aString)
{
    TBool res = ETrue;
    T data;
    int res1 = 0;
    string ss;
    int beg = 0, end = 0;
    end = aString.find(' ');
    ss = aString.substr(beg, end);
    if (ss == mId) {
	beg = end + 1;
	end = aString.find(' ', beg);
	ss = aString.substr(beg, end - beg);
	if (!ss.empty()) {
	    ss >> data;
	    Set(data);
	    res = ETrue;
	}
    }
    return res;
}

template<class T> void HData<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId << " " << mData;
    aString = ss.str();
}

template<class T> TBool HData<T>::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MDataGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	T val;
	dget->DataGet(val);
	Set(val);
	res = ETrue;
    }
    return res;
}

// Vector

template<> const char* MVectGet<float>::TypeSig() { return  "VF";};

template<> const char* MVectGet<float>::Type() { return "MVectGet_float";};

template<> string HVect<float>::mId = "VF";

template<class T> HVect<T>::HVect(DHost* aHost): HBase(aHost), mValid(EFalse)
{
};

template<class T> MIface *HVect<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> HBase* HVect<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty()) {
	int sig_e = aString.find_first_of(' ');
	string sig = aString.substr(0, sig_e);
	if (sig == mId) {
	    res = new HVect<T>(aHost);
	}
    }
    if (res == NULL && aInp != NULL) {
	MVectGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HVect<T>(aHost);
	}
    }
    return res;
}

template<class T> TBool HVect<T>::FromString(const string& aString)
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
	    T data;
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

template<class T> void HVect<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId;
    for (typename Vect<T>::iterator it = mData.begin(); it != mData.end(); it++) {
	T data = *it;
	ss << " " << data;
    }
    aString = ss.str();
}

template<class T> TBool HVect<T>::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MVectGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	dget->VectGet(mData);
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
	res = ETrue;
    }
    return res;
}

template<class T> void HVect<T>::VectGet(Vect<T>& aData)
{
    aData = mData;
}


// Diagonal matrix

template<> const char* MMtrdGet<float>::Type() { return "MMtrdGet_float";};

template<> const char* MMtrdGet<float>::TypeSig() { return  "MDF";};

template<> string HMtrd<float>::mId = "MDF";

template<class T> MIface *HMtrd<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MMtrdGet<T>::Type()) == 0) res = (MMtrdGet<T>*) this;
    return res;
}

template<class T> HBase* HMtrd<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty()) {
	int sig_e = aString.find_first_of(' ');
	string sig = aString.substr(0, sig_e);
	if (sig == mId) {
	    res = new HMtrd<T>(aHost);
	}
    }
    if (res == NULL && aInp != NULL) {
	MMtrdGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    res = new HMtrd<T>(aHost);
	}
    }
    return res;
}

template<class T> HMtrd<T>::HMtrd(DHost* aHost): HBase(aHost)
{
}

template<class T> TBool HMtrd<T>::FromString(const string& aString)
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

template<class T> void HMtrd<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId;
    for (typename Mtrd<T>::iterator it = mData.begin(); it != mData.end(); it++) {
	T data = *it;
	ss << " " << data;
    }
    aString = ss.str();
}

template<class T> TBool HMtrd<T>::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MMtrdGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	dget->MtrdGet(mData);
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
	res = ETrue;
    }
    return res;
}

template<class T> void HMtrd<T>::MtrdGet(Mtrd<T>& aData)
{
    aData = mData;
}


// Matrix

template<> const char* MMtrGet<float>::Type() { return "MMtrGet_float";};

template<> const char* MMtrGet<float>::TypeSig() { return  Mtr<float>::TypeSig();};

template<class T> MIface *HMtr<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> HBase* HMtr<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && Mtr<T>::IsSrepFit(aString)) {
	res = new HMtr<T>(aHost, aString);
    }
    if (res == NULL && aInp != NULL) {
	MMtrGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    Mtr<T> data;
	    dget->MtrGet(data);
	    if (Mtr<T>::IsDataFit(data)) {
		res = new HMtr<T>(aHost, data);
	    }
	}
    }
    return res;
}

template<class T> HMtr<T>::HMtr(DHost* aHost, const string& aCont): HBase(aHost) 
{
    mData.FromString(aCont);
}

template<class T> HMtr<T>::HMtr(DHost* aHost, const Mtr<T>& aData): HBase(aHost), mData(aData)
{
}

template<class T> TBool HMtr<T>::FromString(const string& aString)
{
    return mData.FromString(aString);
}

template<class T> void HMtr<T>::ToString(string& aString)
{
    mData.ToString(aString);
}

template<class T> TBool HMtr<T>::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MMtrGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	Mtr<T> data = mData;
	dget->MtrGet(data);
	if (mData != data) {
	    mData = data; res = ETrue;
	}
    }
    return res;
}

template<class T> void HMtr<T>::MtrGet(Mtr<T>& aData)
{
    aData = mData;
}

// Generic data


template<class T> MIface *HDt<T>::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = dynamic_cast<MDtGet<T>*>(this);
    else if (strcmp(aName, MDtSet<T>::Type()) == 0) {
	res = dynamic_cast<MDtSet<T>*>(this);
    }
    return res;
}

template<class T> HBase* HDt<T>::Create(DHost* aHost, const string& aString, MDVarGet* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && T::IsSrepFit(aString)) {
	res = new HDt<T>(aHost, aString);
    }
    if (res == NULL && aInp != NULL) {
	MDtGet<T>* dget = aInp->GetDObj(dget);
	if (dget != NULL) {
	    T data;
	    dget->DtGet(data);
	    if (T::IsDataFit(data)) {
		res = new HDt<T>(aHost, data);
	    }
	}
    }
    return res;
}

template<class T> HDt<T>::HDt(DHost* aHost, const string& aCont): HBase(aHost) 
{
    mData.FromString(aCont);
}

template<class T> HDt<T>::HDt(DHost* aHost, const T& aData): HBase(aHost), mData(aData)
{
}

template<class T> TBool HDt<T>::FromString(const string& aString)
{
    return mData.FromString(aString);
}

template<class T> void HDt<T>::ToString(string& aString)
{
    mData.ToString(aString);
}

template<class T> TBool HDt<T>::Set(MDVarGet* aInp)
{
    TBool res = EFalse;
    MDtGet<T>* dget = aInp->GetDObj(dget);
    if (dget != NULL) {
	T data = mData;
	dget->DtGet(data);
	if (mData != data) {
	    mData = data;
	    mHost.HUpdateProp();
	    mHost.HNotifyUpdate();
	    res = ETrue;
	}
    }
    else {
	res = mData.mValid != EFalse;;
	mData.mValid = EFalse;
    }
    return res;
}

template<class T> void HDt<T>::DtGet(T& aData)
{
    aData = mData;
}

template<class T> void HDt<T>::DtSet(const T& aData)
{
    if (mData != aData) {
	mData = aData;
	mHost.HUpdateProp();
	mHost.HNotifyUpdate();
    }
}

// Variant base data

BdVar::BdVar(MBdVarHost *aHost): Base(), mData(NULL), mHost(aHost)
{
}

MIface *BdVar::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDVar::Type()) == 0) {
	res = dynamic_cast<MDVar*>(this);
    } else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = dynamic_cast<MDVarGet*>(this);
    } else if (strcmp(aName, MDVarSet::Type()) == 0) {
	res = dynamic_cast<MDVarSet*>(this);
    }
    return res;
}

string BdVar::VarGetIfid()
{
    return mData != NULL ? mData->IfaceGetId() : string();
}

string BdVar::VarGetSIfid()
{
    return mData != NULL ? mData->IfaceSGetId() : string();
}

void *BdVar::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (mData == NULL) {
	Init(aName);
    }
    if (mData != NULL) {
	res = mData->DoGetObj(aName);
    }
    return res;
}

MIface *BdVar::DoGetSDObj(const char *aName)
{
    MIface* res = NULL;
    if (mData != NULL) {
	res = mData->DoGetObj(aName);
    }
    return res;
}


BdVar::~BdVar()
{
    if (mData != NULL) {
	delete mData;
    }
}

TBool BdVar::Init(const string& aString, MDVarGet* aInpv)
{
    TBool res = EFalse;
    if (mData != NULL) {
	delete mData;
	mData == NULL;
    }
    if ((mData = HInt::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HFloat::Create(this, aString, aInpv)) != NULL);
    // else if ((mData = HMtr<float>::Create(this, aString, aInp)) != NULL);
    else if ((mData = HDt<Sdata <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <bool> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Sdata <string> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <int> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Mtr <float> >::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<NTuple>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Enum>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<DMut>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HDt<Vector<string>>::Create(this, aString, aInpv)) != NULL);
    //else if ((mData = HVect<float>::Create(this, aString, aInpv)) != NULL);
    //else if ((mData = HMtrd<float>::Create(this, aString, aInpv)) != NULL);
    else if ((mData = HBool::Create(this, aString, aInpv)) != NULL);
    /* Seems it is not required, Value init should be done on creation phase
       if (mData != NULL && !aString.empty()) {
       res = mData->FromString(aString);
       }
       */
    res = mData != NULL;
    return res;
}

TBool BdVar::FromString(const string& aData) 
{
    TBool res = EFalse;
    if (mData == NULL) {
	res = Init(aData);
    } else {
	res = mData->FromString(aData);
	if (!mData->IsValid() && !mData->IsSigOK()) {
	    // Signature get's not fit, reinit
	    Init(aData);
	    res = ETrue;
	}
    }
    if (res) {
	NotifyUpdate();
    }
    return res;
}

bool BdVar::ToString(string& aData) 
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

TBool BdVar::Update()
{
    TBool res = EFalse;
    string old_value;
    ToString(old_value);
    MDVarGet* vget = mHost->HGetInp(this);
    if (vget != NULL) {
	if (mData == NULL) {
	    Init(string(), vget);
	    res = ETrue;
	}
	if (mData != NULL) {
	    res |= mData->Set(vget);
	}
    }
    return res;
}

void BdVar::NotifyUpdate()
{
    if (mHost != NULL) {
	mHost->HOnDataChanged(this);
    }
}

void BdVar::HUpdateProp()
{
}

void BdVar::HNotifyUpdate()
{
    NotifyUpdate();
}

string BdVar::GetValue()
{
    string res;
    ToString(res);
    return res;
}


// Vector getter

template<> const char* MVectorGet<string>::Type() { return "MVectGet_string";};


