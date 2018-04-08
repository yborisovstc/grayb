
#include "des.h"
#include "edge.h"
#include "prov.h"
#include "menv.h"
#include "vert.h"
#include "mprop.h"


// MDesObserver metadata
MDesObserver::EIfu::EIfu()
{
    RegMethod("OnUpdated", 0);
    RegMethod("OnActivated", 0);
}

MDesObserver::EIfu MDesObserver::mIfu;

// MDesObserver metadata
MDesSyncable::EIfu::EIfu()
{
    RegMethod("Update", 0);
    RegMethod("Confirm", 0);
    RegMethod("IsUpdated", 0);
    RegMethod("SetUpdated", 0);
    RegMethod("ResetUpdated", 0);
    RegMethod("IsActive", 0);
    RegMethod("SetActive", 0);
    RegMethod("ResetActive", 0);
}

MDesSyncable::EIfu MDesSyncable::mIfu;



ATrBase::ATrBase(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrBase::ATrBase(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

string ATrBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

void *ATrBase::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool ATrBase::HandleCompChanged(MElem& aContext, MElem& aComp, const string& aContName)
{
    TBool res = ETrue;
    return res;
}



// Agent base of Int function

ATrInt::ATrInt(const string& aName, MElem* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mData(0)
{
    SetParent(Type());
}

ATrInt::ATrInt(MElem* aMan, MEnv* aEnv): ATrBase(Type(), aMan, aEnv), mData(0)
{
    SetParent(ATrBase::PEType());
}

string ATrInt::PEType()
{
    return ATrBase::PEType() + GUri::KParentSep + Type();
}

void *ATrInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = ATrBase::DoGetObj(aName);
    }
    return res;
}

MDIntGet* ATrInt::GetInp(const string& aInpName)
{
    MDIntGet* res = NULL;
    TIfRange rg = GetInpRg(aInpName);
    if (rg.first != rg.second) {
	res = (MDIntGet*) (*rg.first);
    }
    return res;
}

Elem::TIfRange ATrInt::GetInpRg(const string& aInpName)
{
    TIfRange res;
    string uri = "./../../" + aInpName;
    MElem* einp = GetNode(uri);
    if (einp != NULL) {
	RqContext cont(this);
	res = einp->GetIfi(MDIntGet::Type(), &cont);
    }
    else {
	Logger()->Write(EErr, this, "Cannot get input [%s]", uri.c_str());
    }
    return res;
}

TInt ATrInt::Value()
{
    return 0;
}



// Agent of function "Increment of Int data"

ATrIncInt::ATrIncInt(const string& aName, MElem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrIncInt::ATrIncInt(MElem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetParent(ATrInt::PEType());
}

string ATrIncInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrIncInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName);
    }
    return res;
}

TInt ATrIncInt::Value()
{
    MDIntGet* mget = GetInp("Inp");
    if (mget != NULL) {
	TInt val = mget->Value();
	mData = val + 1;
    }
    else {
	Logger()->Write(EErr, this, "Cannot get data from input [Inp]");
    }
    return mData;
}

// Agent of function "Subtraction of Int data"

ATrSubInt::ATrSubInt(const string& aName, MElem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrSubInt::ATrSubInt(MElem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetParent(ATrInt::PEType());
}

string ATrSubInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrSubInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName);
    }
    return res;
}

TInt ATrSubInt::Value()
{
    TInt res = 0;
    TIfRange range = GetInpRg("Inp");
    for (TIfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res += dget->Value();
	}
    }
    range = GetInpRg("Inp_Sub");
    for (TIfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res -= dget->Value();
	}
    }
    if (res != mData) {
	mData = res;
    }
    return res;
}


// Agent function "Multiplying of Int data"

ATrMplInt::ATrMplInt(const string& aName, MElem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrMplInt::ATrMplInt(MElem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetParent(ATrInt::PEType());
}

string ATrMplInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrMplInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName);
    }
    return res;
}

TInt ATrMplInt::Value()
{
    TInt res = 1;
    TIfRange range = GetInpRg("Inp");
    for (TIfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res *= dget->Value();
	}
    }
    return res;
}

// Agent function "Dividing of Int data"

ATrDivInt::ATrDivInt(const string& aName, MElem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrDivInt::ATrDivInt(MElem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetParent(ATrInt::PEType());
}

string ATrDivInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrDivInt::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName);
    }
    return res;
}

TInt ATrDivInt::Value()
{
    TInt res = 0;
    MDIntGet* dd = GetInp("Inp");
    MDIntGet* dr = GetInp("Inp_DR");
    if (dd != NULL && dr != NULL) {
	TInt drv = dr->Value();
	if (drv != 0) {
	    res = dd->Value() / drv;
	}
	else {
	    Logger()->Write(EErr, this, "Divider value is zero");
	}
    }
    else {
	Logger()->Write(EErr, this, "Inputs aren't exists");
    }
    return res;
}

