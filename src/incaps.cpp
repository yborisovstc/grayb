
#include "incaps.h"
#include "edge.h"
#include "prov.h"
#include "mprop.h"


// TODO [YB] Do we need incaps? Syst also provides same restriction for connections (the only CPs are allowed for connecting)
// The only case is that there can be some Syst CPs that needs to be disabled for connecting. 
// Then these CPs can be moved outside of Incaps Capsula


string Incaps::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Incaps::Incaps(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
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
	res = Elem::DoGetObj(aName, aIncUpHier, aCtx);
    }
    return res;
}

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
	// Reconnect the edge
	Edge* edge = eedge->GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
	const string& pt1u = edge->Point1u();
	if (!pt1u.empty()) {
	    Elem* pt1 = aContext.GetNode(pt1u);
	    if (pt1 != NULL) {
		MVert* pt1v = pt1->GetObj(pt1v);
		if (pt1v != NULL) {
		    edge->SetPoint1(pt1v);
		}
	    }
	    else {
		Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s] - cannot find", Name().c_str(), pt1u.c_str());
	    }
	}
	const string& pt2u = edge->Point2u();
	if (!pt2u.empty()) {
	    Elem* pt2 = aContext.GetNode(pt2u);
	    if (pt2 != NULL) {
		MVert* pt2v = pt2->GetObj(pt2v);
		if (pt2v != NULL) {
		    edge->SetPoint2(pt2v);
		}
	    }
	    else {
		Logger()->WriteFormat("ERR: Incaps [%s] connecting [%s] - cannot find", Name().c_str(), pt2u.c_str());
	    }
	}
	Elem* pt1 = aContext.GetNode(pt1u);
	Elem* pt2 = aContext.GetNode(pt2u);
	Elem* host = iMan->GetMan();
	if (pt1 != NULL && pt2 != NULL) {
	    TBool ispt1ok = IsPtOk(aContext, pt1);
	    TBool ispt2ok = IsPtOk(aContext, pt2);
	    if (ispt1ok && ispt2ok) {
		MVert* pt1v = pt1->GetObj(pt1v);
		MVert* pt2v = pt2->GetObj(pt2v);
		if (pt1v != NULL && pt2v != NULL) {
		    MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
		    MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
		    TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
		    TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
		    if (ispt1cptb && ispt2cptb) {
			// Are compatible - connect
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
			Logger()->WriteFormat("ERR: [%s/%s] connecting [%s - %s] - incompatible", 
				host->Name().c_str(), Name().c_str(), pt1u.c_str(), pt2u.c_str());
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
	} 
	res = ETrue;
    }
    return res;
}

