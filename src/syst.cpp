
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
    if (aIncUpHier) {
	if (res == NULL) {
	    // Redirect to pairs if iface requiested is provided by this CP
	    //Elem* eprov = GetNode("Prop:Provided");
	    Elem* eprov = GetNode("Prop:Provided");
	    Elem* ereq = GetNode("Prop:Required");
	    if (eprov != NULL) {
		MProp* prov = eprov->GetObj(prov);
		if (prov != NULL) {
		    if (prov->Value() == aName) {
			// TODO [YB] Do we need to redirect to hier only here and restrict doing that unconditionally?
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
		    }
		}
	    }
	}
	if (res == NULL) {
	    res = Vert::DoGetObj(aName, aIncUpHier, aCtx);
	}
    }
    return res;
}

TBool ConnPointBase::IsCompatible(Elem* aPair)
{
    TBool res = EFalse;
    // Check roles conformance
    // Point#1 provided
    Elem* ept1prov = GetNode("Prop:Provided");
    MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
    Elem* ept2req = aPair->GetNode("Prop:Required");
    MProp* ppt2req = ept2req->GetObj(ppt2req);
    // Point#2 provided
    Elem* ept2prov = aPair->GetNode("Prop:Provided");
    MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
    Elem* ept1req = GetNode("Prop:Required");
    MProp* ppt1req = ept1req->GetObj(ppt1req);
    res = (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value());
    return res;
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
		    if (ep != NULL) {
			res = ep->DoGetObj(aName, aIncUpHier, &ctx);
		    }
		}
	    }
	}
    }
    return res;
}

TBool ExtenderAgent::IsCompatible(Elem* aPair)
{
    TBool res = EFalse;
    Elem* intcp = GetNode("../../ConnPoint:Int");
    // Check roles conformance, inverting the roles from internal CP
    // Point#1 provided
    Elem* ept1prov = intcp->GetNode("Prop:Required");
    MProp* ppt1prov = ept1prov->GetObj(ppt1prov);
    Elem* ept2req = aPair->GetNode("Prop:Required");
    MProp* ppt2req = ept2req->GetObj(ppt2req);
    // Point#2 provided
    Elem* ept2prov = aPair->GetNode("Prop:Provided");
    MProp* ppt2prov = ept2prov->GetObj(ppt2prov);
    Elem* ept1req = intcp->GetNode("Prop:Provided");
    MProp* ppt1req = ept1req->GetObj(ppt1req);
    res = (ppt1prov->Value() == ppt2req->Value() && ppt2prov->Value() == ppt1req->Value());
    return res;
}



ASocket::ASocket(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *ASocket::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Elem::DoGetObj(aName, EFalse);
    }
    if (res == NULL && aCtx != NULL) {
	Base* master = aCtx->Requestor();
	Elem* emaster = master->GetObj(emaster);
	Base* rqst = aCtx->Ctx()->Requestor();
	Elem* erqst = rqst->GetObj(erqst);
	TBool iscomp = emaster->IsComp(erqst);
	if (iscomp) {
	    res = emaster->DoGetObj(aName, ETrue, &ctx);
	}
	else {
	    Elem* man = iMan->GetMan();
	    // Redirect to internal pins with hier option
	    for (vector<Elem*>::const_iterator it = man->Comps().begin(); it != man->Comps().end() && res == NULL; it++) {
		Elem* eit = (*it);
		if (!ctx.IsInContext(eit) && eit != iMan) {
		    res = (*it)->DoGetObj(aName, aIncUpHier, &ctx);
		}
	    }
	}
    }
    return res;
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

