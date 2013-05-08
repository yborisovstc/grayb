
#include "des.h"
#include "edge.h"
#include "prov.h"
#include "menv.h"
#include "vert.h"
#include "mprop.h"

ATrBase::ATrBase(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
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
    SetEType(Type());
    SetParent(Type());
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

ATrIncInt::ATrIncInt(const string& aName, Elem* aMan, MEnv* aEnv): ATrInt(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
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
    TInt val = mget->Value();
    mData = val + 1;
    return mData;
}



StateAgent::StateAgent(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iActive(ETrue)
{
    SetEType(Type());
    SetParent(Type());
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
		MDesObserver* mobs = eobs->GetObj(mobs);
		if (mobs != NULL) {
		    mobs->OnUpdated();
		}
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
