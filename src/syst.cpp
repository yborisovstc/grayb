
#include "plat.h"
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

ACapsule::ACapsule(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

TBool ACapsule::OnCompChanged(MUnit& aComp, const string& aContName, TBool aModif)
{
#if 0
    if (!aModif) { // Don't care of modifications
	string uri = aComp.GetUri(NULL, ETrue);
	MUnit* nd = GetNode(uri, ETrue);
	if (nd == NULL) {
	    nd = GetNode(uri, ETrue);
	    __ASSERT(nd != NULL);
	}
    }
    Elem::OnCompChanged(aComp, aContName, aModif);
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp, aContName, aModif);
    }
    return ETrue;
#endif
    // Capsule doesn't assume any embedded agents/comp_observers
    // So just redirect to owner
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp, aContName, aModif);
    }
}

TBool ACapsule::OnChanged(MUnit& aComp)
{
    // Capsule doesn't assume any embedded agents/comp_observers
    // So just redirect to owner
    if (iMan != NULL) {
	iMan->OnChanged(aComp);
    }
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

ConnPointBase::ConnPointBase(const string& aName, MUnit* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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
	MUnit* eprov = GetNode("./(Prop:)Provided");
	MUnit* ereq = GetNode("./(Prop:)Required");
	if (eprov != NULL) {
	    MProp* prov = eprov->GetObj(prov);
	    if (prov != NULL && prov->Value() == aName) {
		// Requested provided iface - cannot be obtain via pairs - redirect to host
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    // TODO [YB] Clean up redirecing to mgr. Do we need to have Capsule agt to redirect?
		    MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
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
		    MUnit* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		}
	    }
	}
    }
}

TBool ConnPointBase::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
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

MUnit* ConnPointBase::GetExtd()
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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
    MUnit* eprov = ((MUnit*) this)->GetNode("./Provided");
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
    MUnit* eprov = ((MUnit*) this)->GetNode("./Required");
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

ConnPointMc::ConnPointMc(const string& aName, MUnit* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    //InsertContent("Required");
    //InsertContent("Provided");
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
		MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		rr = mgr->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, mgr, rr);
		resg = resg || (rr.first == rr.second);
	    }
	}
	if (!resg) {
	    if (GetRequired() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    MUnit* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resg = resg || (rr.first == rr.second);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resg = resg || (rr.first == rr.second);
		}
	    }
	}
    }
}


TBool ConnPointMc::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
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

MUnit* ConnPointMc::GetExtd()
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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



// Base of mutli-content ConnPoint unit

const string ConnPointMcu::KContName_Provided = "Provided";
const string ConnPointMcu::KContName_Required = "Required";
const string ConnPointMcu::KContDir = "Direction";
const string ConnPointMcu::KContDir_Val_Regular = "Regular";
const string ConnPointMcu::KContDir_Val_Inp = "Inp";
const string ConnPointMcu::KContDir_Val_Out = "Out";

string ConnPointMcu::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointMcu::ConnPointMcu(const string& aName, MUnit* aMan, MEnv* aEnv): Vertu(aName, aMan, aEnv)
{
    // For native agt (aName is empty) set type as name. For inherited agent set given name
    iName = aName.empty() ? GetType(PEType()) : aName;
    ChangeCont("", ETrue, "Required");
    ChangeCont("", ETrue, "Provided");
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

MIface *ConnPointMcu::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MConnPoint::Type()) == 0) {
	res = (MConnPoint*) this;
    } else {
	res = Vertu::DoGetObj(aName);
    }
    return res;
}

void ConnPointMcu::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
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
		MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		rr = mgr->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, mgr, rr);
		resg = resg || (rr.first == rr.second);
	    }
	}
	if (!resg) {
	    if (GetRequired() == aName) {
		for (set<MVert*>::iterator it = iPairs.begin(); it != iPairs.end(); it++) {
		    MUnit* pe = (*it)->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resg = resg || (rr.first == rr.second);
		    }
		}
		// Responsible pairs not found, redirect to upper layer
		if ((rr.first == rr.second) && iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr = iMan->Name() == "Capsule" ? iMan->GetMan() : iMan;
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resg = resg || (rr.first == rr.second);
		}
	    }
	}
    }
}


