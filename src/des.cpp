
#include <iostream> 
#include <functional> 

#include "des.h"
#include "edge.h"
#include "prov.h"
#include "menv.h"
#include "vert.h"
#include "mprop.h"
#include "data.h"


// MDesObserver metadata
MDesObserver::EIfu::EIfu()
{
    RegMethod("OnUpdated", 0);
    RegMethod("OnActivated", 0);
}

MDesObserver::EIfu MDesObserver::mIfu;

// MDesInpCompObserver metadata
MDesInpObserver::EIfu::EIfu()
{
    RegMethod("OnUpdated", 0);
}

MDesInpObserver::EIfu MDesInpObserver::mIfu;


// MDesSyncable metadata
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



ATrBase::ATrBase(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

string ATrBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

MIface *ATrBase::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MACompsObserver::Type()) == 0)
	res = (MACompsObserver*) this;
    else if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* ATrBase::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
}

TBool ATrBase::HandleCompChanged(MUnit* aContext, MUnit* aComp, const string& aContName)
{
    TBool res = ETrue;
    return res;
}



// Agent base of Int function

ATrInt::ATrInt(const string& aName, MUnit* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mData(0)
{
    SetCrAttr(PEType(), aName);
}

string ATrInt::PEType()
{
    return ATrBase::PEType() + GUri::KParentSep + Type();
}

MIface *ATrInt::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    } else {
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
    string uri = "./../" + aInpName;
    MUnit* einp = GetNode(uri);
    if (einp != NULL) {
	res = einp->GetIfi(MDIntGet::Type(), this);
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

ATrIncInt::ATrIncInt(const string& aName, MUnit* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

string ATrIncInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
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

ATrSubInt::ATrSubInt(const string& aName, MUnit* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

string ATrSubInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
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

ATrMplInt::ATrMplInt(const string& aName, MUnit* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

string ATrMplInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
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

ATrDivInt::ATrDivInt(const string& aName, MUnit* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

string ATrDivInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
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

ATrVar::ATrVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mFunc(NULL)
{
    SetCrAttr(PEType(), aName);
}

MIface *ATrVar::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDVarGet::Type()) == 0) {
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
    MIface* res = NULL;
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
    MUnit* inp = GetNode("./../" + GetInpUri(aId));
    if (inp != NULL) {
	res =  inp->GetIfi(MDVarGet::Type(), this);
    }
    else if (!aOpt) {
	Logger()->Write(EErr, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void ATrVar::OnFuncContentChanged()
{
    OnChanged(this);
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

ATrAddVar::ATrAddVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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
    else if ((mFunc = FAddDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
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

ATrMplVar::ATrMplVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ATrDivVar::ATrDivVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ATrSwitchVar::ATrSwitchVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

MIface *ATrSwitchVar::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    // Needs to redirect request for MDVarGet to func
    if (strcmp(aName, MDVarGet::Type()) == 0) {
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

ATrAtVar::ATrAtVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ATrCpsVectVar::ATrCpsVectVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ATrBcmpVar::ATrBcmpVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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



// Transition function agent base with support of 'combined chain' approach

ATrcBase::ATrcBase(const string& aName, MUnit* aMan, MEnv* aEnv): Vertu(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
}

string ATrcBase::PEType()
{
    return Vertu::PEType() + GUri::KParentSep + Type();
}

MIface *ATrcBase::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = dynamic_cast<MConnPoint*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    } else {
	res = Vertu::DoGetObj(aName);
    }
    return res;
}

void ATrcBase::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);

    if (aName == MDesInpObserver::Type()) {
	// Redirect to pairs
	for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
	    MUnit* pe = (*it)->GetObj(pe);
	    if (!ctx.IsInContext(pe)) {
		rr = pe->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, pe, rr);
	    }
	}
    }
    if (rr.first == rr.second) {
	Vertu::UpdateIfi(aName, aCtx);
    }
}

TBool ATrcBase::IsRequired(const string& aIfName) const
{
    return (aIfName == Required());
}

string ATrcBase::Required() const
{
    return MDesInpObserver::Type();
}

MIface* ATrcBase::MConnPoint_Call(const string& aSpec, string& aRes)
{
    return NULL;
}

string ATrcBase::MConnPoint_Mid() const
{
    return GetUri(NULL, ETrue);
}

MCompatChecker::TDir ATrcBase::GetDir() const
{
    return ERegular;
}

MUnit* ATrcBase::GetExtd()
{
    return NULL;
}

MIface* ATrcBase::MCompatChecker_Call(const string& aSpec, string& aRes)
{
    return NULL;
}

string ATrcBase::MCompatChecker_Mid() const
{
    return GetUri(NULL, ETrue);
}

TBool ATrcBase::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfi(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
		}
	    }
	}
    }
    return res;
}



// Agent of transition of variable type with combined chain

string ATrcVar::PEType()
{
    return ATrcBase::PEType() + GUri::KParentSep + Type();
}

ATrcVar::ATrcVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcBase(aName, aMan, aEnv), mFunc(NULL)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
}

