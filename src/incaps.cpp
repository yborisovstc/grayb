
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

Incaps::Incaps(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    /*
    // Set mutation and mutate
    ChromoNode& mut = iMut->Root();
    ChromoNode ncaps = mut.AddChild(ENt_Node);
    ncaps.SetAttr(ENa_Id, "Capsule");
    ncaps.SetAttr(ENa_Parent, "Elem");
    Mutate();
    */
}

MIface *Incaps::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MACompsObserver::Type()) == 0)
	res = (MACompsObserver*) this;
    else if (strcmp(aName, MAgent::Type()) == 0)
	res = dynamic_cast<MAgent*>(this);
    else
	res = Elem::DoGetObj(aName);
    return res;
}

MIface* Incaps::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MElem::Type())
	res = dynamic_cast<MElem*>(this);
    return res;
}

TBool Incaps::IsPtOk(MElem& aContext, MElem* aPt) {
    __ASSERT(aPt != NULL);
    TBool res = EFalse;
    MElem* man = aContext.GetCompOwning("Incaps", aPt);
    if (man != NULL) {
	if (man->GetMan() == &aContext) {
	    MElem* caps = man->GetNode("./Capsule");
	    res = caps->IsComp(aPt);
	}
    }
    else {
	man = aContext.GetCompOwning("Syst", aPt);
	if ( man != NULL) {
	    if (man->GetMan() == &aContext) {
		// TODO What about extenders? to use more adequate restriction
		//res = aPt->EType() == "ConnPoint";
		MConnPoint* mcp = aPt->GetObj(mcp);
		res = mcp != NULL;
	    }
	}
	else {
	    res = ETrue;
	}
    }
    return res;
}

TBool Incaps::HandleCompChanged(MElem& aContext, MElem& aComp, const string& aContName)
{
    TBool hres = ETrue;
    // Handling both Edge (nodes - properties) and AEdge (multicontent)
    MEdge* edge = aComp.GetObj(edge);	
    if (edge == NULL) {
	MElem* owner = aComp.GetMan();
	edge = owner ? owner->GetObj(edge) : NULL;
    }
    if (edge != NULL) {
	TBool res = ETrue;
	MVert* ref1 = edge->Ref1();
	MVert* ref2 = edge->Ref2();
	MVert* cp1 = edge->Point1();
	MVert* cp2 = edge->Point2();
	if (cp1 != ref1 || cp2 != ref2) {
	    MElem* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
	    MElem* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
	    TBool isptok1 = (ref1 == NULL || IsPtOk(aContext, pt1));
	    TBool isptok2 = (ref2 == NULL || IsPtOk(aContext, pt2));
	    if (isptok1 && isptok2) {
		if (cp1 != NULL && ref1 != cp1) edge->Disconnect(cp1);
		if (cp2 != NULL && ref2 != cp2) edge->Disconnect(cp2);
		if (ref1 != NULL && ref2 != NULL) {
		    cp1 = edge->Point1();
		    cp2 = edge->Point2();
		    // Full connection, compatibility checking is needed
		    MCompatChecker* pt1checker = (MCompatChecker*) pt1->GetSIfiC(MCompatChecker::Type(), this);
		    MCompatChecker* pt2checker = (MCompatChecker*) pt2->GetSIfiC(MCompatChecker::Type(), this);
		    TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
		    TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
		    if (ispt1cptb && ispt2cptb) {
			// Are compatible - connect
			if (cp1 == NULL) res = edge->ConnectP1(ref1);
			if (res && cp2 == NULL) res = edge->ConnectP2(ref2);
			if (!res) {
			    Logger()->Write(EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}
		    }
		    else {
			res = EFalse;
			TBool c1 = pt1checker->IsCompatible(pt2);
			TBool c2 = pt2checker->IsCompatible(pt1);
			Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
		    }

		} else {
		    // Partial connection, compatibility checking isn't needed
		    if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
		    else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		}
	    } else {
		res = EFalse;
		MElem* pt = isptok1 ? pt2 : pt1;
		Logger()->Write(EErr, this, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
	    }
	}
	hres = res;
    }
    return hres;
}
