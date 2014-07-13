
#include "mvert.h"
#include "vert.h"
#include "prop.h"
#include "mdata.h"
#include "func.h"

string FuncBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

FuncBase::FuncBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

FuncBase::FuncBase(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
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
    Elem* caps = aContext.GetNode("Capsule");
    if (caps != NULL) {
	// TODO [YB] To optimize by using proper utility
	Elem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
	cp = cp != NULL ? cp : caps->GetCompOwning("ConnPointOut", &aComp);
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
    Elem* eout = GetNode("../../Capsule/out");
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

TBool FuncBase::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    return EFalse;
}

// Agent base of Int function

string AFunInt::PEType()
{
    return FuncBase::PEType() + GUri::KParentSep + Type();
}

AFunInt::AFunInt(const string& aName, Elem* aMan, MEnv* aEnv): FuncBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type(), FuncBase::PEType());
    SetParent(Type());
}

AFunInt::AFunInt(Elem* aMan, MEnv* aEnv): FuncBase(Type(), aMan, aEnv), mData(0)
{
    SetEType(FuncBase::PEType());
    SetParent(FuncBase::PEType());
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
	Elem* host = iMan->GetMan();
	Logger()->Write(MLogRec::EInfo, host, "Updated [%d <- %d]", aData, mData);
	mData = aData;
	NotifyUpdate();
    }
}

// TODO [YB] The current approach is as: function keeps result calculated. Function recalculates result on each
// OnDataChanged or HandleIoChanged notifications
// The approach is not efficient. There is the alternative:
// func recalculates the result on iface request "Value" and not keeps the result. In this case we just need to propagate
// OnDataChanged to root of function chain, then the root initiate Value request back
TInt AFunInt::Value()
{
    return mData;
}

MDIntGet* AFunInt::GetInp(const string& aInpName)
{
    MDIntGet* res = NULL;
    Elem* einp = GetNode("../../Capsule/" + aInpName);
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


string AIncInt::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AIncInt::AIncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
    SetParent(Type());
}

AIncInt::AIncInt(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetEType(AFunInt::PEType());
    SetParent(AFunInt::PEType());
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

string AFunIntRes::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AFunIntRes::AFunIntRes(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
    SetParent(Type());
}

AFunIntRes::AFunIntRes(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetEType(AFunInt::PEType());
    SetParent(AFunInt::PEType());
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
    Elem* out = GetNode("../../Capsule/out");
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
//    MDIntGet* mget = GetInp("inp");
    Elem* einp = GetNode("../../Capsule/inp");
    __ASSERT(einp != NULL);
    RqContext cont(this);
    MDIntGet* mget = (MDIntGet*) einp->GetSIfi("MDIntGet", &cont);
    // It is possible that MDIntGet is missing in cases of connection process ongoing
    if (mget != NULL) {
	TInt val = mget->Value();
	SetRes(val);
	UpdateOutp();
    }
}

// Agent function "Addition of Int data"

string AAddInt::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AAddInt::AAddInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
    SetParent(Type());
}

AAddInt::AAddInt(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetEType(AFunInt::PEType());
    SetParent(AFunInt::PEType());
}

void *AAddInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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

TBool AAddInt::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    // Checking input change
    if (aCp->Name() == "inp") {
	RqContext cont(this);
	TIfRange range = aCp->GetIfi("MDIntGet", &cont);
	TInt val = 0;
	for (IfIter it = range.first; it != range.second; it++) {
	    MDIntGet* dget = (MDIntGet*) (*it);
	    if (dget != NULL) {
		val += dget->Value();
	    }
	}
	SetRes(val);
	res = ETrue;
    }
    return res;
}

void AAddInt::OnDataChanged()
{
    Elem* einp = GetNode("../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    SetRes(val);
}


// Agent function "Count of data matched a criteria"

string ACountCritInt::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

ACountCritInt::ACountCritInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
    SetParent(Type());
}

void *ACountCritInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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

TBool ACountCritInt::HandleIoChanged(Elem& aContext, Elem* aCp)
{
    TBool res = EFalse;
    // Checking input change
    if (aCp->Name() == "inp") {
	RqContext cont(this);
	TIfRange range = aCp->GetIfi("MDIntGet", &cont);
	TInt val = 0;
	for (IfIter it = range.first; it != range.second; it++) {
	    MDIntGet* dget = (MDIntGet*) (*it);
	    if (dget != NULL) {
		val += dget->Value();
	    }
	}
	SetRes(val);
	res = ETrue;
    }
    return res;
}

void ACountCritInt::OnDataChanged()
{
    Elem* einp = GetNode("../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    SetRes(val);
}


// Function agent base without result caching
const string AFunc::KDiagErr = "ERROR";

string AFunc::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AFunc::AFunc(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

AFunc::AFunc(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

void *AFunc::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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

TBool AFunc::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = ETrue;
    Elem* caps = aContext.GetNode("Capsule");
    if (caps != NULL) {
	Elem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}

void AFunc::NotifyUpdate()
{
    Elem* eout = GetNode("../../Capsule/out");
    __ASSERT(eout != NULL);
    MVert* mvout = eout->GetObj(mvout);
    MVert* mpair = *(mvout->Pairs().begin());
    if (mpair != NULL) {
	MDataObserver* obsr = mpair->EBase()->GetObj(obsr);
	if (obsr != NULL) {
	    obsr->OnDataChanged();
	}
    }
}

void AFunc::OnDataChanged()
{
    NotifyUpdate();
}

TBool AFunc::IsLogeventUpdate()
{
    Elem* node = GetNode("../../Logspec/Update");
    return node != NULL;
}


// Integer Function agent base wo caching

string AFuncInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFuncInt::AFuncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv), mData(0)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFuncInt::AFuncInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv), mData(0)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}


void *AFuncInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFuncInt::Value()
{
    TInt data = GetValue();
    if (data != mData) {
	mData = data;
	iMan->OnContentChanged(*this);
    }
    return mData;
}

void AFuncInt::GetCont(string& aCont)
{
    stringstream ss;
    ss << Value();
    aCont = ss.str();
}

TInt AFuncInt::GetValue()
{
    return 0;
}

// Function agent base wo caching

string AFAddInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFAddInt::AFAddInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFAddInt::AFAddInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}