// Agent of transition of variable type
string ATrVar::PEType()
{
    return ATrBase::PEType() + GUri::KParentSep + Type();
}

ATrVar::ATrVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mFunc(NULL)
{
    SetParent(Type());
}

ATrVar::ATrVar(MElem* aMan, MEnv* aEnv): ATrBase(Type(), aMan, aEnv), mFunc(NULL)
{
    SetParent(ATrBase::PEType());
}

void *ATrVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else {
	res = ATrBase::DoGetObj(aName);
    }
    if (res == NULL) {
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
    }
    return res;
}

string ATrVar::VarGetIfid()
{
    return mFunc != NULL ? mFunc->IfaceGetId() : string();
}

void *ATrVar::DoGetDObj(const char *aName)
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
    return res;
}

string ATrVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else if (aId == Func::EInp3) return "Inp3";
    else if (aId == Func::EInp4) return "Inp4";
    else return string();
}

Elem::TIfRange ATrVar::GetInps(TInt aId, TBool aOpt)
{
    TIfRange res;
    MElem* inp = GetNode("./../../" + GetInpUri(aId));
    if (inp != NULL) {
	RqContext cont(this);
	res =  inp->GetIfi(MDVarGet::Type(), &cont);
    }
    else if (!aOpt) {
	Logger()->Write(EErr, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void ATrVar::OnFuncContentChanged()
{
    OnChanged(*this);
}

TBool ATrVar::GetCont(string& aCont, const string& aName) const
{
    TBool res = EFalse;
    if (mFunc != NULL) {
	mFunc->GetResult(aCont);
	res = ETrue;
    }
    return res;
}

void ATrVar::LogWrite(TLogRecCtg aCtg, const char* aFmt,...)
{
    va_list list;
    va_start(list,aFmt);
    Logger()->Write(aCtg, this, aFmt, list);
}

// Agent function "Addition of Var data"
string ATrAddVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrAddVar::ATrAddVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrAddVar::ATrAddVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrAddVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

void ATrAddVar::Init(const string& aIfaceName)
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
}

string ATrAddVar::GetInpUri(TInt aId) const 
{
    if (aId == FAddBase::EInp) return "Inp";
    else return string();
}


// Agent function "Multiplication of Var data"
string ATrMplVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrMplVar::ATrMplVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrMplVar::ATrMplVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrMplVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

void ATrMplVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplFloat::Create(this, aIfaceName)) != NULL);
}

string ATrMplVar::GetInpUri(TInt aId) const 
{
    if (aId == FMplBase::EInp) return "Inp";
    else return string();
}

// Agent function "Division of Var data"
string ATrDivVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrDivVar::ATrDivVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrDivVar::ATrDivVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrDivVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

void ATrDivVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FDivFloat::Create(this, aIfaceName)) != NULL);
}

string ATrDivVar::GetInpUri(TInt aId) const 
{
    if (aId == FDivBase::EInp) return "Inp";
    else if (aId == FDivBase::EInp_Dvs) return "Inp_Dvs";
    else return string();
}


// Agent function "Switch controlled by var data"
string ATrSwitchVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
}

ATrSwitchVar::ATrSwitchVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrSwitchVar::ATrSwitchVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrSwitchVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    // Needs to redirect request for MDVarGet to func
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
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
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

void ATrSwitchVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    MDVarGet* inp_case = GetInp(FSwithcBase::EInp_Sel);
    MDVarGet* inp2 = GetInp(FSwithcBase::EInp_1);
    MDVarGet* inp3 = GetInp(FSwithcBase::EInp_1 + 1);
    if (inp_case != NULL && inp2 != NULL && inp3 != NULL) {
	string t_case = inp_case->VarGetIfid();
	if ((mFunc = FSwitchBool::Create(this, aIfaceName, t_case)) != NULL);
    }
}

string ATrSwitchVar::GetInpUri(TInt aId) const 
{
    if (aId == FSwithcBase::EInp_Sel) return "Sel";
    else if (aId > FSwithcBase::EInp_Sel) {
	stringstream ss;
	ss <<  "Inp" << (aId - FSwithcBase::EInp_Sel);
	return ss.str();
    }
    else return string();
}


// Agent function "Get n-coord of Vector"
string ATrAtVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrAtVar::ATrAtVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
} 

ATrAtVar::ATrAtVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{ 
    SetParent(ATrVar::PEType());
}

void *ATrAtVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
     }
     else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
} 

