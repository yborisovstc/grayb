
#include "syst.h"
#include "edge.h"
#include "prov.h"
#include "mvert.h"
#include "mprop.h"

ConnPointBase::ConnPointBase(const string& aName, Elem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *ConnPointBase::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Vert::DoGetObj(aName, EFalse, aCtx);
    }
    if (res == NULL) {
	// Redirect to pairs if iface requiested is provided by this CP
	//Elem* eprov = GetNode("Prop:Provided");
	Elem* eprov = GetNode("Prop:Provided");
	Elem* ereq = GetNode("Prop:Required");
	if (eprov != NULL) {
	    MProp* prov = eprov->GetObj(prov);
	    if (prov != NULL) {
		if (prov->Value() == aName) {
		    // Requested provided iface - cannot be obtain via pairs - redirect to host
		    if (iMan != NULL && !ctx.IsInContext(iMan)) {
			// TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
			Elem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
			res = mgr->DoGetObj(aName, aIncUpHier, &ctx);
		    }
		}
	    }
	}
	if (res == NULL && ereq != NULL) {
	    MProp* req = ereq->GetObj(req);
	    if (req != NULL) {
		if (req->Value() == aName) {
		    for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end() && res == NULL; it++) {
			Elem* pe = (*it)->EBase()->GetObj(pe);
			Elem* peprov = pe != NULL ? pe->GetNode("Prop:Provided"): NULL;
			MProp* pprov = peprov != NULL ? peprov->GetObj(pprov): NULL;
			if (pprov != NULL && pprov->Value() == aName && !ctx.IsInContext(pe)) {
			    res = pe->DoGetObj(aName, aIncUpHier, &ctx);
			}
		    }
		    // Responsible pairs not found, redirect to upper layer
		    if (res == NULL && iMan != NULL && !ctx.IsInContext(iMan)) {
			Elem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
			res = mgr->DoGetObj(aName, aIncUpHier, &ctx);
		    }
		}
	    }
	}
    }
    return res;
}

void ConnPointBase::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resg = EFalse;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx = ToCacheRCtx(aCtx);
    Base* rqstr = aCtx != NULL ? aCtx->Requestor() : NULL;
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Vert::DoGetObj(aName.c_str(), EFalse, aCtx);
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    else {
	// Redirect to pairs if iface requiested is provided by this CP
	Elem* eprov = GetNode("Prop:Provided");
	Elem* ereq = GetNode("Prop:Required");
	if (eprov != NULL) {
	    MProp* prov = eprov->GetObj(prov);
	    if (prov != NULL && prov->Value() == aName) {
		// Requested provided iface - cannot be obtain via pairs - redirect to host
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    // TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		    Elem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		    resg = ETrue;
		}
	    }
	}
	if (!resg && ereq != NULL) {
	    MProp* req = ereq->GetObj(req);
	    if (req != NULL && req->Value() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    Elem* pe = (*it)->EBase()->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, pe, rr);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    Elem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
    }
}


TBool ConnPointBase::IsCompatible(Elem* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    Elem *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = aPair->GetObj(pchkr);
    __ASSERT(pchkr != NULL);
    Elem* ecp = pchkr->GetExtd(); 
    if (ecp != NULL ) {
	ext = !ext;
	cp = ecp;
    }
    if (cp != NULL) {
	// Check roles conformance
	Elem* ept1prov = GetNode("Prop:Provided");
	Elem* ept2req = cp->GetNode("Prop:Required");
	Elem* ept2prov = cp->GetNode("Prop:Provided");
	Elem* ept1req = GetNode("Prop:Required");
	if (ept1prov && ept2req && ept2prov && ept1req) {
	    // Point#1 provided
	    MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
	    MProp* ppt2req = ept2req->GetObj(ppt2req);
	    // Point#2 provided
	    MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
	    MProp* ppt1req = ept1req->GetObj(ppt1req);
	    if (ppt1prov && ppt2req && ppt2prov && ppt1req) {
		if (ext) {
		    res = (ppt1prov->Value() == ppt2prov->Value() && ppt2req->Value() == ppt1req->Value());
		}
		else {
		    res = (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value());
		}
	    }
	    else {
		res = EFalse;
	    }
	}
    }
    return res;
}

Elem* ConnPointBase::GetExtd()
{
    return NULL;
}



