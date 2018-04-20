
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

TBool ACapsule::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    if (!aModif) { // Don't care of modifications
	string uri = aComp.GetUri(NULL, ETrue);
	MElem* nd = GetNode(uri, ETrue);
	if (nd == NULL) {
	    nd = GetNode(uri, ETrue);
	    __ASSERT(nd != NULL);
	}
    }
    Elem::OnCompChanged(aComp, aContName, aModif);
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp, aContName, aModif);
    }
    if (iObserver != NULL) {
	iObserver->OnCompChanged(aComp, aContName, aModif);
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

MSocket::EIfu MSocket::mIfu;

MSocket::EIfu::EIfu()
{
    RegMethod("PinsCount", 0);
    RegMethod("GetPin", 1);
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

MIface *ConnPointBase::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = (MConnPoint*) this;
    } else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void ConnPointBase::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TBool resg = EFalse;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    } else {
	bool found = false;
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
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resg = resg || (rr.first == rr.second);
		}
	    }
	}
	if (!resg && ereq != NULL) {
	    MProp* req = ereq->GetObj(req);
	    if (req != NULL && req->Value() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    MElem* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MElem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
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
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
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

// ConnPointBase MConnPoint

MConnPoint::EIfu MConnPoint::mIfu;

// Ifu static initialisation
MConnPoint::EIfu::EIfu()
{
    RegMethod("IsProvided", 1);
    RegMethod("IsRequired", 1);
    RegMethod("Provided", 0);
    RegMethod("Required", 0);
}

TBool ConnPointBase::IsProvided(const string& aIfName) const
{
    return Provided() == aIfName;
}

TBool ConnPointBase::IsRequired(const string& aIfName) const
{
    return Required() == aIfName;
}

string ConnPointBase::Provided() const
{
    string res;
    MElem* eprov = ((MElem*) this)->GetNode("./Provided");
    if (eprov != NULL) {
	MProp* prov = eprov->GetObj(prov);
	if (prov != NULL) {
	    res = prov->Value();
	}
    }
    return res;
}

string ConnPointBase::Required() const
{
    string res;
    MElem* eprov = ((MElem*) this)->GetNode("./Required");
    if (eprov != NULL) {
	MProp* prov = eprov->GetObj(prov);
	if (prov != NULL) {
	    res = prov->Value();
	}
    }
    return res;
}

MIface* ConnPointBase::MConnPoint_Call(const string& aSpec, string& aRes)
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
    if (name == "IsProvided") {
	TBool rr = IsProvided(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "IsRequired") {
	TBool rr = IsRequired(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "Provided") {
	aRes = Provided();
    } else if (name == "Required") {
	aRes = Required();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ConnPointBase::MConnPoint_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}



// Base of mutli-content ConnPoint 

const string ConnPointMc::KContName_Provided = "Provided";
const string ConnPointMc::KContName_Required = "Required";
const string ConnPointMc::KContDir = "Direction";
const string ConnPointMc::KContDir_Val_Regular = "Regular";
const string ConnPointMc::KContDir_Val_Inp = "Inp";
const string ConnPointMc::KContDir_Val_Out = "Out";

string ConnPointMc::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointMc::ConnPointMc(const string& aName, MElem* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetParent(Type());
    //InsertContent("Required");
    //InsertContent("Provided");
    ChangeCont("", ETrue, "Required");
    ChangeCont("", ETrue, "Provided");
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

ConnPointMc::ConnPointMc(MElem* aMan, MEnv* aEnv): Vert(Type(), aMan, aEnv)
{
    SetParent(Vert::PEType());
    ChangeCont("", ETrue, "Required");
    ChangeCont("", ETrue, "Provided");
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

MIface *ConnPointMc::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = (MConnPoint*) this;
    } else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void ConnPointMc::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TBool resg = EFalse;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL) {
	// Redirect to pairs if iface requiested is provided by this CP
	if (GetProvided() == aName) {
	    // Requested provided iface - cannot be obtain via pairs - redirect to host
	    if (iMan != NULL && !ctx.IsInContext(iMan)) {
		// TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		MElem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		rr = mgr->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, mgr, rr);
		resg = resg || (rr.first == rr.second);
	    }
	}
	if (!resg) {
	    if (GetRequired() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    MElem* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resg = resg || (rr.first == rr.second);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MElem* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resg = resg || (rr.first == rr.second);
		}
	    }
	}
    }
}


TBool ConnPointMc::IsCompatible(MElem* aPair, TBool aExt)
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
	    string ppt1prov = Provided();
	    string ppt1req = Required();
	    MConnPoint* mcp = cp->GetObj(mcp);
	    if (mcp != NULL) {
		if (ext) {
		    res = mcp->IsProvided(ppt1prov) && mcp->IsRequired(ppt1req);
		} else {
		    res = mcp->IsProvided(ppt1req) && mcp->IsRequired(ppt1prov);
		}
	    }
	}
    }
    return res;
}