void ATrAtVar::Init(const string& aIfaceName)
{ 
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
     }
    MDVarGet* inp_ind = GetInp(Func::EInp2);
    MDVarGet* inp = GetInp(Func::EInp1);
    if (inp_ind != NULL && inp != NULL) {
	 string t_inp = inp->VarGetIfid();
	//if ((mFunc = FAtVect<float>::Create(this, aIfaceName, t_inp)) != NULL);
	if ((mFunc = FAtMVect<float>::Create(this, aIfaceName, t_inp)) != NULL);
    }
}

string ATrAtVar::GetInpUri(TInt aId) const 
{ 
    if (aId == Func::EInp2) return "Index";
    else if (aId == Func::EInp1) return "Inp";
    else return string();
}


// Agent function "Composing of vector, var data"
string ATrCpsVectVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrCpsVectVar::ATrCpsVectVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrCpsVectVar::ATrCpsVectVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrCpsVectVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

void ATrCpsVectVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FCpsVect<float>::Create(this, aIfaceName)) != NULL);
}

string ATrCpsVectVar::GetInpUri(TInt aId) const 
{
    stringstream ss;
    ss <<  "Inp" << (aId - Func::EInp1);
    return ss.str();
}


// Agent function "Boolean comparition of Var data"
string ATrBcmpVar::PEType()
{
    return ATrVar::PEType() + GUri::KParentSep + Type();
} 

ATrBcmpVar::ATrBcmpVar(const string& aName, MElem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetParent(Type());
}

ATrBcmpVar::ATrBcmpVar(MElem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetParent(ATrVar::PEType());
}

void *ATrBcmpVar::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName);
    }
    return res;
}

string ATrBcmpVar::VarGetIfid()
{
    return MDBoolGet::Type();
}

void ATrBcmpVar::Init(const string& aIfaceName)
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

string ATrBcmpVar::GetInpUri(TInt aId) const 
{
    if (aId == FBcmpBase::EInp_1) return "Inp1";
    else if (aId == FBcmpBase::EInp_2) return "Inp2";
    else return string();
}

FBcmpBase::TFType ATrBcmpVar::GetFType()
{
    FBcmpBase::TFType res = FBcmpBase::EEq;
    if (Name() == "AF_Lt") res = FBcmpBase::ELt;
    else if (Name() == "AF_Le") res = FBcmpBase::ELe;
    else if (Name() == "AF_Eq") res = FBcmpBase::EEq;
    else if (Name() == "AF_Gt") res = FBcmpBase::EGt;
    else if (Name() == "AF_Ge") res = FBcmpBase::EGe;
    else {
	Logger()->Write(EErr, this, "Incorrect type of function [%s]", Name().c_str());
    }
    return res;
}


/* State base agent */

StateAgent::StateAgent(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetParent(Type());
}

StateAgent::StateAgent(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iActive(ETrue)
{
    SetParent(Elem::PEType());
}

string StateAgent::PEType()
{
    return StateAgent::PEType() + GUri::KParentSep + Type();
}

void *StateAgent::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = (MDesSyncable*) this;
    }
    else if (strcmp(aName, MDesObserver::Type()) == 0) {
	res = (MDesObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void StateAgent::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MDesObserver::Type()) == 0) {
	res = (MDesObserver*) this;
    }
    else if (strcmp(aName.c_str(), MDesSyncable::Type()) == 0) {
	res = (MDesSyncable*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str());
    }
    InsertIfCache(aName, aCtx, this, res);
}

TBool StateAgent::IsActive()
{
    return iActive;
}

void StateAgent::SetActive()
{
    iActive = ETrue;
}

void StateAgent::ResetActive()
{
    iActive = EFalse;
}

void StateAgent::Update()
{
    TBool cdata = GetNode("./../../Data") != NULL;
    MElem* eprepu = GetNode(cdata ? "./../../Data/Prepared/Capsule/Upd" : "./../../Prepared/Capsule/Upd");
    if (eprepu != NULL) {
	MUpdatable* upd = (MUpdatable*) eprepu->GetSIfiC(MUpdatable::Type(), this);
	if (upd != NULL) {
	    try {
		if (upd->Update()) {
		    SetUpdated();
		}
	    } catch (std::exception e) {
		Logger()->Write(EErr, this, "Unspecified error on update");
	    }
	    ResetActive();
	}
    }
}

