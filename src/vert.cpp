#include "vert.h"
#include "edge.h"

Vert::Vert(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    //iEType = Type();
    SetEType(Type());
    SetParent(Type());
}

void *Vert::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MVert::Type()) == 0) {
	res = (MVert*) this;
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool Vert::Connect(MVert* aPair)
{
    __ASSERT(iPairs.count(aPair) == 0);
    iPairs.insert(aPair);
}

Base* Vert::EBase()
{
    return (Base*) this;
}

set<MVert*>& Vert::Pairs()
{
    return iPairs;
}

void Vert::Disconnect(MVert* aPair)
{
    __ASSERT(iPairs.count(aPair) > 0);
    iPairs.erase(aPair);
}

void Vert::OnCompDeleting(Elem& aComp)
{
    Elem::OnCompDeleting(aComp);
}

void Vert::OnCompAdding(Elem& aComp)
{
}

void Vert::DoOnCompChanged(Elem& aComp)
{
    if (aComp.EType() == "Edge") {
	// Reconnect the edge
	Edge* edge = aComp.GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
	const string& pt1u = edge->Point1u();
	const string& pt2u = edge->Point2u();
	if (!pt1u.empty() && !pt2u.empty()) {
	    Elem* pt1 = GetNode(pt1u);
	    Elem* pt2 = GetNode(pt2u);
	    if (pt1 != NULL && pt2 != NULL) {
		MVert* pt1v = pt1->GetObj(pt1v);
		MVert* pt2v = pt2->GetObj(pt2v);
		if (pt1v != NULL && pt2v != NULL) {
		    edge->SetPoints(pt1v, pt2v);
		    TBool res = edge->Connect();
		    if (res) {
			Logger()->WriteFormat("Graph [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		    else {
			Logger()->WriteFormat("ERR: Graph [%s] connected [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		}
	    }
	    else {
		Logger()->WriteFormat("ERR: Graph [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
		    (pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
	    }
	}
    }
}