MIface *ATrcVar::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    } else {
	res = ATrcBase::DoGetObj(aName);
    }
    if (res == NULL) {
	if (mFunc != NULL) {
	    res = mFunc->DoGetObj(aName);
	}
    }
    return res;
}

string ATrcVar::VarGetIfid()
{
    return mFunc != NULL ? mFunc->IfaceGetId() : string();
}

void *ATrcVar::DoGetDObj(const char *aName)
{
    MIface* res = NULL;
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

string ATrcVar::GetInpUri(TInt aId) const 
{
    if (aId == Func::EInp1) return "Inp1";
    else if (aId == Func::EInp2) return "Inp2";
    else if (aId == Func::EInp3) return "Inp3";
    else if (aId == Func::EInp4) return "Inp4";
    else return string();
}

Unit::TIfRange ATrcVar::GetInps(TInt aId, TBool aOpt)
{
    TIfRange res;
    MUnit* inp = GetNode("./" + GetInpUri(aId));
    if (inp != NULL) {
	res =  inp->GetIfi(MDVarGet::Type(), this);
    }
    else if (!aOpt) {
	Logger()->Write(EErr, this, "Cannot get input [%s]", GetInpUri(aId).c_str());
    }
    return res;
}

void ATrcVar::OnFuncContentChanged()
{
    OnChanged(this);
}

TBool ATrcVar::GetCont(string& aCont, const string& aName) const
{
    TBool res = EFalse;
    if (mFunc != NULL) {
	mFunc->GetResult(aCont);
	res = ETrue;
    }
    return res;
}

void ATrcVar::LogWrite(TLogRecCtg aCtg, const char* aFmt,...)
{
    va_list list;
    va_start(list,aFmt);
    Logger()->Write(aCtg, this, aFmt, list);
}

TBool ATrcVar::IsProvided(const string& aIfName) const
{
    return (aIfName == Provided());
}

string ATrcVar::Provided() const
{
    return MDVarGet::Type();
}



// Agent function "Addition of Var data"
string ATrcAddVar::PEType()
{
    return ATrcVar::PEType() + GUri::KParentSep + Type();
} 

ATrcAddVar::ATrcAddVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcVar(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
    cp = Provider()->CreateNode("ConnPointMcu", "InpN", this, iEnv);
    __ASSERT(cp != NULL);
    res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);

}

void ATrcAddVar::Init(const string& aIfaceName)
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
    else if ((mFunc = FAddDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddDt<Sdata<bool>>::Create(this, aIfaceName)) != NULL);
}

string ATrcAddVar::GetInpUri(TInt aId) const 
{
    if (aId == FAddBase::EInp) return "Inp";
    else if (aId == FAddBase::EInpN) return "InpN";
    else return string();
}



// Agent function "Multiplication var"

string ATrcMplVar::PEType()
{
    return ATrcVar::PEType() + GUri::KParentSep + Type();
} 

ATrcMplVar::ATrcMplVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcVar(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

void ATrcMplVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMplDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FMplDt<Sdata<bool>>::Create(this, aIfaceName)) != NULL);
}

string ATrcMplVar::GetInpUri(TInt aId) const 
{
    if (aId == FMplBase::EInp) return "Inp";
    else return string();
}


// Agent function "Logical AND"

string ATrcAndVar::PEType()
{
    return ATrcVar::PEType() + GUri::KParentSep + Type();
} 

ATrcAndVar::ATrcAndVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcVar(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

void ATrcAndVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FBAndDt::Create(this)) != NULL);
}

string ATrcAndVar::GetInpUri(TInt aId) const 
{
    if (aId == FMplBase::EInp) return "Inp";
    else return string();
}


// Agent function "Logical negation"

string ATrcNegVar::PEType()
{
    return ATrcVar::PEType() + GUri::KParentSep + Type();
} 