ExtenderAgent::ExtenderAgent(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *ExtenderAgent::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName, EFalse);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	Elem* intcp = GetNode("../../*:Int");
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    res = intcp->DoGetObj(aName, ETrue, &ctx);
	}
	else {
	    Elem* host = iMan->GetMan();
	    MVert* vhost = host->GetObj(vhost);
	    if (vhost != NULL) {
		for (set<MVert*>::const_iterator it = vhost->Pairs().begin(); it != vhost->Pairs().end(); it++) {
		    Base* ep = (*it)->EBase();
		    if (ep != NULL && !ctx.IsInContext(ep)) {
			res = ep->DoGetObj(aName, aIncUpHier, &ctx);
		    }
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (res == NULL && iMan != NULL && !ctx.IsInContext(iMan)) {
	Elem* host = iMan->GetMan();
	Elem* hostmgr = host->GetMan();
	Elem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    res = mgr->DoGetObj(aName, aIncUpHier, &ctx);
	}
    }
    return res;
}

void ExtenderAgent::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    Elem* host = iMan->GetMan();
    TICacheRCtx rctx = ToCacheRCtx(aCtx->Ctx());
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	Elem* intcp = GetNode("../../*:Int");
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, aCtx);
	    host->InsertIfCache(aName, rctx, intcp, rr);
	}
	else {
	    Elem* host = iMan->GetMan();
	    MVert* vhost = host->GetObj(vhost);
	    if (vhost != NULL) {
		for (set<MVert*>::const_iterator it = vhost->Pairs().begin(); it != vhost->Pairs().end(); it++) {
		    Elem* ep = (*it)->EBase()->GetObj(ep);
		    if (ep != NULL && !ctx.IsInContext(ep)) {
			rr = ep->GetIfi(aName, aCtx);
			host->InsertIfCache(aName, rctx, ep, rr);
		    }
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	Elem* host = iMan->GetMan();
	Elem* hostmgr = host->GetMan();
	Elem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, aCtx);
	    host->InsertIfCache(aName, rctx, mgr, rr);
	}
    }

}

TBool ExtenderAgent::IsCompatible(Elem* aPair, TBool aExt)
{
    TBool res = EFalse;
    Elem* intcp = GetNode("../../*:Int");
    MCompatChecker* mint = intcp->GetObj(mint);
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

Elem* ExtenderAgent::GetExtd()
{
    return GetNode("../../Int");
}



ASocket::ASocket(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *ASocket::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    // TODO [YB] the current routing model is not optimal. Socket doesn't known itself if
    // it supports iface or not (only pins know). So socket routes to pins first in hope
    // they redirect properly. But what if not? Pin routes back to host, so loop happens, that stops 
    // further routing.
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName, EFalse);
    }
    if (res == NULL && aCtx != NULL) {
	Base* master = aCtx->Requestor();
	Elem* emaster = master->GetObj(emaster);
	Base* rqst = aCtx->Ctx() != NULL ? aCtx->Ctx()->Requestor(): NULL;
	if (rqst != NULL) {
	    Elem* erqst = rqst->GetObj(erqst);
	    TBool iscomp = emaster->IsComp(erqst);
	    if (iscomp) {
		// Request comes from internal CP - forward it to upper layer
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    // TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		    Elem* host = iMan->GetMan();
		    Elem* hostmgr = host->GetMan();
		    Elem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		    if (mgr != NULL && !ctx.IsInContext(mgr)) {
			res = mgr->DoGetObj(aName, aIncUpHier, &ctx);
		    }
		}
	    }
	}
	if (res == NULL) {
	    Elem* man = iMan->GetMan();
	    // Redirect to internal pins. Add host into context, this will prevent internals to redirect
	    // TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
	    for (vector<Elem*>::const_iterator it = man->Comps().begin(); it != man->Comps().end() && res == NULL; it++) {
		Elem* eit = (*it);
		if (!ctx.IsInContext(eit) && eit != iMan) {
		    res = (*it)->DoGetObj(aName, aIncUpHier, &ctx);
		}
	    }
	}
	// Redirect to pair. 
	// TODO [YB] To add checking if requiested iface is supported, ref md "sec_refac_conncomp"
	// TODO [YB] Probably routing to pair needs to be done first, before the routing to pins
	if (res == NULL) {
	    Elem* man = iMan->GetMan();
	    Vert* vman = man->GetObj(vman);
	    for (set<MVert*>::iterator it = vman->Pairs().begin(); it != vman->Pairs().end() && res == NULL; it++) {
		Elem* pe = (*it)->EBase()->GetObj(pe);
		if (!ctx.IsInContext(pe)) {
		    res = pe->DoGetObj(aName, aIncUpHier, &ctx);
		}
	    }
	}
    }
    return res;
}

