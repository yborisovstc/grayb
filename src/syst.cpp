
#include "syst.h"
#include "edge.h"
#include "prov.h"
#include "mvert.h"
#include "mchromo.h"
#include "mprop.h"
#include <stdexcept> 

// Caplule

string ACapsule::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ACapsule::ACapsule(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

ACapsule::ACapsule(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *ACapsule::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) res = this;
    else res = Elem::DoGetObj(aName);
    return res;
}

TBool ACapsule::OnCompChanged(MElem& aComp)
{
    Elem::OnCompChanged(aComp);
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnCompChanged(aComp);
    }
    return ETrue;
}

// Base of ConnPoint reimplement obj provider iface to redirect the request to the hier mgr

MCompatChecker::EIfu MCompatChecker::mIfu;

// Ifu static initialisation
MCompatChecker::EIfu::EIfu()
{
    RegMethod("IsCompatible", 2);
    RegMethod("GetExtd", 0);
    RegMethod("GetDir", 0);
}


string ConnPointBase::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBase::ConnPointBase(const string& aName, MElem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetParent(Type());
}

ConnPointBase::ConnPointBase(MElem* aMan, MEnv* aEnv): Vert(Type(), aMan, aEnv)
{
    SetParent(Vert::PEType());
}

void *ConnPointBase::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void ConnPointBase::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resg = EFalse;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx(aCtx);
    //ToCacheRCtx(aCtx, rctx);
    Base* rqstr = aCtx != NULL ? aCtx->Requestor() : NULL;
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Vert::DoGetObj(aName.c_str());
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    else {
	// Redirect to pairs if iface requiested is provided by this CP
	MElem* eprov = GetNode("./(Prop:)Provided");
	MElem* ereq = GetNode("./(Prop:)Required");
	if (eprov != NULL) {
	    MProp* prov = eprov->GetObj(prov);
	    if (prov != NULL && prov->Value() == aName) {
		// Requested provided iface - cannot be obtain via pairs - redirect to host
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    // TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		    MElem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
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
		    MElem* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, &ctx);
			InsertIfCache(aName, rctx, pe, rr);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MElem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
    }
}


TBool ConnPointBase::IsCompatible(MElem* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MElem *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MElem* ecp = pchkr->GetExtd(); 
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    // Check roles conformance
	    MElem* ept1prov = GetNode("./(Prop:)Provided");
	    MElem* ept2req = cp->GetNode("./(Prop:)Required");
	    MElem* ept2prov = cp->GetNode("./(Prop:)Provided");
	    MElem* ept1req = GetNode("./(Prop:)Required");
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

MElem* ConnPointBase::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir ConnPointBase::GetDir() const
{
    return ERegular;
}

MIface* ConnPointBase::MCompatChecker_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MCompatChecker::mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = MCompatChecker::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "GetDir") {
	TInt rr = GetDir();
	aRes = Ifu::FromInt(rr);
    } else if (name == "GetExtd") {
	res = GetExtd();
    } else if (name == "IsCompatible") {
	MElem* pair = GetNode(args.at(0), ETrue);
	if (pair == NULL) {
	    throw (runtime_error("Cannot find pair: " + args.at(0)));
	}
	TBool ext = Ifu::ToBool(args.at(1));
	TBool rr = IsCompatible(pair, ext);
	aRes = Ifu::FromBool(rr);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ConnPointBase::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}



// Input ConnPoint base
string ConnPointBaseInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBaseInp::ConnPointBaseInp(const string& aName, MElem* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetParent(Type());
}

ConnPointBaseInp::ConnPointBaseInp(MElem* aMan, MEnv* aEnv): ConnPointBase(Type(), aMan, aEnv)
{
    SetParent(ConnPointBase::PEType());
}

void *ConnPointBaseInp::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ConnPointBase::DoGetObj(aName);
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

ConnPointBaseOut::ConnPointBaseOut(const string& aName, MElem* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetParent(Type());
}

ConnPointBaseOut::ConnPointBaseOut(MElem* aMan, MEnv* aEnv): ConnPointBase(Type(), aMan, aEnv)
{
    SetParent(ConnPointBase::PEType());
}

void *ConnPointBaseOut::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ConnPointBase::DoGetObj(aName);
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

ExtenderAgent::ExtenderAgent(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

ExtenderAgent::ExtenderAgent(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *ExtenderAgent::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void ExtenderAgent::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    RqContext ctx(this, aCtx);
    MElem* host = iMan->GetMan();
    TICacheRCtx rctx(aCtx);
    //ToCacheRCtx(aCtx, rctx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str());
    }
    if (res != NULL) {
	InsertIfCache(aName, rctx, this, res);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	Elem* intcp = GetNodeE("./../../Int");
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, &ctx);
	    InsertIfCache(aName, rctx, intcp, rr);
	}
	else {
	    MElem* host = iMan->GetMan();
	    MVert* vhost = host->GetObj(vhost);
	    if (vhost != NULL) {
		TInt count = vhost->PairsCount();
		for (TInt ct = 0; ct < count; ct++) {
		    MVert* pair = vhost->GetPair(ct);
		    Elem* ep = pair->GetObj(ep);
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
	Elem* host = ToElem(iMan->GetMan());
	Elem* hostmgr = ToElem(host->GetMan());
	Elem* mgr = hostmgr->Name() == "Capsule" ? ToElem(hostmgr->GetMan()) : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, &ctx);
	    InsertIfCache(aName, rctx, mgr, rr);
	}
    }

}