void *AFAddInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFAddInt::Value()
{
    Elem* einp = GetNode("../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    return val;
}

// Function Sub agent wo caching

string AFSubInt::PEType()
{
    return AFuncInt::PEType() + GUri::KParentSep + Type();
}

AFSubInt::AFSubInt(const string& aName, Elem* aMan, MEnv* aEnv): AFuncInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFuncInt::PEType());
    SetParent(Type());
}

AFSubInt::AFSubInt(Elem* aMan, MEnv* aEnv): AFuncInt(Type(), aMan, aEnv)
{
    SetEType(AFuncInt::PEType());
    SetParent(AFuncInt::PEType());
}


void *AFSubInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFuncInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFSubInt::GetValue()
{
    RqContext cont(this);
    TInt val = 0;
    // Positives
    Elem* einp = GetNode("../../Capsule/InpP");
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    // Negatives
    einp = GetNode("../../Capsule/InpN");
    range = einp->GetIfi("MDIntGet", &cont);
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val -= dget->Value();
	}
    }
    return val;
}


// Restriction of value from top and bottom

string AFLimInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFLimInt::AFLimInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFLimInt::AFLimInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}


void *AFLimInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFLimInt::Value()
{
    TInt val = 0;
    MDIntGet* minp = (MDIntGet*) GetSIfi("../../Capsule/Inp", "MDIntGet");
    MDIntGet* mlimu = (MDIntGet*) GetSIfi("../../Capsule/Inp_LimU", "MDIntGet");
    MDIntGet* mliml = (MDIntGet*) GetSIfi("../../Capsule/Inp_LimL", "MDIntGet");
    if (minp != NULL && mlimu != NULL && mliml != NULL) {
	TInt inp = minp->Value();
	TInt limu = mlimu->Value();
	TInt liml = mliml->Value();
	val = (inp > limu) ? limu : inp;
	val = (val < liml) ? liml : val;
    }
    else {
	if (minp == NULL) {
	    Logger()->Write(MLogRec::EErr, Host(), "Inp not connected");
	}
	else if (mlimu == NULL) {
	    Logger()->Write(MLogRec::EErr, Host(), "Inp_LimU not connected");
	}
	else if (mliml == NULL) {
	    Logger()->Write(MLogRec::EErr, Host(), "Inp_LimL not connected");
	}
    }
    return val;
}


// Restriction of value from top and bottom

string AFDivInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFDivInt::AFDivInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFDivInt::AFDivInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}

void *AFDivInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFDivInt::Value()
{
    TInt val = 0;
    MDIntGet* mdvd = (MDIntGet*) GetSIfi("../../Capsule/Inp_DD", "MDIntGet");
    MDIntGet* mdvr = (MDIntGet*) GetSIfi("../../Capsule/Inp_DR", "MDIntGet");
    if (mdvd != NULL && mdvr != NULL) {
	TInt dvd = mdvd->Value();
	TInt dvr = mdvr->Value();
	// TODO [YB] To add errors handling
	TBool lupd = IsLogeventUpdate();
	GUri fullpath;
	if (lupd) {
	    Elem* host = iMan->GetMan();
	    host->GetUri(fullpath);
	}
	if (dvr != 0) {
	    val = dvd / dvr;
	    if (lupd) {
		Logger()->WriteFormat("[%s]: Result = %d", fullpath.GetUri(ETrue).c_str(), val);
	    }
	}
	else {
	    if (lupd) {
		Logger()->WriteFormat("[%s]: Error: null devider", fullpath.GetUri(ETrue).c_str());
	    }
	}
    }
    return val;
}


// Inputs to vector conversion

string AFIntToVect::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFIntToVect::AFIntToVect(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

void *AFIntToVect::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MVIntGet::Type()) == 0) {
	res = (MVIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

vector<TInt> AFIntToVect::Value()
{
    vector<TInt> res;
    Elem* einp = GetNode("../../Capsule/inp");
    __ASSERT(einp != NULL);
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res.push_back(dget->Value());
	}
    }
    return res;
}



// Convolution of inputs. Used external working function.

string AFConvInt::PEType()
{
    return AFuncInt::PEType() + GUri::KParentSep + Type();
}

AFConvInt::AFConvInt(const string& aName, Elem* aMan, MEnv* aEnv): AFuncInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFuncInt::PEType());
    SetParent(Type());
    iSampleHolder.iHost = this;
}

AFConvInt::AFConvInt(Elem* aMan, MEnv* aEnv): AFuncInt(Type(), aMan, aEnv)
{
    SetEType(AFuncInt::PEType());
    SetParent(AFuncInt::PEType());
    iSampleHolder.iHost = this;
}

void *AFConvInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	// If iface requested from working function input - redirect to Sample iface holder
	Elem* wfinp = GetNode("../../Capsule/Out_WFarg");
	if (aCtx->IsInContext(wfinp)) {
	    res = (MDIntGet*) &iSampleHolder;
	}
	else {
	    res = (MDIntGet*) this;
	}
    }
    else {
	res = AFuncInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFConvInt::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resg = EFalse;
    RqContext ctx(this, aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFuncInt::DoGetObj(aName.c_str(), EFalse, aCtx);
    }
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
}

TInt AFConvInt::GetValue()
{
    TInt val = 0;
    Elem* einp = GetNode("../../Capsule/inp");
    __ASSERT(einp != NULL);
    Elem* einpwf = GetNode("../../Capsule/Inp_WFres");
    __ASSERT(einpwf != NULL);
    Elem* eargwf = GetNode("../../WFArg/Capsule/inp");
    __ASSERT(eargwf != NULL);
    RqContext cont(this);
    MDIntSet* wfarg = (MDIntSet*) eargwf->GetSIfi(MDIntSet::Type(), &cont);
    MDIntGet* wfres = (MDIntGet*) einpwf->GetSIfi("MDIntGet", &cont);
    if (wfres != NULL && wfarg != NULL) {
	TBool lupd = IsLogeventUpdate();
	GUri fullpath;
	if (lupd) {
	    Elem* host = iMan->GetMan();
	    host->GetUri(fullpath);
	}
	TIfRange range = einp->GetIfi("MDIntGet", &cont);
	for (IfIter it = range.first; it != range.second; it++) {
	    MDIntGet* dget = (MDIntGet*) (*it);
	    if (dget != NULL) {
		TInt sample = dget->Value();
		wfarg->SetValue(sample);
		val += wfres->Value();
	    }
	}
	if (lupd) {
	    Logger()->Write(MLogRec::EDbg, this,  "Result = %d", val);
	}
    }
    return val;
}




// Function agent base with multitype support

