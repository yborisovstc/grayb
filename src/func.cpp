
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

string AFunInt::PEType()
{
    return FuncBase::PEType() + GUri::KParentSep + Type();
}

AFunInt::AFunInt(const string& aName, Elem* aMan, MEnv* aEnv): FuncBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type(), FuncBase::PEType());
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
	Elem* host = iMan->GetMan();
	Logger()->WriteFormat("[%s/%s] updated [%d <- %d]", host->Name().c_str(), Name().c_str(), mData, aData);
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


string AIncInt::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AIncInt::AIncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
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

string AFunIntRes::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AFunIntRes::AFunIntRes(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetEType(Type(), AFunInt::PEType());
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
//    MDIntGet* mget = GetInp("inp");
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:inp");
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
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:inp");
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
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:inp");
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
    Elem* caps = aContext.GetNode("Elem:Capsule");
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
    Elem* node = GetNode("../../Elem:Logspec/DataLogevent:Update");
    return node != NULL;
}


// Integer Function agent base wo caching

string AFuncInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFuncInt::AFuncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
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

void AFuncInt::GetCont(string& aCont)
{
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
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFSubInt::AFSubInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
    SetParent(Type());
}

void *AFSubInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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

TInt AFSubInt::Value()
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
	GUri fullpath;
	Elem* host = iMan->GetMan();
	host->GetUri(fullpath);
	if (minp == NULL) {
	    Logger()->WriteFormat("[%s]: Error - Inp not connected", fullpath.GetUri(ETrue).c_str());
	}
	else if (mlimu == NULL) {
	    Logger()->WriteFormat("[%s]: Error - Inp_LimU not connected", fullpath.GetUri(ETrue).c_str());
	}
	else if (mliml == NULL) {
	    Logger()->WriteFormat("[%s]: Error - Inp_LimL not connected", fullpath.GetUri(ETrue).c_str());
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
    Elem* einp = GetNode("../../Elem:Capsule/ConnPoint:inp");
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
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFConvInt::AFConvInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv)
{
    SetEType(Type(), AFunc::PEType());
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
	res = AFunc::DoGetObj(aName, aIncUpHier);
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
	res = AFunc::DoGetObj(aName.c_str(), EFalse, aCtx);
    }
    if (res != NULL) {
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
    }
}

TInt AFConvInt::Value()
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
	    Logger()->WriteFormat("[%s]: Result = %d", fullpath.GetUri(ETrue).c_str(), val);
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
	map<string, void*>::iterator ri = iExecs.find(aName);
	if (ri != iExecs.end()) {
	    res = ri->second;
	}
    }
    return res;
}

TBool AFuncm::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = ETrue;
    Elem* caps = aContext.GetNode("Elem:Capsule");
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
    Elem* einp = iHost.GetNode("../../Elem:Capsule/ConnPoint:inp");
    RqContext cont(&iHost);
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


