#include "edge.h"
#include "mvert.h"
#include "mprov.h"
#include "mprop.h"

// Points iterator
Edge::IterImplEdge::IterImplEdge(Elem& aElem, GUri::TElem aId, TBool aToEnd): IterImplBase(aElem, aId, aToEnd)
{
    Edge* edge = iElem.GetObj(edge);
    __ASSERT(edge != NULL);
    iPIter = EEnd;
    if (!aToEnd) {
	MVert* mpt1 = edge->iPoint1;
	if (mpt1 != NULL) {
	    Elem* pt1 = mpt1->EBase()->GetObj(pt1);
	    if ((iId.second.second == "*" || pt1->Name() == iId.second.second) && (iId.first == "*" || pt1->EType() == iId.first)) {
		iPIter = EP1;
	    }
	}
	if (iPIter == EEnd) {
	    MVert* mpt2 = edge->iPoint2;
	    if (mpt2 != NULL) {
		Elem* pt2 = mpt2->EBase()->GetObj(pt2);
		if ((iId.second.second == "*" || pt2->Name() == iId.second.second) && (iId.first == "*" || pt2->EType() == iId.first)) {
		    iPIter = EP2;
		}
	    }
	}
    }
};

Edge::IterImplEdge::IterImplEdge(const IterImplEdge& aIt): IterImplBase(aIt),
    iPIter(aIt.iPIter)
{
};

void Edge::IterImplEdge::Set(const IterImplBase& aImpl)
{
    const IterImplEdge* src = static_cast<const IterImplEdge*>(aImpl.DoGetObj(Type()));
    __ASSERT(src != NULL);
    iPIter = src->iPIter; 
}

void Edge::IterImplEdge::PostIncr()
{
    if (iCIter != iCIterRange.second) {
	IterImplBase::PostIncr();
    }
    else {
	Edge* edge = iElem.GetObj(edge);
	if (iPIter == EP1) {
	    iPIter = EEnd;
	    MVert* mpt2 = edge->iPoint2;
	    if (mpt2 != NULL) {
		Elem* pt2 = mpt2->EBase()->GetObj(pt2);
		if ((iId.second.second == "*" || pt2->Name() == iId.second.second) && (iId.first == "*" || pt2->EType() == iId.first)) {
		    iPIter = EP2;
		}
	    }
	}
	if (iPIter == EP2) {
	    iPIter = EEnd;
	}
    }
}

TBool Edge::IterImplEdge::IsCompatible(const IterImplBase& aImpl) const
{
    const IterImplEdge* src = static_cast<const IterImplEdge*>(aImpl.DoGetObj(Type()));
    return src != NULL;
}

TBool Edge::IterImplEdge::IsEqual(const IterImplBase& aImpl) const
{
    TBool res = IterImplBase::IsEqual(aImpl);
    if (res) {
	const IterImplEdge* src = static_cast<const IterImplEdge*>(aImpl.DoGetObj(Type()));
	__ASSERT(src != NULL);
	res = iPIter == src->iPIter;
    }
    return res;
}

void *Edge::IterImplEdge::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    return res;
}

const void *Edge::IterImplEdge::DoGetObj(const char *aName) const
{
    const void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    return res;
}

Elem*  Edge::IterImplEdge::GetElem()
{
    Elem* res = NULL;
    if (iCIter != iCIterRange.second) {
	res = IterImplBase::GetElem();
    }
    else {
	Edge* edge = iElem.GetObj(edge);
	MVert* mpt = NULL;
	if (iPIter == EP1) {
	    mpt = edge->iPoint1;
	}
	else if (iPIter == EP2) {
	    mpt = edge->iPoint2;
	}
	if (mpt != NULL) {
	    res = mpt->EBase()->GetObj(res);
	}
    }
    return res;
}

