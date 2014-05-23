
#include "des.h"
#include "edge.h"
#include "prov.h"
#include "menv.h"
#include "vert.h"
#include "mprop.h"

ATrBase::ATrBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

ATrBase::ATrBase(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

string ATrBase::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

void *ATrBase::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

// Agent base of Int function

ATrInt::ATrInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mData(0)
{
    SetEType(Type(), ATrBase::PEType());
    SetParent(Type());
}

ATrInt::ATrInt(Elem* aMan, MEnv* aEnv): ATrBase(Type(), aMan, aEnv), mData(0)
{
    SetEType(ATrBase::PEType());
    SetParent(ATrBase::PEType());
}

string ATrInt::PEType()
{
    return ATrBase::PEType() + GUri::KParentSep + Type();
}

void *ATrInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDIntGet::Type()) == 0) {
	res = (MDIntGet*) this;
    }
    else {
	res = ATrBase::DoGetObj(aName, aIncUpHier);
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
    string uri = "../../" + aInpName;
    Elem* einp = GetNode(uri);
    if (einp != NULL) {
	RqContext cont(this);
	res = einp->GetIfi(MDIntGet::Type(), &cont);
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Cannot get input [%s]", uri.c_str());
    }
    return res;
}

TInt ATrInt::Value()
{
    return 0;
}



// Agent of function "Increment of Int data"

ATrIncInt::ATrIncInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type(), ATrInt::PEType());
    SetParent(Type());
}

ATrIncInt::ATrIncInt(Elem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetEType(ATrInt::PEType());
    SetParent(ATrInt::PEType());
}

string ATrIncInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrIncInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName, aIncUpHier);
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
	Logger()->Write(MLogRec::EErr, this, "Cannot get data from input [Inp]");
    }
    return mData;
}

// Agent of function "Subtraction of Int data"

ATrSubInt::ATrSubInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type(), ATrInt::PEType());
    SetParent(Type());
}

ATrSubInt::ATrSubInt(Elem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetEType(ATrInt::PEType());
    SetParent(ATrInt::PEType());
}

string ATrSubInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrSubInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt ATrSubInt::Value()
{
    TInt res = 0;
    TIfRange range = GetInpRg("Inp");
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res += dget->Value();
	}
    }
    range = GetInpRg("Inp_Sub");
    for (IfIter it = range.first; it != range.second; it++) {
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

ATrMplInt::ATrMplInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type(), ATrInt::PEType());
    SetParent(Type());
}

ATrMplInt::ATrMplInt(Elem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetEType(ATrInt::PEType());
    SetParent(ATrInt::PEType());
}

string ATrMplInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrMplInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TInt ATrMplInt::Value()
{
    TInt res = 1;
    TIfRange range = GetInpRg("Inp");
    for (IfIter it = range.first; it != range.second; it++) {
	MDIntGet* dget = (MDIntGet*) (*it);
	if (dget != NULL) {
	    res *= dget->Value();
	}
    }
    return res;
}

// Agent function "Dividing of Int data"

ATrDivInt::ATrDivInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type(), ATrInt::PEType());
    SetParent(Type());
}

ATrDivInt::ATrDivInt(Elem* aMan, MEnv* aEnv): ATrInt(Type(), aMan, aEnv)
{
    SetEType(ATrInt::PEType());
    SetParent(ATrInt::PEType());
}

string ATrDivInt::PEType()
{
    return ATrInt::PEType() + GUri::KParentSep + Type();
}

void *ATrDivInt::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrInt::DoGetObj(aName, aIncUpHier);
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
	    Logger()->Write(MLogRec::EErr, this, "Divider value is zero");
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Inputs aren't exists");
    }
    return res;
}

// Agent of transition of variable type
string ATrVar::PEType()
{
    return ATrBase::PEType() + GUri::KParentSep + Type();
}

ATrVar::ATrVar(const string& aName, Elem* aMan, MEnv* aEnv): ATrBase(aName, aMan, aEnv), mFunc(NULL)
{
    SetEType(Type(), ATrBase::PEType());
    SetParent(Type());
}

ATrVar::ATrVar(Elem* aMan, MEnv* aEnv): ATrBase(Type(), aMan, aEnv), mFunc(NULL)
{
    SetEType(ATrBase::PEType());
    SetParent(ATrBase::PEType());
}

