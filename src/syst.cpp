
#include "syst.h"
#include "edge.h"
#include "prov.h"
#include "mvert.h"
#include "mchromo.h"
#include "mprop.h"

string ConnPointBase::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBase::ConnPointBase(const string& aName, Elem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetEType(Type(), Vert::PEType());
    SetParent(Type());
}

ConnPointBase::ConnPointBase(Elem* aMan, MEnv* aEnv): Vert(Type(), aMan, aEnv)
{
    SetEType(Vert::PEType());
    SetParent(Vert::PEType());
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
	Elem* eprov = GetNode("./(Prop:)Provided");
	Elem* ereq = GetNode("./(Prop:)Required");
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
			Elem* peprov = pe != NULL ? pe->GetNode("./(Prop:)Provided"): NULL;
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
    TICacheRCtx rctx;
    ToCacheRCtx(aCtx, rctx);
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
	Elem* eprov = GetNode("./(Prop:)Provided");
	Elem* ereq = GetNode("./(Prop:)Required");
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
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	Elem* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    Elem* ept1prov = GetNode("./(Prop:)Provided");
	    Elem* ept2req = cp->GetNode("./(Prop:)Required");
	    Elem* ept2prov = cp->GetNode("./(Prop:)Provided");
	    Elem* ept1req = GetNode("./(Prop:)Required");
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
    }
    return res;
}

Elem* ConnPointBase::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir ConnPointBase::GetDir() const
{
    return ERegular;
}

void ConnPointBase::GetMajorIdep(TMDep& aDep)
{
    Elem* eprov = GetNode("./(Prop:)Provided");
    Elem* ereq = GetNode("./(Prop:)Required");
    eprov->GetMajorDep(aDep, ENt_Change, MChromo::EDp_Direct, MChromo::EDl_Critical);
    ereq->GetMajorDep(aDep, ENt_Change, MChromo::EDp_Direct, MChromo::EDl_Critical);
}


// Input ConnPoint base
string ConnPointBaseInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBaseInp::ConnPointBaseInp(const string& aName, Elem* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetEType(Type(), ConnPointBase::PEType());
    SetParent(Type());
}

ConnPointBaseInp::ConnPointBaseInp(Elem* aMan, MEnv* aEnv): ConnPointBase(Type(), aMan, aEnv)
{
    SetEType(ConnPointBase::PEType());
    SetParent(ConnPointBase::PEType());
}

void *ConnPointBaseInp::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ConnPointBase::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ConnPointBaseInp::GetDir() const
{
    return EInp;
}


// Output ConnPoint base
string ConnPointBaseOut::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBaseOut::ConnPointBaseOut(const string& aName, Elem* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetEType(Type(), ConnPointBase::PEType());
    SetParent(Type());
}

ConnPointBaseOut::ConnPointBaseOut(Elem* aMan, MEnv* aEnv): ConnPointBase(Type(), aMan, aEnv)
{
    SetEType(ConnPointBase::PEType());
    SetParent(ConnPointBase::PEType());
}

void *ConnPointBaseOut::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ConnPointBase::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ConnPointBaseOut::GetDir() const
{
    return EOut;
}




string ExtenderAgent::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ExtenderAgent::ExtenderAgent(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

ExtenderAgent::ExtenderAgent(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
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
	Elem* intcp = GetNode("./../../Int");
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
    TICacheRCtx rctx;
    ToCacheRCtx(aCtx, rctx);
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
	Elem* intcp = GetNode("./../../Int");
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, &ctx);
	    InsertIfCache(aName, rctx, intcp, rr);
	}
	else {
	    Elem* host = iMan->GetMan();
	    MVert* vhost = host->GetObj(vhost);
	    if (vhost != NULL) {
		for (set<MVert*>::const_iterator it = vhost->Pairs().begin(); it != vhost->Pairs().end(); it++) {
		    Elem* ep = (*it)->EBase()->GetObj(ep);
		    if (ep != NULL && !ctx.IsInContext(ep)) {
			rr = ep->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, ep, rr);
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
	    rr = mgr->GetIfi(aName, &ctx);
	    InsertIfCache(aName, rctx, mgr, rr);
	}
    }

}