string AFuncm::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AFuncm::AFuncm(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

void *AFuncm::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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
    if (res == NULL) {
	map<string, Exec*>::iterator ri = mExecs.find(aName);
	if (ri != mExecs.end()) {
	    res = ri->second;
	}
    }
    return res;
}

TBool AFuncm::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = ETrue;
    Elem* caps = aContext.GetNode("Capsule");
    if (caps != NULL) {
	Elem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}

void AFuncm::NotifyUpdate()
{
    Elem* eout = GetNode("../../Capsule/out");
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

void AFuncm::OnDataChanged()
{
    NotifyUpdate();
}


string AFuncmAdd::PEType()
{
    return AFuncm::PEType() + GUri::KParentSep + Type();
}

AFuncmAdd::AFuncmAdd(const string& aName, Elem* aMan, MEnv* aEnv): AFuncm(aName, aMan, aEnv)
{
    SetEType(Type(), AFuncm::PEType());
    SetParent(Type());
}

void *AFuncmAdd::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFuncm::DoGetObj(aName, aIncUpHier);
    }
   return res;
}

TInt AFuncmAdd::ExcInt::Value()
{
    Elem* einp = mHost.GetNode("../../Capsule/inp");
    RqContext cont(&mHost);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    return val;
}

// Agent for Greater Than Int functin

string AFGTInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFGTInt::AFGTInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFGTInt::AFGTInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}

void *AFGTInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDBoolGet::Type()) == 0) {
	res = (MDBoolGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TBool AFGTInt::Value()
{
    Elem* einp1 = GetNode("../../Capsule/Inp1");
    Elem* einp2 = GetNode("../../Capsule/Inp2");
    __ASSERT(einp1 != NULL && einp2 != NULL);
    RqContext cont(this);
    MDIntGet* minp1 = (MDIntGet*) einp1->GetSIfi("MDIntGet", &cont);
    MDIntGet* minp2 = (MDIntGet*) einp2->GetSIfi("MDIntGet", &cont);
    TBool res = EFalse;
    if (minp1 != NULL && minp2 != NULL) {
	res = (minp1->Value() > minp2->Value());
    }
    return res;
}


// Agent for Bool to Int functin

string AFBoolToInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFBoolToInt::AFBoolToInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFBoolToInt::AFBoolToInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}

void *AFBoolToInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt AFBoolToInt::Value()
{
    Elem* einp = GetNode("../../Capsule/Inp");
    __ASSERT(einp != NULL);
    RqContext cont(this);
    MDBoolGet* minp = (MDBoolGet*) einp->GetSIfi("MDBoolGet", &cont);
    TInt res = 0;
    if (minp != NULL) {
	res = minp->Value() ? 1 : 0;
    }
    return res;
}


// Agent of function of variable type

string AFunVar::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFunVar::AFunVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv), mFunc(NULL)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

AFunVar::AFunVar(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv), mFunc(NULL)
{
    SetEType(AFunc::PEType());
    SetParent(AFunc::PEType());
}

void *AFunVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName, aIncUpHier);
    }
    if (res == NULL) {
	if (mFunc == NULL) {
	    Init(aName);
	    if (mFunc != NULL) {
		res = mFunc->DoGetObj(aName, aIncUpHier);
	    }
	}
	else {
	    res = mFunc->DoGetObj(aName, aIncUpHier);
	    if (res == NULL) {
		Init(aName);
		if (mFunc != NULL) {
		    res = mFunc->DoGetObj(aName, aIncUpHier);
		}
	    }
	}
    }
    return res;
}

TBool AFunVar::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = ETrue;
    Elem* caps = aContext.GetNode("Capsule");
    if (caps != NULL) {
	Elem* cp = caps->GetCompOwning("Inp_FVar", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}


string AFunVar::VarGetIfid() const
{
    return mFunc != NULL ? mFunc->IfaceGetId() : string();
}

void AFunVar::OnFuncContentChanged()
{
    OnContentChanged(*this);
}

Elem* AFunVar::VarGetBase() 
{
    return this;
}

Elem::TIfRange AFunVar::GetInps(TInt aId)
{
    TIfRange res;
    Elem* inp = GetNode("../../Capsule/" + GetInpUri(aId));
    if (inp != NULL) {
	RqContext cont(this);
	res =  inp->GetIfi(MDVarGet::Type(), &cont);
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void AFunVar::LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...)
{
    va_list list;
    va_start(list,aFmt);
    Logger()->Write(aCtg, this, aFmt, list);
}

string AFunVar::GetInpUri(TInt aId) 
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else if (aId == Func::EInp3) return "Inp3";
    else if (aId == Func::EInp4) return "Inp4";
    else return string();
}

void AFunVar::GetCont(string& aCont)
{
    if (mFunc != NULL) {
	mFunc->GetResult(aCont);
    }
}


// Addintion, variable 
string AFAddVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFAddVar::AFAddVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFAddVar::AFAddVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFAddVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFAddVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FAddInt::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddFloat::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddData<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddVect<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddMtrd<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddMtr<float>::Create(this, aIfaceName)) != NULL);
}

string AFAddVar::GetInpUri(TInt aId) 
{
    if (aId == FAddBase::EInp) return "Inp";
    else return string();
}

// Int function
Func* FAddInt::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDIntGet::Type()) {
	res = new FAddInt(*aHost);
    }
    return res;
}

void *FAddInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDIntGet::Type()) == 0) res = (MDIntGet*) this;
    return res;
}

TInt FAddInt::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    TInt val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDIntGet* diget = (MDIntGet*) dgetbase->GetObj(diget);
	if (diget != NULL) {
	    val += diget->Value();
	}
    }
    return val;
}


// Float function
Func* FAddFloat::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDFloatGet::Type()) {
	res = new FAddFloat(*aHost);
    }
    return res;
}

void *FAddFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FAddFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDFloatGet* dfget = (MDFloatGet*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    val += dfget->Value();
	}
	else {
	    MDIntGet* diget = (MDIntGet*) dgetbase->GetSIfi(MDIntGet::Type());
	    if (diget != NULL) {
		val += (float) diget->Value();
	    }
	}
    }
    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FAddFloat::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}

// Scalar data addition
template<class T> Func* FAddData<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDataGet<T>::Type()) {
	res = new FAddData<T>(*aHost);
    }
    return res;
}

template<class T> void *FAddData<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDataGet<T>::Type()) == 0) res = (MDataGet<T>*) this;
    return res;
}

