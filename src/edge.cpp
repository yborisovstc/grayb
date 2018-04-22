#include "edge.h"
#include "mvert.h"
#include "mprov.h"
#include "mprop.h"
#include <stdexcept> 

MEdge::EIfu MEdge::mIfu;

// Ifu static initialisation
MEdge::EIfu::EIfu()
{
    RegMethod("ConnectP1", 1);
    RegMethod("ConnectP2", 1);
    RegMethod("Disconnect", 1);
    RegMethod("Disconnect#2", 0);
    RegMethod("Point1", 0);
    RegMethod("Point2", 0);
    RegMethod("Ref1", 0);
    RegMethod("Ref2", 0);
}


string Edge::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Edge::Edge(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    SetParent(Type());
    // Adding properties "Points"
    Elem* p1 = Provider()->CreateNode("Prop", "P1", this, iEnv);
    Elem* p2 = Provider()->CreateNode("Prop", "P2", this, iEnv);
    __ASSERT(p1 != NULL && p2 != NULL);
    TBool res = AppendComp(p1);
    __ASSERT(res);
    res = AppendComp(p2);
    __ASSERT(res);
}

Edge::Edge(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    SetParent(Elem::PEType());
    // Adding properties "Points"
    Elem* p1 = Provider()->CreateNode("Prop", "P1", this, iEnv);
    Elem* p2 = Provider()->CreateNode("Prop", "P2", this, iEnv);
    __ASSERT(p1 != NULL && p2 != NULL);
    TBool res = AppendComp(p1);
    __ASSERT(res);
    res = AppendComp(p2);
    __ASSERT(res);
}

Edge::~Edge() 
{
    Disconnect();
}

