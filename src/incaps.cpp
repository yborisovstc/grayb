
#include "incaps.h"
#include "edge.h"
#include "prov.h"
#include "mprop.h"


// TODO [YB] Do we need incaps? Syst also provides same restriction for connections (the only CPs are allowed for connecting)
// The only case is that there can be some Syst CPs that needs to be disabled for connecting. 
// Then these CPs can be moved outside of Incaps Capsula

Incaps::Incaps(const string& aName, Elem* aMan, MEnv* aEnv): Syst(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
    /*
    // Set mutation and mutate
    ChromoNode& mut = iMut->Root();
    ChromoNode ncaps = mut.AddChild(ENt_Node);
    ncaps.SetAttr(ENa_Id, "Capsule");
    ncaps.SetAttr(ENa_Parent, "Elem");
    Mutate();
    */
}

void *Incaps::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else {
	res = Syst::DoGetObj(aName, aIncUpHier, aCtx);
    }
    return res;
}

/*
void Incaps::HandleCompChanged(Elem& aContext, Elem& aComp)
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
				Logger()->WriteFormat("Incaps [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - incompatible roles", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		} // pt1man->Name() == "Capsule"
		else {
		    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - not capsule cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
	    } // pt1 != NULL ...
	    else {
		Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
			(pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}
*/

TBool Incaps::IsPtOk(Elem& aContext, Elem* aPt) {
    TBool res = EFalse;
    Elem* man = aContext.GetCompOwning("Incaps", aPt);
    if (man != NULL) {
	if (man->GetMan() == &aContext) {
	    Elem* caps = man->GetNode("Elem:Capsule");
	    res = caps->IsComp(aPt);
	}
    }
    else {
	man = aContext.GetCompOwning("Syst", aPt);
	if ( man != NULL) {
	    if (man->GetMan() == &aContext) {
		res = aPt->EType() == "ConnPoint";
	    }
	}
	else {
	    res = ETrue;
	}
    }
    return res;
}