template<class T>  void FAddData<T>::DataGet(T& aData)
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    T val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDataGet<T>* dfget = (MDataGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    T arg;
	    dfget->DataGet(arg);
	    val += arg;
	}
    }
    mRes = val;
    mHost.OnFuncContentChanged();
    aData = val;
}

template<class T> void FAddData<T>::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  MDataGet<T>::TypeSig() << " " << mRes;
    aResult = ss.str();
}


// Vector addition
template<class T> Func* FAddVect<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MVectGet<T>::Type()) {
	res = new FAddVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FAddVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> void FAddVect<T>::VectGet(Vect<T>& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range = mHost.GetInps(EInp);
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MVectGet<T>* dfget = (MVectGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Vect<T> arg;
	    arg.resize(size);
	    dfget->VectGet(arg);
	    if (arg.size() == size) {
		for (TInt cnt = 0; cnt < size; cnt++) {
		    if (it == range.first) {
			aData.at(cnt) = 0.0;
		    }
		    aData.at(cnt) += arg.at(cnt);
		}
		mRes = aData;
		mHost.OnFuncContentChanged();
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", dgetbase->GetUri().c_str());
	    }
	}
    }
}

template<class T> void FAddVect<T>::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  MVectGet<T>::TypeSig();
    for (typename Vect<T>::const_iterator it = mRes.begin(); it != mRes.end(); it++) {
	ss << " " << *it;
    }
    aResult = ss.str();
}

MDVarGet* Func::Host::GetInp(TInt aInpId)
{
    MDVarGet* res = NULL;
    Elem::TIfRange inpr = GetInps(aInpId);
    if (inpr.first != inpr.second) {
	res = (MDVarGet*) *inpr.first;
    }
    return res;
}


// Diagonal matrix addition
template<class T> Func* FAddMtrd<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MMtrdGet<T>::Type()) {
	res = new FAddMtrd<T>(*aHost);
    }
    return res;
}

template<class T> void *FAddMtrd<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrdGet<T>::Type()) == 0) res = (MMtrdGet<T>*) this;
    return res;
}

template<class T> void FAddMtrd<T>::MtrdGet(Mtrd<T>& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range = mHost.GetInps(EInp);
    T val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MMtrdGet<T>* dfget = (MMtrdGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Mtrd<T> arg;
	    dfget->MtrdGet(arg);
	    if (arg.size() == size) {
		for (TInt cnt = 0; cnt < size; cnt++) {
		    if (it == range.first) {
			aData.at(cnt) = 0.0;
		    }
		    aData.at(cnt) += arg.at(cnt);
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", dgetbase->GetUri().c_str());
	    }
	}
    }
}

// Matrix addition
template<class T> Func* FAddMtr<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MMtrGet<T>::Type()) {
	res = new FAddMtr<T>(*aHost);
    }
    return res;
}

template<class T> void *FAddMtr<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> void FAddMtr<T>::MtrGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp);
    T val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MMtrGet<T>* dfget = (MMtrGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Mtr<T> arg = aData;
	    dfget->MtrGet(arg);
	    if (arg.mValid) {
		if (arg.mDim == aData.mDim) {
		    for (TInt cntr = 0; cntr < aData.mDim.first; cntr++) {
			for (TInt cntc = 0; cntc < aData.mDim.second; cntc++) {
			    TInt cnt = aData.mDim.second*cntr + cntc;
			    if (it == range.first) {
				aData.mData.at(cnt) = 0.0;
			    }
			    aData.mData.at(cnt) += arg.mData.at(cnt);
			}
		    }
		}
		else {
		    mHost.LogWrite(MLogRec::EErr, "Incorrect dimensions of argument [%s]", dgetbase->GetUri().c_str());
		    res = EFalse;
		    break;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]", dgetbase->GetUri().c_str());
		res = EFalse;
		break;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]", dgetbase->GetUri().c_str());
	    res = EFalse;
	    break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}


template<class T> void FAddMtr<T>::GetResult(string& aResult)
{
    mRes.ToString(aResult);
}

// Composing vector from components

string AFCpsVectVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFCpsVectVar::AFCpsVectVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFCpsVectVar::AFCpsVectVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFCpsVectVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFCpsVectVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FCpsVect<float>::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange AFCpsVectVar::GetInps(TInt aId)
{
    stringstream ss;
    ss <<  "../../Capsule/Inp" << (aId - Func::EInp1);
    string inp_uri = ss.str();
    Elem* inp = GetNode(inp_uri);
    RqContext cont(this);
    return inp->GetIfi(MDVarGet::Type(), &cont);
}

const TInt FCpsVBase::KIndMax = 30;

// Composing vector from components: float
template<class T> Func* FCpsVect<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MVectGet<T>::Type()) {
	res = new FCpsVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FCpsVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> void FCpsVect<T>::VectGet(Vect<T>& aData)
{
    mData.clear();
    for (TInt ind = Func::EInp1; ind != KIndMax; ind++) {
	Elem::TIfRange inp = mHost.GetInps(ind);
	if (inp.first == inp.second) {
	    break;
	}
	float comp;
	MDVarGet* dget = (MDVarGet*) *(inp.first);
	Elem* dgetbase = dget->VarGetBase();
	MDFloatGet* dfget = dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    comp = dfget->Value();
	}
	else {
	    MDIntGet* diget = dgetbase->GetObj(diget);
	    if (dget != NULL) {
		comp = (float) diget->Value();
	    }
	    else {
		// Unsupported type of input
		break;
	    }
	}
	mData.push_back(comp);
    }
    aData = mData;
}

template<class T> void FCpsVect<T>::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  MVectGet<T>::TypeSig();
    for (typename Vect<T>::const_iterator it = mData.begin(); it != mData.end(); it++) {
	ss << " " << *it;
    }
    aResult = ss.str();
}


// Multiplication, variable type
string AFMplVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFMplVar::AFMplVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFMplVar::AFMplVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFMplVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFMplVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FMplFloat::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange AFMplVar::GetInps(TInt aId)
{
    if (aId == FAddBase::EInp) {
	Elem* inp = GetNode("../../Capsule/Inp");
	__ASSERT(inp != NULL);
	RqContext cont(this);
	return inp->GetIfi(MDVarGet::Type(), &cont);
    }
}

// Multiplication, variable type - Float function
Func* FMplFloat::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDFloatGet::Type()) {
	res = new FMplFloat(*aHost);
    }
    return res;
}

