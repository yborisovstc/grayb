
#include "mvert.h"
#include "vert.h"
#include "prop.h"
#include "mdata.h"
#include "func.h"
#include "log.h"
#include <stdio.h>

string FuncBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

FuncBase::FuncBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

FuncBase::FuncBase(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *FuncBase::DoGetObj(const char *aName)
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
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool FuncBase::HandleCompChanged(MElem& aContext, MElem& aComp)
{
    TBool res = ETrue;
    MElem* caps = aContext.GetNode("./Capsule");
    if (caps != NULL) {
	// TODO [YB] To optimize by using proper utility
	MElem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
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
    Elem* eout = GetNodeE("./../../Capsule/out");
    if (eout != NULL) {
	MVert* mvout = eout->GetObj(mvout);
	MVert* mpair = *(mvout->Pairs().begin());
	if (mpair != NULL) {
	    Elem* epair = mpair->EBase()->GetObj(epair);
	    if (epair != NULL) {
		MDataObserver* obsr = (MDataObserver*) epair->GetSIfiC(MDataObserver::Type(), this);
		if (obsr != NULL) {
		    obsr->OnDataChanged();
		}
	    }
	}
    }
}

TBool FuncBase::HandleIoChanged(MElem& aContext, MElem* aCp)
{
    return ETrue;
}

// Agent base of Int function

string AFunInt::PEType()
{
    return FuncBase::PEType() + GUri::KParentSep + Type();
}

AFunInt::AFunInt(const string& aName, Elem* aMan, MEnv* aEnv): FuncBase(aName, aMan, aEnv), mData(0)
{
    SetParent(Type());
}

AFunInt::AFunInt(Elem* aMan, MEnv* aEnv): FuncBase(Type(), aMan, aEnv), mData(0)
{
    SetParent(FuncBase::PEType());
}

void *AFunInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = FuncBase::DoGetObj(aName);
    }
    return res;
}

void AFunInt::SetRes(TInt aData)
{
    if (mData != aData) {
	Elem* host = ToElem(iMan->GetMan());
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
    Elem* einp = GetNodeE("./../../Capsule/" + aInpName);
    if (einp != NULL) {
	Vert* vert = einp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    Elem* epair = pair->EBase()->GetObj(epair);
	    if (epair != NULL) {
		res = (MDIntGet*) epair->GetSIfiC(MDIntGet::Type(), this);
	    }
	}
    }
    return res;
}


string AIncInt::PEType()
{
    return AFunInt::PEType() + GUri::KParentSep + Type();
}

AIncInt::AIncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunInt(aName, aMan, aEnv)
{
    SetParent(Type());
}

AIncInt::AIncInt(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetParent(AFunInt::PEType());
}

void *AIncInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunInt::DoGetObj(aName);
    }
    return res;
}

TBool AIncInt::HandleIoChanged(MElem& aContext, MElem* aCp)
{
    TBool res = ETrue;
    // Checking input change
    if (aCp->Name() == "inp") {
	MDIntGet* dget = (MDIntGet*) aCp->GetSIfiC(MDIntGet::Type(), this);
	if (dget != NULL) {
	    TInt val = dget->Value();
	    SetRes(val + 1);
	}
	else {
	    res = EFalse;
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
    SetParent(Type());
}

AFunIntRes::AFunIntRes(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetParent(AFunInt::PEType());
}

void *AFunIntRes::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = FuncBase::DoGetObj(aName);
    }
    return res;
}

TBool AFunIntRes::HandleIoChanged(MElem& aContext, MElem* aCp)
{
    TBool res = ETrue;
    Vert* vert = aCp->GetObj(vert);
    MVert* pair = *(vert->Pairs().begin());
    if (pair != NULL) {
	Vert* vpair = pair->EBase()->GetObj(vpair);
	if (aCp->Name() == "inp") {
	    // Check input change
	    MDIntGet* dget = (MDIntGet*) vpair->GetSIfiC(MDIntGet::Type(), this);
	    if (dget != NULL) {
		TInt val = dget->Value();
		SetRes(val);
		UpdateOutp();
	    }
	    else {
		res = EFalse;
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
    Elem* out = GetNodeE("./../../Capsule/out");
    if (out != NULL) {
	Vert* vert = out->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    Vert* vpair = pair->EBase()->GetObj(vpair);
	    MDIntSet* dset = (MDIntSet*) vpair->GetSIfiC(MDIntSet::Type(), this);
	    if (dset != NULL) {
		dset->SetValue(mData);
	    }
	}
    }
}

void AFunIntRes::OnDataChanged()
{
//    MDIntGet* mget = GetInp("inp");
    Elem* einp = GetNodeE("./../../Capsule/inp");
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
    SetParent(Type());
}

AAddInt::AAddInt(Elem* aMan, MEnv* aEnv): AFunInt(Type(), aMan, aEnv)
{
    SetParent(AFunInt::PEType());
}

void *AAddInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunInt::DoGetObj(aName);
    }
    return res;
}

TBool AAddInt::HandleIoChanged(MElem& aContext, MElem* aCp)
{
    TBool res = ETrue;
    // Checking input change
    if (aCp->Name() == "inp") {
	RqContext cont(this);
	TIfRange range = aCp->GetIfi("MDIntGet", &cont);
	TInt val = 0;
	for (TIfIter it = range.first; it != range.second; it++) {
	    MDIntGet* dget = (MDIntGet*) (*it);
	    if (dget != NULL) {
		val += dget->Value();
	    }
	}
	SetRes(val);
    }
    return res;
}

void AAddInt::OnDataChanged()
{
    Elem* einp = GetNodeE("./../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

void *ACountCritInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunInt::DoGetObj(aName);
    }
    return res;
}

TBool ACountCritInt::HandleIoChanged(MElem& aContext, MElem* aCp)
{
    TBool res = ETrue;
    // Checking input change
    if (aCp->Name() == "inp") {
	RqContext cont(this);
	TIfRange range = aCp->GetIfi("MDIntGet", &cont);
	TInt val = 0;
	for (TIfIter it = range.first; it != range.second; it++) {
	    MDIntGet* dget = (MDIntGet*) (*it);
	    if (dget != NULL) {
		val += dget->Value();
	    }
	}
	SetRes(val);
    }
    return res;
}

void ACountCritInt::OnDataChanged()
{
    Elem* einp = GetNodeE("./../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

AFunc::AFunc(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *AFunc::DoGetObj(const char *aName)
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
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool AFunc::HandleCompChanged(MElem& aContext, MElem& aComp)
{
    TBool res = ETrue;
    MElem* caps = aContext.GetNode("./Capsule");
    if (caps != NULL) {
	MElem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}

void AFunc::NotifyUpdate()
{
    Elem* eout = GetNodeE("./../../Capsule/out");
    __ASSERT(eout != NULL);
    MVert* mvout = eout->GetObj(mvout);
    MVert* mpair = *(mvout->Pairs().begin());
    if (mpair != NULL) {
	Elem* epair = mpair->EBase()->GetObj(epair);
	if (epair != NULL) {
	    MDataObserver* obsr = (MDataObserver*) epair->GetSIfiC(MDataObserver::Type(), this);
	    if (obsr != NULL) {
		obsr->OnDataChanged();
	    }
	}
    }
}

void AFunc::OnDataChanged()
{
    NotifyUpdate();
}

TBool AFunc::IsLogeventUpdate()
{
    Elem* node = GetNodeE("./../../Logspec/Update");
    return node != NULL;
}


// Integer Function agent base wo caching

string AFuncInt::PEType()
{
    return AFunc::PEType() + GUri::KParentSep + Type();
}

AFuncInt::AFuncInt(const string& aName, Elem* aMan, MEnv* aEnv): AFunc(aName, aMan, aEnv), mData(0)
{
    SetParent(Type());
}

AFuncInt::AFuncInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv), mData(0)
{
    SetParent(AFunc::PEType());
}


void *AFuncInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
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

void AFuncInt::GetCont(string& aCont, const string& aName)
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
    SetParent(Type());
}

AFAddInt::AFAddInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetParent(AFunc::PEType());
}

void *AFAddInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

TInt AFAddInt::Value()
{
    Elem* einp = GetNodeE("./../../Capsule/inp");
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

AFSubInt::AFSubInt(Elem* aMan, MEnv* aEnv): AFuncInt(Type(), aMan, aEnv)
{
    SetParent(AFuncInt::PEType());
}


void *AFSubInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFuncInt::DoGetObj(aName);
    }
    return res;
}