MElem* ConnPointMc::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir ConnPointMc::GetDir() const
{
    TDir res = ERegular;
    string cdir = GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

MIface* ConnPointMc::MCompatChecker_Call(const string& aSpec, string& aRes)
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

string ConnPointMc::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

TBool ConnPointMc::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    // If compatibility gets broken disconnect pair, ref UC_061
    TBool res = Vert::ChangeCont(aVal, aRtOnly, aName);
    if (res && (aName == KContName_Required || aName == KContName_Provided)) {
	Disconnect();
    }
    return res;
}

// ConnPointMc MConnPoint

TBool ConnPointMc::IsProvided(const string& aIfName) const
{
    return GetProvided() == aIfName;
}

TBool ConnPointMc::IsRequired(const string& aIfName) const
{
    return GetRequired() == aIfName;
}

string ConnPointMc::Provided() const
{
    return GetProvided();
}

string ConnPointMc::Required() const
{
    return GetRequired();
}

MIface* ConnPointMc::MConnPoint_Call(const string& aSpec, string& aRes)
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
    if (name == "IsProvided") {
	TBool rr = IsProvided(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "IsRequired") {
	TBool rr = IsRequired(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "Provided") {
	aRes = Provided();
    } else if (name == "Required") {
	aRes = Required();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ConnPointMc::MConnPoint_Mid() const
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

MCompatChecker::TDir ConnPointBaseOut::GetDir() const
{
    return EOut;
}




string ExtenderAgent::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ExtenderAgent::ExtenderAgent(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mCompatChecker(*this)
{
    SetParent(Type());
}

ExtenderAgent::ExtenderAgent(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mCompatChecker(*this)
{
    SetParent(Elem::PEType());
}

MIface *ExtenderAgent::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(&mCompatChecker);
    } else if (strcmp(aName, MAgent::Type()) == 0) {
	res = (MAgent*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void ExtenderAgent::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TBool resg = EFalse;
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	MElem* intcp = GetExtd();
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, intcp, rr);
	    resg = resg || (rr.first == rr.second);
	}
	else {
	    MVert* vhost = Host() != NULL ? Host()->GetObj(vhost) : NULL;
	    if (vhost != NULL) {
		TInt count = vhost->PairsCount();
		for (TInt ct = 0; ct < count; ct++) {
		    MVert* pair = vhost->GetPair(ct);
		    MElem* ep = pair->GetObj(ep);
		    if (ep != NULL && !ctx.IsInContext(ep)) {
			rr = ep->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, ep, rr);
			resg = resg || (rr.first == rr.second);
		    }
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	MElem* hostmgr = Host() != NULL ? Host()->GetMan() : NULL;
	MElem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first == rr.second);
	}
    }
}

TBool ExtenderAgent::IsCompatible(MElem* aPair, TBool aExt)
{
    TBool res = EFalse;
    MElem* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MElem* ExtenderAgent::GetExtd()
{
    return (iMan != NULL) ? GetNode("./../Int") : NULL;
}

MCompatChecker::TDir ExtenderAgent::GetDir() const
{
    return MCompatChecker::ERegular;
}

MIface* ExtenderAgent::MCompatChecker_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MCompatChecker::mIfu.CheckMname(name);
    if (!name_ok) {
	return Elem::Call(aSpec, aRes);
    }
    TBool args_ok = MCompatChecker::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "IsCompatible") {
	MElem* pair = GetNode(args.at(0));
	if (pair == NULL) {
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	TBool ext = Ifu::ToBool(args.at(1));
	TBool rr = IsCompatible(pair, ext);
	aRes = Ifu::FromBool(rr);
    } else if (name == "GetExtd") {
	res = GetExtd();
    } else if (name == "GetDir") {
	MCompatChecker::TDir dir = GetDir();
	aRes = Ifu::FromInt(dir);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

string ExtenderAgent::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

MIface* ExtenderAgent::MAgent_DoGetIface(const string& aUid)
{
    MIface* res = NULL;
    if (aUid == MElem::Type())
	res = (MElem*) this;
    else if (aUid == MCompatChecker::Type())
	res = &mCompatChecker;
    return res;
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

MCompatChecker::TDir ExtenderAgentInp::GetDir() const
{
    return MCompatChecker::EInp;
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

MCompatChecker::TDir ExtenderAgentOut::GetDir() const
{
    return MCompatChecker::EOut;
}

// Extender agent, multicontent


string AExtender::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AExtender::AExtender(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

AExtender::AExtender(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

MIface *AExtender::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MAgent::Type()) == 0) {
	res = dynamic_cast<MAgent*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* AExtender::MAgent_DoGetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MElem::Type())
	res = dynamic_cast<MElem*>(this);
    return res;
}

void AExtender::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TBool resg = EFalse;
    TIfRange rr;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL) {
	// Redirect to internal point or pair depending on the requiestor
	MElem* intcp = GetExtd();
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, intcp, rr);
	    resg = resg || (rr.first != rr.second);
	}
	else {
	    MVert* vhost = Host() != NULL ? Host()->GetObj(vhost) : NULL;
	    if (vhost != NULL) {
		TInt count = vhost->PairsCount();
		for (TInt ct = 0; ct < count; ct++) {
		    MVert* pair = vhost->GetPair(ct);
		    MElem* ep = pair->GetObj(ep);
		    if (ep != NULL && !ctx.IsInContext(ep)) {
			rr = ep->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, ep, rr);
			resg = resg || (rr.first != rr.second);
		    }
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	MElem* hostmgr = Host()->GetMan();
	MElem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first != rr.second);
	}
    }
}

TBool AExtender::IsCompatible(MElem* aPair, TBool aExt)
{
    TBool res = EFalse;
    MElem* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MElem* AExtender::GetExtd()
{
    return (iMan != NULL) ? GetNode("./../Int") : NULL;
}

MCompatChecker::TDir AExtender::GetDir() const
{
    TDir res = ERegular;
    string cdir = Host()->GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

MIface* AExtender::Call(const string& aSpec, string& aRes)
{
    __ASSERT(EFalse);
    return  NULL;
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

MIface *ASocket::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MSocket::Type()) == 0) {
	res = (MSocket*) this;
    } else if (strcmp(aName, MAgent::Type()) == 0) {
	res = dynamic_cast<MAgent*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

MIface* ASocket::MAgent_DoGetIface(const string& aUid)
{
    MIface* res = NULL;
    if (aUid == MElem::Type())
	res = dynamic_cast<MElem*>(this);
    else if (aUid == MCompatChecker::Type())
	res = dynamic_cast<MCompatChecker*>(this);
    else if (aUid == MSocket::Type())
	res = dynamic_cast<MSocket*>(this);
    return res;
}

void ASocket::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    // TODO [YB] the current routing model is not optimal. Socket doesn't known itself if
    // it supports iface or not (only pins know). So socket routes to pins first in hope
    // they redirect properly. But what if not? Pin routes back to host, so loop happens, that stops 
    // further routing.
    TIfRange rr;
    TBool resok = EFalse;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    if (res == NULL && !aCtx.empty()) {
	Base* master = aCtx.back();
	MElem* emaster = master->GetObj(emaster);
	Base* rqst = aCtx.size() > 1 ? aCtx.at(aCtx.size() - 2): NULL;
	if (rqst != NULL) {
	    // Requestor is specified, so try to redirect basing on it
	    MElem* erqst = rqst->GetObj(erqst);
	    TBool iscomp = emaster->IsComp(erqst);
	    if (iscomp) {
		// Request comes from internal CP - forward it to upper layer
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    MElem* mgr =  GetMan()->GetMan();
		    if (mgr != NULL && !ctx.IsInContext(mgr)) {
			rr = mgr->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, mgr, rr);
			resok = resok || (rr.first != rr.second);
		    }
		}
	    }
	    else {
		// Request from not internals
		// Find associated pair in context
		Base* apair = NULL;
		MElem* pcomp = NULL;
		Base* ctxe = rqst;
		TICacheRCtx cct(aCtx); if (!cct.empty()) cct.pop_back();
		MElem* host = GetMan();
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
		    if (!cct.empty()) {
			cct.pop_back();
			ctxe = !cct.empty() ? cct.back() : NULL;
		    }
		    if (apair != NULL && ctxe != NULL) {
			// Find associated pairs pin within the context
			// TODO [YB] Checking pair for being ASocket (implemenetation) is wrong way
			// We need to use ifaces instead of impl. Knowledge of impl should be denied.
			MSocket* psock = apair->GetObj(psock);
			if (psock != NULL) {
			    MElem* pereq = psock->GetPin(cct);
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
		    rr = pcomp->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, pcomp, rr);
		    resok = resok || (rr.first != rr.second);
		}
	    }
	    // Redirect to pair. 
	    // TODO [YB] To add checking if requiested iface is supported, ref md "sec_refac_conncomp"
	    // TODO [YB] Probably routing to pair needs to be done first, before the routing to pins
	    if (rr.first == rr.second) {
		MElem* man = GetMan();
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			MElem* pe = pair->GetObj(pe);
			if (!ctx.IsInContext(pe)) {
			    rr = pe->GetIfi(aName, ctx);
			    InsertIfCache(aName, aCtx, pe, rr);
			    resok = resok || (rr.first != rr.second);
			}
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL) {
		MElem* host = GetMan();
		MElem* hostmgr = host->GetMan();
		MElem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resok = resok || (rr.first != rr.second);
		}
	    }
	}
	else {
	    // Requestor not specified, anonymous request
	    if (!resok) {
		MElem* man = GetMan();
		// Redirect to internal pins. Add host into context, this will prevent internals to redirect
		// TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
		for (TInt ci = 0; ci < man->CompsCount() && res == NULL; ci++) {
		    MElem* eit = man->GetComp(ci);
		    if (!ctx.IsInContext(eit) && eit != iMan) {
			rr = eit->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, eit, rr);
			resok = resok || (rr.first != rr.second);
		    }
		}
	    }
	    // Redirect to pair. 
	    // TODO [YB] To add checking if requiested iface is supported, ref md "sec_refac_conncomp"
	    // TODO [YB] Probably routing to pair needs to be done first, before the routing to pins
	    if (rr.first == rr.second) {
		MElem* man = GetMan();
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			MElem* pe = pair->GetObj(pe);
			if (!ctx.IsInContext(pe)) {
			    rr = pe->GetIfi(aName, ctx);
			    InsertIfCache(aName, aCtx, pe, rr);
			    resok = resok || (rr.first != rr.second);
			}
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL && !ctx.IsInContext(iMan)) {
		MElem* host = GetMan();
		MElem* hostmgr = host->GetMan();
		MElem* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resok = resok || (rr.first != rr.second);
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
	    MElem* host = GetMan();
	    for (TInt ci = 0; ci < host->CompsCount() && res; ci++) {
		MElem *comp = host->GetComp(ci);
		if (comp != this && comp->Name() != "Logspec") {
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

MElem* ASocket::GetPin(const TICacheRCtx& aCtx)
{
    MElem* res = NULL;
    MElem* host = GetMan();
    for (TInt ci = 0; ci < host->CompsCount() && res == NULL; ci++) {
	MElem *comp = host->GetComp(ci);
	if (comp != this && comp->Name() != "Logspec") {
	    if (aCtx.IsInContext(comp)) {
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

// ASocket  MSocket


TInt ASocket::PinsCount() const
{
    TInt res = 0;
    MElem* host = iMan;
    __ASSERT(host != NULL);
    for (TInt ci = 0; ci < host->CompsCount() && res; ci++) {
	MElem *comp = host->GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), (Base*) this);
	if (checker != NULL) {
	    res++;
	}
    }
    return res;
}

MElem* ASocket::GetPin(TInt aInd)
{
    MElem* res = 0;
    MElem* host = GetMan();
    __ASSERT(host != NULL);
    for (TInt ci = 0, ct = 0; ci < host->CompsCount() && res; ci++) {
	MElem *comp = host->GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), (Base*) this);
	if (checker != NULL) {
	    if (ct == aInd) {
		res = comp; break;
	    }
	    ct++;
	}
    }
    return res;
}

MIface* ASocket::MSocket_Call(const string& aSpec, string& aRes)
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
    if (name == "PinsCount") {
	TInt rr = PinsCount();
	aRes = Ifu::FromInt(rr);
    } else if (name == "GetPin") {
	TInt ind = Ifu::ToInt(args.at(0));
	res = GetPin(ind);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return  NULL;
}

string ASocket::MSocket_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

MIface* ASocket::MCompatChecker_Call(const string& aSpec, string& aRes)
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

string ASocket::MCompatChecker_Mid() const
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

MCompatChecker::TDir ASocketOut::GetDir() const
{
    return EOut;
}


// Socket agent, multicontent: redirects iface requests to pins
string ASocketMc::PEType()
{
    return ASocket::PEType() + GUri::KParentSep + Type();
}

ASocketMc::ASocketMc(const string& aName, MElem* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetParent(Type());
}

ASocketMc::ASocketMc(MElem* aMan, MEnv* aEnv): ASocket(Type(), aMan, aEnv)
{
    SetParent(ASocket::PEType());
}

MCompatChecker::TDir ASocketMc::GetDir() const
{
    TDir res = ERegular;
    string cdir = Host()->GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
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

void Syst::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
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
    Vert::OnCompDeleting(aComp, aSoft, aModif);
}


TBool Syst::IsPtOk(MElem* aPt) {
    return ETrue;
}

TBool Syst::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool res = Elem::OnCompChanged(aComp, aContName, aModif);
    if (!res) return res;
    TBool hres = ETrue;
    MEdge* edge = aComp.GetObj(edge);	
    if (edge != NULL) {
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
			    MElem* host = GetMan();
			    Logger()->Write(EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}
		    }
		    else {
			TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			MElem* host = this;
			Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
		    }

		} else {
		    // Partial connection, compatibility checking isn't needed
		    if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
		    else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		}
	    } else {
		MElem* pt = isptok1 ? pt2 : pt1;
		MElem* host = this;
		Logger()->Write(EErr, this, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
	    }
	}
    }
    return hres;
}