void *FMplFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FMplFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDFloatGet* dfget = (MDFloatGet*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    val *= dfget->Value();
	}
	else {
	    MDIntGet* diget = (MDIntGet*) dgetbase->GetSIfi(MDIntGet::Type());
	    if (diget != NULL) {
		val *= (float) diget->Value();
	    }
	}
    }
    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FMplFloat::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}

// Non commutative Multiplication, variable type
string AFMplncVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFMplncVar::AFMplncVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFMplncVar::AFMplncVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFMplncVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFMplncVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FMplMtrdVect<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplMtr<float>::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange AFMplncVar::GetInps(TInt aId)
{
    Elem::TIfRange res;
    string uri;
    if (aId == FMplncBase::EInp1) {
	uri = "../../Capsule/Inp1";
    }
    else if (aId == FMplncBase::EInp2) { 
	uri = "../../Capsule/Inp2";
    }
    if (!uri.empty()) {
	Elem* inp = GetNode(uri);
	__ASSERT(inp != NULL);
	RqContext cont(this);
	res = inp->GetIfi(MDVarGet::Type(), &cont);
    }
    return res;
}

// Multiplication, diag matrix * vector
template<class T> Func* FMplMtrdVect<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MVectGet<T>::Type()) {
	res = new FMplMtrdVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FMplMtrdVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> void FMplMtrdVect<T>::VectGet(Vect<T>& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range1 = mHost.GetInps(EInp1);
    Elem::TIfRange range2 = mHost.GetInps(EInp2);
    if (range1.first != range1.second && range2.first != range2.second) {
	MDVarGet* dget = (MDVarGet*) (*range1.first);
	Elem* dgetbase = dget->VarGetBase();
	MMtrdGet<T>* dfget1 = (MMtrdGet<T>*) dgetbase->GetObj(dfget1);
	dget = (MDVarGet*) (*range2.first);
	dgetbase = dget->VarGetBase();
	MVectGet<T>* dfget2 = (MVectGet<T>*) dgetbase->GetObj(dfget2);
	if (dfget1 != NULL && dfget2 != NULL) {
	    Vect<T> vect;
	    vect.resize(size);
	    dfget2->VectGet(vect);
	    Mtrd<T> mtr;
	    mtr.resize(size);
	    dfget1->MtrdGet(mtr);
	    int vsize = vect.size();
	    if (mtr.size() == size) {
		for (TInt cnt = 0; cnt < size; cnt++) {
		    T vdata = vect.at(cnt);
		    T mdata = mtr.at(cnt);
		    aData.at(cnt) = vdata * mdata;
		}
		mRes = aData;
		mHost.OnFuncContentChanged();
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", dgetbase->GetUri().c_str());
	    }
	}
    }
}

template<class T> void FMplMtrdVect<T>::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  MVectGet<T>::TypeSig();
    for (typename Vect<T>::const_iterator it = mRes.begin(); it != mRes.end(); it++) {
	ss << " " << *it;
    }
    aResult = ss.str();
}

// Matrix mutltiplication
template<class T> Func* FMplMtr<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MMtrGet<T>::Type()) {
	res = new FMplMtr<T>(*aHost);
    }
    return res;
}

template<class T> void *FMplMtr<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> void FMplMtr<T>::MtrGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range1 = mHost.GetInps(EInp1);
    Elem::TIfRange range2 = mHost.GetInps(EInp2);
    if (range1.first != range1.second && range2.first != range2.second) {
	MDVarGet* dget = (MDVarGet*) (*range1.first);
	Elem* dgetbase = dget->VarGetBase();
	MMtrGet<T>* dfget1 = (MMtrGet<T>*) dgetbase->GetObj(dfget1);
	dget = (MDVarGet*) (*range2.first);
	dgetbase = dget->VarGetBase();
	MMtrGet<T>* dfget2 = (MMtrGet<T>*) dgetbase->GetObj(dfget2);
	if (dfget1 != NULL && dfget2 != NULL) {
	    Mtr<T> arg1;
	    arg1.mDim.first = aData.mDim.first;
	    Mtr<T> arg2;
	    arg2.mDim.second = aData.mDim.second;
	    dfget1->MtrGet(arg1);
	    dfget2->MtrGet(arg2);
	    if (arg1.mValid && arg2.mValid) {
		if (arg1.mDim.second == arg2.mDim.first && arg1.mDim.first == aData.mDim.first && arg2.mDim.second == aData.mDim.second) {
		    MtrBase::TMtrType restype = MtrBase::EMt_Regular;
		    if (arg1.mType == MtrBase::EMt_Diagonal) {
			restype = arg2.mType;
		    }
		    else if (arg2.mType == MtrBase::EMt_Diagonal) {
			restype = arg1.mType;
		    }
		    aData.mType = restype;
		    if (restype == MtrBase::EMt_Regular) {
			for (TInt cntr = 0; cntr < aData.mDim.first; cntr++) {
			    for (TInt cntc = 0; cntc < aData.mDim.second; cntc++) {
				TInt cnt = aData.mDim.second*cntr + cntc;
				T data = T();
				if (arg1.mType == MtrBase::EMt_Diagonal &&  arg2.mType == MtrBase::EMt_Diagonal) {
				    data += arg1.GetElem(cntr, cntr) * arg2.GetElem(cntc, cntc);
				}
				else if (arg1.mType == MtrBase::EMt_Diagonal) {
				    data += arg1.GetElem(cntr, cntr) * arg2.GetElem(cntr, cntc);
				}
				else if (arg2.mType == MtrBase::EMt_Diagonal) {
				    data += arg1.GetElem(cntr, cntc) * arg2.GetElem(cntc, cntc);
				}
				else {
				    for (TInt cntm = 0; cntm < arg1.mDim.second; cntm++) {
					TInt ind1 = arg1.mDim.second*cntr + cntm;
					TInt ind2 = arg2.mDim.second*cntm + cntc;
					data += arg1.mData.at(ind1) * arg2.mData.at(ind2);
				    }
				}
				aData.mData.at(cnt) = data;
			    }
			}
		    }
		}
		else {
		    mHost.LogWrite(MLogRec::EErr, "Incorrect dimensions of argument [%s]", dgetbase->GetUri().c_str());
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]", dgetbase->GetUri().c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]", dgetbase->GetUri().c_str());
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplMtr<T>::GetResult(string& aResult)
{
    mRes.ToString(aResult);
}


// Inversion for multiplication operation, variable
string AFMplinvVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFMplinvVar::AFMplinvVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFMplinvVar::AFMplinvVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFMplinvVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFMplinvVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    //if ((mFunc = FMplinvMtrd<float>::Create(this, aIfaceName)) != NULL);
    if ((mFunc = FMplinvMtr<float>::Create(this, aIfaceName)) != NULL);
}


string AFMplinvVar::GetInpUri(TInt aId) 
{
    if (aId == FMplinvBase::EInp) return "Inp"; else return string();
}

// Inversion for multiplication operation: diagonal matrix
template<class T> Func* FMplinvMtrd<T>::Create(Host* aHost, const string& aOutIid)
{
    Func* res = NULL;
    if (aOutIid == MMtrdGet<T>::Type()) {
	res = new FMplinvMtrd<T>(*aHost);
    }
    return res;
}

template<class T> void *FMplinvMtrd<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrdGet<T>::Type()) == 0) res = (MMtrdGet<T>*) this;
    return res;
}