TBool ExtenderAgent::IsCompatible(MElem* aPair, TBool aExt)
{
    TBool res = EFalse;
    Elem* intcp = GetNodeE("./../../Int");
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MElem* ExtenderAgent::GetExtd()
{
    return GetNodeE("./../../Int");
}

MCompatChecker::TDir ExtenderAgent::GetDir() const
{
    return ERegular;
}

MIface* ExtenderAgent::Call(const string& aSpec, string& aRes)
{
    __ASSERT(EFalse);
    return  NULL;
}

string ExtenderAgent::Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}



// Input Extender Agent
string ExtenderAgentInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ExtenderAgentInp::ExtenderAgentInp(const string& aName, MElem* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetParent(Type());
}

ExtenderAgentInp::ExtenderAgentInp(MElem* aMan, MEnv* aEnv): ExtenderAgent(Type(), aMan, aEnv)
{
    SetParent(ExtenderAgent::PEType());
}

void *ExtenderAgentInp::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ExtenderAgent::DoGetObj(aName);
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

ExtenderAgentOut::ExtenderAgentOut(const string& aName, MElem* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetParent(Type());
}

ExtenderAgentOut::ExtenderAgentOut(MElem* aMan, MEnv* aEnv): ExtenderAgent(Type(), aMan, aEnv)
{
    SetParent(ExtenderAgent::PEType());
}

void *ExtenderAgentOut::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ExtenderAgent::DoGetObj(aName);
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

