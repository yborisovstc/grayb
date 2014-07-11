
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
	    Elem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
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

TBool DVar::Init(const string& aString, Elem* aInp)
{
    TBool res = EFalse;
    if (mData != NULL) {
	delete mData;
	mData == NULL;
    }
    if ((mData = HInt::Create(this, aString, aInp)) != NULL);
    else if ((mData = HFloat::Create(this, aString, aInp)) != NULL);
    else if ((mData = HMtr<float>::Create(this, aString, aInp)) != NULL);
    else if ((mData = HVect<float>::Create(this, aString, aInp)) != NULL);
    else if ((mData = HMtrd<float>::Create(this, aString, aInp)) != NULL);
    else if ((mData = HBool::Create(this, aString, aInp)) != NULL);
    if (mData != NULL && !aString.empty()) {
	res = mData->FromString(aString);
    }
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
	if (!res) {
	    res = Init(aData);
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
    ToString(old_value);
    Elem* inp = GetInp();
    if (inp != NULL) {
	RqContext ctx(this);
	MDVarGet* vget = (MDVarGet*) inp->GetSIfi(MDVarGet::Type(), &ctx);
	if (vget != NULL) {
	    Elem* eget = vget->VarGetBase();
	    if (eget != NULL) {
		if (mData == NULL) {
		    Init(string(), eget);
		}
		if (mData != NULL) {
		    res = mData->Set(eget);
		    if (res && IsLogeventUpdate()) {
			string new_value;
			ToString(new_value);
			Logger()->Write(MLogRec::EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
		    }
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

DVar::HBase* DVar::HBool::Create(DVar* aHost, const string& aString, Elem* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'B') {
	res = new HBool(aHost);
    }
    if (res == NULL && aInp != NULL) {
	MDBoolGet* dget = (MDBoolGet*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HBool(aHost);
	}
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

DVar::HBase* DVar::HInt::Create(DVar* aHost, const string& aString, Elem* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'I') {
	res = new HInt(aHost);
    }
    if (res == NULL && aInp != NULL) {
	MDIntGet* dget = (MDIntGet*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HInt(aHost);
	}
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

DVar::HBase* DVar::HFloat::Create(DVar* aHost, const string& aString, Elem* aInp)
{
    HBase* res = NULL;
    if (!aString.empty() && aString.at(0) == 'F') {
	res = new HFloat(aHost);
    }
    if (res == NULL && aInp != NULL) {
	MDFloatGet* dget = (MDFloatGet*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HFloat(aHost);
	}
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

// Scalar data
template<> const char* MDataGet<float>::Type() { return "MDataGet_float";};

template<> const char* MDataGet<float>::TypeSig() { return  "DF";};

template<> string DVar::HData<float>::mId = "DF";

template<class T> void *DVar::HData<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDataGet<T>::Type()) == 0) res = (MDataGet<T>*) this;
    return res;
}

template<class T> DVar::HBase* DVar::HData<T>::Create(DVar* aHost, const string& aString, Elem* aInp)
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
	MVectGet<T>* dget = (MVectGet<T>*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HData<T>(aHost);
	}
    }
    return res;
}

template<class T> TBool DVar::HData<T>::FromString(const string& aString)
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

template<class T> void DVar::HData<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId << " " << mData;
    aString = ss.str();
}

template<class T> TBool DVar::HData<T>::Set(Elem* aInp)
{
    TBool res = EFalse;
    MDataGet<T>* dget = (MDataGet<T>*) aInp->GetObj(dget);
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

template<> string DVar::HVect<float>::mId = "VF";

template<class T> DVar::HVect<T>::HVect(DVar* aHost): HBase(aHost) 
{
};

template<class T> void *DVar::HVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> DVar::HBase* DVar::HVect<T>::Create(DVar* aHost, const string& aString, Elem* aInp)
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
	MVectGet<T>* dget = (MVectGet<T>*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HVect<T>(aHost);
	}
    }
    return res;
}

template<class T> TBool DVar::HVect<T>::FromString(const string& aString)
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

template<class T> void DVar::HVect<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId;
    for (typename Vect<T>::iterator it = mData.begin(); it != mData.end(); it++) {
	T data = *it;
	ss << " " << data;
    }
    aString = ss.str();
}

template<class T> TBool DVar::HVect<T>::Set(Elem* aInp)
{
    TBool res = EFalse;
    MVectGet<T>* dget = (MVectGet<T>*) aInp->GetObj(dget);
    if (dget != NULL) {
	dget->VectGet(mData);
	mHost.UpdateProp();
	mHost.NotifyUpdate();
	res = ETrue;
    }
    return res;
}

template<class T> void DVar::HVect<T>::VectGet(Vect<T>& aData)
{
    aData = mData;
}


// Diagonal matrix

template<> const char* MMtrdGet<float>::Type() { return "MMtrdGet_float";};

template<> const char* MMtrdGet<float>::TypeSig() { return  "MDF";};

template<> string DVar::HMtrd<float>::mId = "MDF";

template<class T> void *DVar::HMtrd<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrdGet<T>::Type()) == 0) res = (MMtrdGet<T>*) this;
    return res;
}