template<class T> void FMplinvMtrd<T>::MtrdGet(Mtrd<T>& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range = mHost.GetInps(EInp);
    if (range.first != range.second) {
	MDVarGet* dget = (MDVarGet*) (*range.first);
	Elem* dgetbase = dget->VarGetBase();
	MMtrdGet<T>* dfget = (MMtrdGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Mtrd<T> arg;
	    arg.resize(size);
	    dfget->MtrdGet(arg);
	    if (arg.size() == size) {
		for (TInt cnt = 0; cnt < size; cnt++) {
		    aData.at(cnt) = 1 / arg.at(cnt);
		}
		mRes = aData;
		mHost.OnFuncContentChanged();
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", dgetbase->GetUri().c_str());
	    }
	}
    }
}

template<class T> void FMplinvMtrd<T>::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  MMtrdGet<T>::TypeSig();
    for (typename Mtrd<T>::const_iterator it = mRes.begin(); it != mRes.end(); it++) {
	ss << " " << *it;
    }
    aResult = ss.str();
}


// Inversion for multiplication operation:  matrix

template<class T> Func* FMplinvMtr<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MMtrGet<T>::Type()) {
	res = new FMplinvMtr<T>(*aHost);
    }
    return res;
}

template<class T> void *FMplinvMtr<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> void FMplinvMtr<T>::MtrGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp);
    if (range.first != range.second) {
	MDVarGet* dget = (MDVarGet*) (*range.first);
	Elem* dgetbase = dget->VarGetBase();
	MMtrGet<T>* dfget = (MMtrGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Mtr<T> arg = aData;
	    dfget->MtrGet(arg);
	    if (arg.mValid) {
		if ((aData.mDim.first == 0 || arg.mDim.first == aData.mDim.first) && (aData.mDim.second == 0 || arg.mDim.second == aData.mDim.second)) {
		    if (arg.mType == MtrBase::EMt_Diagonal) {
			// Only diag mtr is supported for now
			aData = arg;
			for (TInt cntr = 0; cntr < aData.mDim.first; cntr++) {
			    T sdata = arg.mData.at(cntr);
			    aData.mData.at(cntr) = 1 / sdata;
			}
		    }
		}
		else {
		    mHost.LogWrite(MLogRec::EErr, "Incorrect dimensions of argument [%s]", dgetbase->GetUri().c_str());
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]", dgetbase->GetUri().c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]", dgetbase->GetUri().c_str());
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}


template<class T> void FMplinvMtr<T>::GetResult(string& aResult)
{
    mRes.ToString(aResult);
}



// Composing of diag matrix, variable
string AFCpsMtrdVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFCpsMtrdVar::AFCpsMtrdVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFCpsMtrdVar::AFCpsMtrdVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFCpsMtrdVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFCpsMtrdVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    if ((mFunc = FCpsMtrdVect<float>::Create(this, aIfaceName, inp1)) != NULL);
}

string AFCpsMtrdVar::GetInpUri(TInt aId) 
{
    if (aId == Func::EInp1) return "Inp"; else return string();
}

// Composing of diag matrix, from vector
template<class T> Func* FCpsMtrdVect<T>::Create(Host* aHost, const string& aString, MDVarGet* aInp1Var)
{
    Func* res = NULL;
    if (aString == MMtrGet<T>::Type()) {
	res = new FCpsMtrdVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FCpsMtrdVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MMtrGet<T>::Type()) == 0) res = (MMtrGet<T>*) this;
    return res;
}

template<class T> void FCpsMtrdVect<T>::MtrGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp1);
    if (range.first != range.second) {
	MDVarGet* dget = (MDVarGet*) (*range.first);
	Elem* dgetbase = dget->VarGetBase();
	MMtrGet<T>* dfget = (MMtrGet<T>*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    Mtr<T> arg;
	    arg.mDim = MtrBase::TMtrDim(aData.mDim.first, 1);
	    dfget->MtrGet(arg);
	    if (arg.mValid) {
		aData.mType = MtrBase::EMt_Diagonal;
		aData.mDim.first = arg.mDim.first;
		aData.mDim.second = arg.mDim.first;
		aData.mData.resize(arg.mDim.first);
		for (TInt cnt = 0; cnt < arg.mDim.first; cnt++) {
		    aData.mData.at(cnt) = arg.mData.at(cnt);
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Invalid argument [%s]", dgetbase->GetUri().c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]", dgetbase->GetUri().c_str());
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FCpsMtrdVect<T>::GetResult(string& aResult)
{
    mRes.ToString(aResult);
}


// Division, variable
string AFDivVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFDivVar::AFDivVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFDivVar::AFDivVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFDivVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFDivVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FDivFloat::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange AFDivVar::GetInps(TInt aId)
{
    if (aId == FAddBase::EInp) {
	Elem* inp = GetNode("../../Capsule/Inp");
	__ASSERT(inp != NULL);
	RqContext cont(this);
	return inp->GetIfi(MDVarGet::Type(), &cont);
    }
}

// Division, variable type - Float function
Func* FDivFloat::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDFloatGet::Type()) {
	res = new FDivFloat(*aHost);
    }
    return res;
}

void *FDivFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FDivFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDFloatGet* dfget = (MDFloatGet*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    val *= dfget->Value();
	}
	else {
	    MDIntGet* diget = (MDIntGet*) dgetbase->GetSIfi(MDIntGet::Type());
	    if (diget != NULL) {
		val *= (float) diget->Value();
	    }
	}
    }
    range = mHost.GetInps(EInp_Dvs);
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MDFloatGet* dfget = (MDFloatGet*) dgetbase->GetObj(dfget);
	float dvs = 0.0;
	if (dfget != NULL) {
	    dvs = dfget->Value();
	}
	else {
	    MDIntGet* diget = (MDIntGet*) dgetbase->GetSIfi(MDIntGet::Type());
	    if (diget != NULL) {
		dvs = (float) diget->Value();
	    }
	}
	if (dvs != 0.0) {
	    val /= dvs;
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Null divisor [%s]", dgetbase->GetUri().c_str());
	}
    }

    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FDivFloat::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}