void StateAgent::Confirm()
{
    TBool cdata = GetNode("./../../Data") != NULL;
    MElem* econfu = GetNode(cdata ? "./../../Data/Confirmed/Capsule/Upd" : "./../../Confirmed/Capsule/Upd");
    if (econfu != NULL) {
	MUpdatable* upd = (MUpdatable*) econfu->GetSIfiC(MUpdatable::Type(), this);
	if (upd != NULL) {
	    if (upd->Update()) {
		// Activate dependencies
		MElem* eobs = GetNode("./../../Capsule/Out/Int/PinObs");
		RqContext ctx(this);
		// Request w/o context because of possible redirecting request to itself
		// TODO [YB] To check if iterator is not damage during the cycle, to cache to vector if so
		TIfRange range = eobs->GetIfi(MDesObserver::Type(), NULL);
		for (TIfIter it = range.first; it != range.second; it++) {
		    MDesObserver* mobs = (MDesObserver*) (*it);
		    if (mobs != NULL) {
			mobs->OnUpdated();
		    }
		}
		// Activate mgr
	    }
	    ResetUpdated();
	}
    }
}

TBool StateAgent::IsUpdated()
{
    return iUpdated;
}

void StateAgent::SetUpdated()
{
    iUpdated = ETrue;
}

void StateAgent::ResetUpdated()
{
    iUpdated = EFalse;
}

MIface* StateAgent::MDesSyncable_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesSyncable::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesSyncable::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "Update") {
	Update();
    } else if (name == "Confirm") {
	Confirm();
    } else if (name == "IsUpdated") {
	TBool rr = IsUpdated();
	aRes = Ifu::FromBool(rr);
    } else if (name == "SetUpdated") {
	SetUpdated();
    } else if (name == "ResetUpdated") {
	ResetUpdated();
    } else if (name == "IsActive") {
	TBool rr = IsActive();
	aRes = Ifu::FromBool(rr);
    } else if (name == "SetActive") {
	SetActive();
    } else if (name == "ResetActive") {
	ResetActive();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string StateAgent::MDesSyncable_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

void StateAgent::OnUpdated()
{
    // Mark active
    SetActive();
}

void StateAgent::OnActivated()
{
}

MIface* StateAgent::MDesObserver_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesObserver::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesObserver::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "OnUpdated") {
	OnUpdated();
    } else if (name == "OnActivated") {
	OnActivated();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string StateAgent::MDesObserver_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

/* DES base agent */

string ADes::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADes::ADes(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetParent(Type());
}

ADes::ADes(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iActive(ETrue)
{
    SetParent(Elem::PEType());
}

void *ADes::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = (MDesSyncable*) this;
    }
    else if (strcmp(aName, MDesObserver::Type()) == 0) {
	res = (MDesObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool ADes::IsActive()
{
    return iActive;
}

void ADes::SetActive()
{
    iActive = ETrue;
}

void ADes::ResetActive()
{
    iActive = EFalse;
}

void ADes::Update()
{
    // Update all the DES components avoiding agents and capsule
    MElem* host = iMan->GetMan();
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MElem* eit = host->GetComp(ci);
	if (eit != iMan && eit->Name() != "Capsule") {
	    MDesSyncable* msync = (MDesSyncable*) eit->GetSIfiC(MDesSyncable::Type(), this);
	    if (msync != NULL) {
		if (msync->IsActive()) {
		    try {
			msync->Update();
			SetUpdated();
		    } catch (std::exception e) {
			Logger()->Write(EErr, this, "Error on update [%s]", eit->GetUri().c_str());
		    }
		}
	    }
	}
    }
}

void ADes::Confirm()
{
    // Confirm all the DES components
    MElem* host = iMan->GetMan();
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MElem* eit = host->GetComp(ci);
	MDesSyncable* msync = (MDesSyncable*) eit->GetSIfiC(MDesSyncable::Type(), this);
	if (msync != NULL) {
	    if (msync->IsUpdated()) {
		msync->Confirm();
		ResetUpdated();
	    }
	}
    }
}

TBool ADes::IsUpdated()
{
    return iUpdated;
}

void ADes::SetUpdated()
{
    iUpdated = ETrue;
}

void ADes::ResetUpdated()
{
    iUpdated = EFalse;
}

MIface* ADes::MDesSyncable_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesSyncable::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesSyncable::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "Update") {
	Update();
    } else if (name == "Confirm") {
	Confirm();
    } else if (name == "IsUpdated") {
	TBool rr = IsUpdated();
	aRes = Ifu::FromBool(rr);
    } else if (name == "SetUpdated") {
	SetUpdated();
    } else if (name == "ResetUpdated") {
	ResetUpdated();
    } else if (name == "IsActive") {
	TBool rr = IsActive();
	aRes = Ifu::FromBool(rr);
    } else if (name == "SetActive") {
	SetActive();
    } else if (name == "ResetActive") {
	ResetActive();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ADes::MDesSyncable_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

void ADes::OnUpdated()
{
    // Mark active
    SetActive();
}

void ADes::OnActivated()
{
}

MIface* ADes::MDesObserver_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesObserver::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesObserver::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "OnUpdated") {
	OnUpdated();
    } else if (name == "OnActivated") {
	OnActivated();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ADes::MDesObserver_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