TInt AFSubInt::GetValue()
{
    RqContext cont(this);
    TInt val = 0;
    // Positives
    Elem* einp = GetNodeE("./../../Capsule/InpP");
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    for (TIfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    val += dget->Value();
	}
    }
    // Negatives
    einp = GetNodeE("./../../Capsule/InpN");
    range = einp->GetIfi("MDIntGet", &cont);
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

AFLimInt::AFLimInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetParent(AFunc::PEType());
}


void *AFLimInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

TInt AFLimInt::Value()
{
    TInt val = 0;
    MDIntGet* minp = (MDIntGet*) GetSIfi("./../../Capsule/Inp", "MDIntGet");
    MDIntGet* mlimu = (MDIntGet*) GetSIfi("./../../Capsule/Inp_LimU", "MDIntGet");
    MDIntGet* mliml = (MDIntGet*) GetSIfi("./../../Capsule/Inp_LimL", "MDIntGet");
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
    SetParent(Type());
}

AFDivInt::AFDivInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetParent(AFunc::PEType());
}

void *AFDivInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

TInt AFDivInt::Value()
{
    TInt val = 0;
    MDIntGet* mdvd = (MDIntGet*) GetSIfi("./../../Capsule/Inp_DD", "MDIntGet");
    MDIntGet* mdvr = (MDIntGet*) GetSIfi("./../../Capsule/Inp_DR", "MDIntGet");
    if (mdvd != NULL && mdvr != NULL) {
	TInt dvd = mdvd->Value();
	TInt dvr = mdvr->Value();
	// TODO [YB] To add errors handling
	TBool lupd = IsLogeventUpdate();
	GUri fullpath;
	if (lupd) {
	    MElem* host = iMan->GetMan();
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
    SetParent(Type());
}

void *AFIntToVect::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MVIntGet::Type()) == 0) {
	res = (MVIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

vector<TInt> AFIntToVect::Value()
{
    vector<TInt> res;
    Elem* einp = GetNodeE("./../../Capsule/inp");
    __ASSERT(einp != NULL);
    RqContext cont(this);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
    iSampleHolder.iHost = this;
}

AFConvInt::AFConvInt(Elem* aMan, MEnv* aEnv): AFuncInt(Type(), aMan, aEnv)
{
    SetParent(AFuncInt::PEType());
    iSampleHolder.iHost = this;
}

void *AFConvInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    } else {
	res = AFuncInt::DoGetObj(aName);
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
	res = AFuncInt::DoGetObj(aName.c_str());
    }
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
}

TInt AFConvInt::GetValue()
{
    TInt val = 0;
    Elem* einp = GetNodeE("./../../Capsule/inp");
    __ASSERT(einp != NULL);
    Elem* einpwf = GetNodeE("./../../Capsule/Inp_WFres");
    __ASSERT(einpwf != NULL);
    Elem* eargwf = GetNodeE("./../../WFArg/Capsule/inp");
    __ASSERT(eargwf != NULL);
    RqContext cont(this);
    MDIntSet* wfarg = (MDIntSet*) eargwf->GetSIfi(MDIntSet::Type(), &cont);
    MDIntGet* wfres = (MDIntGet*) einpwf->GetSIfi("MDIntGet", &cont);
    if (wfres != NULL && wfarg != NULL) {
	TBool lupd = IsLogeventUpdate();
	GUri fullpath;
	if (lupd) {
	    MElem* host = iMan->GetMan();
	    host->GetUri(fullpath);
	}
	TIfRange range = einp->GetIfi("MDIntGet", &cont);
	for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

void *AFuncm::DoGetObj(const char *aName)
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
	res = Elem::DoGetObj(aName);
    }
    if (res == NULL) {
	map<string, Exec*>::iterator ri = mExecs.find(aName);
	if (ri != mExecs.end()) {
	    res = ri->second;
	}
    }
    return res;
}

TBool AFuncm::HandleCompChanged(MElem& aContext, MElem& aComp)
{
    TBool res = ETrue;
    MElem* caps = aContext.GetNode("./Capsule");
    if (caps != NULL) {
	MElem* cp = caps->GetCompOwning("ConnPointInp", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}

void AFuncm::NotifyUpdate()
{
    Elem* eout = GetNodeE("./../../Capsule/out");
    if (eout != NULL) {
	MVert* mvout = eout->GetObj(mvout);
	MVert* mpair = *(mvout->Pairs().begin());
	if (mpair != NULL) {
	    Elem* epair = mpair->EBase()->GetObj(epair);
	    if (epair != NULL) {
		MDataObserver* obsr = (MDataObserver*) epair->GetSIfiC(MDataObserver::Type(), this);
		if (obsr != NULL) {
		    obsr->OnDataChanged();
		}
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
    SetParent(Type());
}

void *AFuncmAdd::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFuncm::DoGetObj(aName);
    }
   return res;
}

TInt AFuncmAdd::ExcInt::Value()
{
    MElem* einp = mHost.GetNode("./../../Capsule/inp");
    RqContext cont(&mHost);
    TIfRange range = einp->GetIfi("MDIntGet", &cont);
    TInt val = 0;
    for (TIfIter it = range.first; it != range.second; it++) {
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
    SetParent(Type());
}

AFGTInt::AFGTInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetParent(AFunc::PEType());
}

void *AFGTInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDBoolGet::Type()) == 0) {
	res = (MDBoolGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

TBool AFGTInt::Value()
{
    Elem* einp1 = GetNodeE("./../../Capsule/Inp1");
    Elem* einp2 = GetNodeE("./../../Capsule/Inp2");
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
    SetParent(Type());
}

AFBoolToInt::AFBoolToInt(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv)
{
    SetParent(AFunc::PEType());
}

void *AFBoolToInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    return res;
}

TInt AFBoolToInt::Value()
{
    Elem* einp = GetNodeE("./../../Capsule/Inp");
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
    SetParent(Type());
}

AFunVar::AFunVar(Elem* aMan, MEnv* aEnv): AFunc(Type(), aMan, aEnv), mFunc(NULL)
{
    SetParent(AFunc::PEType());
}

void *AFunVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else {
	res = AFunc::DoGetObj(aName);
    }
    /* TODO [YB] Avoid init internal data via obj resolver, do we need to keep access at least?
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
    */
    return res;
}

TBool AFunVar::HandleCompChanged(MElem& aContext, MElem& aComp)
{
    TBool res = ETrue;
    MElem* caps = aContext.GetNode("./Capsule");
    if (caps != NULL) {
	MElem* cp = caps->GetCompOwning("Inp_FVar", &aComp);
	if (cp != NULL) {
	    NotifyUpdate();
	}
    }
    return res;
}

string AFunVar::VarGetIfid()
{
    // Iface negitiation sequence is starting here. It is possible that
    // the executive func is not set yet. In this case try to init w/o output iface info
    if (mFunc == NULL) 
	Init(string());
    return mFunc != NULL ? mFunc->IfaceGetId() : string();
}

void AFunVar::OnFuncContentChanged()
{
    OnContentChanged(*this);
}

void *AFunVar::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (mFunc == NULL) {
	Init(aName);
	if (mFunc != NULL) {
	    res = mFunc->DoGetObj(aName);
	}
    }
    else {
	res = mFunc->DoGetObj(aName);
	if (res == NULL) {
	    Init(aName);
	    if (mFunc != NULL) {
		res = mFunc->DoGetObj(aName);
	    }
	}
    }
    mIfaceReq = aName;
    return res;
}