MIface* Edge::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MEdge::Type()) == 0) {
	res = dynamic_cast<MEdge*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MVert* Edge::Point1() const
{
    return iPoint1;
}

MVert* Edge::Point2() const
{
    return iPoint2;
}

TBool Edge::ConnectP1(MVert* aPoint)
{
    TBool res = EFalse;
    __ASSERT(iPoint1 == NULL);
    if (iPoint2 != NULL) {
	// Try full connection
	res = aPoint->Connect(iPoint2);
       	if (res) {
	    iPoint1 = aPoint;
	}
    } else {
	// Just set one part connection
	iPoint1 = aPoint;
	res = ETrue;
    }
    return res;
}

TBool Edge::ConnectP2(MVert* aPoint)
{
    TBool res = EFalse;
    __ASSERT(iPoint2 == NULL);
    if (iPoint1 != NULL) {
	// Try full connection
	res = aPoint->Connect(iPoint1);
       	if (res) {
	    iPoint2 = aPoint;
	}
    } else {
	// Just set one part connection
	iPoint2 = aPoint;
	res = ETrue;
    }
    return res;
}

TBool Edge::Connect(MElem* aCp)
{
    TBool res = EFalse;
    if (aCp == Point1p()) {
	MVert* pv = Point1v();
	if (pv != NULL) {
	    res = ConnectP1(pv);
	}
    }
    else if (aCp == Point2p()) {
	MVert* pv = Point2v();
	if (pv != NULL) {
	    res = ConnectP2(pv);
	}
    }
    else {
	__ASSERT(EFalse);
    }
    return res;
}

void Edge::Disconnect(MVert* aPoint)
{
    if (aPoint != NULL) {
	if (aPoint == iPoint1) {
	    if (iPoint2 != NULL) {
		iPoint1->Disconnect(iPoint2);
		iPoint2->Disconnect(iPoint1);
	    }
	    iPoint1 = NULL;
	}
	if (aPoint == iPoint2) {
	    if (iPoint1 != NULL) {
		iPoint2->Disconnect(iPoint1);
		iPoint1->Disconnect(iPoint2);
	    }
	    iPoint2 = NULL;
	}
    }
}

void Edge::Disconnect()
{
    if (iPoint1 != NULL) {
	Disconnect(iPoint1);
    }
    if (iPoint2 != NULL) {
	Disconnect(iPoint2);
    }
}

void Edge::Disconnect(MElem* aCp)
{
    if (aCp == Point1p()) {
	Disconnect(iPoint1);
    }
    else if (aCp == Point2p()) {
	Disconnect(iPoint2);
    }
}

const string& Edge::Point1u()
{
    MElem* pte = GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

const string& Edge::Point2u()
{
    MElem* pte = GetNode("./P2");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

const string& Edge::Pointu(MElem* aCp)
{
    MProp* pt = aCp->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

MElem* Edge::Point1rc()
{
    MElem* res = NULL;
    if (iPoint1 != NULL) {
	res = iPoint1->GetObj(res);
    }
    else {
	res = Point1r();
    }
    return res;
}

MElem* Edge::Point2rc()
{
    MElem* res = NULL;
    if (iPoint2 != NULL) {
	res = iPoint2->GetObj(res);
    }
    else {
	res = Point2r();
    }
    return res;
}

MElem* Edge::Point1r()
{
    MElem* res = NULL;
    MElem* pte = GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	res = pte->GetNode(uri);
    }
    return res;
}

MElem* Edge::Point2r()
{
    MElem* res = NULL;
    MElem* pte = GetNode("./P2");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	res = pte->GetNode(uri);
    }
    return res;
}

MElem* Edge::Pointr(MElem* aCp)
{
    __ASSERT(aCp == Point1p() || aCp == Point2p());
    MElem* res = NULL;
    const string& uri = Pointu(aCp);
    if (!uri.empty()) {
	res = aCp->GetNode(uri);
    }
    return res;

}

MVert* Edge::Point1v()
{
    MVert* res = NULL;
    MElem* pte = GetNode("./P1");
    const string& uri = Point1u();
    if (!uri.empty()) {
	MElem* pr = pte->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

MVert* Edge::Point2v()
{
    MVert* res = NULL;
    MElem* pte = GetNode("./P2");
    const string& uri = Point2u();
    if (!uri.empty()) {
	MElem* pr = pte->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

MVert* Edge::Ref1() const
{
    MVert* res = NULL;
    MElem* pte = ((Elem*) this)->GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	MElem* pr = pte->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}

MVert* Edge::Ref2() const
{
    MVert* res = NULL;
    MElem* pte = ((Elem*) this)->GetNode("./P2");
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	MElem* pr = pte->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}

MVert* Edge::Pointv(MElem* aCp)
{
    __ASSERT(aCp == Point1p() || aCp == Point2p());
    MVert* res = NULL;
    const string& uri = Pointu(aCp);
    if (!uri.empty()) {
	MElem* pr = aCp->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

MElem* Edge::Point1p()
{
    return GetNode("./P1");
}

MElem* Edge::Point2p()
{
    return GetNode("./P2");
}

TBool Edge::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    Elem::OnCompChanged(aComp, aContName, aModif);
    // Propagate notification to upper level
    if (iMan != NULL) {
	iMan->OnCompChanged(*this, aContName, aModif);
    }
    for (auto observer : iObservers) {
	observer->OnCompChanged(*this, aContName, aModif);
    }
    return ETrue;
}

void Edge::SetRemoved(TBool aModif)
{
    Disconnect();
    Elem::SetRemoved(aModif);
}

MIface* Edge::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MEdge::mIfu.CheckMname(name);
    if (!name_ok) {
	return Elem::Call(aSpec, aRes);
    }
    TBool args_ok = MEdge::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "ConnectP1") {
	MElem* pair = GetNode(args.at(0));
	if (pair == NULL) {
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVert* vpair = pair->GetObj(vpair);
	if (vpair == NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = ConnectP1(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "ConnectP2") {
	MElem* pair = GetNode(args.at(0));
	if (pair == NULL) {
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVert* vpair = pair->GetObj(vpair);
	if (vpair == NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = ConnectP2(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "Point1") {
	res = Point1();
    } else if (name == "Point2") {
	res = Point2();
    } else if (name == "Ref1") {
	res = Ref1();
    } else if (name == "Ref2") {
	res = Ref2();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

void Edge::SetPoint1(const string& aRef)
{
    MElem* p = Point1p();
    GUri uri = GUri("./..") + aRef;
    p->AppendMutation(TMut(ENt_Cont, ENa_Ref, uri));
    p->Mutate(false, true, true, GetRoot());
}

void Edge::SetPoint2(const string& aRef)
{
    MElem* p = Point2p();
    GUri uri = GUri("./..") + aRef;
    p->AppendMutation(TMut(ENt_Cont, ENa_Ref, uri));
    p->Mutate(false, true, true, GetRoot());
}




// Edge agent using named content to keep the refs to conn points

const string& Aedge::mP1ContName = "P1";
const string& Aedge::mP2ContName = "P2";

string Aedge::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Aedge::Aedge(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    SetParent(Type());
}

Aedge::Aedge(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    SetParent(Elem::PEType());
}

Aedge::~Aedge() 
{
    Disconnect();
}

MIface* Aedge::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MEdge::Type()) == 0) {
	res = dynamic_cast<MEdge*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MVert* Aedge::Point1() const
{
    return iPoint1;
}

MVert* Aedge::Point2() const
{
    return iPoint2;
}

TBool Aedge::ConnectP1(MVert* aPoint)
{
    TBool res = EFalse;
    __ASSERT(iPoint1 == NULL);
    if (iPoint2 != NULL) {
	// Try full connection
	res = aPoint->Connect(iPoint2);
       	if (res) {
	    iPoint1 = aPoint;
	}
    } else {
	// Just set one part connection
	iPoint1 = aPoint;
	res = ETrue;
    }
    return res;
}

TBool Aedge::ConnectP2(MVert* aPoint)
{
    TBool res = EFalse;
    __ASSERT(iPoint2 == NULL);
    if (iPoint1 != NULL) {
	// Try full connection
	res = aPoint->Connect(iPoint1);
       	if (res) {
	    iPoint2 = aPoint;
	}
    } else {
	// Just set one part connection
	iPoint2 = aPoint;
	res = ETrue;
    }
    return res;
}

void Aedge::Disconnect(MVert* aPoint)
{
    if (aPoint != NULL) {
	if (aPoint == iPoint1) {
	    if (iPoint2 != NULL) {
		iPoint1->Disconnect(iPoint2);
		iPoint2->Disconnect(iPoint1);
	    }
	    iPoint1 = NULL;
	}
	if (aPoint == iPoint2) {
	    if (iPoint1 != NULL) {
		iPoint2->Disconnect(iPoint1);
		iPoint1->Disconnect(iPoint2);
	    }
	    iPoint2 = NULL;
	}
    }
}

void Aedge::Disconnect()
{
    if (iPoint1 != NULL) {
	Disconnect(iPoint1);
    }
    if (iPoint2 != NULL) {
	Disconnect(iPoint2);
    }
}

MVert* Aedge::Pair(const MVert* aPoint)
{
    return aPoint == iPoint1 ? iPoint2 : iPoint1;
}

const string& Aedge::Point1u() const
{
    return mPoint1Uri;
}

const string& Aedge::Point2u() const
{
    return mPoint2Uri;
}

Base* Aedge::EBase()
{
    return (Base*) this;
}

const Base* Aedge::EBase() const
{
    return (const Base*) this;
}

/*
void Aedge::GetCont(string& aCont, const string& aName) const
{
    if (aName == mP1ContName) aCont = mPoint1Uri;
    else if (aName == mP2ContName) aCont = mPoint2Uri;
}
*/

TBool Aedge::GetCont(TInt aInd, string& aName, string& aCont) const
{
    TBool res = ETrue;
    if (aInd == ECnt_P1) { aName = mP1ContName; aCont = mPoint1Uri; }
    else if (aInd == ECnt_P2) { aName = mP2ContName; aCont = mPoint2Uri; }
    else res = EFalse;
    return res;
}

TBool Aedge::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    TBool changed = EFalse;
    if (aName == mP1ContName) {
	if (aVal != mPoint1Uri) { mPoint1Uri = aVal; changed = ETrue; }
    } else if (aName == mP2ContName) {
	if (aVal != mPoint2Uri) { mPoint2Uri = aVal; changed = ETrue; }
    }
    res = Elem::ChangeCont(aVal, aRtOnly, aName);
    /*
    if (res && changed) {
	if (aRtOnly) iMan->OnChanged(*this);
	else iMan->OnCompChanged(*this, aName);
    }
    */
    return res;
}

MIface* Aedge::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MEdge::mIfu.CheckMname(name);
    if (!name_ok) {
	return Elem::Call(aSpec, aRes);
    }
    TBool args_ok = MEdge::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "ConnectP1") {
	MElem* pair = GetNode(args.at(0));
	if (pair != NULL) {
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVert* vpair = pair->GetObj(vpair);
	if (vpair != NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = ConnectP1(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "ConnectP2") {
	MElem* pair = GetNode(args.at(0));
	if (pair != NULL) {
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVert* vpair = pair->GetObj(vpair);
	if (vpair != NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = ConnectP2(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "Point1") {
	res = Point1();
    } else if (name == "Point2") {
	res = Point2();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

MVert* Aedge::Ref1() const
{
    MVert* res = NULL;
    const string& uri = Point1u();
    if (!uri.empty()) {
	MElem* pr = ((MElem*) this)->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}


MVert* Aedge::Ref2() const
{
    MVert* res = NULL;
    const string& uri = Point2u();
    if (!uri.empty()) {
	MElem* pr = ((MElem*) this)->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}

void Aedge::SetPoint1(const string& aRef)
{
    AppendMutation(TMut(ENt_Cont, ENa_Id, mP1ContName, ENa_Ref, aRef));
    Mutate(false, true, true, GetRoot());
}

void Aedge::SetPoint2(const string& aRef)
{
    AppendMutation(TMut(ENt_Cont, ENa_Id, mP2ContName, ENa_Ref, aRef));
    Mutate(false, true, true, GetRoot());
}