TBool ConnPointMcu::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Checking if the pair is Extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type(), this);
    // Consider all pairs not supporting MCompatChecker as not compatible 
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
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

MUnit* ConnPointMcu::GetExtd()
{
    return NULL;
}

MCompatChecker::TDir ConnPointMcu::GetDir() const
{
    TDir res = ERegular;
    string cdir = GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

MIface* ConnPointMcu::MCompatChecker_Call(const string& aSpec, string& aRes)
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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

string ConnPointMcu::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

TBool ConnPointMcu::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    // If compatibility gets broken disconnect pair, ref UC_061
    TBool res = Vertu::ChangeCont(aVal, aRtOnly, aName);
    if (res && (aName == KContName_Required || aName == KContName_Provided)) {
	Disconnect();
    }
    return res;
}

TBool ConnPointMcu::IsProvided(const string& aIfName) const
{
    return GetProvided() == aIfName;
}

TBool ConnPointMcu::IsRequired(const string& aIfName) const
{
    return GetRequired() == aIfName;
}

string ConnPointMcu::Provided() const
{
    return GetProvided();
}

string ConnPointMcu::Required() const
{
    return GetRequired();
}

MIface* ConnPointMcu::MConnPoint_Call(const string& aSpec, string& aRes)
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

string ConnPointMcu::MConnPoint_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}




// Input ConnPoint base
string ConnPointBaseInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ConnPointBaseInp::ConnPointBaseInp(const string& aName, MUnit* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ConnPointBaseOut::ConnPointBaseOut(const string& aName, MUnit* aMan, MEnv* aEnv): ConnPointBase(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

MCompatChecker::TDir ConnPointBaseOut::GetDir() const
{
    return EOut;
}




string ExtenderAgent::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ExtenderAgent::ExtenderAgent(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mCompatChecker(*this)
{
    SetCrAttr(PEType(), aName);
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
	MUnit* intcp = GetExtd();
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
		    MUnit* ep = pair->GetObj(ep);
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
	MUnit* hostmgr = Host() != NULL ? Host()->GetMan() : NULL;
	MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first == rr.second);
	}
    }
}

TBool ExtenderAgent::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    MUnit* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MUnit* ExtenderAgent::GetExtd()
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
    TBool args_ok = MCompatChecker::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "IsCompatible") {
	MUnit* pair = GetNode(args.at(0));
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
    if (aUid == MUnit::Type())
	res = (MUnit*) this;
    else if (aUid == MCompatChecker::Type())
	res = &mCompatChecker;
    return res;
}


// Input Extender Agent
string ExtenderAgentInp::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

ExtenderAgentInp::ExtenderAgentInp(const string& aName, MUnit* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ExtenderAgentOut::ExtenderAgentOut(const string& aName, MUnit* aMan, MEnv* aEnv): ExtenderAgent(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

AExtender::AExtender(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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
    if (aName == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
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
	MUnit* intcp = GetExtd();
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
		    MUnit* ep = pair->GetObj(ep);
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
	MUnit* hostmgr = Host()->GetMan();
	MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first != rr.second);
	}
    }
}

TBool AExtender::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    MUnit* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MUnit* AExtender::GetExtd()
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


// Extender agent, monolitic, multicontent

string AExtd::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

AExtd::AExtd(const string& aName, MUnit* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

MIface *AExtd::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void AExtd::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
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
	MUnit* intcp = GetExtd();
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, intcp, rr);
	    resg = resg || (rr.first != rr.second);
	}
	else {
	    TInt count = PairsCount();
	    for (TInt ct = 0; ct < count; ct++) {
		MVert* pair = GetPair(ct);
		MUnit* ep = pair->GetObj(ep);
		if (ep != NULL && !ctx.IsInContext(ep)) {
		    rr = ep->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, ep, rr);
		    resg = resg || (rr.first != rr.second);
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	MUnit* hostmgr = GetMan();
	MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first != rr.second);
	}
    }
}