ATrcNegVar::ATrcNegVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcVar(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

void ATrcNegVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FBnegDt::Create(this)) != NULL);
}

string ATrcNegVar::GetInpUri(TInt aId) const 
{
    if (aId == FMplBase::EInp) return "Inp";
    else return string();
}

// Agent function "Max var"

string ATrcMaxVar::PEType()
{
    return ATrcVar::PEType() + GUri::KParentSep + Type();
} 

ATrcMaxVar::ATrcMaxVar(const string& aName, MUnit* aMan, MEnv* aEnv): ATrcVar(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

void ATrcMaxVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc = NULL;
    }
    if ((mFunc = FMaxDt<Sdata<int>>::Create(this, aIfaceName)) != NULL);
}

string ATrcMaxVar::GetInpUri(TInt aId) const 
{
    if (aId == FMplBase::EInp) return "Inp";
    else return string();
}





/* State base agent */

StateAgent::StateAgent(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetCrAttr(PEType(), aName);
}

string StateAgent::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

MIface *StateAgent::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = (MDesSyncable*) this;
    }
    else if (strcmp(aName, MDesInpObserver::Type()) == 0) {
	res = (MDesInpObserver*) this;
    }
    else if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* StateAgent::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
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
    TBool cdata = GetNode("./../Data") != NULL;
    MUnit* eprepu = GetNode(cdata ? "./../Data/Prepared/Capsule/Upd" : "./../Prepared/Capsule/Upd");
    if (eprepu != NULL) {
	MUpdatable* upd = (MUpdatable*) eprepu->GetSIfi(MUpdatable::Type(), this);
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
    TBool cdata = GetNode("./../Data") != NULL;
    MUnit* econfu = GetNode(cdata ? "./../Data/Confirmed/Capsule/Upd" : "./../Confirmed/Capsule/Upd");
    if (econfu != NULL) {
	MUpdatable* upd = (MUpdatable*) econfu->GetSIfi(MUpdatable::Type(), this);
	if (upd != NULL) {
	    if (upd->Update()) {
		// Activate dependencies
		MUnit* eobs = GetNode("./../Capsule/Out/Int/PinObs");
		// Request w/o context because of possible redirecting request to itself
		// TODO [YB] To check if iterator is not damage during the cycle, to cache to vector if so
		TIfRange range = eobs->GetIfi(MDesInpObserver::Type());
		for (TIfIter it = range.first; it != range.second; it++) {
		    MDesInpObserver* mobs = (MDesInpObserver*) (*it);
		    if (mobs != NULL) {
			mobs->OnInpUpdated();
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

void StateAgent::OnInpUpdated()
{
    // Mark active
    SetActive();
}

MIface* StateAgent::MDesInpObserver_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesInpObserver::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesInpObserver::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "OnUpdated") {
	OnInpUpdated();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string StateAgent::MDesInpObserver_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}



/* State base agent, monolitic, using unit base organs */

const string AState::KContVal = "Value";

AState::AState(const string& aName, MUnit* aMan, MEnv* aEnv): Vertu(aName, aMan, aEnv), iUpdated(ETrue), iActive(ETrue),
    mPdata(NULL), mCdata(NULL)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    mPdata = new BdVar(this);
    mCdata = new BdVar(this);
}

string AState::PEType()
{
    return Vertu::PEType() + GUri::KParentSep + Type();
}

MIface *AState::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = dynamic_cast<MDesSyncable*>(this);
    } else if (strcmp(aName, MDesInpObserver::Type()) == 0) {
	res = dynamic_cast<MDesInpObserver*>(this);
    } else if (strcmp(aName, MAgent::Type()) == 0) {
	res = dynamic_cast<MAgent*>(this);
    } else if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = dynamic_cast<MConnPoint*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    } else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = dynamic_cast<MDVarGet*>(mCdata);
    } else {
	res = Vertu::DoGetObj(aName);
    }
    return res;
}

MIface* AState::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
}

TBool AState::IsActive()
{
    return iActive;
}

void AState::SetActive()
{
    iActive = ETrue;
}

void AState::ResetActive()
{
    iActive = EFalse;
}