void *ATrVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MDVarGet::Type()) == 0) {
	res = (MDVarGet*) this;
    }
    else {
	res = ATrBase::DoGetObj(aName, aIncUpHier);
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

string ATrVar::VarGetIfid() const
{
    return mFunc != NULL ? mFunc->IfaceGetId() : string();
}

Elem* ATrVar::VarGetBase() 
{
    return this;
}

Elem::TIfRange ATrVar::GetInps(TInt aId)
{
    return TIfRange(IfIter(), IfIter());
}

void ATrVar::LogWrite(MLogRec::TLogRecCtg aCtg, const char* aFmt,...)
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

ATrAddVar::ATrAddVar(const string& aName, Elem* aMan, MEnv* aEnv): ATrVar(aName, aMan, aEnv)
{
    SetEType(Type(), ATrVar::PEType());
    SetParent(Type());
}

ATrAddVar::ATrAddVar(Elem* aMan, MEnv* aEnv): ATrVar(Type(), aMan, aEnv)
{
    SetEType(ATrVar::PEType());
    SetParent(ATrVar::PEType());
}

void *ATrAddVar::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ATrVar::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void ATrAddVar::Init(const string& aIfaceName)
{
    if (mFunc != NULL) {
	delete mFunc;
	mFunc == NULL;
    }
    if ((mFunc = FAddInt::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddFloat::Create(this, aIfaceName)) != NULL);
    else if ((mFunc = FAddVFloat::Create(this, aIfaceName)) != NULL);
}

Elem::TIfRange ATrAddVar::GetInps(TInt aId)
{
    if (aId == FAddBase::EInp) {
	Elem* inp = GetNode("../../Inp");
	__ASSERT(inp != NULL);
	RqContext cont(this);
	return inp->GetIfi(MDVarGet::Type(), &cont);
    }
}




/* State base agent */

StateAgent::StateAgent(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

StateAgent::StateAgent(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iActive(ETrue)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

string StateAgent::PEType()
{
    return StateAgent::PEType() + GUri::KParentSep + Type();
}

void *StateAgent::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void StateAgent::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx = ToCacheRCtx(aCtx);
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
	res = Elem::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
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
    Elem* eprepu = GetNode("../../Prepared/Capsule/Upd");
    if (eprepu != NULL) {
	MUpdatable* upd = eprepu->GetObj(upd);
	if (upd != NULL) {
	    if (upd->Update()) {
		SetUpdated();
	    }
	    ResetActive();
	}
    }
}

void StateAgent::Confirm()
{
    Elem* econfu = GetNode("../../Confirmed/Capsule/Upd");
    if (econfu != NULL) {
	MUpdatable* upd = econfu->GetObj(upd);
	if (upd != NULL) {
	    if (upd->Update()) {
		// Activate dependencies
		Elem* eobs = GetNode("../../Capsule/Out/Int/PinObs");
		RqContext ctx(this);
		// Request w/o context because of possible redirecting request to itself
		// TODO [YB] To check if iterator is not damage during the cycle, to cache to vector if so
		TIfRange range = eobs->GetIfi(MDesObserver::Type());
		for (IfIter it = range.first; it != range.second; it++) {
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

void StateAgent::OnUpdated()
{
    // Mark active
    SetActive();
}

void StateAgent::OnActivated()
{
}

/* DES base agent */

string ADes::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADes::ADes(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

ADes::ADes(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iActive(ETrue)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

void *ADes::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
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
	res = Elem::DoGetObj(aName, aIncUpHier);
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
    Elem* host = iMan->GetMan();
    for (vector<Elem*>::iterator it = host->Comps().begin(); it != host->Comps().end(); it++) {
	Elem* eit = (*it);
	if (eit != iMan && eit->Name() != "Capsule") {
	    MDesSyncable* msync = eit->GetObj(msync);
	    if (msync != NULL) {
		if (msync->IsActive()) {
		    msync->Update();
		    SetUpdated();
		}
	    }
	}
    }
}

void ADes::Confirm()
{
    // Confirm all the DES components
    Elem* host = iMan->GetMan();
    for (vector<Elem*>::iterator it = host->Comps().begin(); it != host->Comps().end(); it++) {
	Elem* eit = (*it);
	MDesSyncable* msync = eit->GetObj(msync);
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

void ADes::OnUpdated()
{
    // Mark active
    SetActive();
}

void ADes::OnActivated()
{
}