template<class T> DVar::HBase* DVar::HMtrd<T>::Create(DVar* aHost, const string& aString, Elem* aInp)
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
	MMtrdGet<T>* dget = (MMtrdGet<T>*) aInp->GetObj(dget);
	if (dget != NULL) {
	    res = new HMtrd<T>(aHost);
	}
    }
    return res;
}

template<class T> DVar::HMtrd<T>::HMtrd(DVar* aHost): HBase(aHost)
{
}

template<class T> TBool DVar::HMtrd<T>::FromString(const string& aString)
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

template<class T> void DVar::HMtrd<T>::ToString(string& aString)
{
    stringstream ss;
    ss << mId;
    for (typename Mtrd<T>::iterator it = mData.begin(); it != mData.end(); it++) {
	T data = *it;
	ss << " " << data;
    }
    aString = ss.str();
}

template<class T> TBool DVar::HMtrd<T>::Set(Elem* aInp)
{
    TBool res = EFalse;
    MMtrdGet<T>* dget = (MMtrdGet<T>*) aInp->GetObj(dget);
    if (dget != NULL) {
	dget->MtrdGet(mData);
	mHost.UpdateProp();
	mHost.NotifyUpdate();
	res = ETrue;
    }
    return res;
}

template<class T> void DVar::HMtrd<T>::MtrdGet(Mtrd<T>& aData)
{
    aData = mData;
}


// Matrix

template<> const char* MMtrGet<float>::Type() { return "MMtrGet_float";};

template<> const char* MMtrGet<float>::TypeSig() { return  "MF";};

template<class T> void *DVar::HMtr<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> DVar::HBase* DVar::HMtr<T>::Create(DVar* aHost, const string& aString, Elem* aInp)
{
    HBase* res = NULL;
    if (!aString.empty()) {
	MtrBase::TMtrType mtype;
	MtrBase::TMtrDim mdim;
	string sig;
	ParseSigPars(aString, sig, mtype, mdim);
	if (sig == MMtrGet<T>::TypeSig() && mtype != MtrBase::EMt_Unknown) {
	    res = new HMtr<T>(aHost, mtype, mdim);
	}
    }
    if (res == NULL && aInp != NULL) {
	MMtrGet<T>* dget = (MMtrGet<T>*) aInp->GetObj(dget);
	if (dget != NULL) {
	    Mtr<T> data;
	    dget->MtrGet(data);
	    if (data.mType != MtrBase::EMt_Unknown && data.mDim.first != 0 && data.mDim.second != 0) {
		res = new HMtr<T>(aHost, data.mType, data.mDim);
	    }
	}
    }
    return res;
}

template<class T> DVar::HMtr<T>::HMtr(DVar* aHost, const string& aCont): HBase(aHost) 
{
    string sig;
    ParseSigPars(aCont, sig, mData.mType, mData.mDim);
    int size = mData.mDim.first*mData.mDim.second;
    mData.mData.reserve(size);
}

template<class T> DVar::HMtr<T>::HMtr(DVar* aHost, const MtrBase::TMtrType& aType, const MtrBase::TMtrDim& aDim): HBase(aHost)
{
    mData.mType = aType;
    mData.mDim = aDim;
    int size = mData.mDim.first*mData.mDim.second;
    mData.mData.reserve(size);
}