TBool Incaps::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = EFalse;
    Elem* eedge = aContext.GetCompOwning("Edge", &aComp);
    if (eedge != NULL) {
	//if (aComp.EType() == "Edge") {
	// Reconnect the edge
	Edge* edge = eedge->GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
	const string& pt1u = edge->Point1u();
	const string& pt2u = edge->Point2u();
	if (!pt1u.empty() && !pt2u.empty()) {
	    Elem* pt1 = aContext.GetNode(pt1u);
	    Elem* pt2 = aContext.GetNode(pt2u);
	    if (pt1 != NULL && pt2 != NULL) {
#if 1
		TBool ispt1ok = IsPtOk(aContext, pt1);
		TBool ispt2ok = IsPtOk(aContext, pt2);
		if (ispt1ok && ispt2ok) {
		    MVert* pt1v = pt1->GetObj(pt1v);
		    MVert* pt2v = pt2->GetObj(pt2v);
		    if (pt1v != NULL && pt2v != NULL) {
			MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
			MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
			if (pt1checker->IsCompatible(pt2) && pt2checker->IsCompatible(pt1)) {
			    // Are compatible - connect
			    edge->SetPoints(pt1v, pt2v);
			    TBool res = edge->Connect();
			    if (res) {
				if (IsLogeventCreOn()) {
				    Logger()->WriteFormat("Incaps [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
				}
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - incompatible", 
				    Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - ends aren't vertexes", 
				Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		}
		else {
		    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - not allowed cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
#endif

#if 0
		// There can be two cases: connecting and extending
		Elem* caps = aContext.GetNode("Elem:Capsule");
		Elem* pt1man = pt1->GetMan();
		Elem* pt2man = pt2->GetMan(); 
		TBool pt1caps = pt1man->Name() == "Capsule";
		TBool pt2caps = pt2man->Name() == "Capsule";
		TBool pt1eint = caps->IsComp(pt1);
		TBool pt2eint = caps->IsComp(pt2);
		TBool pt1int = pt1caps && aContext.IsComp(pt1);
		TBool pt2int = pt2caps && aContext.IsComp(pt2);
		// Connecting: One CP is in context capsula and another is out of context
		TBool isconn = pt1caps && pt2caps;
		// Extending: One CP is in context capsula and another is within the context
		TBool isext = pt1int && pt2eint || pt2int && pt1eint;
		if (isconn || isext) {
		    MVert* pt1v = pt1->GetObj(pt1v);
		    MVert* pt2v = pt2->GetObj(pt2v);
		    if (pt1v != NULL && pt2v != NULL) {
			MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
			MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
			if (pt1checker->IsCompatible(pt2) && pt2checker->IsCompatible(pt1)) {
			    // Are compatible - connect
			    edge->SetPoints(pt1v, pt2v);
			    TBool res = edge->Connect();
			    if (res) {
				Logger()->WriteFormat("Incaps [%s] extended [%s-%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] extending [%s-%s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s-%s] - incompatible roles", 
				    Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s-%s] - ends aren't vertexes", 
				Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		}
		else {
		    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - not capsule cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
#endif
#if 0
		// There can be two cases: connecting and extending
		// Checking if CPs belongs to capsule, or CP is internal point of extender
		Elem* caps = aContext.GetNode("Elem:Capsule");
		Elem* pt1man = pt1->GetMan();
		Elem* pt2man = pt2->GetMan(); 
		TBool pt1caps = pt1man->Name() == "Capsule";
		TBool pt2caps = pt2man->Name() == "Capsule";
		TBool pt1eint = caps->IsComp(pt1) && pt1->Name() == "Int" && pt1man->EType() == "Extender";
		TBool pt2eint = caps->IsComp(pt2) && pt2->Name() == "Int" && pt2man->EType() == "Extender";
		TBool pt1int = pt1caps && aContext.IsComp(pt1);
		TBool pt2int = pt2caps && aContext.IsComp(pt2);
		if (pt1caps && pt2caps || pt1int && pt2eint || pt2int && pt1eint) {
		    // CPs belongs to capsule, or CP is internal point of extender
		    MVert* pt1v = pt1->GetObj(pt1v);
		    MVert* pt2v = pt2->GetObj(pt2v);
		    if (pt1v != NULL && pt2v != NULL) {
			// Check roles conformance
			Elem* pt1r = pt1eint ? pt1man : pt1;
			Elem* pt2r = pt2eint ? pt2man : pt2;
			// Point#1 provided
			Elem* ept1prov = pt1r->GetNode("Prop:Provided");
			MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
			Elem* ept2req = pt2r->GetNode("Prop:Required");
			MProp* ppt2req = ept2req->GetObj(ppt2req);
			// Point#2 provided
			Elem* ept2prov = pt2r->GetNode("Prop:Provided");
			MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
			Elem* ept1req = pt1->GetNode("Prop:Required");
			MProp* ppt1req = ept1req->GetObj(ppt1req);
			if (pt1int && pt2eint || pt2int && pt1eint) {
			    // Extending
			    if (ppt1prov->Value() == ppt2prov->Value() && ppt2req->Value() == ppt1req->Value()) {
				// Roles are compatible - connect
				edge->SetPoints(pt1v, pt2v);
				TBool res = edge->Connect();
				if (res) {
				    Logger()->WriteFormat("Incaps [%s] extended [%s-%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
				}
				else {
				    Logger()->WriteFormat("ERR: Incaps [%s] extending [%s-%s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
				}
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] extending [%s-%s] - incompatible roles", 
					Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else if (pt1caps && pt2caps) {
			    // Connecting
			    if (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value()) {
				// Roles are compatible - connect
				edge->SetPoints(pt1v, pt2v);
				TBool res = edge->Connect();
				if (res) {
				    Logger()->WriteFormat("Incaps [%s] connected [%s-%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
				}
				else {
				    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s-%s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
				}
			    }
			    else {
				Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s-%s] - incompatible roles", 
					Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s-%s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		} // pt1man->Name() == "Capsule"
		else {
		    Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - not capsule cp", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		}
#endif
	    } // pt1 != NULL ...
	    else {
		Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
			(pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
	res = ETrue;
    }
    return res;
}

