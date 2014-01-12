
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

/*
MDIntGet* ATrInt::GetInp(const string& aInpName)
{
    MDIntGet* res = NULL;
    Elem* einp = GetNode("../../ConnPoint:" + aInpName);
    if (einp != NULL) {
	Vert* vert = einp->GetObj(vert);
	MVert* pair = *(vert->Pairs().begin());
	if (pair != NULL) {
	    res = pair->EBase()->GetObj(res);
	}
	// Attempt to use the iface getting rule basing on vert pairs was denied
	//res = einp->GetObj(res);
    }
    return res;
}
*/

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


// Agent of function "Increment of Int data"

ATrIncInt::ATrIncInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type(), ATrInt::PEType());
    SetParent(Type());
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



/* State base agent */

StateAgent::StateAgent(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
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
    Elem* eprepu = GetNode("../../DataSCInt:Prepared/Elem:Capsule/ConnPoint:Upd");
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
    Elem* econfu = GetNode("../../DataSCInt:Confirmed/Elem:Capsule/ConnPoint:Upd");
    if (econfu != NULL) {
	MUpdatable* upd = econfu->GetObj(upd);
	if (upd != NULL) {
	    if (upd->Update()) {
		// Activate dependencies
		Elem* eobs = GetNode("../../Elem:Capsule/Extender:Out/StOutSocket:Int/ConnPoint:PinObs");
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

/*
void State::OnCompChanged(Elem& aComp)
{
    if (aComp.EType() == "Edge") {
	// Reconnect the edge
	Edge* edge = aComp.GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
	const string& pt1u = edge->Point1u();
	const string& pt2u = edge->Point2u();
	if (!pt1u.empty() && !pt2u.empty()) {
	    Elem* pt1 = GetNode(pt1u);
	    Elem* pt2 = GetNode(pt2u);
	    if (pt1 != NULL && pt2 != NULL) {
		// Check if CPs belongs to capsule
		Elem* pt1man = pt1->GetMan();
		Elem* pt2man = pt2->GetMan(); 
		if (pt1man->Name() == "Capsule" && pt2man->Name() == "Capsule") {
		    MVert* pt1v = pt1->GetObj(pt1v);
		    MVert* pt2v = pt2->GetObj(pt2v);
		    if (pt1v != NULL && pt2v != NULL) {
			// Check roles conformance
			// Point#1 provided
			Elem* ept1prov = pt1->GetNode("Prop:Provided");
			MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
			Elem* ept2req = pt2->GetNode("Prop:Required");
			MProp* ppt2req = ept2req->GetObj(ppt2req);
			// Point#2 provided
			Elem* ept2prov = pt2->GetNode("Prop:Provided");
			MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
			Elem* ept1req = pt1->GetNode("Prop:Required");
			MProp* ppt1req = ept1req->GetObj(ppt1req);
			if (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value()) {
			    // Roles are compatible - connect
			    edge->SetPoints(pt1v, pt2v);
			    TBool res = edge->Connect();
			    if (res) {
				Logger()->WriteFormat("State [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			    else {
				Logger()->WriteFormat("ERR: State [%s] connecting [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: State [%s] connecting [%s - %s] - incompatible roles", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: State [%s] connecting [%s - %s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		} // pt1man->Name() == "Capsule"
		else {
		    Logger()->WriteFormat("ERR: State [%s] connecting [%s - %s] - not capsule cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
	    } // pt1 != NULL ...
	    else {
		Logger()->WriteFormat("ERR: State [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
			(pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}
*/
