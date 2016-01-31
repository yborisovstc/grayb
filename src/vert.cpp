#include "mprov.h"
#include "edge.h"
#include "vert.h"

// Vertex

string Vert::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Vert::Vert(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
    // Create component for run-time extentions
    Elem* agents = Provider()->CreateNode("Elem", "Agents", this, iEnv);
    __ASSERT(agents != NULL);
    TBool res = AppendComp(agents);
    __ASSERT(res);
}

Vert::Vert(Elem* aMan, MEnv* aEnv):Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
    // Create component for run-time extentions
    //Elem* parent = Provider()->GetNode("Elem");
    Elem* agents = Provider()->CreateNode("Elem", "Agents", this, iEnv);
    /*
    if (parent != NULL) {
	parent->AppendChild(agents);
    }
    */
    __ASSERT(agents != NULL);
    TBool res = AppendComp(agents);
    __ASSERT(res);

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

void Vert::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx;
    ToCacheRCtx(aCtx, rctx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MVert::Type()) == 0) {
	res = (MVert*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str());
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    // Support run-time extentions on the base layer, ref md#sec_refac_iface
    if (res == NULL) {
	MElem* agents = GetComp("Elem", "Agents");
	if (agents != NULL) {
	    for (vector<MElem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && res == NULL; it++) {
		Elem* eit = ToElem(*it);
		if (!ctx.IsInContext(eit)) {
		    rr = eit->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, eit, rr);
		}
	    }
	}
    }
}

Vert::~Vert()
{
    // Solution for UC_015: vertex "locally" (w/o involving upper vertex) disconnects from edges, 
    // not affecting edges chromo but only runtime data. This is aligned with common concept that disables internal mutations.
    // So edge still will be pointing to deleted vertex causing error.
    Disconnect();
}

TBool Vert::Connect(MVert* aPair)
{
    TBool res = ETrue;
//    __ASSERT(aPair != NULL && iPairs.count(aPair) == 0);
    if(aPair != NULL) {
	if(iPairs.count(aPair) == 0) {
	    // Invalidate ifaces cache
	    InvalidateIfCache();
	    iPairs.insert(aPair);
	    // We need to two part connection here. Otherwise CompChanged handling will be incorrect
	    // owner can notify the pair and get back request for iface, so need to have full connection
	    // So request pair connection if not connected yet
	    if(aPair->Pairs().count(this) == 0) {
		aPair->Connect(this);
	    }
	    __ASSERT(iMan != NULL);
	    iMan->OnCompChanged(*this);
	}
	else {
	    // TODO [YB] Seems this happens constantly. To analyze why
	    Base* bp = aPair->EBase();
	    Elem* ep = bp->GetObj(ep);
	    Logger()->Write(MLogRec::EErr, this, "Connecting [%s] - already connected, failed", ep->GetUri().c_str());
	    res = EFalse;
	}
    }
    return res;
}

TBool Vert::Connect(MEdge* aEdge)
{
    TBool res = ETrue;
    Edge* ee = aEdge->EBase()->GetObj(ee);
    __ASSERT(iMEdges.find(TNMKey(ee->Name())) == iMEdges.end());
    if (aEdge->Pair(this) != NULL) {
	res = Connect(aEdge->Pair(this));
    }
    if (res) {
	iMEdges.insert(pair<TNMKey, MEdge*>(TNMKey(ee->Name()), aEdge));
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
    // TODO [YB] To redisign Edge (ref comment in OnCompChanged and restore assert
    // __ASSERT(aPair != NULL && iPairs.count(aPair) > 0);
    if (aPair != NULL && iPairs.count(aPair) > 0) {
	iPairs.erase(aPair);
	// Invalidate ifaces cache
	InvalidateIfCache();
	__ASSERT(iMan != NULL);
	iMan->OnCompChanged(*this);
    }
}

void Vert::Disconnect()
{
    // Cache edges first because disconnecting will affect iMEdges
    vector<MEdge*> edges;
    for (TEdgesMap::iterator it = iMEdges.begin(); it != iMEdges.end(); it++) {
	edges.push_back(it->second);
    }
    // Disconnect edges
    for (vector<MEdge*>::iterator ite = edges.begin(); ite != edges.end(); ite++) {
	MEdge* edge = *ite;
	edge->Disconnect(this);
    }
    // TODO [YB] To add notif of edge about deleting
    iMEdges.clear();
    iPairs.clear();
}

void Vert::Disconnect(MEdge* aEdge)
{
    Edge* ee = aEdge->EBase()->GetObj(ee);
    TEdgesMap::iterator found = iMEdges.find(TNMKey(ee->Name()));
    //__ASSERT(found != iMEdges.end());
    if (found != iMEdges.end()) {
	RemoveFromMap(aEdge, TNMKey(ee->Name()));
	//__ASSERT(aEdge->Pair(this) != NULL);
	if (aEdge->Pair(this) != NULL) {
	    Disconnect(aEdge->Pair(this));
	}
    }
}

void Vert::RemoveFromMap(MEdge* aEdge, const TNMKey& aKey)
{
    __ASSERT(aEdge != NULL);
    TBool found = EFalse;
    TEdgesMap::iterator lb = iMEdges.lower_bound(aKey);
    TEdgesMap::iterator ub = iMEdges.upper_bound(aKey);
    for (TEdgesMap::iterator it = lb; it != ub && !found; it++) {
	if (it->second == aEdge) {
	    iMEdges.erase(it); found = true;
	}
    }
    __ASSERT(found);

}

TBool Vert::OnCompChanged(MElem& aComp)
{
    TBool hres = Elem::OnCompChanged(aComp);
    if (hres) return ETrue;
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
	    if (cp1 != NULL && ref1 != cp1) edge->Disconnect(cp1);
	    if (cp2 != NULL && ref2 != cp2) edge->Disconnect(cp2);
	    cp1 = edge->Point1();
	    cp2 = edge->Point2();
	    TBool res = ETrue;
	    if (cp1 == NULL && ref1 != NULL) res = edge->ConnectP1(ref1);
	    else if (cp2 == NULL && ref2 != NULL) res = edge->ConnectP2(ref2);
	    if (!res) {
		Logger()->Write(MLogRec::EErr, ToElem(&aComp), "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
	    }
	}
    }
    return hres;
}

void Vert::SetRemoved()
{
    Disconnect();
    Elem::SetRemoved();
}


