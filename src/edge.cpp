#include "edge.h"
#include "mvert.h"
#include "mprov.h"
#include "mprop.h"

bool Edge::iInit = false;

void Edge::Init()
{
    Elem::Init();
//    iCompsTypes.insert("Prop");
    iInit = true;
}

Edge::Edge(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), iPoint1(NULL), iPoint2(NULL)
{
    if (!iInit) 
	Init();
    SetEType(Type());
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

void Edge::SetPoints(MVert* aPoint1, MVert* aPoint2)
{
    __ASSERT(iPoint1 == NULL && iPoint2 == NULL && aPoint1 != NULL && aPoint2 != NULL);
    iPoint1 = aPoint1;
    iPoint2 = aPoint2;
}

TBool Edge::Connect()
{
    TBool res = EFalse;
    //res = iPoint1->Connect(iPoint2) && iPoint2->Connect(iPoint1);
    res = iPoint1->Connect(this) && iPoint2->Connect(this);
    return res;
}

void Edge::Disconnect()
{
    if (iPoint1 != NULL && iPoint2 != NULL) {
	//iPoint1->Disconnect(iPoint2);
	//iPoint2->Disconnect(iPoint1);
	iPoint1->Disconnect(this);
	iPoint2->Disconnect(this);
	iPoint1 = NULL;
	iPoint2 = NULL;
    }
}

MVert* Edge::Pair(const MVert* aPoint)
{
    return aPoint == iPoint1 ? iPoint2 : iPoint1;
}

const string& Edge::Point1u()
{
    Elem* pte = GetNode("Prop:P1");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
}

const string& Edge::Point2u()
{
    Elem* pte = GetNode("Prop:P2");
    __ASSERT(pte != NULL);
    MProp* pt = pte->GetObj(pt);
    __ASSERT(pt != NULL);
    return pt->Value();
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
    if (p1 != NULL && (aElem.second == "*" || aElem.second != "*" && p1->Name() == aElem.second) && 
	    (aElem.first == "*" || aElem.first != "*" && p1->EType() == aElem.first)) {
	pres = p1;
    }
    else if (p2 != NULL && (aElem.second == "*" || aElem.second != "*" && p2->Name() == aElem.second) && 
	    (aElem.first == "*" || aElem.first != "*" && p2->EType() == aElem.first)) {
	pres = p1;
    }
    if (pres != NULL) {
	if (res == NULL) {
	    res = pres;
	}
	else {
	    Logger()->WriteFormat("ERR: Vert [%s] URI elem [%s:%s] - resolution conflict", Name().c_str(), aElem.second.c_str(), aElem.first.c_str()); 
	    res = NULL;
	}
    }
    return res;
}
