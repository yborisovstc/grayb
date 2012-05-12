#include "vert.h"
#include "edge.h"

Vert::Vert(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    //iEType = Type();
    SetEType(Type());
    SetParent(Type());
}

void *Vert::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MVert::Type()) == 0) {
	res = (MVert*) this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

Vert::~Vert()
{
    // TODO [YB] To add notif of edge about deleting
    iEdges.clear();
    iPairs.clear();
}

TBool Vert::Connect(MVert* aPair)
{
    TBool res = ETrue;
    __ASSERT(iPairs.count(aPair) == 0);
    iPairs.insert(aPair);
    __ASSERT(iMan != NULL);
    iMan->OnCompChanged(*this);
    return res;
}

TBool Vert::Connect(MEdge* aEdge)
{
    TBool res = ETrue;
    Edge* ee = aEdge->EBase()->GetObj(ee);
    __ASSERT(iEdges.find(TCkey(ee->Name(), ee->EType())) == iEdges.end());
    res = Connect(aEdge->Pair(this));
    if (res) {
	iEdges.insert(pair<TCkey, MEdge*>(TCkey(ee->Name(), ee->EType()), aEdge));
//	iMComps.insert(pair<TCkey, Elem*>(TCkey(ee->Name(), "*"), aEdge));
	iEdges.insert(pair<TCkey, MEdge*>(TCkey("*", ee->EType()), aEdge));
    }
    return res;
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

void Vert::Disconnect(MEdge* aEdge)
{
    Edge* ee = aEdge->EBase()->GetObj(ee);
    multimap<TCkey,MEdge*>::iterator found = iEdges.find(TCkey(ee->Name(), ee->EType()));
    __ASSERT(found != iEdges.end());
    Disconnect(aEdge->Pair(this));
    MEdge* edge = (*found).second;
    multimap<TCkey,MEdge*>::iterator fbytypelb = iEdges.lower_bound(TCkey("*", ee->EType()));
    multimap<TCkey,MEdge*>::iterator fbytypeub = iEdges.upper_bound(TCkey("*", ee->EType()));
    for (multimap<TCkey,MEdge*>::iterator it = fbytypelb; it != fbytypeub; it++) {
	if (it->second == edge) {
	    iEdges.erase(it); break;
	}
    }
    iEdges.erase(found);
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
    Elem* eedge = GetCompOwning("Edge", &aComp);
    /*
    if (aComp.EType() == "Edge") {
	eedge = &aComp;
    }
    else if (aComp.GetMan() != this && aComp.GetMan()->EType() == "Edge") {
	eedge = aComp.GetMan();
    }
    */
    //if (aComp.EType() == "Edge") {
    if (eedge != NULL) {
	// Reconnect the edge
	Edge* edge = eedge->GetObj(edge);	
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


Elem* Vert::GetNodeLoc(const GUri::TElem& aElem)
{
    Elem* res = NULL;
    // Try hier first
    res = Elem::GetNodeLoc(aElem);
    // Check edge then
    multimap<TCkey,MEdge*>::iterator found = iEdges.find(TCkey(aElem.second, aElem.first));
    if (found != iEdges.end()) {
	if (res == NULL) {
	    Base* bres = (*found).second->EBase();
	    res = bres->GetObj(res);
	}
	else {
	    Logger()->WriteFormat("ERR: Vert [%s] URI elem [%s:%s] - resolution conflict", Name().c_str(), aElem.second.c_str(), aElem.first.c_str()); 
	    res = NULL;
	}
    }
    return res;
}
