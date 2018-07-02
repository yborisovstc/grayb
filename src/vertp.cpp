#include "vertp.h"
#include <stdexcept> 

// Vertex

string Vertp::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

const string& Vertp::KContent_Edges = "Edges";
const string& Vertp::KContent_Connpoints = "Connpoints";
const string& Vertp::KContent_P1 = "P1";
const string& Vertp::KContent_P2 = "P2";

Vertp::Vertp(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

Vertp::Vertp(MElem* aMan, MEnv* aEnv):Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

MIface* Vertp::MVert_DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MElem::Type()) == 0) {
	res = dynamic_cast<MElem*>(this);
    } else if (strcmp(aName, MCompatChecker::Type()) == 0) {
	res = dynamic_cast<MCompatChecker*>(this);
    }
    return res;
}

MIface *Vertp::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MVert::Type()) == 0) {
	res = dynamic_cast<MVert*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

Vertp::~Vertp()
{
    mPairs.clear();
}

TBool Vertp::Connect(MVert* aPair)
{
    TBool res = ETrue;
    __ASSERT(aPair != NULL);
    if(!IsPair(aPair)) {
	// Invalidate ifaces cache
	InvalidateIfCache();
	mPairs.insert(TPairsRegElem(aPair, ""));
	// We need to two part connection here. Otherwise CompChanged handling will be incorrect
	// owner can notify the pair and get back request for iface, so need to have full connection
	// So request pair connection if not connected yet
	if(!aPair->IsPair(this)) {
	    res = aPair->Connect(this);
	}
	__ASSERT(iMan != NULL);
	res = res && iMan->OnChanged(*this);
    } else {
	MElem* ep = aPair->GetObj(ep);
	Logger()->Write(EErr, this, "Connecting [%s] - already connected, failed", ep->GetUri().c_str());
	res = EFalse;
    }
    return res;
}

TInt Vertp::PairsCount() const
{
    return mPairs.size();
}

MVert* Vertp::GetPair(TInt aInd) const
{
    MVert* res = NULL;
    if (aInd < PairsCount()) {
	TPairsReg::const_iterator it = mPairs.begin(); 
	for (; it != mPairs.end() && aInd != 0; it++, aInd--) {
	}
	res = it->first;
    }
    return res;
}

TBool Vertp::IsPair(const MVert* aPair) const
{
    return mPairs.count((MVert*) aPair) > 0;
}

void Vertp::Disconnect(MVert* aPair)
{
    // TODO [YB] To redisign Edge (ref comment in OnCompChanged and restore assert
    // __ASSERT(aPair != NULL && mPairs.count(aPair) > 0);
    if (aPair != NULL && mPairs.count(aPair) > 0) {
	mPairs.erase(aPair);
	// Invalidate ifaces cache
	InvalidateIfCache();
	__ASSERT(iMan != NULL);
	iMan->OnChanged(*this);
    }
}

TBool Vertp::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool res = Elem::OnCompChanged(aComp, aContName, aModif);
    if (res) {
	TBool isedge = IsContentCompOf(aContName, KContent_Edges);
	if (isedge) {
	    string edgeName = GetContentOwner(aContName);
	    if (ContentHasComp(edgeName, KContent_P1) && ContentHasComp(edgeName, KContent_P2) ) {
		string puri1 = GetContentValue(ContentKey(edgeName, KContent_P1));
		string puri2 = GetContentValue(ContentKey(edgeName, KContent_P2));
		MElem* e1 = GetNode(puri1);
		MElem* e2 = GetNode(puri2);
		MVert* v1 = e1->GetObj(v1);
		MVert* v2 = e2->GetObj(v2);
		if (v1 && v2) {
		    res = v1->Connect(v2);
		    if (!res) {
			Logger()->Write(EErr, &aComp, "Connecting [%s - %s]: failed", puri1.c_str(), puri2.c_str());
		    }
		} else {
		    Logger()->Write(EErr, &aComp, "Connecting [%s - %s]: isn't vertex", puri1.c_str(), puri2.c_str());
		}
	    }
	}
    }
    return res;
}

MIface* Vertp::Call(const string& aSpec, string& aRes)
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

void Vertp::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    // Disconnect the binding edges if the comp is vert connected
    MVert* vert = aComp.GetObj(vert);
    if (vert != NULL && vert->PairsCount() > 0) {
	TInt ind = 0;
	while (vert->PairsCount() > 0) {
	    MVert* pair = vert->GetPair(ind);
	    pair->Disconnect(vert);
	    vert->Disconnect(pair);
	}
    }
    Elem::OnCompDeleting(aComp, aSoft, aModif);
}
