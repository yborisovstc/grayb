
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
	Elem* cp = caps->GetCompOwning("ConnPoint", &aComp);
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
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
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
	Elem* cp = caps->GetCompOwning("ConnPoint", &aComp);
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

Elem* AFunVar::VarGetBase() 
{
    return this;
}

Elem::TIfRange AFunVar::GetInps(TInt aId)
{
    return TIfRange(IfIter(), IfIter());
}

void AFunVar::LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...)
{
    va_list list;
    va_start(list,aFmt);
    Logger()->Write(aCtg, this, aFmt, list);
}



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
    else if ((mFunc = FAddVFloat::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange AFAddVar::GetInps(TInt aId)
{
    if (aId == FAddBase::EInp) {
	Elem* inp = GetNode("../../Capsule/Inp");
	__ASSERT(inp != NULL);
	RqContext cont(this);
	return inp->GetIfi(MDVarGet::Type(), &cont);
    }
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
    return val;
}


// Float vector addition
Func* FAddVFloat::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MVFloatGet::Type()) {
	res = new FAddVFloat(*aHost);
    }
    return res;
}

void *FAddVFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MVFloatGet::Type()) == 0) res = (MVFloatGet*) this;
    return res;
}

void FAddVFloat::VFloatGet(VFloat& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	Elem* dgetbase = dget->VarGetBase();
	MVFloatGet* dfget = (MVFloatGet*) dgetbase->GetObj(dfget);
	if (dfget != NULL) {
	    VFloat arg;
	    dfget->VFloatGet(arg);
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


MDVarGet* Func::Host::GetInp(TInt aInpId)
{
    MDVarGet* res = NULL;
    Elem::TIfRange inpr = GetInps(aInpId);
    if (inpr.first != inpr.second) {
	res = (MDVarGet*) *inpr.first;
    }
    return res;
}

Func* FBcmpFloat::Create(Host* aHost, const string& aOutIid, const string& aInp1Iid, const string& aInp2Iid, TFType aFType)
{
    Func* res = NULL;
    if (aOutIid == MDBoolGet::Type() && (aInp1Iid == MDFloatGet::Type() || aInp1Iid == MDIntGet::Type()) 
	  && (aInp2Iid == MDFloatGet::Type() || aInp2Iid == MDIntGet::Type())) {
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
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	FBcmpBase::TFType ftype = GetFType();
	if ((mFunc = FBcmpFloat::Create(this, aIfaceName, t1, t2, ftype)) != NULL);
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
	if ((mFunc = FAtVFloat::Create(this, aIfaceName, t1)) != NULL);
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
Func* FAtVFloat::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (aOutIid == MDFloatGet::Type() && aInpIid == MVFloatGet::Type()) {
	res = new FAtVFloat(*aHost);
    }
    return res;
}

void *FAtVFloat::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FAtVFloat::Value()
{
    float res = 0;
    MVFloatGet* arg = GetArg();
    if (arg != NULL) {
	TInt ind = GetInd();
	VFloat data;
	arg->VFloatGet(data);
	res = data.at(ind);
    }
    return res;
}

MVFloatGet* FAtVFloat::GetArg()
{
    MVFloatGet* res = NULL;
    MDVarGet* iv = mHost.GetInp(EInp1);
    if (iv != NULL) {
	string ifi = iv->VarGetIfid();
	if (!ifi.empty()) {
	    void* inp = iv->VarGetBase()->GetObj(ifi.c_str());
	    if (ifi == MVFloatGet::Type()) {
		res = (MVFloatGet*) inp;
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