void ASocket::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resok = EFalse;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx = ToCacheRCtx(aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str(), EFalse);
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    if (res == NULL && aCtx != NULL) {
	Base* master = aCtx->Requestor();
	Elem* emaster = master->GetObj(emaster);
	const RqContext* hostctx = aCtx->Ctx();
	Base* rqst = aCtx->Ctx() != NULL ? aCtx->Ctx()->Requestor(): NULL;
	if (rqst != NULL) {
	    // Requestor is specified, so try to redirect basing on it
	    Elem* erqst = rqst->GetObj(erqst);
	    TBool iscomp = emaster->IsComp(erqst);
	    if (iscomp) {
		// Request comes from internal CP - forward it to upper layer
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    Elem* mgr =  iMan->GetMan()->GetMan();
		    if (mgr != NULL && !ctx.IsInContext(mgr)) {
			rr = mgr->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, mgr, rr);
			resok = ETrue;
		    }
		}
	    }
	    else {
		// Request from not internals
		// Find associated pair in context
		Base* apair = NULL;
		Elem* pcomp = NULL;
		Base* ctxe = rqst;
		const RqContext* cct = aCtx->Ctx();
		Elem* host =  iMan->GetMan();
		TBool isextd = EFalse;
		// TODO [YB] To cleanup
		while (ctxe != NULL && pcomp == NULL) {
		    MCompatChecker* cp = ctxe->GetObj(cp);
		    // Update extention option if met extention in context
		    if (cp != NULL) {
//			isextd ^= cp->GetExtd() != NULL;
			apair = NULL;
			if (cp->IsCompatible(host, isextd)) {
			    isextd ^= ETrue;
			//if (cp->IsCompatible(host)) {
			    Elem* extd = cp->GetExtd();
			    if (extd != host) {
				apair = extd != NULL ? extd : ctxe;
			    }
			}
		    }
		    ctxe = NULL;
		    if (cct != NULL) {
			cct = cct->Ctx();
			ctxe = cct != NULL ? cct->Requestor() : NULL;
		    }
		    if (apair != NULL && ctxe != NULL) {
			// Find associated pairs pin within the context
			Elem* pereq = ctxe->GetObj(pereq);
			GUri uri;
			uri.AppendElem(pereq->EType(), pereq->Name());
			pcomp = host->GetNode(uri);
		    }
		}
		if (pcomp != NULL && !ctx.IsInContext(pcomp)) {
		    // Found associated pairs pin within the context, so redirect to it's pair in current socket
		    rr = pcomp->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, pcomp, rr);
		}
	    }
	    // Redirect to pair. 
	    // TODO [YB] To add checking if requiested iface is supported, ref md "sec_refac_conncomp"
	    // TODO [YB] Probably routing to pair needs to be done first, before the routing to pins
	    if (rr.first == rr.second) {
		Elem* man = iMan->GetMan();
		Vert* vman = man->GetObj(vman);
		for (set<MVert*>::iterator it = vman->Pairs().begin(); it != vman->Pairs().end() && res == NULL; it++) {
		    Elem* pe = (*it)->EBase()->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			//rr = pe->GetIfi(aName, &ctx);
			rr = pe->GetIfi(aName, aCtx);
			InsertIfCache(aName, rctx, pe, rr);
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL) {
		Elem* host = iMan->GetMan();
		Elem* hostmgr = host->GetMan();
		Elem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
	else {
	    // Requestor not specified, anonymous request
	    if (!resok) {
		Elem* man = iMan->GetMan();
		// Redirect to internal pins. Add host into context, this will prevent internals to redirect
		// TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
		for (vector<Elem*>::const_iterator it = man->Comps().begin(); it != man->Comps().end() && res == NULL; it++) {
		    Elem* eit = (*it);
		    if (!ctx.IsInContext(eit) && eit != iMan) {
			rr = eit->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, eit, rr);
		    }
		}
	    }
	    // Redirect to pair. 
	    // TODO [YB] To add checking if requiested iface is supported, ref md "sec_refac_conncomp"
	    // TODO [YB] Probably routing to pair needs to be done first, before the routing to pins
	    if (rr.first == rr.second) {
		Elem* man = iMan->GetMan();
		Vert* vman = man->GetObj(vman);
		for (set<MVert*>::iterator it = vman->Pairs().begin(); it != vman->Pairs().end() && res == NULL; it++) {
		    Elem* pe = (*it)->EBase()->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, pe, rr);
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL && !ctx.IsInContext(iMan)) {
		Elem* host = iMan->GetMan();
		Elem* hostmgr = host->GetMan();
		Elem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
    }
}


TBool ASocket::IsCompatible(Elem* aPair, TBool aExt)
{
    // Going thru non-trivial components and check their compatibility
    // TODO [YB] Needs to clean up this chunk
    TBool res = ETrue;
    TBool ext = aExt;
    Elem *cp = aPair;
    MCompatChecker* pchkr = aPair->GetObj(pchkr);
    __ASSERT(pchkr != NULL);
    Elem* ecp = pchkr->GetExtd(); 
    // Checking if the pair is Extender
    if (ecp != NULL ) {
	ext = !ext;
	cp = ecp;
    }
    if (cp != NULL) {
	Elem* host = iMan->GetMan();
	for (vector<Elem*>::const_iterator it = host->Comps().begin(); it != host->Comps().end() && res; it++) {
	    Elem *comp = (*it);
	    if (comp->Name() != "Agents" && comp->Name() != "Logspec") {
		MCompatChecker* checker = comp->GetObj(checker);
		if (checker != NULL) {
		    GUri uri;
		    uri.AppendElem(comp->EType(), comp->Name());
		    Elem *pcomp = cp->GetNode(uri);
		    if (pcomp != NULL) {
			res = checker->IsCompatible(pcomp, ext);
		    }
		    else {
			res = EFalse;
		    }
		}
	    }
	}
    }
    return res;
}

Elem* ASocket::GetExtd()
{
    return NULL;
}




Syst::Syst(const string& aName, Elem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *Syst::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    /* Removing agents for they are supported on Vert level
       else  {
       Elem* agents = GetComp("Elem", "Agents");
       if (agents != NULL) {
       for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && res == NULL; it++) {
       res = (*it)->DoGetObj(aName, EFalse);
       }
       }
       }
       */
    if (res == NULL) {
	res = Vert::DoGetObj(aName, aIncUpHier, aCtx);
    }
    return res;
}

void Syst::DoOnCompChanged(Elem& aComp)
{
    Elem* eedge = GetCompOwning("Edge", &aComp);
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
		    // Check roles conformance
#if 0
		    // Point#1 provided
		    //Elem* ept1provi = pt1->GetNode("*:Prov");
		    Elem* ept1prov = pt1->GetNode("Prop:Provided");
		    MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
		    Elem* ept2req = pt2->GetNode("Prop:Required");
		    MProp* ppt2req = ept2req->GetObj(ppt2req);
		    // Point#2 provided
		    //Elem* ept2provi = pt2->GetNode("*:Prov");
		    Elem* ept2prov = pt2->GetNode("Prop:Provided");
		    MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
		    Elem* ept1req = pt1->GetNode("Prop:Required");
		    MProp* ppt1req = ept1req->GetObj(ppt1req);
		    if (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value()) {
#endif
			MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
			MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
			if (pt1checker->IsCompatible(pt2) && pt2checker->IsCompatible(pt1)) {
			    // Are compatible - connect
			    edge->SetPoints(pt1v, pt2v);
			    TBool res = edge->Connect();
			    if (res) {
				Logger()->WriteFormat("Syst [%s] connected [%s - %s]", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			    else {
				Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] failed", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			    }
			}
			else {
			    Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - incompatible roles", Name().c_str(), pt1u.c_str(), pt2u.c_str());
			}
		    }
		    else {
			Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - ends aren't vertexes", Name().c_str(), pt1u.c_str(), pt2u.c_str());
		    }
		}
		else {
		    Logger()->WriteFormat("ERR: Syst [%s] connecting [%s - %s] - cannot find [%s]", Name().c_str(), pt1u.c_str(), pt2u.c_str(), 
			    (pt1 == NULL)? pt1u.c_str(): pt2u.c_str());
		}
	    }
	}
    }