Elem::TIfRange AFunVar::GetInps(TInt aId, TBool aOpt)
{
    TIfRange res;
    Elem* inp = GetNodeE("./../../Capsule/" + GetInpUri(aId));
    if (inp != NULL) {
	RqContext cont(this);
	res =  inp->GetIfi(MDVarGet::Type(), &cont);
    }
    else if (!aOpt) {
	Logger()->Write(MLogRec::EErr, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void AFunVar::LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...)
{
    char buf[GLogRec::KLogRecBufSize];
    va_list list;
    va_start(list,aFmt);
    vsprintf(buf, aFmt, list);
    Logger()->Write(aCtg, this, buf);
    va_end(list);
}

string AFunVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else if (aId == Func::EInp3) return "Inp3";
    else if (aId == Func::EInp4) return "Inp4";
    else return string();
}

void AFunVar::GetCont(string& aCont, const string& aName)
{
    if (mFunc != NULL) {
	mFunc->GetResult(aCont);
    }
    else {
	aCont = "Init ERR";
    }
}

TInt AFunVar::GetInpsCount() const
{
    TInt res = 0;
    TInt cpscnt = GetInpCpsCount();
    for (TInt ind = 0; ind < cpscnt || cpscnt == -1; ind++) {
	Elem::TIfRange range = ((AFunVar*) this)->GetInps(ind);
	if (range.first == range.second) {
	    if (cpscnt == -1) break;
	    else {
		res++;
		continue;
	    }
	}
	for (TIfIter it = range.first; it != range.second; it++) {
	    res++;
	}
    }
    return res;
}

void AFunVar::GetContInp(TInt aInd, string& aName, string& aCont) const
{
    TInt cind = 0;
    TInt cpscnt = GetInpCpsCount();
    for (TInt ind = 0; ind < cpscnt || cpscnt == -1; ind++) {
	Elem::TIfRange range = ((AFunVar*) this)->GetInps(ind);
	if (range.first != range.second) {
	    TInt cnt = 0;
	    for (TIfIter it = range.first; it != range.second; it++) {
		if (cind == aInd) {
		    MDVarGet* inp = (MDVarGet*) (*it);
		    // Get name
		    stringstream ss;
		    ss <<  "Inp" << Elem::Fmt::mSepContName << GetInpUri(ind);
		    if (cnt > 0) ss << Elem::Fmt::mSepContName << cnt;
		    aName = ss.str();
		    if (mFunc != NULL) {
			// Get requested iface
			aCont.append(mFunc->GetInpExpType(ind));
			// Get value
			aCont.append(Elem::Fmt::mSepContInp);
			if (mFunc->mInps.count(inp) > 0) {
			    aCont.append(mFunc->mInps.at(inp));
			} else {
			    aCont.append("<unknown>");
			}
			// Get provided iface
			/*
			   MDVarGet* inp = (MDVarGet*) (*it);
			   string ifid;
			   if (inp != NULL) {
			   ifid = inp->VarGetIfid();
			   if (ifid.empty()) ifid = "<?>";
			   } else {
			   ifid = "<unknown>";
			   }
			   aCont.append(Elem::Fmt::mSepContInp);
			   aCont.append(ifid);
			   */
		    } else {
			aCont.append("<Init Err>");
		    }
		    return;
		}
		cnt++;
		cind++;
	    }
	} else {
	    if (cind == aInd) {
		// Get name
		aName = "Inp~ " + GetInpUri(aInd);
		// Get requested iface
		if (mFunc != NULL) {
		    aCont.append(mFunc->GetInpExpType(aInd));
		} else {
		    aCont.append("??");
		}
		// Get value
		aCont.append("<none>");
		return;
	    }
	    if (cpscnt == -1) return;
	    cind++;
	}
    }
}

void AFunVar::GetContInpTypes(string& aCont) const
{
    for (TInt ind = 0; ind < GetInpCpsCount() || GetInpCpsCount() == -1; ind++) {
	if (ind != 0) {
	    aCont.append("; ");
	}
	string inp_uri = GetInpUri(ind);
	if (inp_uri.empty() && GetInpCpsCount() == -1) break;
	aCont.append(inp_uri + ": ");
	Elem::TIfRange range = ((AFunVar*) this)->GetInps(ind);
	if (range.first != range.second) {
	    for (TIfIter it = range.first; it != range.second; it++) {
		if (it != range.first) {
		    aCont.append(", ");
		}
		MDVarGet* inp = (MDVarGet*) (*it);
		string ifid;
		if (inp != NULL) {
		    ifid = inp->VarGetIfid();
		    if (ifid.empty()) ifid = "<?>";
		} else {
		    ifid = "<unknown>";
		}
		aCont.append(ifid);
	    }
	} else {
	    aCont.append("<none>");
	}
    }
}

TBool AFunVar::GetCont(TInt aInd, string& aName, string& aCont) const
{
    if (aInd == 0) {
	if (mFunc != NULL) {
	    mFunc->GetResult(aCont);
	} else { aCont = "Init ERR"; }
    } else if (aInd == 1) {
	aName = "OutpType";
	aCont = mIfaceReq;
    } else if (aInd == 2) {
	aName = "InpsTypes";
	GetContInpTypes(aCont);
    } else if (aInd > 2) {
	TInt inpscnt = GetInpsCount();
	if (aInd - 3 < inpscnt) {
	    GetContInp(aInd - 3, aName, aCont);
	} else if (mFunc != NULL) {
	    mFunc->GetCont(aInd - 3 - inpscnt, aName, aCont);
	} 
    }
}

TInt AFunVar::GetContCount() const
{
    TInt res = 3 + GetInpsCount();
    if (mFunc != NULL) {
	res += mFunc->GetContCount();
    }
    return res;
}

TBool Func::GetCont(TInt aInd, string& aName, string& aCont) const
{
    if (aInd == 0) {
	aName = "Result";
	GetResult(aCont);
    }
};

// Addintion, variable 
string AFAddVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFAddVar::AFAddVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFAddVar::AFAddVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFAddVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFAddVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FAddInt::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddFloat::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddData<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddVect<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddMtrd<float>::Create(this, aIfaceName)) != NULL);
    //else if ((mFunc = FAddMtr<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Sdata<int> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Sdata<bool> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Sdata<float> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Mtr<int> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Mtr<float> >::Create(this, aIfaceName)) != NULL);
}

string AFAddVar::GetInpUri(TInt aId) const 
{
    if (aId == FAddBase::EInp) return "Inp";
    else if (aId == FAddBase::EInpN) return "InpN";
    else return string();
}

TInt AFAddVar::GetInpCpsCount() const
{
    TInt res = 1;
    if (((AFunVar*) this)->GetInp(FAddBase::EInpN, ETrue) != NULL) res = 2;
    return res;
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

void *FAddInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDIntGet::Type()) == 0) res = (MDIntGet*) this;
    return res;
}

TInt FAddInt::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    TInt val = 0;
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDIntGet* diget = dget->GetDObj(diget);
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

void *FAddFloat::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FAddFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDFloatGet* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    val += dfget->Value();
	}
	else {
	    MDIntGet* diget = dget->GetDObj(diget);
	    if (diget != NULL) {
		val += (float) diget->Value();
	    }
	}
    }
    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FAddFloat::GetResult(string& aResult) const
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}

TBool FAddFloat::GetCont(TInt aInd, string& aName, string& aCont) const
{
    if (aInd == 0) {
	aName = "";
	GetResult(aCont);
    }
    else {
	Elem::TIfRange range = mHost.GetInps(EInp);
	TInt cnt = 1;
	for (MElem::TIfIter it = range.first; it != range.second; it++) {
	    if (cnt == aInd) {
		MDVarGet* dget = (MDVarGet*) (*it);
		MDFloatGet* dfget = dget->GetDObj(dfget);
		stringstream sn;
		sn << "Inp " << cnt;
		stringstream sv;
		if (dfget != NULL) {
		    sv << dfget->Value();
		}
		else {
		    MDIntGet* diget = dget->GetDObj(diget);
		    if (diget != NULL) {
			sv << (float) diget->Value();
		    }
		}
		aName = sn.str();
		aCont = sv.str();
		break;
	    }
	    cnt++;
	}
    }
}