void AState::Update()
{
    MUpdatable* upd = mPdata;
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

void AState::Confirm()
{
    MUpdatable* upd = mCdata;
    if (upd != NULL) {
	string old_value;
	mCdata->ToString(old_value);
	if (upd->Update()) {
	    // Activate dependencies
	    MUnit* eobs = GetNode("./../Capsule/Out/Int/PinObs");
	    // Request w/o context because of possible redirecting request to itself
	    // TODO [YB] To check if iterator is not damage during the cycle, to cache to vector if so
	    TIfRange range = eobs->GetIfi(MDesInpObserver::Type());
	    for (TIfIter it = range.first; it != range.second; it++) {
		MDesInpObserver* mobs = (MDesInpObserver*) (*it);
		if (mobs != NULL) {
		    mobs->OnInpUpdated();
		}
	    }
	    if (IsLogeventUpdate()) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	}
	ResetUpdated();
    }
}

TBool AState::IsUpdated()
{
    return iUpdated;
}

void AState::SetUpdated()
{
    iUpdated = ETrue;
}

void AState::ResetUpdated()
{
    iUpdated = EFalse;
}

MIface* AState::MDesSyncable_Call(const string& aSpec, string& aRes)
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

string AState::MDesSyncable_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

void AState::OnInpUpdated()
{
    // Mark active
    SetActive();
}

MIface* AState::MDesInpObserver_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesInpObserver::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesInpObserver::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "OnUpdated") {
	OnInpUpdated();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string AState::MDesInpObserver_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

MDVarGet* AState::HGetInp(const Base* aRmt)
{
    MDVarGet* res = NULL;
    if (aRmt == mPdata) {
	MUnit* uinp = GetMan()->GetNode("./TransOut");
	MVert* vinp = uinp->GetObj(vinp);
	if (vinp->PairsCount() > 0) {
	    MVert* pair = vinp->GetPair(0);
	    MUnit* upair = (MUnit*) pair->MVert_DoGetObj(MUnit::Type());
	    res = (MDVarGet*) upair->GetSIfi(MDVarGet::Type(), this);
	}
    } else {
	res = mPdata->GetObj(res);
    }
    return res;
}

void AState::HOnDataChanged(const Base* aRmt)
{
}

TBool AState::IsProvided(const string& aIfName) const
{
    return aIfName == Provided();
}

TBool AState::IsRequired(const string& aIfName) const
{
    return aIfName == Required();
}

string AState::Provided() const
{
    return MDVarGet::Type();
}

string AState::Required() const
{
    return string();
}

TBool AState::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfi(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
		}
	    }
	}
    }
    return res;
}

MUnit* AState::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir AState::GetDir() const
{
    return EInp;
}

MIface* AState::MCompatChecker_Call(const string& aSpec, string& aRes)
{
}

string AState::MCompatChecker_Mid() const
{
}

MIface* AState::MConnPoint_Call(const string& aSpec, string& aRes)
{
}

string AState::MConnPoint_Mid() const
{
}

TBool AState::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    return Vertu::OnCompChanged(aComp, aContName, aModif);
}

TEhr AState::ProcessCompChanged(const MUnit* aComp, const string& aContName)
{
    TEhr res = EEHR_Ignored;
    if (aComp == this && aContName == KContVal) {
	string val = GetContent(KContVal);
	TBool sres = mPdata->FromString(val);
	sres = sres && mCdata->FromString(val);
	if (sres) {
	    res = EEHR_Accepted;
	}  else {
	    Logger()->Write(EErr, this, "[%s] Error on applying content [%s]", GetUri(NULL, true).c_str(), aContName.c_str());
	    res = EEHR_Denied;
	}
    } else {
	res = Vertu::ProcessCompChanged(aComp, aContName);
    }
    return res;
}
	

TBool AState::IsLogeventUpdate() 
{
    string upd = GetContent("Debug.Update");
    return upd == "y";
}


/* Connection point - input of combined chain state AStatec */

string CpStatecInp::PEType()
{
    return ConnPointMcu::PEType() + GUri::KParentSep + Type();
}

CpStatecInp::CpStatecInp(const string& aName, MUnit* aMan, MEnv* aEnv): ConnPointMcu(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    TBool res = ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

/* Connection point - output of combined chain state AStatec */

string CpStatecOutp::PEType()
{
    return ConnPointMcu::PEType() + GUri::KParentSep + Type();
}

CpStatecOutp::CpStatecOutp(const string& aName, MUnit* aMan, MEnv* aEnv): ConnPointMcu(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    TBool res = ChangeCont("{Provided:'MDVarGet' Required:'MDesInpObserver'}");
    __ASSERT(res);
}




/* State base agent, monolitic, using unit base organs, combined data-observer chain */

const string AStatec::KContVal = "Value";

AStatec::AStatec(const string& aName, MUnit* aMan, MEnv* aEnv): Vertu(aName, aMan, aEnv), iUpdated(ETrue), iActive(ETrue),
    mPdata(NULL), mCdata(NULL)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    mPdata = new BdVar(this);
    mCdata = new BdVar(this);
    Unit* cp = Provider()->CreateNode("ConnPointMcu", "Inp", this, iEnv);
    __ASSERT(cp != NULL);
    TBool res = AppendComp(cp);
    __ASSERT(res);
    res = cp->ChangeCont("{Provided:'MDesInpObserver' Required:'MDVarGet'}");
    __ASSERT(res);
}

