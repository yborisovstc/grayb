#include "vert.h"
#include "mprov.h"
#include "edge.h"

// Vertex

string Vert::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Vert::Vert(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
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

Vert::Vert(Elem* aMan, MEnv* aEnv):Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
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

void *Vert::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MVert::Type()) == 0) {
	res = (MVert*) this;
    }
    else {
	res = Elem::DoGetObj(aName, EFalse);
    }
    // Support run-time extentions on the base layer, ref md#sec_refac_iface
    if (res == NULL) {
	Elem* agents = GetComp("Elem", "Agents");
	if (agents != NULL) {
	    for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && res == NULL; it++) {
		Elem* eit = (*it);
		if (!ctx.IsInContext(eit)) {
		    res = (*it)->DoGetObj(aName, aIncUpHier, &ctx);
		}
	    }
	}
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
	res = Elem::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    // Support run-time extentions on the base layer, ref md#sec_refac_iface
    if (res == NULL) {
	Elem* agents = GetComp("Elem", "Agents");
	if (agents != NULL) {
	    for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && res == NULL; it++) {
		Elem* eit = (*it);
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

void Vert::SetRemoved()
{
    Disconnect();
    Elem::SetRemoved();
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
	    res = iMan->OnCompChanged(*this);
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

void Vert::OnCompDeleting(Elem& aComp)
{
    Elem::OnCompDeleting(aComp);
}

void Vert::OnCompAdding(Elem& aComp)
{
    Elem::OnCompAdding(aComp);
}


void Vert::DoOnCompChanged(Elem& aComp)
{
    Elem* eedge = GetCompOwning("Edge", &aComp);
    if (eedge != NULL) {
	Edge* edge = eedge->GetObj(edge);	
	TBool res = EFalse;
	if (&aComp == edge->Point1p()) {
	    edge->Disconnect(edge->Point1());
	    if (!edge->Point1u().empty()) {
		MVert* pt1v = edge->Point1v();
		if (pt1v != NULL) {
		    res = edge->ConnectP1(pt1v);
		}
		else {
		    Logger()->Write(MLogRec::EErr, this, "Connecting [%s] - cannot find or not vertex", edge->Point1u().c_str());
		}
	    }
	}
	else if (&aComp == edge->Point2p()) {
	    edge->Disconnect(edge->Point2());
	    MVert* pt2v = edge->Point2v();
	    if (pt2v != NULL) {
		res = edge->ConnectP2(pt2v);
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Connecting [%s] - cannot find or not vertex", edge->Point2u().c_str());
	    }
	}
	if (edge->Point1r() != NULL && edge->Point2r() != NULL) {
	    if (res) {
		Logger()->Write(MLogRec::EInfo, this, "Connected [%s - %s]", edge->Point1u().c_str(), edge->Point2u().c_str());
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Connection [%s - %s] failed", edge->Point1u().c_str(), edge->Point2u().c_str());
	    }
	}
    }
    Elem::DoOnCompChanged(aComp);
}