TInt FAddFloat::GetContCount() const
{
    TInt res = 1;
    Elem::TIfRange range = mHost.GetInps(EInp);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	res++;
    }
    return res;
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

template<class T> void *FAddData<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDataGet<T>::Type()) == 0) res = (MDataGet<T>*) this;
    return res;
}

template<class T>  void FAddData<T>::DataGet(T& aData)
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    T val = 0;
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDataGet<T>* dfget = dget->GetDObj(dfget);
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

template<class T> void FAddData<T>::GetResult(string& aResult) const
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

template<class T> void *FAddVect<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MVectGet<T>::Type()) == 0) res = (MVectGet<T>*) this;
    return res;
}

template<class T> void FAddVect<T>::VectGet(Vect<T>& aData)
{
    TInt size = aData.size();
    Elem::TIfRange range = mHost.GetInps(EInp);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MVectGet<T>* dfget = dget->GetDObj(dfget);
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
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", mHost.GetInpUri(EInp1).c_str());
	    }
	}
    }
}

template<class T> void FAddVect<T>::GetResult(string& aResult) const
{
    stringstream ss;
    ss <<  MVectGet<T>::TypeSig();
    for (typename Vect<T>::const_iterator it = mRes.begin(); it != mRes.end(); it++) {
	ss << " " << *it;
    }
    aResult = ss.str();
}

MDVarGet* Func::Host::GetInp(TInt aInpId, TBool aOpt)
{
    MDVarGet* res = NULL;
    Elem::TIfRange inpr = GetInps(aInpId, aOpt);
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

template<class T> void *FAddMtrd<T>::DoGetObj(const char *aName)
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
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MMtrdGet<T>* dfget = dget->GetDObj(dfget);
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
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]", mHost.GetInpUri(EInp).c_str());
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

template<class T> void *FAddMtr<T>::DoGetObj(const char *aName)
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
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MMtrGet<T>* dfget = dget->GetDObj(dfget);
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
		    Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incorrect dimensions of argument [%s]",  mHost.GetInpUri(EInp).c_str());
		    res = EFalse;
		    break;
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse;
		break;
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Non-matrix argument  [%s]",  mHost.GetInpUri(EInp).c_str());
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

template<class T> void FAddMtr<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}


// Generic data addition
template<class T> Func* FAddDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString.empty()) {
	// Weak negotiation, basing on inputs only
	TBool inpok = ETrue;
	Elem::TIfRange range = aHost->GetInps(EInp);
	for (MElem::TIfIter it = range.first; it != range.second; it++) {
	    MDVarGet* dget = (MDVarGet*) (*it);
	    MDtGet<T>* dfget = dget->GetDObj(dfget);
	    if (dfget == 0) {
		inpok = EFalse; break;
	    }
	}
	if (inpok) {
	    res = new FAddDt<T>(*aHost);
	}
    } else if (aString == MDtGet<T>::Type()) {
	res = new FAddDt<T>(*aHost);
    }
    return res;
}

template<class T> void *FAddDt<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = (MDtGet<T>*) this;
    return res;
}

template<class T> void FAddDt<T>::DtGet(T& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    mInps[dget] = arg.DtBase::ToString();
	    if (arg.mValid) {
		if (it == range.first) {
		    aData = arg;
		}
		else {
		    aData += arg;
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse; break;
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incompatible argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    res = EFalse; break;
	}
    }
    range = mHost.GetInps(EInpN, ETrue);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    mInps[dget] = arg.DtBase::ToString();
	    if (arg.mValid) {
		aData -= arg;
	    }
	    else {
		Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse; break;
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incompatible argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    res = EFalse; break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FAddDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

template <class T> string FAddDt<T>::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp) {
	res = MDtGet<T>::Type();
    }
    return res;
}


// Composing vector from components

string AFCpsVectVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFCpsVectVar::AFCpsVectVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFCpsVectVar::AFCpsVectVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFCpsVectVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFCpsVectVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FCpsVect<int>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FCpsVect<float>::Create(this, aIfaceName)) != NULL);
}

// Composing vector from components: float
template<class T> Func* FCpsVect<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<Mtr<T> >::Type()) {
	res = new FCpsVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FCpsVect<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Mtr<T> >::Type()) == 0) res = (MDtGet<Mtr<T> >*) this;
    return res;
}

template<class T> void FCpsVect<T>::DtGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    if (aData.mDim.second == 1 && (aData.mType == MtrBase::EMt_Unknown || aData.mType == MtrBase::EMt_Regular)) {
	if (aData.mType == MtrBase::EMt_Unknown) {
	    aData.mType = MtrBase::EMt_Regular;
	    aData.SetIntSize(aData.mDim.first*aData.mDim.second);
	}
	for (TInt cnt = 0; cnt < aData.mDim.first && res; cnt++) {
	    MDVarGet* vinp = mHost.GetInp(EInp1 + cnt);
	    if (vinp != NULL) {
		MDtGet<Sdata<T> >* inp = vinp->GetDObj(inp);
		if (inp != NULL) {
		    Sdata<T> arg;
		    inp->DtGet(arg);
		    string inpv;
		    arg.ToString(inpv);
		    mInps[vinp] = inpv;
		    if (arg.mValid) {
			aData.Elem(cnt, 0) = arg.mData;
		    }
		    else {
			mHost.LogWrite(MLogRec::EErr, "Invalid argument");
			res = EFalse;
		    }
		}
		else {
		    mHost.LogWrite(MLogRec::EErr, "Incorrect input type");
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Inputs number is less that the length of result");
		res = EFalse;
	    }
	}
    }
    else {
	mHost.LogWrite(MLogRec::EErr, "Only vector result is supported");
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> string FCpsVect<T>::GetInpExpType(TInt aId) const
{
    return MDtGet<Sdata<T> >::Type();
}


// Multiplication, variable type
string AFMplVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFMplVar::AFMplVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFMplVar::AFMplVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFMplVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFMplVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplFloat::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplDt<Sdata<bool> >::Create(this, aIfaceName)) != NULL);
}

string AFMplVar::GetInpUri(TInt aId) const
{
    if (aId == FAddBase::EInp) return "Inp"; else return string();
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

void *FMplFloat::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FMplFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDFloatGet* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    val *= dfget->Value();
	}
	else {
	    MDIntGet* diget = dget->GetDObj(diget);
	    if (diget != NULL) {
		val *= (float) diget->Value();
	    }
	}
    }
    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FMplFloat::GetResult(string& aResult) const
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}

// Multiplication: Generic data
template<class T> Func* FMplDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<T>::Type()) {
	res = new FMplDt<T>(*aHost);
    }
    return res;
}

template<class T> void *FMplDt<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = (MDtGet<T>*) this;
    return res;
}

template<class T> TBool FMplDt<T>::GetCont(TInt aInd, string& aName, string& aCont) const
{
    if (aInd == 0) {
	FMplBase::GetCont(aInd, aName, aCont);
    } else if (aInd == 1) {
	// Inputs values
	aName = "Inp_values";
	aCont.append(mHost.GetInpUri(EInp) + ": ");
	Elem::TIfRange range = mHost.GetInps(EInp);
	for (MElem::TIfIter it = range.first; it != range.second; it++) {
	    if (it != range.first) {
		aCont.append(", ");
	    }
	    MDVarGet* dget = (MDVarGet*) (*it);
	    MDtGet<T>* dfget = dget->GetDObj(dfget);
	    if (dfget != NULL) {
		T arg;
		dfget->DtGet(arg);
		string args;
		arg.ToString(args);
		aCont.append(args);
	    }
	}
    }
}

template<class T> void FMplDt<T>::DtGet(T& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    T arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		if (it == range.first) {
		    aData = arg;
		}
		else {
		    aData *= arg;
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse; break;
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, mHost.GetAgent(), "Incompatible argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    res = EFalse; break;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}


// Non commutative Multiplication, variable type
string AFMplncVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFMplncVar::AFMplncVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFMplncVar::AFMplncVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFMplncVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFMplncVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplMtrdVect<float>::Create(this, aIfaceName)) != NULL);
    //else if ((mFunc = FMplMtr<float>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplncDt<Mtr<float> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplncDt<Mtr<int> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplncScMtr<float>::Create(this, aIfaceName)) != NULL);
}

