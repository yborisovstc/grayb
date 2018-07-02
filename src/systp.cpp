#include "systp.h"


// System
string Systp::PEType()
{
    return Vertp::PEType() + GUri::KParentSep + Type();
}

Systp::Systp(const string& aName, MElem* aMan, MEnv* aEnv): Vertp(aName, aMan, aEnv)
{
    SetParent(Type());
}

Systp::Systp(MElem* aMan, MEnv* aEnv): Vertp(Type(), aMan, aEnv)
{
    SetParent(Vertp::PEType());
}

void Systp::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    Vertp::OnCompDeleting(aComp, aSoft, aModif);
}

TBool Systp::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool res = Vertp::OnCompChanged(aComp, aContName, aModif);
    if (res) {
	TBool iscp = IsContentCompOf(aContName, KContent_Connpoints);
	TBool isedge = IsContentCompOf(aContName, KContent_Edges);
	if (isedge) {
	    string edgeName = GetContentOwner(aContName);
	    if (ContentHasComp(edgeName, KContent_P1) && ContentHasComp(edgeName, KContent_P2) ) {
		string puri1 = GetContentValue(ContentKey(edgeName, KContent_P1));
		string puri2 = GetContentValue(ContentKey(edgeName, KContent_P2));
		MElem* e1 = GetNode(puri1);
		MElem* e2 = GetNode(puri2);
		MVert* v1 = e1->GetObj(v1);
		MVert* v2 = e2->GetObj(v2);
	    }
	}
    }

#if 0
	if (!res) return res;
	TBool hres = ETrue;
	MEdge* edge = aComp.GetObj(edge);	
	if (edge != NULL) {
	    MVert* ref1 = edge->Ref1();
	    MVert* ref2 = edge->Ref2();
	    MVert* cp1 = edge->Point1();
	    MVert* cp2 = edge->Point2();
	    if (cp1 != ref1 || cp2 != ref2) {
	    MElem* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
	    MElem* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
	    TBool isptok1 = (ref1 == NULL || IsPtOk(pt1));
	    TBool isptok2 = (ref2 == NULL || IsPtOk(pt2));
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
			    MElem* host = GetMan();
			    Logger()->Write(EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}
		    }
		    else {
			TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			MElem* host = this;
			Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
		    }

		} else {
		    // Partial connection, compatibility checking isn't needed
		    if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
		    else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		}
	    } else {
		MElem* pt = isptok1 ? pt2 : pt1;
		MElem* host = this;
		Logger()->Write(EErr, this, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
	    }
	}
    }
#endif
    return res;
}