string Edge::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Edge::Edge(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    SetEType(Type(), Elem::PEType());
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
    SetEType(Elem::PEType());
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

void* Edge::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MEdge::Type()) == 0) {
	res = (MEdge*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
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

TBool Edge::Connect(Elem* aCp)
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

void Edge::Disconnect(Elem* aCp)
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
    Elem* pte = GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

const string& Edge::Point2u()
{
    Elem* pte = GetNode("./P2");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

const string& Edge::Pointu(Elem* aCp)
{
    MProp* pt = aCp->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

Elem* Edge::Point1rc()
{
    Elem* res = NULL;
    if (iPoint1 != NULL) {
	res = iPoint1->EBase()->GetObj(res);
    }
    else {
	res = Point1r();
    }
    return res;
}

Elem* Edge::Point2rc()
{
    Elem* res = NULL;
    if (iPoint2 != NULL) {
	res = iPoint2->EBase()->GetObj(res);
    }
    else {
	res = Point2r();
    }
    return res;
}

Elem* Edge::Point1r()
{
    Elem* res = NULL;
    Elem* pte = GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	res = pte->GetNode(uri);
    }
    return res;
}

Elem* Edge::Point2r()
{
    Elem* res = NULL;
    Elem* pte = GetNode("./P2");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	res = pte->GetNode(uri);
    }
    return res;
}

Elem* Edge::Pointr(Elem* aCp)
{
    __ASSERT(aCp == Point1p() || aCp == Point2p());
    Elem* res = NULL;
    const string& uri = Pointu(aCp);
    if (!uri.empty()) {
	res = aCp->GetNode(uri);
    }
    return res;

}

MVert* Edge::Point1v()
{
    MVert* res = NULL;
    Elem* pte = GetNode("./P1");
    const string& uri = Point1u();
    if (!uri.empty()) {
	Elem* pr = pte->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

MVert* Edge::Point2v()
{
    MVert* res = NULL;
    Elem* pte = GetNode("./P2");
    const string& uri = Point2u();
    if (!uri.empty()) {
	Elem* pr = pte->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

MVert* Edge::Ref1() const
{
    MVert* res = NULL;
    Elem* pte = ((Elem*) this)->GetNode("./P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	Elem* pr = pte->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(MLogRec::EErr, pte, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}

MVert* Edge::Ref2() const
{
    MVert* res = NULL;
    Elem* pte = ((Elem*) this)->GetNode("./P2");
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    const string& uri = pt->Value();
    if (!uri.empty()) {
	Elem* pr = pte->GetNode(uri);
	if (pr != NULL) res = pr->GetObj(res);
	if (res == NULL) {
	    Logger()->Write(MLogRec::EErr, pte, "Referencing to [%s] - cannot find or isn't vertex", uri.c_str());
	}
    }
    return res;
}

MVert* Edge::Pointv(Elem* aCp)
{
    __ASSERT(aCp == Point1p() || aCp == Point2p());
    MVert* res = NULL;
    const string& uri = Pointu(aCp);
    if (!uri.empty()) {
	Elem* pr = aCp->GetNode(uri);
	if (pr != NULL) {
	    res = pr->GetObj(res);
	}
    }
    return res;
}

Elem* Edge::Point1p()
{
    return GetNode("./P1");
}

Elem* Edge::Point2p()
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

Elem* Edge::GetNodeLoc(const GUri::TElem& aElem)
{
    Elem* res = NULL;
    // Try hier first
    res = Elem::GetNodeLoc(aElem);
    // Check points then
    Elem* p1 = iPoint1 != NULL ? iPoint1->EBase()->GetObj(p1) : NULL;
    Elem* p2 = iPoint2 != NULL ? iPoint2->EBase()->GetObj(p1) : NULL;
    Elem* pres = NULL;
    if (p1 != NULL && (aElem.second.second == "*" || aElem.second.second != "*" && p1->Name() == aElem.second.second) && 
	    (aElem.first == "*" || aElem.first != "*" && p1->EType() == aElem.first)) {
	pres = p1;
    }
    else if (p2 != NULL && (aElem.second.second == "*" || aElem.second.second != "*" && p2->Name() == aElem.second.second) && 
	    (aElem.first == "*" || aElem.first != "*" && p2->EType() == aElem.first)) {
	pres = p1;
    }
    if (pres != NULL) {
	if (res == NULL) {
	    res = pres;
	}
	else {
	    Logger()->Write(MLogRec::EErr, this,  "URI elem [%s:%s] - resolution conflict", aElem.second.second.c_str(), aElem.first.c_str()); 
	    res = NULL;
	}
    }
    return res;
}
/*
Elem::Iterator Edge::NodesLoc_Begin(const GUri::TElem& aId)
{
    return Iterator(new IterImplEdge(*this, aId));
}

Elem::Iterator Edge::NodesLoc_End(const GUri::TElem& aId)
{
    return Iterator(new IterImplEdge(*this, aId, ETrue));
}
*/

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