string AStatec::PEType()
{
    return Vertu::PEType() + GUri::KParentSep + Type();
}

MIface *AStatec::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = dynamic_cast<MDesSyncable*>(this);
    } else if (strcmp(aName, MDesInpObserver::Type()) == 0) {
	res = dynamic_cast<MDesInpObserver*>(this);
    } else if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = dynamic_cast<MConnPoint*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    } else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = dynamic_cast<MDVarGet*>(mCdata);
    } else if (strcmp(aName, MDVarSet::Type()) == 0) {
	res = dynamic_cast<MDVarSet*>(mPdata);
    } else {
	res = Vertu::DoGetObj(aName);
    }
    return res;
}

#if 0
void AStatec::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TBool resg = EFalse;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL) {
	// Redirect to pairs if iface requiested is provided by this CP
	if (GetProvided() == aName) {
	    // Requested provided iface - cannot be obtain via pairs - redirect to host
	    if (iMan != NULL && !ctx.IsInContext(iMan)) {
		// TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		rr = mgr->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, mgr, rr);
		resg = resg || (rr.first == rr.second);
	    }
	}
	if (!resg) {
	    if (GetRequired() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    MUnit* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resg = resg || (rr.first == rr.second);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resg = resg || (rr.first == rr.second);
		}
	    }
	}
    }
}
#endif

TBool AStatec::IsActive()
{
    return iActive;
}

void AStatec::SetActive()
{
    iActive = ETrue;
    // Propagate activation to owner
    MUnit* downer = iMan;
    if (downer != NULL) {
	MDesObserver* mobs = (MDesObserver*) downer->GetSIfi(MDesObserver::Type(), this);
	if (mobs != NULL) {
	    mobs->OnActivated();
	}
    }
}

void AStatec::ResetActive()
{
    iActive = EFalse;
}

void AStatec::Update()
{
    MUpdatable* upd = mPdata;
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

void AStatec::NotifyInpsUpdated()
{
    // It would be better to request MDesObserver interface from self. But there is the problem
    // with using this approach because self implements this iface. So accoriding to iface resolution
    // rules UpdateIfi has to try local ifaces first, so self will be selected. Solution here could be
    // not following this rule and try pairs first for MDesObserver. But we choose another solution ATM -
    // to request pairs directly.
    for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
	MUnit* pe = (*it)->GetObj(pe);
	// Don't add self to if request context to enable routing back to self
	TIfRange range = pe->GetIfi(MDesInpObserver::Type());
	for (TIfIter it = range.first; it != range.second; it++) {
	    MDesInpObserver* mobs = (MDesInpObserver*) (*it);
	    if (mobs != NULL) {
		mobs->OnInpUpdated();
	    }
	}
    }
}


void AStatec::Confirm()
{
    MUpdatable* upd = mCdata;
    if (upd != NULL) {
	string old_value;
	mCdata->ToString(old_value);
	if (upd->Update()) {
	    // Notify dependencies
	    NotifyInpsUpdated();
	    if (IsLogeventUpdate()) {
		string new_value;
		mCdata->ToString(new_value);
		Logger()->Write(EInfo, this, "Updated [%s <- %s]", new_value.c_str(), old_value.c_str());
	    }
	}
	ResetUpdated();
    }
}

TBool AStatec::IsUpdated()
{
    return iUpdated;
}

void AStatec::SetUpdated()
{
    iUpdated = ETrue;
    // Propagate updation to the owner
    MUnit* downer = iMan;
    if (downer != NULL) {
	MDesObserver* mobs = (MDesObserver*) downer->GetSIfi(MDesObserver::Type(), this);
	if (mobs != NULL) {
	    mobs->OnUpdated();
	}
    }
}

void AStatec::ResetUpdated()
{
    iUpdated = EFalse;
}

MIface* AStatec::MDesSyncable_Call(const string& aSpec, string& aRes)
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

