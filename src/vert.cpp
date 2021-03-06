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

Vert::Vert(const string& aName, MUnit* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

MIface* Vert::MVert_DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MUnit::Type()) == 0) {
	res = dynamic_cast<MUnit*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    }
    return res;
}

MIface *Vert::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MVert::Type()) == 0) {
	res = dynamic_cast<MVert*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

Vert::~Vert()
{
    Disconnect();
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
	    iMan->OnChanged(this);
	}
	else {
	    // TODO [YB] Seems this happens constantly. To analyze why
	    MUnit* ep = aPair->GetObj(ep);
	    Logger()->Write(EErr, this, "Connecting [%s] - already connected, failed", ep->GetUri(NULL, ETrue).c_str());
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
	iMan->OnChanged(this);
    }
}

TBool Vert::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    TBool res = ETrue;
    TEhr pres = Elem::ProcessCompChanged(aComp, aContName);
    if (pres == EEHR_Ignored) {
	MEdge* edge = const_cast<MUnit*>(aComp)->GetObj(edge);	
	if (edge == NULL) {
	    MUnit* owner = const_cast<MUnit*>(aComp)->GetMan();
	    edge = owner ? owner->GetObj(edge) : NULL;
	}
	if (edge != NULL) {
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
		    MUnit* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
		    MUnit* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
		    Logger()->Write(EErr, aComp, "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
		}
	    }
	}
    } else {
	res = (pres == EEHR_Accepted);
    }
    return res;
}

MIface* Vert::MVert_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MVert::mIfu.CheckMname(name);
    TBool args_ok = MVert::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Connect") {
	MUnit* pair = GetNode(args.at(0));
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
	MUnit* earg = GetNode(args.at(0));
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

void Vert::OnCompDeleting(const MUnit* aComp, TBool aSoft, TBool aModif)
{
    // Disconnect the binding edges if the comp is vert connected
    MVert* vert = const_cast<MUnit*>(aComp)->GetObj(vert);
    if (vert != NULL && vert->PairsCount() > 0) {
	for (auto& it : iMComps) {
	    MUnit* comp = it.second;
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
	MVert* pair = *it;
	Disconnect(pair);
	pair->Disconnect(this);
    }
    iPairs.clear();
}

TBool Vert::IsLinked(const MVert* aPair, TBool aDirect) const
{
    TBool res = false;
    res = IsPair(aPair);
    if (!res) {
	for (auto compr : iMComps) {
	    MVert* compv = compr.second->GetObj(compv);
	    if (compv) {
		if (aDirect) {
		    res = aPair->IsPair(compv);
		} else {
		    res = aPair->IsLinked(compv, ETrue);
		}
		if (res) break;
	    }
	}
    }
    return res;
}

void Vert::DoSpecificMut(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx)
{
    TNodeType rnotype = aSpec.Type();
    if (rnotype == ENt_Conn) {
	if (aSpec.AttrExists(ENa_P) && aSpec.AttrExists(ENa_Q)) {
	    string argP = aSpec.Attr(ENa_P);
	    string argQ = aSpec.Attr(ENa_Q);
	    const TNs& ns = aCtx.mNs;
	    Connect(argP, argQ, ns);
	} else {
	    Logger()->Write(EErr, this, "Connecting nodes: missing required attr");
	}
	if (!aRunTime && !aTrialMode) {
	    ChromoNode chn = iChromo->Root().AddChild(aSpec, ETrue, EFalse);
	    NotifyNodeMutated(chn, aCtx);
	}
    } else if (rnotype == ENt_Disconn) {
	if (aSpec.AttrExists(ENa_P) && aSpec.AttrExists(ENa_Q)) {
	    string argP = aSpec.Attr(ENa_P);
	    string argQ = aSpec.Attr(ENa_Q);
	    const TNs& ns = aCtx.mNs;
	    Disconnect(argP, argQ, ns);
	} else {
	    Logger()->Write(EErr, this, "Disconnecting nodes: missing required attr");
	}
	if (!aRunTime && !aTrialMode) {
	    ChromoNode chn = iChromo->Root().AddChild(aSpec, ETrue, EFalse);
	    NotifyNodeMutated(chn, aCtx);
	}
    } else {
	Vert::DoSpecificMut(aSpec, aRunTime, aTrialMode, aCtx);
    }
}

void Vert::Connect(const string& argP, const string& argQ, const TNs& aNs)
{
    MUnit* ucpP = GetNodeByName(argP, aNs);
    MUnit* ucpQ = GetNodeByName(argQ, aNs);
    if (ucpP != NULL && ucpQ != NULL) {
	MVert* vcpP = ucpP->GetObj(vcpP);
	MVert* vcpQ = ucpQ->GetObj(vcpQ);
	if (vcpP != NULL && vcpQ != NULL) {
	    MCompatChecker* pt1checker = (MCompatChecker*) ucpP->GetSIfi(MCompatChecker::Type(), this);
	    MCompatChecker* pt2checker = (MCompatChecker*) ucpQ->GetSIfi(MCompatChecker::Type(), this);
	    TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(ucpQ);
	    TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(ucpP);
	    if (ispt1cptb && ispt2cptb) {
		// Are compatible - connect
		TBool res = vcpP->Connect(vcpQ);
		if (!res) {
		    Logger()->Write(EErr, this, "Connecting [%s - %s] failed", argP.c_str(), argQ.c_str());
		}
	    }
	    else {
		TBool ispt1cptb = pt1checker == NULL || pt1checker->IsCompatible(ucpQ);
		TBool ispt2cptb = pt2checker == NULL || pt2checker->IsCompatible(ucpP);
		Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible roles", argP.c_str(), argQ.c_str());
	    }
	} else {
	    Logger()->Write(EErr, this, "Connecting nodes: pair [%s] isn't vertex", ((vcpP == NULL) ? argP : argQ).c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Connecting nodes: pair [%s] not found", ((ucpP == NULL) ? argP : argQ).c_str());
    }
}

void Vert::Disconnect(const string& argP, const string& argQ, const TNs& aNs)
{
    MUnit* ucpP = GetNodeByName(argP, aNs);
    MUnit* ucpQ = GetNodeByName(argQ, aNs);
    if (ucpP != NULL && ucpQ != NULL) {
	MVert* vcpP = ucpP->GetObj(vcpP);
	MVert* vcpQ = ucpQ->GetObj(vcpQ);
	if (vcpP != NULL && vcpQ != NULL) {
	    if (vcpP->IsPair(vcpQ)) {
		vcpP->Disconnect(vcpQ);
		vcpQ->Disconnect(vcpP);
	    } else {
		Logger()->Write(EErr, this, "Disonnecting nodes: vertex [%s] isn't pair of [%s]", argQ.c_str(), argP.c_str());
	    }
	} else {
	    Logger()->Write(EErr, this, "Disonnecting nodes: pair [%s] isn't vertex", ((vcpP == NULL) ? argP : argQ).c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Disonnecting nodes: pair [%s] not found", ((ucpP == NULL) ? argP : argQ).c_str());
    }
}




// Vertex unit

string Vertu::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

Vertu::Vertu(const string& aName, MUnit* aMan, MEnv* aEnv): Unit(aName, aMan, aEnv)
{
    // For native agt (aName is empty) set type as name. For inherited agent set given name
    iName = aName.empty() ? GetType(PEType()) : aName;
}

MIface* Vertu::MVert_DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MUnit::Type()) == 0) {
	res = dynamic_cast<MUnit*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    }
    return res;
}

MIface *Vertu::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MVert::Type()) == 0) {
	res = dynamic_cast<MVert*>(this);
    } else {
	res = Unit::DoGetObj(aName);
    }
    return res;
}