template<class T> int DVar::HMtr<T>::ParseSigPars(const string& aCont, string& aSig, MtrBase::TMtrType& aType, MtrBase::TMtrDim& aDim)
{
    size_t res = string::npos;
    if (!aCont.empty()) {
	int beg = 0;
	int sigp_e = aCont.find_first_of(' ');
	res = sigp_e;
	string sigp = aCont.substr(beg, sigp_e);
	int sig_e = sigp.find_first_of(',');
	aSig = sigp.substr(0, sig_e);
	if (!aSig.empty()) {
	    beg = sig_e + 1;
	    int par_e = sigp.find_first_of(',', beg);
	    string par = sigp.substr(beg, par_e - beg);
	    if (par.length() == 1) {
		char ts = par.at(0);
		switch (ts) {
		    case 'R': aType = MtrBase::EMt_Regular; break;
		    case 'V': aType = MtrBase::EMt_Vector; break;
		    case 'S': aType = MtrBase::EMt_String; break;
		    case 'D': aType = MtrBase::EMt_Diagonal; break;
		    case 'U': aType = MtrBase::EMt_Utriang; break;
		    case 'B': aType = MtrBase::EMt_Btriang; break;
		}
		beg = par_e + 1;
		par_e = sigp.find_first_of(',', beg);
		par = sigp.substr(beg, par_e - beg);
		if (!par.empty()) {
		    int dp1 = 0, dp2 = 0;
		    istringstream sstr(par);
		    sstr >> dp1;
		    beg = par_e + 1;
		    par = sigp.substr(beg);
		    if (dp1 != 0) {
			istringstream sstr(par);
			sstr >> dp2;
			aDim.first = dp1; aDim.second = dp2; 
			if (dp2 == 0) {
			    aDim.second = dp1;
			    if (aType == MtrBase::EMt_Vector) {
				aDim.first = dp1; aDim.second = 1;
			    }
			    else if (aType == MtrBase::EMt_String) {
				aDim.first = 1; aDim.second = dp1;
			    }
			}
			else if (aType == MtrBase::EMt_Vector || aType == MtrBase::EMt_String || aType == MtrBase::EMt_Diagonal) {
			    aDim.first = 0; aDim.second = 0; aType = MtrBase::EMt_Unknown;
			}
		    }
		}
	    }
	}
    }
    return res;
}

template<class T> TBool DVar::HMtr<T>::FromString(const string& aString)
{
    TBool res = ETrue;
    int res1 = 0;
    string ss;
    int beg = 0, end = 0;
    MtrBase::TMtrType mtype;
    MtrBase::TMtrDim mdim;
    string sig;
    end = ParseSigPars(aString, sig, mtype, mdim);
    if (sig == MMtrGet<T>::TypeSig() && mtype != MtrBase::EMt_Unknown) {
	mData.mType = mtype;
	mData.mDim = mdim;
	mData.mData.clear();
	TBool fin = EFalse;
	int cnt = 0;
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
		    mData.mData.push_back(data);
		}
		else {
		    res = EFalse;
		}
		cnt++;
	    }
	    if (end == string::npos) {
		fin = ETrue;
	    }
	} while (!fin);
	if (cnt != mData.mDim.first*mData.mDim.second) {
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
    }
    return res;
}

template<class T> void DVar::HMtr<T>::ToString(string& aString)
{
    mData.ToString(aString);
}

template<class T> TBool DVar::HMtr<T>::Set(Elem* aInp)
{
    TBool res = EFalse;
    MMtrGet<T>* dget = (MMtrGet<T>*) aInp->GetObj(dget);
    if (dget != NULL) {
	res = dget->MtrGet(mData);
	if (res) {
	    mHost.UpdateProp();
	    mHost.NotifyUpdate();
	    res = ETrue;
	}
    }
    return res;
}

template<class T> TBool DVar::HMtr<T>::MtrGet(Mtr<T>& aData)
{
    aData = mData;
    return ETrue;
}

template<class T> void Mtr<T>::ToString(string& aString) const
{
    stringstream ss;
    ss << MMtrGet<T>::TypeSig();
    if (mType != MtrBase::EMt_Unknown) {
	char mt = 'R';
	switch (mType) {
	    case MtrBase::EMt_Diagonal: mt = 'D'; break;
	    case MtrBase::EMt_Vector: mt = 'V'; break;
	    case MtrBase::EMt_String: mt = 'S'; break;
	    case MtrBase::EMt_Utriang: mt = 'U'; break;
	    case MtrBase::EMt_Btriang: mt = 'B'; break;
	}
	ss << ',' << mt << ',' << mDim.first;
	if (mDim.second != 0) {
	    ss << ',' << mDim.second;
	}
	for (typename vector<T>::const_iterator it = mData.begin(); it != mData.end(); it++) {
	    T data = *it;
	    ss << " " << data;
	}
    }
    aString = ss.str();

}