string AStatec::MDesSyncable_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

void AStatec::OnInpUpdated()
{
    // Mark active
    SetActive();
}

MIface* AStatec::MDesInpObserver_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MDesInpObserver::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MDesInpObserver::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "OnUpdated") {
	OnInpUpdated();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string AStatec::MDesInpObserver_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

MDVarGet* AStatec::HGetInp(const Base* aRmt)
{
    MDVarGet* res = NULL;
    if (aRmt == mPdata) {
	MUnit* uinp = GetNode("./Inp");
	res = (MDVarGet*) uinp->GetSIfi(MDVarGet::Type(), this);
    } else {
	res = mPdata->GetObj(res);
    }
    return res;
}

void AStatec::HOnDataChanged(const Base* aRmt)
{
    if (aRmt == mPdata) {
	SetUpdated();
    }
}

TBool AStatec::IsProvided(const string& aIfName) const
{
    return aIfName == Provided();
}

TBool AStatec::IsRequired(const string& aIfName) const
{
    return aIfName == Required();
}

string AStatec::Provided() const
{
    return MDVarGet::Type();
}

string AStatec::Required() const
{
    return MDesInpObserver::Type();
}

TBool AStatec::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfi(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
		}
	    }
	}
    }
    return res;
}

MUnit* AStatec::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir AStatec::GetDir() const
{
    return ERegular;
}

MIface* AStatec::MCompatChecker_Call(const string& aSpec, string& aRes)
{
}

string AStatec::MCompatChecker_Mid() const
{
}

MIface* AStatec::MConnPoint_Call(const string& aSpec, string& aRes)
{
}

string AStatec::MConnPoint_Mid() const
{
}

TBool AStatec::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    return Vertu::OnCompChanged(aComp, aContName, aModif);
}

TEhr AStatec::ProcessCompChanged(const MUnit* aComp, const string& aContName)
{
    // There is not still nice solution for changing state value
    // The following is used atm: change both prepared and congifmed and notify
    // obs chain and deps (i.e maximum notification). It doesn't seem optimal
    TEhr res = EEHR_Ignored;
    if (aComp == this && aContName == KContVal) {
	string val = GetContent(KContVal);
	TBool sres = mPdata->FromString(val);
	sres = sres && mCdata->FromString(val);
	if (mPdata->IsValid() && mCdata->IsValid()) {
	    res = EEHR_Accepted;
	    if (sres) {
		SetActive();
		SetUpdated();
		NotifyInpsUpdated();
	    }
	}  else {
	    Logger()->Write(EErr, this, "[%s] Error on applying content [%s]", GetUri(NULL, true).c_str(), aContName.c_str());
	    res = EEHR_Denied;
	}
    } else {
	res = Vertu::ProcessCompChanged(aComp, aContName);
    }
    return res;
}
	
TBool AStatec::IsLogeventUpdate() 
{
    string upd = GetContent("Debug.Update");
    return upd == "y";
}

string AStatec::VarGetSIfid()
{
    return mPdata == NULL ? string() : mPdata->VarGetSIfid();
}

MIface *AStatec::DoGetSDObj(const char *aName)
{
    return mPdata == NULL ? NULL : mPdata->DoGetSDObj(aName);
}



/* DES base agent */

string ADes::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADes::ADes(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetCrAttr(PEType(), aName);
}

MIface *ADes::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MDesSyncable::Type()) == 0) {
	res = (MDesSyncable*) this;
    }
    else if (strcmp(aName, MDesObserver::Type()) == 0) {
	res = (MDesObserver*) this;
    }
    else if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* ADes::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
}

TBool ADes::IsActive()
{
    return iActive;
}

void ADes::SetActive()
{
    iActive = ETrue;
    // Propagate activation to owner
    MUnit* host = iMan;
    MUnit* downer = host->GetMan();
    if (downer != NULL) {
	MDesObserver* mobs = (MDesObserver*) downer->GetSIfi(MDesObserver::Type(), this);
	if (mobs != NULL) {
	    mobs->OnActivated();
	}
    }
}

void ADes::ResetActive()
{
    iActive = EFalse;
}