ASocket::ASocket(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

ASocket::ASocket(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

void *ASocket::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void ASocket::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    // TODO [YB] the current routing model is not optimal. Socket doesn't known itself if
    // it supports iface or not (only pins know). So socket routes to pins first in hope
    // they redirect properly. But what if not? Pin routes back to host, so loop happens, that stops 
    // further routing.
    void* res = NULL;
    TIfRange rr;
    TBool resok = EFalse;
    RqContext ctx(this, aCtx);
    TICacheRCtx rctx(aCtx);
    //ToCacheRCtx(aCtx, rctx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    }
    else {
	res = Elem::DoGetObj(aName.c_str());
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
		    Elem* mgr =  ToElem(iMan->GetMan()->GetMan());
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
		Elem* host =  ToElem(iMan->GetMan());
		TBool isextd = EFalse;
		// TODO [YB] To cleanup
		while (ctxe != NULL && pcomp == NULL) {
		    MCompatChecker* cp = ctxe->GetObj(cp);
		    // Update extention option if met extention in context
		    if (cp != NULL) {
			apair = NULL;
			if (cp->IsCompatible(host, isextd)) {
			    isextd ^= ETrue;
			    MElem* extd = cp->GetExtd();
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
				pcomp = host->GetNodeE(uri);
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
		Elem* man = ToElem(iMan->GetMan());
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			Elem* pe = pair->GetObj(pe);
			if (!ctx.IsInContext(pe)) {
			    rr = pe->GetIfi(aName, &ctx);
			    InsertIfCache(aName, rctx, pe, rr);
			}
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL) {
		Elem* host = ToElem(iMan->GetMan());
		Elem* hostmgr = ToElem(host->GetMan());
		Elem* mgr = hostmgr->Name() == "Capsule" ? ToElem(hostmgr->GetMan()) : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
	else {
	    // Requestor not specified, anonymous request
	    if (!resok) {
		Elem* man = ToElem(iMan->GetMan());
		// Redirect to internal pins. Add host into context, this will prevent internals to redirect
		// TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
		for (TInt ci = 0; ci < man->CompsCount() && res == NULL; ci++) {
		    MElem* eit = man->GetComp(ci);
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
		Elem* man = ToElem(iMan->GetMan());
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			Elem* pe = pair->GetObj(pe);
			if (!ctx.IsInContext(pe)) {
			    rr = pe->GetIfi(aName, &ctx);
			    InsertIfCache(aName, rctx, pe, rr);
			}
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL && !ctx.IsInContext(iMan)) {
		Elem* host = ToElem(iMan->GetMan());
		Elem* hostmgr = ToElem(host->GetMan());
		Elem* mgr = hostmgr->Name() == "Capsule" ? ToElem(hostmgr->GetMan()) : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, &ctx);
		    InsertIfCache(aName, rctx, mgr, rr);
		}
	    }
	}
    }
}


TBool ASocket::IsCompatible(MElem* aPair, TBool aExt)
{
    // Going thru non-trivial components and check their compatibility
    // TODO [YB] Needs to clean up this chunk
    TBool res = ETrue;
    TBool ext = aExt;
    MElem *cp = aPair;
    // Requesing anonymously because can be returned to itself vie extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type());
    if (pchkr != NULL) {
	MElem* ecp = pchkr->GetExtd(); 
	// Checking if the pair is Extender
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    Elem* host = ToElem(iMan->GetMan());
	    for (TInt ci = 0; ci < host->CompsCount() && res; ci++) {
		MElem *comp = host->GetComp(ci);
		if (comp->Name() != "Agents" && comp->Name() != "Logspec") {
		    MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), this);
		    if (checker != NULL) {
			GUri uri;
		//	uri.AppendElem(comp->EType(), comp->Name());
			uri.AppendElem("*", comp->Name());
			MElem *pcomp = cp->GetNode(uri);
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

MElem* ASocket::GetExtd()
{
    return NULL;
}

Elem* ASocket::GetPin(const RqContext* aCtx)
{
    Elem* res = NULL;
    Elem* host = ToElem(iMan->GetMan());
    for (TInt ci = 0; ci < host->CompsCount() && res == NULL; ci++) {
	MElem *mcomp = host->GetComp(ci);
	Elem* comp = ToElem(mcomp);
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

MIface* ASocket::Call(const string& aSpec, string& aRes)
{
    __ASSERT(EFalse);
    return  NULL;
}

string ASocket::Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}


// Input Socket Agent
string ASocketInp::PEType()
{
    return ASocket::PEType() + GUri::KParentSep + Type();
}

ASocketInp::ASocketInp(const string& aName, MElem* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetParent(Type());
}

ASocketInp::ASocketInp(MElem* aMan, MEnv* aEnv): ASocket(Type(), aMan, aEnv)
{
    SetParent(ASocket::PEType());
}

void *ASocketInp::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ASocket::DoGetObj(aName);
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

ASocketOut::ASocketOut(const string& aName, MElem* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetParent(Type());
}

ASocketOut::ASocketOut(MElem* aMan, MEnv* aEnv): ASocket(Type(), aMan, aEnv)
{
    SetParent(ASocket::PEType());
}

void *ASocketOut::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = ASocket::DoGetObj(aName);
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

Syst::Syst(const string& aName, MElem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetParent(Type());
}

Syst::Syst(MElem* aMan, MEnv* aEnv): Vert(Type(), aMan, aEnv)
{
    SetParent(Vert::PEType());
}

void *Syst::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    if (res == NULL) {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void Syst::OnCompDeleting(MElem& aComp, TBool aSoft)
{
    MElem* eedge = GetCompOwning("Edge", &aComp);
    if (eedge != NULL) {
	// Reconnect the edge
	MEdge* edge = eedge->GetObj(edge);
	// TODO [YB] For comp hard removing the edge destructor called first then the notif issued
	// So iface reolver doesn't work. To consider some solution.
	if (edge != NULL) {
	    edge->Disconnect();
	}
    }
    Vert::OnCompDeleting(aComp, aSoft);
}


TBool Syst::IsPtOk(MElem* aPt) {
    return ETrue;
}

TBool Syst::OnCompChanged(MElem& aComp)
{
    TBool res = Elem::OnCompChanged(aComp);
    if (res) return res;
    TBool hres = EFalse;
    MEdge* edge = aComp.GetObj(edge);	
    if (edge != NULL) {
	hres = ETrue;
	MVert* ref1 = edge->Ref1();
	MVert* ref2 = edge->Ref2();
	MVert* cp1 = edge->Point1();
	MVert* cp2 = edge->Point2();
	if (cp1 != ref1 || cp2 != ref2) {
	    MElem* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
	    MElem* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
	    TBool isptok1 = (ref1 == NULL || IsPtOk(pt1));
	    TBool isptok2 = (ref2 == NULL || IsPtOk(pt2));
	    if (isptok1 && isptok2) {
		if (cp1 != NULL && ref1 != cp1) edge->Disconnect(cp1);
		if (cp2 != NULL && ref2 != cp2) edge->Disconnect(cp2);
		if (ref1 != NULL && ref2 != NULL) {
		    cp1 = edge->Point1();
		    cp2 = edge->Point2();
		    // Full connection, compatibility checking is needed
		    MCompatChecker* pt1checker = (MCompatChecker*) pt1->GetSIfiC(MCompatChecker::Type(), this);
		    MCompatChecker* pt2checker = (MCompatChecker*) pt2->GetSIfiC(MCompatChecker::Type(), this);
		    TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
		    TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
		    if (ispt1cptb && ispt2cptb) {
			// Are compatible - connect
			TBool res = ETrue;
			if (cp1 == NULL) res = edge->ConnectP1(ref1);
			if (res && cp2 == NULL) res = edge->ConnectP2(ref2);
			if (!res) {
			    MElem* host = iMan->GetMan();
			    Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}
		    }
		    else {
			TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			MElem* host = this;
			Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
		    }

		} else {
		    // Partial connection, compatibility checking isn't needed
		    if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
		    else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		}
	    } else {
		MElem* pt = isptok1 ? pt2 : pt1;
		MElem* host = this;
		Logger()->Write(MLogRec::EErr, &aComp, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
	    }
	}
    }
    return hres;
}