string AFMplncVar::GetInpUri(TInt aId) const
{
    if (aId == FMplncBase::EInp1) return "Inp1";
    else if (aId == FMplncBase::EInp2) return "Inp2";
    else return string();
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

template<class T> void *FMplMtrdVect<T>::DoGetObj(const char *aName)
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
	MMtrdGet<T>* dfget1 = dget->GetDObj(dfget1);
	dget = (MDVarGet*) (*range2.first);
	MVectGet<T>* dfget2 = dget->GetDObj(dfget2);
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
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument");
	    }
	}
    }
}

template<class T> void FMplMtrdVect<T>::GetResult(string& aResult) const
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

template<class T> void *FMplMtr<T>::DoGetObj(const char *aName)
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
	MMtrGet<T>* dfget1 = dget->GetDObj(dfget1);
	dget = (MDVarGet*) (*range2.first);
	MMtrGet<T>* dfget2 = dget->GetDObj(dfget2);
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
		    mHost.LogWrite(MLogRec::EErr, "Incorrect dimensions of argument");
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument");
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument");
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplMtr<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

// Mutltiplication non-commutative, generic data
template<class T> Func* FMplncDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<T>::Type()) {
	MDVarGet* vget = aHost->GetInp(EInp1);
	if (vget != NULL) {
	    MDtGet<T>* a1 = vget->GetDObj(a1);
	    if (a1 != NULL) {
		res = new FMplncDt<T>(*aHost);
	    }
	}
    } else {
	// Weak negotiation - just base on input type
	MDVarGet* vget = aHost->GetInp(EInp1);
	MDVarGet* vget2 = aHost->GetInp(EInp2);
	if (vget != NULL && vget->VarGetIfid() == MDtGet<T>::Type() &&
		vget2 != NULL && vget2->VarGetIfid() == MDtGet<T>::Type()) {
	    res = new FMplncDt<T>(*aHost);
	}
    }

    return res;
}

template<class T> void *FMplncDt<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = (MDtGet<T>*) this;
    return res;
}

template<class T> void FMplncDt<T>::DtGet(T& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range1 = mHost.GetInps(EInp1);
    Elem::TIfRange range2 = mHost.GetInps(EInp2);
    if (range1.first != range1.second && range2.first != range2.second) {
	MDVarGet* dget = (MDVarGet*) (*range1.first);
	MDtGet<T>* dfget1 = dget->GetDObj(dfget1);
	MDVarGet* dget2 = (MDVarGet*) (*range2.first);
	MDtGet<T>* dfget2 = dget2->GetDObj(dfget2);
	if (dfget1 != NULL && dfget2 != NULL) {
	    aData.mValid = ETrue;
	    T arg1;
	    T arg2;
	    arg1.SetMplncArg1Hint(aData, arg2);
	    dfget1->DtGet(arg1);
	    mInps[dget] = arg1.DtBase::ToString();
	    arg2.SetMplncArg2Hint(aData, arg1);
	    dfget2->DtGet(arg2);
	    mInps[dget2] = arg2.DtBase::ToString();
	    if (arg1.mValid && arg2.mValid) {
		aData.Mpl(arg1, arg2);
		res = aData.mValid;
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument");
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument");
	    res = EFalse;
	}
    } else {
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplncDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

template <class T> string FMplncDt<T>::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp1 || aId == EInp2) {
	res = MDtGet<T>::Type();
    }
    return res;
}



// Mutltiplication non-commutative, scalar to matrix
template<class T> Func* FMplncScMtr<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<Mtr<T> >::Type()) {
	MDVarGet* vget = aHost->GetInp(EInp1);
	if (vget != NULL) {
	    MDtGet<Sdata<T> >* sd = vget->GetDObj(sd);
	    if (sd != NULL) {
		res = new FMplncScMtr<T>(*aHost);
	    }
	}
    }
    return res;
}

template<class T> void *FMplncScMtr<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Mtr<T> >::Type()) == 0) res = (MDtGet<Mtr<T> >*) this;
    return res;
}

template<class T> void FMplncScMtr<T>::DtGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<Sdata<T> >* dfget1 = dget->GetDObj(dfget1);
    dget = mHost.GetInp(EInp2);
    MDtGet<Mtr<T> >* dfget2 = dget->GetDObj(dfget2);
    if (dfget1 != NULL && dfget2 != NULL) {
	aData.mValid = ETrue;
	Sdata<T> arg1;
	Mtr<T> arg2 = aData;
	dfget1->DtGet(arg1);
	dfget2->DtGet(arg2);
	if (arg1.mValid && arg2.mValid) {
	    aData = arg2;
	    aData.Mpl(&(arg1.mData));
	    res = aData.mValid;
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Incorrect argument");
	    res = EFalse;
	}
    }
    else {
	mHost.LogWrite(MLogRec::EErr, "Non-compatible argument");
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplncScMtr<T>::GetResult(string& aResult) const
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
    SetParent(Type());
}

AFMplinvVar::AFMplinvVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFMplinvVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFMplinvVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    //if ((mFunc = FMplinvMtrd<float>::Create(this, aIfaceName)) != NULL);
    //if ((mFunc = FMplinvMtr<float>::Create(this, aIfaceName)) != NULL);
    if ((mFunc = FMplinvDt<Mtr<float> >::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplinvDt<Sdata<float> >::Create(this, aIfaceName)) != NULL);
}

string AFMplinvVar::GetInpUri(TInt aId) const 
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

template<class T> void *FMplinvMtrd<T>::DoGetObj(const char *aName)
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
	MMtrdGet<T>* dfget = dget->GetDObj(dfget);
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
		mHost.LogWrite(MLogRec::EErr, "Incorrect size of argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    }
	}
    }
}

template<class T> void FMplinvMtrd<T>::GetResult(string& aResult) const
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

template<class T> void *FMplinvMtr<T>::DoGetObj(const char *aName)
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
	MMtrGet<T>* dfget = dget->GetDObj(dfget);
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
		    mHost.LogWrite(MLogRec::EErr, "Incorrect dimensions of argument [%s]",  mHost.GetInpUri(EInp).c_str());
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplinvMtr<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}


// Inversion for multiplication operation:  generic data
template<class T> Func* FMplinvDt<T>::Create(Host* aHost, const string& aString)
{
    Func* res = NULL;
    if (aString == MDtGet<T>::Type()) {
	MDVarGet* vget = aHost->GetInp(EInp);
	if (vget != NULL) {
	    MDtGet<T>* a1 = vget->GetDObj(a1);
	    if (a1 != NULL) {
		res = new FMplinvDt<T>(*aHost);
	    }
	}
    }
    return res;
}

template<class T> void *FMplinvDt<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = (MDtGet<T>*) this;
    return res;
}

template<class T> void FMplinvDt<T>::DtGet(T& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp);
    if (range.first != range.second) {
	MDVarGet* dget = (MDVarGet*) (*range.first);
	MDtGet<T>* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    aData.mValid = ETrue;
	    T arg = aData;
	    dfget->DtGet(arg);
	    if (arg.mValid) {
		aData.Invm(arg);
		res = aData.mValid;
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]",  mHost.GetInpUri(EInp).c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]",  mHost.GetInpUri(EInp).c_str());
	    res = EFalse;
	}
    }
    else {
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T> void FMplinvDt<T>::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}


// Casting, variable
string AFCastVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFCastVar::AFCastVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFCastVar::AFCastVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFCastVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) { res = this; }
    else { res = AFunVar::DoGetObj(aName); }
    return res;
}

void AFCastVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    // Checking if input type is defined explicitly
    string ifi;
    Elem* inptd = GetNodeE("./../../InpType");
    if (inptd != NULL) {
	inptd->GetCont(ifi);
    }
    // If not, trying to get type from input
    if (ifi.empty()) {
	MDVarGet* inp = GetInp(Func::EInp1);
	if (inp != NULL) {
	    ifi = inp->VarGetIfid();
	}
    }
    if (!ifi.empty()) {
	if ((mFunc = FCastDt<Mtr<int> , Mtr<float> >::Create(this, aIfaceName, ifi)) != NULL);
	else if ((mFunc = FCastDt<Mtr<float> , Mtr<int> >::Create(this, aIfaceName, ifi)) != NULL);
	else if ((mFunc = FCastDt<Sdata<float> , Sdata<int> >::Create(this, aIfaceName, ifi)) != NULL);
    }
}

string AFCastVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp"; else return string();
}


// Casting, variable:  generic data
template<class T, class TA> Func* FCastDt<T, TA>::Create(Host* aHost, const string& aString, const string& aArtTypeId)
{
    Func* res = NULL;
    if (aString == MDtGet<T>::Type() && aArtTypeId == MDtGet<TA>::Type()) {
	res = new FCastDt<T, TA>(*aHost);
    }
    return res;
}

template<class T, class TA> void *FCastDt<T, TA>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<T>::Type()) == 0) res = (MDtGet<T>*) this;
    return res;
}

template<class T, class TA> void FCastDt<T, TA>::DtGet(T& aData)
{
    TBool res = ETrue;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<TA>* dfget = dget->GetDObj(dfget);
    if (dfget != NULL) {
	TA arg = aData;
	dfget->DtGet(arg);
	if (arg.mValid) {
	    aData.mValid = ETrue;
	    aData = arg;
	    //aData.CastDown(arg);
	    res = aData.mValid;
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Incorrect argument [%s]",  mHost.GetInpUri(EInp1).c_str());
	    res = EFalse;
	}
    }
    else {
	mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]",  mHost.GetInpUri(EInp1).c_str());
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template<class T, class TA> void FCastDt<T, TA>::GetResult(string& aResult) const
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
    SetParent(Type());
}

AFCpsMtrdVar::AFCpsMtrdVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFCpsMtrdVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFCpsMtrdVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    if ((mFunc = FCpsMtrdVect<float>::Create(this, aIfaceName, inp1)) != NULL);
}

string AFCpsMtrdVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp"; else return string();
}


// Composing of diag matrix, from vector
template<class T> Func* FCpsMtrdVect<T>::Create(Host* aHost, const string& aString, MDVarGet* aInp1Var)
{
    Func* res = NULL;
    if (aString == MDtGet<Mtr<T> >::Type()) {
	res = new FCpsMtrdVect<T>(*aHost);
    }
    return res;
}

template<class T> void *FCpsMtrdVect<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Mtr<T> >::Type()) == 0) res = (MDtGet<Mtr<T> >*) this;
    return res;
}

template<class T> void FCpsMtrdVect<T>::DtGet(Mtr<T>& aData)
{
    TBool res = ETrue;
    Elem::TIfRange range = mHost.GetInps(EInp1);
    if (range.first != range.second) {
	MDVarGet* dget = (MDVarGet*) (*range.first);
	MDtGet<Mtr<T> >* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    Mtr<T> arg;
	    arg.mDim = MtrBase::TMtrDim(aData.mDim.first, 1);
	    dfget->DtGet(arg);
	    if (arg.mValid && arg.mDim.second == 1) {
		aData.mType = MtrBase::EMt_Diagonal;
		aData.mDim.first = arg.mDim.first;
		aData.mDim.second = arg.mDim.first;
		aData.mData.resize(arg.IntSize());
		for (TInt r = 0; r < arg.mDim.first; r++) {
		    aData.Elem(r,r) = arg.GetElem(r, 0);
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Invalid argument [%s]",  mHost.GetInpUri(EInp1).c_str());
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Non-matrix argument [%s]",  mHost.GetInpUri(EInp1).c_str());
	    res = EFalse;
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template <class T> string FCpsMtrdVect<T>::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp1) {
	res = MDtGet<Mtr<T> >::Type();
    }
    return res;
}



// Division, variable
string AFDivVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFDivVar::AFDivVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFDivVar::AFDivVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFDivVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFDivVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FDivFloat::Create(this, aIfaceName)) != NULL);
}

string AFDivVar::GetInpUri(TInt aId) const
{
    if (aId == FDivBase::EInp) return "Inp";
    else if (aId == FDivBase::EInp_Dvs) return "Inp_Dvs";
    else return string();
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

void *FDivFloat::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDFloatGet::Type()) == 0) res = (MDFloatGet*) this;
    return res;
}

float FDivFloat::Value()
{
    Elem::TIfRange range = mHost.GetInps(EInp);
    float val = 0;
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDFloatGet* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    val *= dfget->Value();
	}
	else {
	    MDIntGet* diget = dget->GetDObj(diget);
	    if (diget != NULL) {
		val *= (float) diget->Value();
	    }
	}
    }
    range = mHost.GetInps(EInp_Dvs);
    for (MElem::TIfIter it = range.first; it != range.second; it++) {
	MDVarGet* dget = (MDVarGet*) (*it);
	MDFloatGet* dfget = dget->GetDObj(dfget);
	float dvs = 0.0;
	if (dfget != NULL) {
	    dvs = dfget->Value();
	}
	else {
	    MDIntGet* diget = dget->GetDObj(diget);
	    if (diget != NULL) {
		dvs = (float) diget->Value();
	    }
	}
	if (dvs != 0.0) {
	    val /= dvs;
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Null divisor");
	}
    }

    mRes = val;
    mHost.OnFuncContentChanged();
    return val;
}

void FDivFloat::GetResult(string& aResult) const
{
    stringstream ss;
    ss <<  "F " << mRes;
    aResult = ss.str();
}




void FBcmpBase::GetResult(string& aResult) const
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

void *FBcmpFloat::DoGetObj(const char *aName)
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
	    void* inp = iv->GetDObj(ifi.c_str());
	    if (ifi == MDFloatGet::Type()) {
		MDFloatGet* dget = iv->GetDObj(dget);
		res = dget->Value();
	    }
	    else if (ifi == MDIntGet::Type()) {
		MDIntGet* dget = iv->GetDObj(dget);
		res = (float) dget->Value();
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
	    void* inp = aInpVar->GetDObj(MDFloatGet::Type());
	    if (inp == NULL) {
		inp = aInpVar->GetDObj(MDIntGet::Type());
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
    SetParent(Type());
}

AFBcmpVar::AFBcmpVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFBcmpVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFBcmpVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp1 = GetInp(FBcmpBase::EInp_1);
    MDVarGet* inp2 = GetInp(FBcmpBase::EInp_2);
    if (inp1 != NULL && inp2 != NULL) {
	FBcmpBase::TFType ftype = GetFType();
	if ((mFunc = FBcmpFloat::Create(this, aIfaceName, inp1, inp2, ftype)) != NULL);
    }
}

string AFBcmpVar::GetInpUri(TInt aId) const
{
    if (aId == FBcmpBase::EInp_1) return "Inp1";
    else if (aId == FBcmpBase::EInp_2) return "Inp2";
    else return string();
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
    SetParent(Type());
}

AFCmpVar::AFCmpVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFCmpVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFCmpVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (aIfaceName == MDtGet<Sdata<bool> >::Type() && inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string t2 = inp2->VarGetIfid();
	FCmpBase::TFType ftype = GetFType();
	if ((mFunc = FCmp<Sdata<int> >::Create(this, t1, t2, ftype)) != NULL);
	else if ((mFunc = FCmp<Enum>::Create(this, t1, t2, ftype)) != NULL);
	/* Debuggng
	   if (mFunc != NULL) {
	//Func* func = new FCmpBase(*this, ftype);
	Func* func = new FCmp<Sdata<bool> >(*this, ftype);
	MDtGet<Sdata<bool> >* fget = func->GetObj(fget);
	//MDtGet<Sdata<bool> >* fget = mFunc->GetObj(fget);
	Sdata<bool> data;
	fget->DtGet(data);
	}
	*/
    }
}

string AFCmpVar::GetInpUri(TInt aId) const
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else return string();
}