void ADes::Update()
{
    // Update all the DES components avoiding agents and capsule
    // TODO Why not using local Iface cache for MDesSyncable ?
    MUnit* host = iMan;
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MUnit* eit = host->GetComp(ci);
	if (eit != this && eit->Name() != "Capsule") {
	    MDesSyncable* msync = (MDesSyncable*) eit->GetSIfi(MDesSyncable::Type(), this);
	    if (msync != NULL) {
		if (msync->IsActive()) {
		    try {
			msync->Update();
		    } catch (std::exception e) {
			Logger()->Write(EErr, this, "Error on update [%s]", eit->GetUri().c_str());
		    }
		}
	    }
	}
    }
    SetUpdated();
    ResetActive();
}

void ADes::Confirm()
{
    // Confirm all the DES components
    MUnit* host = iMan;
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MUnit* eit = host->GetComp(ci);
	if (eit != this && eit->Name() != "Capsule") {
	    MDesSyncable* msync = (MDesSyncable*) eit->GetSIfi(MDesSyncable::Type(), this);
	    if (msync != NULL) {
		if (msync->IsUpdated()) {
		    msync->Confirm();
		    ResetUpdated();
		}
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
    // Propagate updated to owner
    MUnit* host = iMan;
    MUnit* downer = host->GetMan();
    if (downer != NULL) {
	MDesObserver* mobs = (MDesObserver*) downer->GetSIfi(MDesObserver::Type(), this);
	if (mobs != NULL) {
	    mobs->OnUpdated();
	}
    }
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
    // Upstem notification, mark itself updated and propagate
    SetUpdated();
}

void ADes::OnActivated()
{
    // Upstem notification, mark itself activated and propagate
    SetActive();
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

void ADes::DumpActive()
{
    MUnit* host = iMan;
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MUnit* eit = host->GetComp(ci);
	if (eit != this && eit->Name() != "Capsule") {
	    MDesSyncable* msync = (MDesSyncable*) eit->GetSIfi(MDesSyncable::Type(), this);
	    if (msync != NULL) {
		if (msync->IsActive()) {
		    cout << msync->Uid() << endl;
		    msync->DumpActive();
		}
	    }
	}
    }
}

void ADes::DumpUpdated()
{
    MUnit* host = iMan;
    for (TInt ci = 0; ci < host->CompsCount(); ci++) {
	MUnit* eit = host->GetComp(ci);
	if (eit != this && eit->Name() != "Capsule") {
	    MDesSyncable* msync = (MDesSyncable*) eit->GetSIfi(MDesSyncable::Type(), this);
	    if (msync != NULL) {
		if (msync->IsUpdated()) {
		    cout << msync->Uid() << endl;
		    msync->DumpUpdated();
		}
	    }
	}
    }
}


// DES launcher

const string KCont_TopDesUri = "Path";

string ADesLauncher::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADesLauncher::ADesLauncher(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mStop(EFalse)
{
    SetCrAttr(PEType(), aName);
    InsertContent(KCont_TopDesUri);
}

MIface *ADesLauncher::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MLauncher::Type()) == 0) {
	res = dynamic_cast<MLauncher*>(this);
    } else if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* ADesLauncher::MAgent_DoGetIface(const string& aName)
{
    return DoGetObj(aName.c_str());
}

TBool ADesLauncher::Run()
{
    TBool res = ETrue;
    const string path = GetContent(KCont_TopDesUri);
    MUnit* desu = GetNode(path, ETrue);
    if (desu != NULL) {
	MDesSyncable* sync = dynamic_cast<MDesSyncable*>(desu->GetSIfi(MDesSyncable::Type()));
	if (sync) {
	    mStop = EFalse;
	    while (!mStop) {
		if (sync->IsActive()) {
		    sync->Update();
		} else if (sync->IsUpdated()) {
		    sync->Confirm();
		} else {
		    OnIdle();
		}
	    }

	} else {
	    Logger()->Write(EErr, this, "Cannot get DES syncable from [%s]", path.c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Cannot get DES to run [%s]", path.c_str());
    }
    return res;
}

TBool ADesLauncher::Stop()
{
    mStop = ETrue;
}

void ADesLauncher::OnIdle()
{
}




// DES adapter base
//
const string KCont_AgentUri = "AgentUri";

string AAdp::PEType()
{
    return Unit::PEType() + GUri::KParentSep + Type();
}

AAdp::AAdp(const string& aName, MUnit* aMan, MEnv* aEnv): Unit(aName, aMan, aEnv), mMag(NULL)
{
    iName = aName.empty() ? GetType(PEType()) : iName = aName;
    InsertContent(KCont_AgentUri);
}

MIface* AAdp::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
    return res;
}

TEhr AAdp::ProcessCompChanged(const MUnit* aComp, const string& aContName)
{
    TEhr res = EEHR_Ignored;
    if (aComp == this && aContName == KCont_AgentUri) {
	string val = GetContent(KCont_AgentUri);
	mMag = GetNode(val);
	if (mMag) {
	    res = EEHR_Accepted;
	    // Notify states of update
		NotifyInpsUpdated();
	}  else {
	    Logger()->Write(EErr, this, "[%s] Error on applying content [%s]", GetUri(NULL, true).c_str(), aContName.c_str());
	    res = EEHR_Denied;
	}
    } else {
	res = Unit::ProcessCompChanged(aComp, aContName);
    }
    return res;
}

void AAdp::NotifyInpsUpdated()
{
    // It is AMunitAdp responsibility to provide the ifaces
    TIfRange range = GetIfi(MDesInpObserver::Type());
    for (TIfIter it = range.first; it != range.second; it++) {
	MDesInpObserver* mobs = dynamic_cast<MDesInpObserver*>(*it);
	if (mobs) {
	    mobs->OnInpUpdated();
	}
    }
}

MIface *AAdp::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else {
	res = Unit::DoGetObj(aName);
    }
    return res;
}

template <typename T> TBool AAdp::GetData(MUnit* aDvget, T& aData)
{
    TBool res = EFalse;
    MDVarGet* vget = aDvget->GetSIfit(vget);
    if (vget) {
	MDtGet<Sdata<T>>* gsd = vget->GetDObj(gsd);
	if (gsd) {
	    Sdata<T> st;
	    gsd->DtGet(st);
	    aData = st.mData;
	    res = ETrue;
	}
    }
    return res;
}


// Access point

template <typename T> void* AAdp::AdpPap<T>::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (aName == MDtGet<Sdata<T> >::Type()) {
	res = dynamic_cast<MDtGet<Sdata<T> >*>(this);
    }
    return res;
}