void FBcmpBase::GetResult(string& aResult)
{
    stringstream ss;
    ss <<  "B " << std::boolalpha << mRes;
    aResult = ss.str();
}

Func* FBcmpFloat::Create(Host* aHost, const string& aOutIid, MDVarGet* aInp1Var, MDVarGet* aInp2Var, TFType aFType)
{
    Func* res = NULL;
    TBool inp1ok = IsInpComatible(aInp1Var);
    TBool inp2ok = IsInpComatible(aInp2Var);
    // Using two-side negotiation: first func requests type of inputs, if defined then tries to create corresponding
    // func. If not then attemp funcs from upper types to lower and request inputs types.
    if (aOutIid == MDBoolGet::Type() && inp1ok && inp2ok) {
	res = new FBcmpFloat(*aHost, aFType);
    }
    return res;
}

void *FBcmpFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDBoolGet::Type()) == 0) res = (MDBoolGet*) this;
    return res;
}

TBool FBcmpFloat::Value()
{
    TBool res = 0;
    float arg1 = GetArg(EInp1);
    float arg2 = GetArg(EInp2);
    if (mFType == ELt) 
	res = arg1 < arg2;
    else if (mFType == ELe) 
	res = arg1 <= arg2;
    else if (mFType == EEq) 
	res = arg1 == arg2;
    else if (mFType == EGt) 
	res = arg1 > arg2;
    else if (mFType == EGe) 
	res = arg1 >= arg2;
    mRes = res;
    mHost.OnFuncContentChanged();
    return res;
}

float FBcmpFloat::GetArg(TInt aInpId)
{
    float res = 0.0;
    MDVarGet* iv = mHost.GetInp(aInpId);
    if (iv != NULL) {
	string ifi = iv->VarGetIfid();
	if (!ifi.empty()) {
	    void* inp = iv->VarGetBase()->GetObj(ifi.c_str());
	    if (ifi == MDFloatGet::Type()) {
		res = ((MDFloatGet*) inp)->Value();
	    }
	    else if (ifi == MDIntGet::Type()) {
		res = (float) ((MDIntGet*) inp)->Value();
	    }
	}
    }
    return res;
}

TBool FBcmpFloat::IsInpComatible(MDVarGet* aInpVar)
{
    TBool res = EFalse;
    if (aInpVar != NULL) {
	string ifi = aInpVar->VarGetIfid();
	if (!ifi.empty()) {
	    res = (ifi == MDFloatGet::Type() || ifi == MDIntGet::Type());
	}
	else {
	    // No arg type is stated yet, to negotiate
	    void* inp = aInpVar->VarGetBase()->GetObj(MDFloatGet::Type());
	    if (inp == NULL) {
		inp = aInpVar->VarGetBase()->GetObj(MDIntGet::Type());
	    }
	    res = inp != NULL;
	}
    }
    return res;
}

// Greater-than function, variable data
string AFBcmpVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFBcmpVar::AFBcmpVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFBcmpVar::AFBcmpVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFBcmpVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFBcmpVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    MDVarGet* inp1 = GetInp(FBcmpBase::EInp_1);
    MDVarGet* inp2 = GetInp(FBcmpBase::EInp_2);
    if (inp1 != NULL && inp2 != NULL) {
	FBcmpBase::TFType ftype = GetFType();
	if ((mFunc = FBcmpFloat::Create(this, aIfaceName, inp1, inp2, ftype)) != NULL);
    }
}

Elem::TIfRange AFBcmpVar::GetInps(TInt aId)
{
    __ASSERT(aId == FBcmpBase::EInp_1 || aId == FBcmpBase::EInp_2);
    Elem* inp = NULL;
    inp = GetNode(aId == FBcmpBase::EInp_1 ? "../../Capsule/Inp1" : "../../Capsule/Inp2");
    __ASSERT(inp != NULL);
    RqContext cont(this);
    return inp->GetIfi(MDVarGet::Type(), &cont);
}

FBcmpBase::TFType AFBcmpVar::GetFType()
{
    FBcmpBase::TFType res = FBcmpBase::EEq;
    if (Name() == "AF_Lt") res = FBcmpBase::ELt;
    else if (Name() == "AF_Le") res = FBcmpBase::ELe;
    else if (Name() == "AF_Eq") res = FBcmpBase::EEq;
    else if (Name() == "AF_Gt") res = FBcmpBase::EGt;
    else if (Name() == "AF_Ge") res = FBcmpBase::EGe;
    else {
	Logger()->Write(MLogRec::EErr, this, "Incorrect type of function [%s]", Name().c_str());
    }
    return res;
}

// Comparition, variable data
string AFCmpVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFCmpVar::AFCmpVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFCmpVar::AFCmpVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFCmpVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFCmpVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	if ((mFunc = FCmpFloat::Create(this, aIfaceName, t1, t2)) != NULL);
    }
}

Elem::TIfRange AFCmpVar::GetInps(TInt aId)
{
    __ASSERT(aId == Func::EInp1 || aId == Func::EInp2);
    Elem* inp = NULL;
    inp = GetNode(aId == Func::EInp1 ? "../../Capsule/Inp1" : "../../Capsule/Inp2");
    __ASSERT(inp != NULL);
    RqContext cont(this);
    return inp->GetIfi(MDVarGet::Type(), &cont);
}

// Comparition  function, float
Func* FCmpFloat::Create(Host* aHost, const string& aOutIid, const string& aInp1Iid, const string& aInp2Iid)
{
    Func* res = NULL;
    if (aOutIid == MDBoolGet::Type() && (aInp1Iid == MDFloatGet::Type() || aInp1Iid == MDIntGet::Type()) 
	  && (aInp2Iid == MDFloatGet::Type() || aInp2Iid == MDIntGet::Type())) {
	res = new FCmpFloat(*aHost);
    }
    return res;
}

void *FCmpFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDIntGet::Type()) == 0) res = (MDIntGet*) this;
    return res;
}