FCmpBase::TFType AFCmpVar::GetFType()
{
    FCmpBase::TFType res = FCmpBase::EEq;
    if (Name() == "AF_Lt") res = FCmpBase::ELt;
    else if (Name() == "AF_Le") res = FCmpBase::ELe;
    else if (Name() == "AF_Eq") res = FCmpBase::EEq;
    else if (Name() == "AF_Gt") res = FCmpBase::EGt;
    else if (Name() == "AF_Ge") res = FCmpBase::EGe;
    else {
	Logger()->Write(MLogRec::EErr, this, "Incorrect type of function [%s]", Name().c_str());
    }
    return res;
}


// Comparition  function

FCmpBase::~FCmpBase()
{
}

string FCmpBase::IfaceGetId() const
{ 
    return MDtGet<Sdata<bool> >::Type();
}

void FCmpBase::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

void *FCmpBase::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<bool> >::Type()) == 0) res = (MDtGet<Sdata<bool> >*) this;
    return res;
}

void FCmpBase::DtGet(Sdata<bool>& aData)
{
    aData.Set(ETrue);
}

template <class T> Func* FCmp<T>::Create(Host* aHost, const string& aInp1Iid, const string& aInp2Iid, FCmpBase::TFType aFType)
{
    Func* res = NULL;
    if (aInp1Iid == MDtGet<T>::Type() && aInp2Iid == MDtGet<T>::Type()) {
	res = new FCmp<T>(*aHost, aFType);
    }
    return res;
}

template <class T> void FCmp<T>::DtGet(Sdata<bool>& aData)
{
    TBool res = ETrue;
    MDVarGet* av1 = mHost.GetInp(EInp1);
    MDVarGet* av2 = mHost.GetInp(EInp2);
    if (av1 != NULL && av2 != NULL) {
	MDtGet<T>* a1 = av1->GetDObj(a1);
	MDtGet<T>* a2 = av2->GetDObj(a2);
	if (a1 != NULL && a2 != NULL) {
	    T arg1, arg2;
	    a1->DtGet(arg1);
	    a2->DtGet(arg2);
	    if (arg1.mValid && arg2.mValid && arg1.IsCompatible(arg2)) {
		TBool res;
		if (mFType == ELt) res = arg1 < arg2;
		else if (mFType == ELe) res = arg1 <= arg2;
		else if (mFType == EEq) res = arg1 == arg2;
		else if (mFType == EGt) res = arg1 > arg2;
		else if (mFType == EGe) res = arg1 >= arg2;
		aData.Set(res);
	    }
	    else {
		res = EFalse;
	    }
	}
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }

}


// Getting component of container
string AFAtVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFAtVar::AFAtVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFAtVar::AFAtVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFAtVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFAtVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp1 = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    if (inp1 != NULL && inp2 != NULL) {
	string t1 = inp1->VarGetIfid();
	string tind = inp2->VarGetIfid();
	if ((mFunc = FAtMVect<float>::Create(this, aIfaceName, t1)) != NULL);
	else if ((mFunc = FAtMVect<int>::Create(this, aIfaceName, t1)) != NULL);
	else if ((mFunc = FAtNTuple::Create(this, t1, tind)) != NULL);
	else {
	    LogWrite(MLogRec::EErr, "Init error, outp_iface [%s], inp_iface", aIfaceName.c_str(), t1.c_str());
	}
    }
}

/*
   Elem::TIfRange AFAtVar::GetInps(TInt aId)
   {
   __ASSERT(aId == Func::EInp1 || aId == Func::EInp2);
   Elem* inp = NULL;
   inp = GetNodeE(aId == Func::EInp1 ? "./../../Capsule/Inp" : "./../../Capsule/Index");
   __ASSERT(inp != NULL);
   RqContext cont(this);
   return inp->GetIfi(MDVarGet::Type(), &cont);
   }
   */

string AFAtVar::GetInpUri(TInt aId) const
{
    if (aId == Func::EInp1) return "Inp";
    else if (aId == Func::EInp2) return "Index";
    else return string();
}


// Getting component of container: matrix-vector
template <class T> Func* FAtMVect<T>::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (!aOutIid.empty()) {
	if (aOutIid == MDtGet<Sdata<T> >::Type() && aInpIid == MDtGet<Mtr<T> >::Type()) {
	    res = new FAtMVect<T>(*aHost);
	}
    } else {
	// Weak negotiation - just base on input type
	if (aInpIid == MDtGet<Mtr<T> >::Type()) {
	    res = new FAtMVect<T>(*aHost);
	}
    }
    return res;
}

template <class T> void *FAtMVect<T>::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<T> >::Type()) == 0) res = (MDtGet<Sdata<T> >*) this;
    return res;
}

template <class T> void FAtMVect<T>::DtGet(Sdata<T>& aData)
{
    TBool res = ETrue;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<Mtr<T> >* dfget = dget->GetDObj(dfget);
    dget = mHost.GetInp(EInp2);
    MDtGet<Sdata<int> >* diget = dget->GetDObj(diget);
    if (dfget != NULL && diget != NULL) {
	Mtr<T> arg;
	dfget->DtGet(arg);
	Sdata<int> ind;
	diget->DtGet(ind);
	if (arg.mValid && ind.mValid ) {
	    if (arg.mDim.second == 1) {
		if (ind.mData < arg.mDim.first) {
		    aData.mData = arg.GetElem(ind.mData, 0);
		    aData.mValid = ETrue;
		}
		else {
		    mHost.LogWrite(MLogRec::EErr, "Index is exceeded");
		    res = EFalse;
		}
	    }
	    else {
		mHost.LogWrite(MLogRec::EErr, "Argument must be vector");
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Incorrect argument");
	    res = EFalse;
	}
    }
    else {
	mHost.LogWrite(MLogRec::EErr, "Non-matrix argument");
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

template <class T> string FAtMVect<T>::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp1) {
	res = MDtGet<Mtr<T> >::Type();
    } else if (aId == EInp2) {
	res = MDtGet<Sdata<int> >::Type();
    }
    return res;
}


// 	Getting component of container: named tuple 

FAtNTuple::IfProxyBase::~IfProxyBase()
{
}

Func* FAtNTuple::Create(Host* aHost, const string& aInpId, const string& aInpIndId)
{
    Func* res = NULL;
    if (aInpId == MDtGet<NTuple>::Type() && aInpIndId == MDtGet<Sdata<string> >::Type()) {
	res = new FAtNTuple(*aHost);
    }
    return res;
}

void FAtNTuple::Init()
{
    string iid = IfaceGetId();
    if (mIfProxy != NULL) {
	string proxy_iid; 
	mIfProxy->GetIfaceId(proxy_iid);
	if (proxy_iid != iid) {
	    delete mIfProxy; mIfProxy = NULL;
	}
    }
    if (mIfProxy == NULL) {
	if (iid == MDtGet<Sdata<int> >::Type()) mIfProxy = new IfProxy<Sdata<int> >(this);
	else if (iid == MDtGet<Sdata<float> >::Type()) mIfProxy = new IfProxy<Sdata<float> >(this);
	else if (iid == MDtGet<Sdata<bool> >::Type()) mIfProxy = new IfProxy<Sdata<bool> >(this);
    }
}

void *FAtNTuple::DoGetObj(const char *aName)
{
    void *res = NULL;
    if (mIfProxy == NULL || mIfProxy->DoGetObj(aName) == NULL) {
	Init();
    }
    return mIfProxy != NULL ? mIfProxy->DoGetObj(aName) : NULL;
}


void FAtNTuple::GetField()
{
    TBool res = ETrue;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<NTuple>* dfget = dget->GetDObj(dfget);
    dget = mHost.GetInp(EInp2);
    NTuple arg;
    DtBase* dres = NULL;
    MDtGet<Sdata<string> >* diget = NULL;
    if (dget != NULL) 
	diget = dget->GetDObj(diget);
    if (dfget != NULL && diget != NULL) {
	dfget->DtGet(arg);
	Sdata<string> ind;
	diget->DtGet(ind);
	if (arg.mValid && ind.mValid ) {
	    dres = arg.GetElem(ind.mData);
	    if (dres == NULL) {
		mHost.LogWrite(MLogRec::EErr, "Cannot find field for given index");
		res = EFalse;
	    }
	}
	else {
	    mHost.LogWrite(MLogRec::EErr, "Incorrect argument");
	    res = EFalse;
	}
    }
    else {
	mHost.LogWrite(MLogRec::EErr, "Missing or incorrect argument");
	res = EFalse;
    }
    if (dres != NULL) {
	if (mRes != NULL) {
	    delete mRes; mRes = NULL;
	}
	mRes = dres->Clone();
	mRes->mValid = res;
	mHost.OnFuncContentChanged();
    }
}

string FAtNTuple::IfaceGetId() const
{
    string type;
    MDVarGet* dget = mHost.GetInp(EInp1);
    MDtGet<NTuple>* dfget = dget->GetDObj(dfget);
    dget = mHost.GetInp(EInp2);
    MDtGet<Sdata<string> >* diget = NULL;
    if (dget != NULL) 
	diget = dget->GetDObj(diget);
    if (dfget != NULL && diget != NULL) {
	NTuple arg;
	dfget->DtGet(arg);
	Sdata<string> ind;
	diget->DtGet(ind);
	if (arg.mValid && ind.mValid ) {
	    DtBase* dres = arg.GetElem(ind.mData);
	    if (dres != NULL && dres->mValid) {
		string tsig = dres->GetTypeSig();
		MDtGetBase::GetType(tsig, type);
	    }
	}
    }
    /*
       DtBase data; 
       ((FAtNTuple*) this)->GetField(data);
       */
    return type;
}

template <class T> void *FAtNTuple::IfProxy<T>::DoGetObj(const char *aName)
{
    return (strcmp(aName, MDtGet<T>::Type()) == 0) ? (MDtGet<T>*) this: NULL;
}

template <class T>  void FAtNTuple::IfProxy<T>::DtGet(T& aData)
{
    mHost->GetField();
    if (mHost->mRes != NULL) {
	T* data = dynamic_cast<T*>(mHost->mRes);
	if (data != NULL) 
	    aData = *data;
	else 
	    aData.mValid = EFalse;
    } 
}

string FAtNTuple::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp1) {
	res = MDtGet<NTuple>::Type();
    } else if (aId == EInp2) {
	res = MDtGet<Sdata<string> >::Type();
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
    SetParent(Type());
}