// MUnit DES adapter

const string K_CompIdxInpUri = "./InpCompIdx";

AMunitAdp::AMunitAdp(const string& aName, MUnit* aMan, MEnv* aEnv): AAdp(aName, aMan, aEnv)
{
    // TODO This iName init seems far from elegant. To find solution, ref ds_i_icnau 
    iName = aName.empty() ? GetType(PEType()) : iName = aName;
    InsertContent(KCont_AgentUri);
}

string AMunitAdp::PEType()
{
    return AAdp::PEType() + GUri::KParentSep + Type();
}

void AMunitAdp::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);

    if (aName == MDesInpObserver::Type()) {
	// Redirect to parameter states
	MUnit* cmpCount = Host()->GetNode("./CompsCount");
	__ASSERT(cmpCount);
	if (!ctx.IsInContext(cmpCount)) {
	    rr = cmpCount->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, cmpCount, rr);
	}
	MUnit* cmpUid = Host()->GetNode("./CompUid");
	__ASSERT(cmpUid);
	if (!ctx.IsInContext(cmpUid)) {
	    rr = cmpUid->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, cmpUid, rr);
	}
    } else if (aName == MDVarGet::Type()) {
	MUnit* cmpCount = Host()->GetNode("./CompsCount");
	if (ctx.IsInContext(cmpCount)) {
	    MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpCount);
	    InsertIfCache(aName, aCtx, cmpCount, iface);
	}
	MUnit* cmpUid = Host()->GetNode("./CompUid");
	if (ctx.IsInContext(cmpUid)) {
	    MIface* iface = dynamic_cast<MDVarGet*>(&mApCmpUid);
	    InsertIfCache(aName, aCtx, cmpUid, iface);
	}
    } else  {
	Unit::UpdateIfi(aName, aCtx);
    }
}

void AMunitAdp::GetCompsCount(Sdata<TInt>& aData)
{
    aData.mData = mMag->CompsCount();
}

void AMunitAdp::GetCompUid(Sdata<string>& aData)
{
    TBool res = EFalse;
    MUnit* inp = Host()->GetNode(K_CompIdxInpUri);
    __ASSERT(inp != NULL);
    TInt idx = 0;
    res = GetData(inp, idx);
    if (res) {
	if (idx < mMag->CompsCount()) {
	    MUnit* comp = mMag->GetComp(idx);
	    aData.mData = comp->Uid();
	} else {
	    Logger()->Write(EErr, this, "Comp index [%i] is out of range", idx);
	}
    } else {
	    Logger()->Write(EErr, this, "Cannot get comp index");
    }
}



