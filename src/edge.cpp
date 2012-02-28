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
    iEType = Type();
    // Adding properties "Points"
    Elem* p1 = Provider()->CreateNode("Prop", "P1", this, iEnv);
    Elem* p2 = Provider()->CreateNode("Prop", "P2", this, iEnv);
    __ASSERT(p1 != NULL && p2 != NULL);
    TBool res = AppendComp(p1);
    __ASSERT(res);
    res = AppendComp(p2);
    __ASSERT(res);
}

void* Edge::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
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
    res = iPoint1->Connect(iPoint2) && iPoint2->Connect(iPoint1);
    return res;
}

void Edge::Disconnect()
{
    if (iPoint1 != NULL && iPoint2 != NULL) {
	iPoint1->Disconnect(iPoint2);
	iPoint2->Disconnect(iPoint1);
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