Vertu::~Vertu()
{
    Disconnect();
}

TBool Vertu::Connect(MVert* aPair)
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
	    iMan->OnChanged(this);
	}
	else {
	    // TODO [YB] Seems this happens constantly. To analyze why
	    MUnit* ep = aPair->GetObj(ep);
	    Logger()->Write(EErr, this, "Connecting [%s] - already connected, failed", ep->GetUri(NULL, ETrue).c_str());
	    res = EFalse;
	}
    }
    return res;
}

TInt Vertu::PairsCount() const
{
    return iPairs.size();
}

MVert* Vertu::GetPair(TInt aInd) const
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

TBool Vertu::IsPair(const MVert* aPair) const
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

void Vertu::Disconnect(MVert* aPair)
{
    // TODO [YB] To redisign Edge (ref comment in OnCompChanged and restore assert
    // __ASSERT(aPair != NULL && iPairs.count(aPair) > 0);
    if (aPair != NULL && iPairs.count(aPair) > 0) {
	iPairs.erase(aPair);
	// Invalidate ifaces cache
	InvalidateIfCache();
	__ASSERT(iMan != NULL);
	iMan->OnChanged(this);
    }
}

TBool Vertu::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    TBool res = ETrue;
    TEhr pres = ProcessCompChanged(aComp, aContName);
    if (pres == EEHR_Ignored) {
	MEdge* edge = const_cast<MUnit*>(aComp)->GetObj(edge);	
	if (edge == NULL) {
	    MUnit* owner = const_cast<MUnit*>(aComp)->GetMan();
	    edge = owner ? owner->GetObj(edge) : NULL;
	}
	if (edge != NULL) {
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
		    MUnit* pt1 = ref1 == NULL ? NULL : ref1->GetObj(pt1);
		    MUnit* pt2 = ref2 == NULL ? NULL : ref2->GetObj(pt2);
		    Logger()->Write(EErr, aComp, "Connecting [%s - %s] failed", pt1->GetUri().c_str(), pt2->GetUri().c_str());
		}
	    }
	}
    } else {
	res = (pres == EEHR_Accepted);
    }
    return res;
}

MIface* Vertu::MVert_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = MVert::mIfu.CheckMname(name);
    TBool args_ok = MVert::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Connect") {
	MUnit* pair = GetNode(args.at(0));
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
	MUnit* earg = GetNode(args.at(0));
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

void Vertu::OnCompDeleting(const MUnit* aComp, TBool aSoft, TBool aModif)
{
    // Disconnect the binding edges if the comp is vert connected
    MVert* vert = const_cast<MUnit*>(aComp)->GetObj(vert);
    if (vert != NULL && vert->PairsCount() > 0) {
	for (auto& it : iMComps) {
	    MUnit* comp = it.second;
	    MEdge* edge = comp->GetObj(edge);
	    if (edge != NULL && (edge->Point1() == vert || edge->Point2() == vert)) {
		edge->Disconnect(vert);
	    }
	}
    }
    Unit::OnCompDeleting(aComp, aSoft, aModif);
}

void Vertu::Disconnect()
{
    while (PairsCount() > 0) {
	set<MVert*>::iterator it = iPairs.begin();
	MVert* pair = *it;
	Disconnect(pair);
	pair->Disconnect(this);
    }
    iPairs.clear();
}

TBool Vertu::IsLinked(const MVert* aPair, TBool aDirect) const
{
    TBool res = false;
    res = IsPair(aPair);
    if (!res) {
	for (auto compr : iMComps) {
	    MVert* compv = compr.second->GetObj(compv);
	    if (compv) {
		if (aDirect) {
		    res = aPair->IsPair(compv);
		} else {
		    res = aPair->IsLinked(compv, ETrue);
		}
		if (res) break;
	    }
	}
    }
    return res;
}
