#include "edge.h"
#include "mvert.h"
#include "mprov.h"
#include "mprop.h"

string Edge::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Edge::Edge(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
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

Edge::Edge(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
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

void* Edge::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MEdge::Type()) == 0) {
	res = (MEdge*) this;
    }
    else {
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
    iPoint1 = aPoint; // Set first to make Pair() working
    res = aPoint->Connect(this);
    if (res) {
	// Connect pair to the point
	/*
	if (iPoint2 != NULL) {
	    iPoint2->Connect(iPoint1);
	}
	*/
    }
    else {
	iPoint1 = NULL;
    }
    return res;
}

TBool Edge::ConnectP2(MVert* aPoint)
{
    TBool res = EFalse;
    __ASSERT(iPoint2 == NULL);
    iPoint2 = aPoint; // Set first to make Pair() working
    res = aPoint->Connect(this);
    if (res) {
	/*
	// Connect pair to the point
	if (iPoint1 != NULL) {
	    iPoint1->Connect(iPoint2);
	}
	*/
    }
    else {
	iPoint2 = NULL;
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
	    iPoint1->Disconnect(this);
	    if (iPoint2 != NULL) {
		iPoint2->Disconnect(iPoint1);
	    }
	    iPoint1 = NULL;
	}
	if (aPoint == iPoint2) {
	    iPoint2->Disconnect(this);
	    if (iPoint1 != NULL) {
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

MVert* Edge::Pair(const MVert* aPoint)
{
    return aPoint == iPoint1 ? iPoint2 : iPoint1;
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
	res = iPoint1->EBase()->GetObj(res);
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
	res = iPoint2->EBase()->GetObj(res);
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
	    Logger()->Write(MLogRec::EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
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
	    Logger()->Write(MLogRec::EErr, this, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
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

Base* Edge::EBase()
{
    return (Base*) this;
}

const Base* Edge::EBase() const
{
    return (const Base*) this;
}

TBool Edge::OnCompChanged(Elem& aComp)
{
    Elem::OnCompChanged(aComp);
    // Propagate notification to upper level
    if (iMan != NULL) {
	iMan->OnCompChanged(*this);
    }
    if (iObserver != NULL) {
	iObserver->OnCompChanged(*this);
    }
    return ETrue;
}

void Edge::SetRemoved()
{
    Disconnect();
    Elem::SetRemoved();
}

