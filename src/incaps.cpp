
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

Incaps::Incaps(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
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
    __ASSERT(aPt != NULL);
    TBool res = EFalse;
    Elem* man = aContext.GetCompOwning("Incaps", aPt);
    if (man != NULL) {
	if (man->GetMan() == &aContext) {
	    Elem* caps = man->GetNode("Capsule");
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
	Elem* host = iMan->GetMan();
	Edge* edge = eedge->GetObj(edge);	
	__ASSERT(edge != NULL);
	if (&aComp == edge->Point1p() || &aComp == edge->Point2p()) {
	    edge->Disconnect(&aComp);
	    if (!edge->Pointu(&aComp).empty()) {
		if (edge->Pointr(&aComp) != NULL) {
		    TBool isptok = IsPtOk(aContext, edge->Pointr(&aComp));
		    if (isptok) {
			if (edge->Pointv(&aComp) != NULL) {
			    if (edge->Point1() == NULL && edge->Point2() == NULL) {
				// Partial connection, compat checking isn't needed
				res = edge->Connect(&aComp);
				if (!res) {
				    Logger()->Write(MLogRec::EErr, host, "Connecting [%s] failed", edge->Pointu(&aComp).c_str());
				}
			    }
			    else {
				// Full connection, compat checking is needed
				Elem* pt1 = edge->Point1r();
				Elem* pt2 = edge->Point2r();
				if (pt1 != NULL && pt2 != NULL) {
				    string pt1u = edge->Point1u();
				    string pt2u = edge->Point2u();
				    MVert* pt1v = pt1->GetObj(pt1v);
				    MVert* pt2v = pt2->GetObj(pt2v);
				    if (pt1v != NULL && pt2v != NULL) {
					// Check roles conformance
					MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
					MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
					TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
					TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
					if (ispt1cptb && ispt2cptb) {
					    // Are compatible - connect
					    res = edge->Connect(&aComp);
					    if (res) {
						Logger()->Write(MLogRec::EInfo, host, "Connected [%s - %s]", pt1u.c_str(), pt2u.c_str());
					    }
					    else {
						Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] failed", pt1u.c_str(), pt2u.c_str());
					    }
					}
					else {
					    Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] - incompatible roles", pt1u.c_str(), pt2u.c_str());
					}
				    }
				    else {
					Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] - ends aren't vertexes", pt1u.c_str(), pt2u.c_str());
				    }
				}
			    }
			}
			else {
			    Logger()->Write(MLogRec::EErr, host, "Connecting [%s] - cannot find or not vertex", edge->Pointu(&aComp).c_str());
			}
		    }
		    else {
			Logger()->Write(MLogRec::EErr, host, "Connecting [%s] - not allowed cp", edge->Pointu(&aComp).c_str());
		    }
		}
		else {
		    Logger()->Write(MLogRec::EErr, host, "Connecting [%s] - cannot find", edge->Pointu(&aComp).c_str());
		}
	    }
	}
    }
    return res;
}
