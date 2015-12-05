
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

void *Incaps::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

TBool Incaps::IsPtOk(Elem& aContext, Elem* aPt) {
    __ASSERT(aPt != NULL);
    TBool res = EFalse;
    Elem* man = aContext.GetCompOwning("Incaps", aPt);
    if (man != NULL) {
	if (man->GetMan() == &aContext) {
	    Elem* caps = man->GetNode("./Capsule");
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

#if 0
// Alternative implementation of HandleCompChanged
TBool Incaps::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool hres = EFalse;
    MEdge* edge = aComp.GetObj(edge);	
    if (edge != NULL) {
	hres = ETrue;
	MVert* ref1 = edge->Ref1();
	MVert* ref2 = edge->Ref2();
	MVert* cp1 = edge->Point1();
	MVert* cp2 = edge->Point2();
	Elem* pt1 = ref1 == NULL ? NULL : ref1->EBase()->GetObj(pt1);
	Elem* pt2 = ref2 == NULL ? NULL : ref2->EBase()->GetObj(pt2);
	if (cp1 != ref1) {
	    TBool isptok = (ref1 == NULL || IsPtOk(aContext, pt1));
	    if (isptok) {
		if (cp1 != NULL && ref1 != cp1) edge->Disconnect(cp1);
		if (ref1 != NULL) {
		    TBool res = EFalse;
		    if (ref1 != NULL && cp2 != NULL) {
			// Full connection, compatibility checking is needed
			MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
			MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
			TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			if (ispt1cptb && ispt2cptb) {
			    // Are compatible - connect
			    res = edge->ConnectP1(ref1);
			}
			else {
			    TBool c1 = pt1checker->IsCompatible(pt2);
			    TBool c2 = pt2checker->IsCompatible(pt1);
			    Elem* host = iMan->GetMan();
			    Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] - incompatible roles", pt1->GetUri().c_str(), pt2->GetUri().c_str());
			}

		    } else {
			// Partial connection, compatibility checking isn't needed
			res = edge->ConnectP1(ref1);
		    }
		    if (!res) {
			Elem* host = iMan->GetMan();
			Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
		    }
		}
	    } else {
		Elem* pt = pt1;
		Elem* host = iMan->GetMan();
		Logger()->Write(MLogRec::EErr, host, "Connecting [%s] - not allowed cp", pt->GetUri().c_str());
	    }
	}
	else if (cp2 != ref2) {
	    TBool isptok = (ref2 == NULL || IsPtOk(aContext, pt2));
	    if (isptok) {
		if (cp2 != NULL && ref2 != cp2) edge->Disconnect(cp2);
		if (ref2 != NULL) {
		    TBool res = EFalse;
		    if (ref2 != NULL && cp1 != NULL) {
			// Full connection, compatibility checking is needed
			MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
			MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
			TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			if (ispt1cptb && ispt2cptb) {
			    // Are compatible - connect
			    res = edge->ConnectP2(ref2);
			}
			else {
			    TBool c1 = pt1checker->IsCompatible(pt2);
			    TBool c2 = pt2checker->IsCompatible(pt1);
			    Elem* host = iMan->GetMan();
			    Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] - incompatible roles", pt1->GetUri().c_str(), pt2->GetUri().c_str());
			}

		    } else {
			// Partial connection, compatibility checking isn't needed
			res = edge->ConnectP2(ref2);
		    }
		    if (!res) {
			Elem* host = iMan->GetMan();
			Logger()->Write(MLogRec::EErr, host, "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
		    }
		}
	    } else {
		Elem* pt = pt2;
		Elem* host = iMan->GetMan();
		Logger()->Write(MLogRec::EErr, host, "Connecting [%s] - not allowed cp", pt->GetUri().c_str());
	    }
	}
    }
    return hres;
}
#endif

TBool Incaps::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool hres = EFalse;
    MEdge* edge = aComp.GetObj(edge);	
    if (edge != NULL) {
	hres = ETrue;
	MVert* ref1 = edge->Ref1();
	MVert* ref2 = edge->Ref2();
	MVert* cp1 = edge->Point1();
	MVert* cp2 = edge->Point2();
	if (cp1 != ref1 || cp2 != ref2) {
	    Elem* pt1 = ref1 == NULL ? NULL : ref1->EBase()->GetObj(pt1);
	    Elem* pt2 = ref2 == NULL ? NULL : ref2->EBase()->GetObj(pt2);
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
			TBool res = ETrue;
			if (cp1 == NULL) res = edge->ConnectP1(ref1);
			if (res && cp2 == NULL) res = edge->ConnectP2(ref2);
			if (!res) {
			    Elem* host = iMan->GetMan();
			    Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}
		    }
		    else {
			TBool c1 = pt1checker->IsCompatible(pt2);
			TBool c2 = pt2checker->IsCompatible(pt1);
			Elem* host = iMan->GetMan();
			Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
		    }

		} else {
		    // Partial connection, compatibility checking isn't needed
		    if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
		    else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		}
	    } else {
		Elem* pt = isptok1 ? pt2 : pt1;
		Elem* host = iMan->GetMan();
		Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
	    }
	}
    }
    return hres;
}