TBool AExtd::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    MUnit* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MUnit* AExtd::GetExtd()
{
    return (iMan != NULL) ? GetNode("./Int") : NULL;
}

MCompatChecker::TDir AExtd::GetDir() const
{
    TDir res = ERegular;
    string cdir = Host()->GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

MIface* AExtd::Call(const string& aSpec, string& aRes)
{
    __ASSERT(EFalse);
    return  NULL;
}

MIface* AExtd::MCompatChecker_Call(const string& aSpec, string& aRes)
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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

string AExtd::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

// Extention agent (monolitic, multicontent, input). Redirects request for iface to internal CP of extention.
string AExtdInp::PEType()
{
    return AExtd::PEType() + GUri::KParentSep + Type();
}

AExtdInp::AExtdInp(const string& aName, MUnit* aMan, MEnv* aEnv): AExtd(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    ChangeCont(ConnPointMc::KContDir_Val_Inp, ETrue, ConnPointMc::KContDir);
}

// Extention agent (monolitic, multicontent, input). Redirects request for iface to internal CP of extention.
string AExtdOut::PEType()
{
    return AExtd::PEType() + GUri::KParentSep + Type();
}

AExtdOut::AExtdOut(const string& aName, MUnit* aMan, MEnv* aEnv): AExtd(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    ChangeCont(ConnPointMc::KContDir_Val_Out, ETrue, ConnPointMc::KContDir);
}



// Extender agent, monolitic, multicontent, unit

string AExtdu::PEType()
{
    return Vertu::PEType() + GUri::KParentSep + Type();
}

AExtdu::AExtdu(const string& aName, MUnit* aMan, MEnv* aEnv): Vertu(aName, aMan, aEnv)
{
    // For native agt (aName is empty) set type as name. For inherited agent set given name
    iName = aName.empty() ? GetType(PEType()) : aName;
    ChangeCont(ConnPointMc::KContDir_Val_Regular, ETrue, ConnPointMc::KContDir);
}

MIface *AExtdu::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else {
	res = Vertu::DoGetObj(aName);
    }
    return res;
}

void AExtdu::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
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
	MUnit* intcp = GetExtd();
	if (intcp != NULL && !ctx.IsInContext(intcp)) {
	    rr = intcp->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, intcp, rr);
	    resg = resg || (rr.first != rr.second);
	}
	else {
	    TInt count = PairsCount();
	    for (TInt ct = 0; ct < count; ct++) {
		MVert* pair = GetPair(ct);
		MUnit* ep = pair->GetObj(ep);
		if (ep != NULL && !ctx.IsInContext(ep)) {
		    rr = ep->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, ep, rr);
		    resg = resg || (rr.first != rr.second);
		}
	    }
	}
    }
    // Responsible pairs not found, redirect to upper layer
    if (rr.first == rr.second && iMan != NULL) {
	MUnit* hostmgr = GetMan();
	MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
	if (mgr != NULL && !ctx.IsInContext(mgr)) {
	    rr = mgr->GetIfi(aName, ctx);
	    InsertIfCache(aName, aCtx, mgr, rr);
	    resg = resg || (rr.first != rr.second);
	}
    }
}

TBool AExtdu::IsCompatible(MUnit* aPair, TBool aExt)
{
    TBool res = EFalse;
    MUnit* intcp = GetExtd();
    MCompatChecker* mint = (intcp != NULL) ? (MCompatChecker*) intcp->GetSIfiC(MCompatChecker::Type(), this) : NULL;
    if (mint != NULL) {
	res = mint->IsCompatible(aPair, !aExt);
    }
    return res;
}

MUnit* AExtdu::GetExtd()
{
    return (iMan != NULL) ? GetNode("./Int") : NULL;
}