TInt FCmpFloat::Value()
{
    TInt res = 0;
    float arg1 = GetArg(EInp1);
    float arg2 = GetArg(EInp2);
    if (arg1 < arg2) res = -1;
    else if (arg1 > arg2) res = 1;
    return res;
}

float FCmpFloat::GetArg(TInt aInpId)
{
    float res = 0.0;
    MDVarGet* iv = mHost.GetInp(aInpId);
    if (iv != NULL) {
	string ifi = iv->VarGetIfid();
	if (!ifi.empty()) {
	    void* inp = iv->VarGetBase()->GetObj(ifi.c_str());
	    if (ifi == MDFloatGet::Type()) {
		res = ((MDFloatGet*) inp)->Value();
	    }
	    else if (ifi == MDIntGet::Type()) {
		res = (float) ((MDIntGet*) inp)->Value();
	    }
	}
    }
    return res;
}



// Getting component of container
string AFAtVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFAtVar::AFAtVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFAtVar::AFAtVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFAtVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFAtVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	if ((mFunc = FAtVect<float>::Create(this, aIfaceName, t1)) != NULL);
    }
}

Elem::TIfRange AFAtVar::GetInps(TInt aId)
{
    __ASSERT(aId == Func::EInp1 || aId == Func::EInp2);
    Elem* inp = NULL;
    inp = GetNode(aId == Func::EInp1 ? "../../Capsule/Inp" : "../../Capsule/Index");
    __ASSERT(inp != NULL);
    RqContext cont(this);
    return inp->GetIfi(MDVarGet::Type(), &cont);
}

TInt FAtBase::GetInd()
{
    TInt res = 0;
    MDVarGet* iv = mHost.GetInp(EInp2);
    if (iv != NULL) {
	MDIntGet* intget = iv->VarGetBase()->GetObj(intget);
	if (intget != NULL) {
	    res = intget->Value();
	}
    }
    return res;
}

// Getting component of container: vector of floats
template <class T> Func* FAtVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (aOutIid == MDataGet<T>::Type() && aInpIid == MVectGet<T>::Type()) {
	res = new FAtVect<T>(*aHost);
    }
    return res;
}

template <class T> void *FAtVect<T>::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDataGet<T>::Type()) == 0) res = (MDataGet<T>*) this;
    return res;
}

template <class T> void FAtVect<T>::DataGet(T& aData)
{
    MVectGet<T>* arg = GetArg();
    if (arg != NULL) {
	TInt ind = GetInd();
	Vect<T> data;
	arg->VectGet(data);
	aData = data.at(ind);
    }
}

template <class T> MVectGet<T>* FAtVect<T>::GetArg()
{
    MVectGet<T>* res = NULL;
    MDVarGet* iv = mHost.GetInp(EInp1);
    if (iv != NULL) {
	string ifi = iv->VarGetIfid();
	if (!ifi.empty()) {
	    void* inp = iv->VarGetBase()->GetObj(ifi.c_str());
	    if (ifi == MVectGet<T>::Type()) {
		res = (MVectGet<T>*) inp;
	    }
	}
    }
    return res;
}

// Boolean case - if
string AFSwitchVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFSwitchVar::AFSwitchVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetEType(Type(), AFunVar::PEType());
    SetParent(Type());
}

AFSwitchVar::AFSwitchVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetEType(AFunVar::PEType());
    SetParent(AFunVar::PEType());
}

void *AFSwitchVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	if (mFunc == NULL) {
	    Init(aName);
	    if (mFunc != NULL) {
		res = mFunc->DoGetObj(aName, aIncUpHier);
	    }
	}
	else {
	    res = mFunc->DoGetObj(aName, aIncUpHier);
	    if (res == NULL) {
		Init(aName);
		if (mFunc != NULL) {
		    res = mFunc->DoGetObj(aName, aIncUpHier);
		}
	    }
	}
    }
    else {
	res = AFunVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AFSwitchVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    MDVarGet* inp_case = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    MDVarGet* inp3 = GetInp(Func::EInp3);
    if (inp_case != NULL && inp2 != NULL && inp3 != NULL) {
	string t_case = inp_case->VarGetIfid();
	if ((mFunc = FSwitchBool::Create(this, aIfaceName, t_case)) != NULL);
    }
}

// TODO [YB] It is simpler to use just mapping Func::EInp* to inputs uris
Elem::TIfRange AFSwitchVar::GetInps(TInt aId)
{
    Elem* inp = NULL;
    string inp_uri = "../../Capsule/Sel";
    if (aId != Func::EInp1) {
	stringstream ss;
	ss <<  "../../Capsule/Inp" << (aId - Func::EInp1);
	inp_uri = ss.str();
    }
    inp = GetNode(inp_uri);
    __ASSERT(inp != NULL);
    RqContext cont(this);
    return inp->GetIfi(MDVarGet::Type(), &cont);
}

// Case - commutation of inputs
Func* FSwitchBool::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (aOutIid == MDVarGet::Type() && aInpIid == MDBoolGet::Type()) {
	res = new FSwitchBool(*aHost);
    }
    return res;
}

void *FSwitchBool::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    // There are two approach of commutation. The first one is to commutate MDVarGet ifase, i.e.
    // that swithcer result is this iface of selected case. The second is that switcher 
    // implements MDVarGet by itselt and does commutatation in this iface methods.
    // The first approach requires iface cache refresh any time the swithcher ctrl is changed.
    // This is not what the cache is intended to and makes overhead. So let's select approach#2 for now.
    if (strcmp(aName, MDVarGet::Type()) == 0) res = (MDVarGet*) this;
    // if (strcmp(aName, MDVarGet::Type()) == 0) res = GetCase();
    return res;
}

TBool FSwitchBool::GetCtrl() const
{
    TBool res = false;
    MDVarGet* iv = mHost.GetInp(EInp1);
    if (iv != NULL) {
	MDBoolGet* getif= iv->VarGetBase()->GetObj(getif);
	if (getif != NULL) {
	    res = getif->Value();
	}
    }
    return res;
}

MDVarGet* FSwitchBool::GetCase() const
{
    MDVarGet* res = NULL;
    TBool case_id = GetCtrl();
    res = mHost.GetInp(case_id ? 2 : 1);
    return res;
}

Elem* FSwitchBool::VarGetBase()
{
    MDVarGet* cs = GetCase();
    return cs != NULL ? cs->VarGetBase() : NULL;
}

string FSwitchBool::VarGetIfid() const
{
    MDVarGet* cs = GetCase();
    return cs->VarGetIfid();
}
