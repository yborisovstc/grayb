#include "vertp.h"
#include <stdexcept> 

// Vertex

string Vertp::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

/** \brief Structure of content */

/** \brief Edge
 *    {P1:'Uri, proposed point 1' P2:'Uri, proposed point 2Uri'
 *         CP1:'Uri, Connected point 1' CP2:'Uri, Connected point 2'}
 *         CP1 and CP2 are internal and cannot be changed from out of vertp
 *         Values of paremeters are URI to vertexes.
 */

const string& Vertp::KContent_Edges = "Edges";
const string& Vertp::KContent_Connpoints = "Connpoints";
const string& Vertp::KContent_P1 = "P1";
const string& Vertp::KContent_P2 = "P2";
const string& Vertp::KContent_CP1 = "CP1";
const string& Vertp::KContent_CP2 = "CP2";

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
	    if (edgeName != KContent_Edges) { // Omitting edge itself, handling cp only
		string namePA = GetContentLName(aContName);
		if (namePA == KContent_P1 || namePA == KContent_P2) {
		    string namePB = KContent_P2, nameCPA = KContent_CP1, nameCPB = KContent_CP2;
		    if (namePA == KContent_P2) {
			namePB = KContent_P1, nameCPA = KContent_CP2, nameCPB = KContent_CP1;
		    } 
		    string puria = GetContentValue(aContName);
		    string cpuria = GetContentValue(ContentKey(edgeName, nameCPA));
		    string cpurib = GetContentValue(ContentKey(edgeName, nameCPB));
		    if (puria != cpuria) {
			if (!cpuria.empty()) {
			    // There is connection established, disconnecting
			    string cpurib = GetContentValue(ContentKey(edgeName, nameCPB));
			    assert(!cpurib.empty());
			    MElem* cea = GetNode(cpuria);
			    assert(cea != NULL);
			    MVert* cva = cea->GetObj(cva);
			    assert(cva != NULL);
			    MElem* ceb = GetNode(cpurib);
			    assert(ceb != NULL);
			    MVert* cvb = ceb->GetObj(cvb);
			    assert(cvb != NULL);
			    cva->Disconnect(cvb);
			    SetContentValue(ContentKey(edgeName, nameCPA), "");
			    SetContentValue(ContentKey(edgeName, nameCPB), "");
			}
			// Establishing new connection
			string purib = GetContentValue(ContentKey(edgeName, namePB));
			if (!purib.empty()) {
			    MElem* ea = GetNode(puria);
			    assert(ea != NULL);
			    MVert* va = ea->GetObj(va);
			    assert(va != NULL);
			    MElem* eb = GetNode(purib);
			    assert(eb != NULL);
			    MVert* vb = eb->GetObj(vb);
			    assert(vb != NULL);
			    res = va->Connect(vb);
			    if (res) {
				SetContentValue(ContentKey(edgeName, nameCPA), puria);
				SetContentValue(ContentKey(edgeName, nameCPB), purib);
			    }
			}
		    }
		} else {
		    // Wrong compoment
		    res = false;
		}
	    } // cont isn't edge itself
	} // isedge
    } // res of Elem::OnCompChanged
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