TBool ExtenderAgent::IsCompatible(Elem* aPair, TBool aExt)
{
    TBool res = EFalse;
    Elem* intcp = GetNode("./../../Int");
    MCompatChecker* mint = intcp->GetObj(mint);
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

Elem* ExtenderAgent::GetExtd()
{
    return GetNode("./../../Int");
}

MCompatChecker::TDir ExtenderAgent::GetDir() const
{
    return ERegular;
}

void ExtenderAgent::GetMajorIdep(TMDep& aDep)
{
    Elem* extd = GetExtd();
    MCompatChecker* conn = extd->GetObj(conn);
    conn->GetMajorIdep(aDep);
}


// Input Extender Agent
string ExtenderAgentInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ExtenderAgentInp::ExtenderAgentInp(const string& aName, Elem* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetEType(Type(), ExtenderAgent::PEType());
    SetParent(Type());
}

ExtenderAgentInp::ExtenderAgentInp(Elem* aMan, MEnv* aEnv): ExtenderAgent(Type(), aMan, aEnv)
{
    SetEType(ExtenderAgent::PEType());
    SetParent(ExtenderAgent::PEType());
}

void *ExtenderAgentInp::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ExtenderAgent::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ExtenderAgentInp::GetDir() const
{
    return EInp;
}


// Output Extender Agent
string ExtenderAgentOut::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ExtenderAgentOut::ExtenderAgentOut(const string& aName, Elem* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetEType(Type(), ExtenderAgent::PEType());
    SetParent(Type());
}

ExtenderAgentOut::ExtenderAgentOut(Elem* aMan, MEnv* aEnv): ExtenderAgent(Type(), aMan, aEnv)
{
    SetEType(ExtenderAgent::PEType());
    SetParent(ExtenderAgent::PEType());
}

void *ExtenderAgentOut::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ExtenderAgent::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ExtenderAgentOut::GetDir() const
{
    return EOut;
}



// Socket agent: redirects iface requests to pins
string ASocket::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ASocket::ASocket(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

ASocket::ASocket(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
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
    /*
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
    */
    return res;
}

void ASocket::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resok = EFalse;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx;
    ToCacheRCtx(aCtx, rctx);
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
			apair = NULL;
			if (cp->IsCompatible(host, isextd)) {
			    isextd ^= ETrue;
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
			ASocket* psock = apair->GetObj(psock);
			if (psock != NULL) {
			    Elem* pereq = psock->GetPin(cct);
			    if (pereq != NULL) {
				GUri uri;
				// Using only name as signature of socket pin. This is because even the compatible types can differ
				uri.AppendElem("*", pereq->Name());
				pcomp = host->GetNode(uri);
			    }
			}
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
			rr = pe->GetIfi(aName, &ctx);
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
    if (pchkr != NULL) {
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
		//	uri.AppendElem(comp->EType(), comp->Name());
			uri.AppendElem("*", comp->Name());
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
    }
    else {
	res = EFalse;
    }
    return res;
}

Elem* ASocket::GetExtd()
{
    return NULL;
}

Elem* ASocket::GetPin(const RqContext* aCtx)
{
    Elem* res = NULL;
    Elem* host = iMan->GetMan();
    for (vector<Elem*>::const_iterator it = host->Comps().begin(); it != host->Comps().end() && res == NULL; it++) {
	Elem *comp = (*it);
	if (comp->Name() != "Agents" && comp->Name() != "Logspec") {
	    if (aCtx->IsInContext(comp)) {
		res = comp;
	    }
	}
    }
    return res;
}

MCompatChecker::TDir ASocket::GetDir() const
{
    return ERegular;
}

void ASocket::GetMajorIdep(TMDep& aDep)
{
}


// Input Socket Agent
string ASocketInp::PEType()
{
    return ASocket::PEType() + GUri::KParentSep + Type();
}

ASocketInp::ASocketInp(const string& aName, Elem* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetEType(Type(), ASocket::PEType());
    SetParent(Type());
}

ASocketInp::ASocketInp(Elem* aMan, MEnv* aEnv): ASocket(Type(), aMan, aEnv)
{
    SetEType(ASocket::PEType());
    SetParent(ASocket::PEType());
}

void *ASocketInp::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ASocket::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ASocketInp::GetDir() const
{
    return EInp;
}


// Output Socket Agent
string ASocketOut::PEType()
{
    return ASocket::PEType() + GUri::KParentSep + Type();
}

ASocketOut::ASocketOut(const string& aName, Elem* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetEType(Type(), ASocket::PEType());
    SetParent(Type());
}

ASocketOut::ASocketOut(Elem* aMan, MEnv* aEnv): ASocket(Type(), aMan, aEnv)
{
    SetEType(ASocket::PEType());
    SetParent(ASocket::PEType());
}

void *ASocketOut::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = ASocket::DoGetObj(aName, EFalse, aCtx);
    }
    return res;
}

