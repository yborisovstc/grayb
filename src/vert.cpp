#include "mprov.h"
#include "edge.h"
#include "vert.h"
#include <stdexcept> 

// Vertex

MVert::EIfu MVert::mIfu;

// Ifu static initialisation
MVert::EIfu::EIfu()
{
    RegMethod("Connect", 1);
    RegMethod("MVert_DoGetObj", 1);
    RegMethod("PairsCount", 0);
    RegMethod("IsPair", 1);
    RegMethod("GetPair", 1);
}

string Vert::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Vert::Vert(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
    // Create component for run-time extentions
    Elem* agents = Provider()->CreateNode("Elem", "Agents", this, iEnv);
    __ASSERT(agents != NULL);
    // TODO [YB] Early construction in this place creates the problem:
    // AppendComp sends notificaion of comps added thru owning chain
    // But this instance of Vert is not registered by owner yet, so
    // if observer tries to get comp added then null will be returned
    // To consider the standard approach of deferred construction.
    TBool res = AppendComp(agents, ETrue);
    __ASSERT(res);
}

Vert::Vert(MElem* aMan, MEnv* aEnv):Elem(Type(), aMan, aEnv)
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
    TBool res = AppendComp(agents, ETrue);
    __ASSERT(res);

}

MIface* Vert::MVert_DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MElem::Type()) == 0) {
	res = dynamic_cast<MElem*>(this);
    }
    else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    }
    return res;
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
    // Added looing at the embedded agents also, ref ds_mi_rm
    if (res == NULL) {
	MElem* agents = GetComp("Elem", "Agents");
	if (agents != NULL) {
	    for (TInt ci = 0; ci < agents->CompsCount() && res == NULL; ci++) {
		MElem* eit = agents->GetComp(ci);
		res = eit->DoGetObj(aName);
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
    TICacheRCtx rctx(aCtx);
    //ToCacheRCtx(aCtx, rctx);
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
	    for (TInt ci = 0; ci < agents->CompsCount() && res == NULL; ci++) {
		MElem* eit = agents->GetComp(ci);
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
    iPairs.clear();
}

TBool Vert::Connect(MVert* aPair)
{
    TBool res = ETrue;
//    __ASSERT(aPair != NULL && iPairs.count(aPair) == 0);
    if(aPair != NULL) {
	if(!IsPair(aPair)) {
	    // Invalidate ifaces cache
	    InvalidateIfCache();
	    iPairs.insert(aPair);
	    // We need to two part connection here. Otherwise CompChanged handling will be incorrect
	    // owner can notify the pair and get back request for iface, so need to have full connection
	    // So request pair connection if not connected yet
	    if(!aPair->IsPair(this)) {
		aPair->Connect(this);
	    }
	    __ASSERT(iMan != NULL);
	    iMan->OnChanged(*this);
	}
	else {
	    // TODO [YB] Seems this happens constantly. To analyze why
	    MElem* ep = aPair->GetObj(ep);
	    Logger()->Write(EErr, this, "Connecting [%s] - already connected, failed", ep->GetUri().c_str());
	    res = EFalse;
	}
    }
    return res;
}

TInt Vert::PairsCount() const
{
    return iPairs.size();
}

MVert* Vert::GetPair(TInt aInd) const
{
    MVert* res = NULL;
    if (aInd < PairsCount()) {
	set<MVert*>::iterator it = iPairs.begin(); 
	for (; it != iPairs.end() && aInd != 0; it++, aInd--) {
	}
	res = *it;
    }
    return res;
}

TBool Vert::IsPair(const MVert* aPair) const
{
    return iPairs.count((MVert*) aPair) > 0;
    /*
       TBool res = EFalse;
       for (vector<MVert*>::const_iterator it = iPairs.begin(); it != iPairs.end() && !res; it++) {
       const MVert* pair = *it;
       res = (pair == aPair);
       }
       return res;
       */ 
}

/*
set<MVert*>& Vert::Pairs()
{
    return iPairs;
}
*/

void Vert::Disconnect(MVert* aPair)
{
    // TODO [YB] To redisign Edge (ref comment in OnCompChanged and restore assert
    // __ASSERT(aPair != NULL && iPairs.count(aPair) > 0);
    if (aPair != NULL && iPairs.count(aPair) > 0) {
	iPairs.erase(aPair);
	// Invalidate ifaces cache
	InvalidateIfCache();
	__ASSERT(iMan != NULL);
	iMan->OnChanged(*this);
    }
}

TBool Vert::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool hres = Elem::OnCompChanged(aComp, aContName, aModif);
    if (!hres) return EFalse;
    MEdge* edge = aComp.GetObj(edge);	
    if (edge != NULL) {
	hres = ETrue;
	MVert* ref1 = edge->Ref1();
	MVert* ref2 = edge->Ref2();
	MVert* cp1 = edge->Point1();
	MVert* cp2 = edge->Point2();
	if (cp1 != ref1 || cp2 != ref2) {
	    if (cp1 != NULL && ref1 != cp1) edge->Disconnect(cp1);
	    if (cp2 != NULL && ref2 != cp2) edge->Disconnect(cp2);
	    cp1 = edge->Point1();
	    cp2 = edge->Point2();
	    TBool res = ETrue;
	    if (cp1 == NULL && ref1 != NULL) {
		res = edge->ConnectP1(ref1);
	    } else if (cp2 == NULL && ref2 != NULL) {
		res = edge->ConnectP2(ref2);
	    }
	    if (!res) {
		MElem* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
		MElem* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
		Logger()->Write(EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
	    }
	}
    }
    return hres;
}

MIface* Vert::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MVert::mIfu.CheckMname(name);
    if (!name_ok) {
	return Elem::Call(aSpec, aRes);
    }
    TBool args_ok = MVert::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Connect") {
	MElem* pair = GetNode(args.at(0));
	if (pair == NULL) {
	    Logger()->Write(EErr, this, "Connecting [%s] - cannot get pair, failed", args.at(0).c_str());
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVert* vpair = pair->GetObj(vpair);
	if (vpair == NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = Connect(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "MVert_DoGetObj") {
	res = MVert_DoGetObj(args.at(0).c_str());
    } else if (name == "PairsCount") {
	TInt pc = PairsCount();
	aRes = Ifu::FromInt(pc);
    } else if (name == "IsPair") {
	MElem* earg = GetNode(args.at(0));
	if (earg == NULL) {
	    throw (runtime_error("Cannot get arg: " + args.at(0)));
	}
	MVert* varg = earg->GetObj(varg);
	if (varg == NULL) {
	    throw (runtime_error("Arg isn't vertex: " + args.at(0)));
	}
	TBool rr = IsPair(varg);
	aRes = Ifu::FromBool(rr);
    } else if (name == "GetPair") {
	TInt ind = Ifu::ToInt(args.at(0));
	res = GetPair(ind);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

string Vert::Mid() const
{
    return Elem::Mid();
}

void Vert::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    // Disconnect the binding edges if the comp is vert connected
    MVert* vert = aComp.GetObj(vert);
    if (vert != NULL && vert->PairsCount() > 0) {
	for (vector<MElem*>::iterator it = iComps.begin(); it != iComps.end(); it++) {
	    MElem* comp = *it;
	    MEdge* edge = comp->GetObj(edge);
	    if (edge != NULL && (edge->Point1() == vert || edge->Point2() == vert)) {
		edge->Disconnect(vert);
	    }
	}
    }
    Elem::OnCompDeleting(aComp, aSoft, aModif);
}

void Vert::Disconnect()
{
    while (PairsCount() > 0) {
	set<MVert*>::iterator it = iPairs.begin();
	Disconnect(*it);
    }
}