AFSwitchVar::AFSwitchVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFSwitchVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void *AFSwitchVar::DoGetDObj(const char *aName)
{
    void* res = NULL;
    // Just translate the request to func
    if (mFunc == NULL) {
	Init(MDVarGet::Type());
    }
    if (mFunc != NULL) {
	MDVarGet* vget = mFunc->GetObj(vget);
	if (vget != NULL) {
	    res = vget->DoGetDObj(aName);
	}
    }
    mIfaceReq = aName;
    return res;
}

void AFSwitchVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp_case = GetInp(Func::EInp1);
    MDVarGet* inp2 = GetInp(Func::EInp2);
    MDVarGet* inp3 = GetInp(Func::EInp3);
    if (inp_case != NULL && inp2 != NULL && inp3 != NULL) {
	string t_case = inp_case->VarGetIfid();
	if ((mFunc = FSwitchBool::Create(this, aIfaceName, t_case)) != NULL);
    }
}

string AFSwitchVar::GetInpUri(TInt aId) const
{
    if (aId == Func::EInp1) return "Sel";
    else {
	stringstream ss;
	ss <<  "Inp" << (aId - Func::EInp1);
	return ss.str();
    }
}

// TODO [YB] It is simpler to use just mapping Func::EInp* to inputs uris
/*
   Elem::TIfRange AFSwitchVar::GetInps(TInt aId)
   {
   Elem* inp = NULL;
   string inp_uri = "./../../Capsule/Sel";
   if (aId != Func::EInp1) {
   stringstream ss;
   ss <<  "./../../Capsule/Inp" << (aId - Func::EInp1);
   inp_uri = ss.str();
   }
   inp = GetNodeE(inp_uri);
   __ASSERT(inp != NULL);
   RqContext cont(this);
   return inp->GetIfi(MDVarGet::Type(), &cont);
   }
   */

// Case - commutation of inputs
Func* FSwitchBool::Create(Host* aHost, const string& aOutIid, const string& aInpIid)
{
    Func* res = NULL;
    if (aOutIid == MDVarGet::Type() && (aInpIid == MDBoolGet::Type() || aInpIid == MDtGet<Sdata<bool> >::Type())) {
	res = new FSwitchBool(*aHost);
    }
    return res;
}

void *FSwitchBool::DoGetObj(const char *aName)
{
    void* res = NULL;
    // There are two approach of commutation. The first one is to commutate MDVarGet ifase, i.e.
    // that switcher result is this iface of selected case. The second is that switcher 
    // implements MDVarGet by itselt and does commutation in this iface methods.
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
	MDBoolGet* getif= iv->GetDObj(getif);
	if (getif != NULL) {
	    res = getif->Value();
	} else {
	    MDtGet<Sdata<bool> >* getif= iv->GetDObj(getif);
	    if (getif != NULL) {
		Sdata<bool> data;
		getif->DtGet(data);
		((Func*) this)->mInps[iv] = data.DtBase::ToString();
		if (data.mValid) {
		    res = data.mData;
		}
	    }
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

string FSwitchBool::VarGetIfid()
{
    MDVarGet* cs = GetCase();
    return cs->VarGetIfid();
}

void *FSwitchBool::DoGetDObj(const char *aName)
{
    MDVarGet* cs = GetCase();
    return cs != NULL ? cs->DoGetDObj(aName) : NULL;
}

string FSwitchBool::GetInpExpType(TInt aId) const
{
    string res;
    if (aId == EInp_Sel) {
	res = MDtGet<Sdata<bool> >::Type();
    } else {
	res = "<any>";
    }
    return res;
}


// Boolean negation
string AFBoolNegVar::PEType()
{
    return AFunVar::PEType() + GUri::KParentSep + Type();
}

AFBoolNegVar::AFBoolNegVar(const string& aName, Elem* aMan, MEnv* aEnv): AFunVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

AFBoolNegVar::AFBoolNegVar(Elem* aMan, MEnv* aEnv): AFunVar(Type(), aMan, aEnv)
{
    SetParent(AFunVar::PEType());
}

void *AFBoolNegVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = AFunVar::DoGetObj(aName);
    }
    return res;
}

void AFBoolNegVar::Init(const string& aIfaceName)
{
    if (mFunc == NULL) {
	mFunc = FBnegDt::Create(this);
    }
}
string AFBoolNegVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp";
    else return string();
}


// 	Negation func, generic data
Func* FBnegDt::Create(Host* aHost)
{
    return new FBnegDt(*aHost);
}

void *FBnegDt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<bool> >::Type()) == 0) res = (MDtGet<Sdata<bool> >*) this;
    return res;
}

void FBnegDt::DtGet(Sdata<bool>& aData)
{
    TBool res = ETrue;
    MDVarGet* dget = mHost.GetInp(EInp1);
    if (dget != NULL) {
	MDtGet<Sdata<bool> >* dfget = dget->GetDObj(dfget);
	if (dfget != NULL) {
	    Sdata<bool> arg;
	    dfget->DtGet(arg);
	    aData = !arg;
	}
    } else {
	dget = mHost.GetInp(EInp1);
	res = EFalse;
    }
    aData.mValid = res;
    if (mRes != aData) {
	mRes = aData;
	mHost.OnFuncContentChanged();
    }
}

void FBnegDt::GetResult(string& aResult) const
{
    mRes.ToString(aResult);
}

TBool FBnegDt::GetCont(TInt aInd, string& aName, string& aCont) const
{
    if (aInd == 2) {
	aName = "";
	GetResult(aCont);
    }
    else if (aInd == 3) {
	aName = "Input";
	MDVarGet* dget = mHost.GetInp(EInp1);
	MDtGet<Sdata<bool> >* dfget = dget->GetDObj(dfget);
	Sdata<bool> arg;
	dfget->DtGet(arg);
	arg.ToString(aCont);
    }
}