MCompatChecker::TDir ASocketOut::GetDir() const
{
    return EOut;
}




// System
string Syst::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

Syst::Syst(const string& aName, Elem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetEType(Type(), Vert::PEType());
    SetParent(Type());
}

Syst::Syst(Elem* aMan, MEnv* aEnv): Vert(Type(), aMan, aEnv)
{
    SetEType(Vert::PEType());
    SetParent(Vert::PEType());
}

void *Syst::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    if (res == NULL) {
	res = Vert::DoGetObj(aName, aIncUpHier, aCtx);
    }
    return res;
}

void Syst::OnCompDeleting(Elem& aComp)
{
    Elem* eedge = GetCompOwning("Edge", &aComp);
    if (eedge != NULL) {
	// Reconnect the edge
	Edge* edge = eedge->GetObj(edge);	
	__ASSERT(edge != NULL);
	edge->Disconnect();
    }
    Vert::OnCompDeleting(aComp);
}

void Syst::DoOnCompChanged(Elem& aComp, const string& aContName)
{
    Elem* eedge = GetCompOwning("Edge", &aComp);
    if (eedge != NULL) {
	Edge* edge = eedge->GetObj(edge);	
	__ASSERT(edge != NULL);
	if (&aComp == edge->Point1p() || &aComp == edge->Point2p()) {
	    edge->Disconnect(&aComp);
	    if (!edge->Pointu(&aComp).empty()) {
		TBool res = EFalse;
		if (edge->Pointv(&aComp) != NULL) {
		    if (edge->Point1() == NULL && edge->Point2() == NULL) {
			// Partial connection, compat checking isn't needed
			res = edge->Connect(&aComp);
			if (!res) {
			    Logger()->Write(MLogRec::EErr, this, "Connecting [%s] failed", edge->Pointu(&aComp).c_str());
			}
		    }
		    else {
			// Full connection, compat checking is needed
			Elem* pt1 = edge->Point1rc();
			Elem* pt2 = edge->Point2rc();
			if (pt1 != NULL && pt2 != NULL) {
			    string pt1u = edge->Point1u();
			    string pt2u = edge->Point2u();
			    MVert* pt1v = pt1->GetObj(pt1v);
			    MVert* pt2v = pt2->GetObj(pt2v);
			    if (pt1v != NULL && pt2v != NULL) {
				// Check roles conformance
				MCompatChecker* pt1checker = pt1->GetObj(pt1checker);
				MCompatChecker* pt2checker = pt2->GetObj(pt2checker);
				TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
				TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
				if (ispt1cptb && ispt2cptb) {
				    // Are compatible - connect
				    res = edge->Connect(&aComp);
				    if (res) {
					//Logger()->Write(MLogRec::EInfo, this, "Connected [%s - %s]", pt1u.c_str(), pt2u.c_str());
				    }
				    else {
					Logger()->Write(MLogRec::EErr, this, "Connecting [%s - %s] failed", pt1u.c_str(), pt2u.c_str());
				    }
				}
				else {
				    Logger()->Write(MLogRec::EErr, this, "Connecting [%s - %s] - incompatible roles", pt1u.c_str(), pt2u.c_str());
				}
			    }
			    else {
				Logger()->Write(MLogRec::EErr, this, "Connecting [%s - %s] - ends aren't vertexes", pt1u.c_str(), pt2u.c_str());
			    }
			}
		    }
		}
		else {
		    Logger()->Write(MLogRec::EErr, this, "Connecting [%s] - cannot find or not vertex", edge->Pointu(&aComp).c_str());
		}
	    }
	}
    }
    else {
	Vert::DoOnCompChanged(aComp, aContName);
    }
}

void Syst::GetImplicitDep(TMDep& aDep, Elem* aObj, Elem* aRef)
{
    Elem* eedge = GetCompOwning("Edge", aObj);
    if (eedge != NULL) {
	MCompatChecker* conn = aRef->GetObj(conn);
	if (conn != NULL) {
	    conn->GetMajorIdep(aDep);
	}
    }
}