MCompatChecker::TDir AExtdu::GetDir() const
{
    TDir res = ERegular;
    string cdir = Host()->GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

MIface* AExtdu::Call(const string& aSpec, string& aRes)
{
    __ASSERT(EFalse);
    return  NULL;
}

MIface* AExtdu::MCompatChecker_Call(const string& aSpec, string& aRes)
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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

string AExtdu::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

// Extention agent, monolitic, multicontent, unit, input. Redirects request for iface to internal CP of extention.
string AExtduInp::PEType()
{
    return AExtdu::PEType() + GUri::KParentSep + Type();
}

AExtduInp::AExtduInp(const string& aName, MUnit* aMan, MEnv* aEnv): AExtdu(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    ChangeCont(ConnPointMc::KContDir_Val_Inp, ETrue, ConnPointMc::KContDir);
}

// Extention agent, monolitic, multicontent, unit, input. Redirects request for iface to internal CP of extention.
string AExtduOut::PEType()
{
    return AExtdu::PEType() + GUri::KParentSep + Type();
}

AExtduOut::AExtduOut(const string& aName, MUnit* aMan, MEnv* aEnv): AExtdu(aName, aMan, aEnv)
{
    iName = aName.empty() ? GetType(PEType()) : aName;
    ChangeCont(ConnPointMc::KContDir_Val_Out, ETrue, ConnPointMc::KContDir);
}



// Socket agent: redirects iface requests to pins
string ASocket::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ASocket::ASocket(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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
    if (aUid == MUnit::Type())
	res = dynamic_cast<MUnit*>(this);
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
	MUnit* emaster = master->GetObj(emaster);
	Base* rqst = aCtx.size() > 1 ? aCtx.at(aCtx.size() - 2): NULL;
	if (rqst != NULL) {
	    // Requestor is specified, so try to redirect basing on it
	    MUnit* erqst = rqst->GetObj(erqst);
	    TBool iscomp = emaster->IsComp(erqst);
	    if (iscomp) {
		// Request comes from internal CP - forward it to upper layer
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr =  GetMan()->GetMan();
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
		MUnit* pcomp = NULL;
		Base* ctxe = rqst;
		TICacheRCtx cct(aCtx); if (!cct.empty()) cct.pop_back();
		MUnit* host = GetMan();
		TBool isextd = EFalse;
		// TODO [YB] To cleanup
		while (ctxe != NULL && pcomp == NULL) {
		    MCompatChecker* cp = ctxe->GetObj(cp);
		    // Update extention option if met extention in context
		    if (cp != NULL) {
			apair = NULL;
			if (cp->IsCompatible(host, isextd)) {
			    isextd ^= ETrue;
			    MUnit* extd = cp->GetExtd();
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
			    MUnit* pereq = psock->GetPin(cct);
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
		MUnit* man = GetMan();
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			MUnit* pe = pair->GetObj(pe);
			if (!ctx.IsInContext(pe)) {
			    rr = pe->GetIfi(aName, ctx);
			    InsertIfCache(aName, aCtx, pe, rr);
			    resok = resok || (rr.first != rr.second);
			}
		    }
		}
	    }
	    // Redirect to upper layer
	    // TODO [YB] Wrong design - we need to redirect only if iMan is not in the context
	    // Ref ds_di_ifsprul for this design issue analysis.
	    if (rr.first == rr.second && iMan != NULL) {
		MUnit* host = GetMan();
		MUnit* hostmgr = host->GetMan();
		MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
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
		MUnit* man = GetMan();
		// Redirect to internal pins. Add host into context, this will prevent internals to redirect
		// TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
		for (TInt ci = 0; ci < man->CompsCount() && res == NULL; ci++) {
		    MUnit* eit = man->GetComp(ci);
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
		MUnit* man = GetMan();
		MVert* vman = man->GetObj(vman);
		if (vman != NULL) {
		    TInt pcount = vman->PairsCount();
		    for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
			MVert* pair = vman->GetPair(ct);
			MUnit* pe = pair->GetObj(pe);
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
		MUnit* host = GetMan();
		MUnit* hostmgr = host->GetMan();
		MUnit* mgr = hostmgr->Name() == "Capsule" ? hostmgr->GetMan() : hostmgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resok = resok || (rr.first != rr.second);
		}
	    }
	}
    }
}


TBool ASocket::IsCompatible(MUnit* aPair, TBool aExt)
{
    // Going thru non-trivial components and check their compatibility
    // TODO [YB] Needs to clean up this chunk
    TBool res = ETrue;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Requesing anonymously because can be returned to itself vie extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type());
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	// Checking if the pair is Extender
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    MUnit* host = GetMan();
	    for (TInt ci = 0; ci < host->CompsCount() && res; ci++) {
		MUnit *comp = host->GetComp(ci);
		if (comp != this && comp->Name() != "Logspec") {
		    MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), this);
		    if (checker != NULL) {
			GUri uri;
			uri.AppendElem("*", comp->Name());
			MUnit *pcomp = cp->GetNode(uri);
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

MUnit* ASocket::GetExtd()
{
    return NULL;
}

MUnit* ASocket::GetPin(const TICacheRCtx& aCtx)
{
    MUnit* res = NULL;
    MUnit* host = GetMan();
    for (TInt ci = 0; ci < host->CompsCount() && res == NULL; ci++) {
	MUnit *comp = host->GetComp(ci);
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
    MUnit* host = iMan;
    __ASSERT(host != NULL);
    for (TInt ci = 0; ci < host->CompsCount() && res; ci++) {
	MUnit *comp = host->GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), (MUnit*) this);
	if (checker != NULL) {
	    res++;
	}
    }
    return res;
}

MUnit* ASocket::GetPin(TInt aInd)
{
    MUnit* res = 0;
    MUnit* host = GetMan();
    __ASSERT(host != NULL);
    for (TInt ci = 0, ct = 0; ci < host->CompsCount() && res; ci++) {
	MUnit *comp = host->GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), this);
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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

ASocketInp::ASocketInp(const string& aName, MUnit* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ASocketOut::ASocketOut(const string& aName, MUnit* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
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

ASocketMc::ASocketMc(const string& aName, MUnit* aMan, MEnv* aEnv): ASocket(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

MCompatChecker::TDir ASocketMc::GetDir() const
{
    TDir res = ERegular;
    string cdir = Host()->GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}



// Socket agent multicontent, monolitic
string ASocketMcm::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ASocketMcm::ASocketMcm(const string& aName, MUnit* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

MIface *ASocketMcm::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = (MCompatChecker*) this;
    } else if (strcmp(aName, MSocket::Type()) == 0) {
	res = (MSocket*) this;
    } else {
	res = Vert::DoGetObj(aName);
    }
    return res;
}

void ASocketMcm::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
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
	Base* rqst = aCtx.back();
	if (rqst != NULL) {
	    // Requestor is specified, so try to redirect basing on it
	    MUnit* erqst = rqst->GetObj(erqst);
	    TBool iscomp = IsComp(erqst);
	    if (iscomp) {
		// Request comes from internal CP - forward it to upper layer
		if (iMan != NULL && !ctx.IsInContext(iMan)) {
		    MUnit* mgr =  GetMan();
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
		MUnit* pcomp = NULL;
		Base* ctxe = rqst;
		TICacheRCtx cct(aCtx);
		TBool isextd = EFalse;
		while (ctxe != NULL && pcomp == NULL) {
		    // MUnit* cpe = ctxe->GetObj(cpe); // Comment it out. For debug purpose only
		    MCompatChecker* cp = ctxe->GetObj(cp);
		    // Update extention option if met extention in context
		    if (cp != NULL) {
			apair = NULL;
			if (cp->IsCompatible(this, isextd)) {
			    isextd ^= ETrue;
			    MUnit* extd = cp->GetExtd();
			    if (extd != this) {
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
			// TODO [YB] Checking pair for being ASocketMcm (implemenetation) is wrong way
			// We need to use ifaces instead of impl. Knowledge of impl should be denied.
			MSocket* psock = apair->GetObj(psock);
			if (psock != NULL) {
			    MUnit* pereq = psock->GetPin(cct);
			    if (pereq != NULL) {
				GUri uri;
				// Using only name as signature of socket pin. This is because even the compatible types can differ
				uri.AppendElem("*", pereq->Name());
				pcomp = GetNode(uri);
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
		TInt pcount = PairsCount();
		for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
		    MVert* pair = GetPair(ct);
		    MUnit* pe = pair->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resok = resok || (rr.first != rr.second);
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL) {
		MUnit* mgr = GetMan();
		mgr = mgr->Name() == "Capsule" ? mgr->GetMan() : mgr;
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
		// Redirect to internal pins. Add host into context, this will prevent internals to redirect
		// TODO [YB] To avoid routing directly from agent excluding host. This causes incorrect context
		for (TInt ci = 0; ci < CompsCount() && res == NULL; ci++) {
		    MUnit* eit = GetComp(ci);
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
		TInt pcount = PairsCount();
		for (TInt ct = 0; ct < pcount && res == NULL; ct++) {
		    MVert* pair = GetPair(ct);
		    MUnit* pe = pair->GetObj(pe);
		    if (!ctx.IsInContext(pe)) {
			rr = pe->GetIfi(aName, ctx);
			InsertIfCache(aName, aCtx, pe, rr);
			resok = resok || (rr.first != rr.second);
		    }
		}
	    }
	    // Redirect to upper layer
	    if (rr.first == rr.second && iMan != NULL && !ctx.IsInContext(iMan)) {
		MUnit* mgr = GetMan();
		mgr = mgr->Name() == "Capsule" ? mgr->GetMan() : mgr;
		if (mgr != NULL && !ctx.IsInContext(mgr)) {
		    rr = mgr->GetIfi(aName, ctx);
		    InsertIfCache(aName, aCtx, mgr, rr);
		    resok = resok || (rr.first != rr.second);
		}
	    }
	}
    }
}

TBool ASocketMcm::IsCompatible(MUnit* aPair, TBool aExt)
{
    // Going thru non-trivial components and check their compatibility
    // TODO [YB] Needs to clean up this chunk
    TBool res = ETrue;
    TBool ext = aExt;
    MUnit *cp = aPair;
    // Requesing anonymously because can be returned to itself vie extender
    MCompatChecker* pchkr = (MCompatChecker*) aPair->GetSIfiC(MCompatChecker::Type());
    if (pchkr != NULL) {
	MUnit* ecp = pchkr->GetExtd(); 
	// Checking if the pair is Extender
	if (ecp != NULL ) {
	    ext = !ext;
	    cp = ecp;
	}
	if (cp != NULL) {
	    for (TInt ci = 0; ci < CompsCount() && res; ci++) {
		MUnit *comp = GetComp(ci);
		if (comp != this && comp->Name() != "Logspec") {
		    MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), this);
		    if (checker != NULL) {
			GUri uri;
			uri.AppendElem("*", comp->Name());
			MUnit *pcomp = cp->GetNode(uri);
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
    } else {
	res = EFalse;
    }
    return res;
}

MUnit* ASocketMcm::GetExtd()
{
    return NULL;
}

MUnit* ASocketMcm::GetPin(const TICacheRCtx& aCtx)
{
    MUnit* res = NULL;
    for (TInt ci = 0; ci < CompsCount() && res == NULL; ci++) {
	MUnit *comp = GetComp(ci);
	if (aCtx.IsInContext(comp)) {
	    res = comp;
	}
    }
    return res;
}


MCompatChecker::TDir ASocketMcm::GetDir() const
{
    TDir res = ERegular;
    string cdir = GetContent(ConnPointMc::KContDir);
    if (cdir == ConnPointMc::KContDir_Val_Inp) res = EInp;
    else if (cdir == ConnPointMc::KContDir_Val_Out) res = EOut;
    return res;
}

TInt ASocketMcm::PinsCount() const
{
    TInt res = 0;
    for (TInt ci = 0; ci < CompsCount() && res; ci++) {
	MUnit *comp = const_cast<ASocketMcm*>(this)->GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), (MUnit*) this);
	if (checker != NULL) {
	    res++;
	}
    }
    return res;
}

MUnit* ASocketMcm::GetPin(TInt aInd)
{
    MUnit* res = 0;
    for (TInt ci = 0, ct = 0; ci < CompsCount() && res; ci++) {
	MUnit *comp = GetComp(ci);
	MCompatChecker* checker = (MCompatChecker*) comp->GetSIfiC(MCompatChecker::Type(), this);
	if (checker != NULL) {
	    if (ct == aInd) {
		res = comp; break;
	    }
	    ct++;
	}
    }
    return res;
}

MIface* ASocketMcm::MSocket_Call(const string& aSpec, string& aRes)
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

string ASocketMcm::MSocket_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

MIface* ASocketMcm::MCompatChecker_Call(const string& aSpec, string& aRes)
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
	MUnit* pair = GetNode(args.at(0), ETrue);
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

string ASocketMcm::MCompatChecker_Mid() const
{
    return GetUri(iEnv->Root(), ETrue);
}

// Socket agent, input, multicontent, monolitic
string ASocketInpMcm::PEType()
{
    return ASocketMcm::PEType() + GUri::KParentSep + Type();
}

ASocketInpMcm::ASocketInpMcm(const string& aName, MUnit* aMan, MEnv* aEnv): ASocketMcm(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    ChangeCont(ConnPointMc::KContDir_Val_Inp, ETrue, ConnPointMc::KContDir);
}

// Socket agent, output, multicontent, monolitic
string ASocketOutMcm::PEType()
{
    return ASocketMcm::PEType() + GUri::KParentSep + Type();
}

ASocketOutMcm::ASocketOutMcm(const string& aName, MUnit* aMan, MEnv* aEnv): ASocketMcm(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
    ChangeCont(ConnPointMc::KContDir_Val_Out, ETrue, ConnPointMc::KContDir);
}


// System
string Syst::PEType()
{
    return Vert::PEType() + GUri::KParentSep + Type();
}

Syst::Syst(const string& aName, MUnit* aMan, MEnv* aEnv): Vert(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

void Syst::OnCompDeleting(MUnit& aComp, TBool aSoft, TBool aModif)
{
    MUnit* eedge = GetCompOwning("Edge", &aComp);
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


TBool Syst::IsPtOk(MUnit* aPt) {
    return ETrue;
}

TBool Syst::OnCompChanged(MUnit& aComp, const string& aContName, TBool aModif)
{
    TBool res = ETrue;
    TEhr pres = Elem::ProcessCompChanged(aComp, aContName);
    if (pres == EEHR_Ignored) {
	// Hasn't processed by CompObservers, process locally
	MEdge* edge = aComp.GetObj(edge);	
	if (edge == NULL) {
	    MUnit* owner = aComp.GetMan();
	    edge = owner ? owner->GetObj(edge) : NULL;
	}
	if (edge != NULL) {
	    MVert* ref1 = edge->Ref1();
	    MVert* ref2 = edge->Ref2();
	    MVert* cp1 = edge->Point1();
	    MVert* cp2 = edge->Point2();
	    if (cp1 != ref1 || cp2 != ref2) {
		MUnit* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
		MUnit* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
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
				MUnit* host = GetMan();
				Logger()->Write(EErr, &aComp, "Connecting [%s - %s] failed", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			    }
			}
			else {
			    TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(pt2);
			    TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(pt1);
			    MUnit* host = this;
			    Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible roles", pt1->GetUri(NULL, ETrue).c_str(), pt2->GetUri(NULL, ETrue).c_str());
			}

		    } else {
			// Partial connection, compatibility checking isn't needed
			if (cp1 == NULL && ref1 != NULL) edge->ConnectP1(ref1);
			else if (cp2 == NULL && ref2 != NULL) edge->ConnectP2(ref2);
		    }
		} else {
		    MUnit* pt = isptok1 ? pt2 : pt1;
		    MUnit* host = this;
		    Logger()->Write(EErr, this, "Connecting [%s] - not allowed cp", pt->GetUri(NULL, ETrue).c_str());
		}
	    }
	}
    } else {
	res = (pres == EEHR_Accepted);
    }
    return res;
}
