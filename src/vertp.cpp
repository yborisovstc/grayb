#include <iostream>

#include "vertp.h"
#include <stdexcept> 

// Vertex

MVertp::EIfu MVertp::mIfu;

// Ifu static initialisation
MVertp::EIfu::EIfu()
{
    RegMethod("Connect", 1);
    RegMethod("MVertp_DoGetObj", 1);
    RegMethod("PairsCount", 0);
    RegMethod("IsPair", 1);
    RegMethod("GetPair", 1);
}


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

const string Vertp::KContent_Edges = "Edges";
const string Vertp::KContent_Connpoints = "ConnPoints";
const string Vertp::KContent_P1 = "P1";
const string Vertp::KContent_P2 = "P2";
const string Vertp::KContent_CP1 = "CP1";
const string Vertp::KContent_CP2 = "CP2";

Vertp::Vertp(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

Vertp::Vertp(MElem* aMan, MEnv* aEnv):Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

MIface *Vertp::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MVertp::Type()) == 0) {
	res = dynamic_cast<MVertp*>(this);
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

Vertp::~Vertp()
{
}

MIface* Vertp::MVertp_DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MElem::Type()) == 0) {
	res = dynamic_cast<MElem*>(this);
    }
    return res;
}

TBool Vertp::Connect(MVertp* aPair, const string& aCp)
{
    TBool res = ETrue;
    __ASSERT(aPair != NULL);
    if(!IsPair(aPair)) {
	// Invalidate ifaces cache
	InvalidateIfCache();
	mPairToCpReg.insert(TPairToCpRegElem(aPair, aCp));
	mCpToPairReg.insert(TCpToPairRegElem(aCp, aPair));
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
    return mPairToCpReg.size();
}

MVertp* Vertp::GetPair(TInt aInd) const
{
    MVertp* res = NULL;
    if (aInd < PairsCount()) {
	TPairToCpReg::const_iterator it = mPairToCpReg.begin(); 
	for (; it != mPairToCpReg.end() && aInd != 0; it++, aInd--) {
	}
	res = it->first;
    }
    return res;
}

TBool Vertp::IsPair(const MVertp* aPair) const
{
    return mPairToCpReg.count((MVertp*) aPair) > 0;
}

void Vertp::Disconnect(MVertp* aPair)
{
    // TODO [YB] To redisign Edge (ref comment in OnCompChanged and restore assert
    // __ASSERT(aPair != NULL && mPairs.count(aPair) > 0);
    if (aPair != NULL && mPairToCpReg.count(aPair) > 0) {
	RemovePairFromReg(aPair);
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
			    MVertp* cva = cea->GetObj(cva);
			    assert(cva != NULL);
			    MElem* ceb = GetNode(cpurib);
			    assert(ceb != NULL);
			    MVertp* cvb = ceb->GetObj(cvb);
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
			    MVertp* va = ea->GetObj(va);
			    assert(va != NULL);
			    MElem* eb = GetNode(purib);
			    assert(eb != NULL);
			    MVertp* vb = eb->GetObj(vb);
			    assert(vb != NULL);
			    res = va->Connect(vb);
			    res = res && vb->Connect(va);
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
    TBool name_ok = MVertp::mIfu.CheckMname(name);
    if (!name_ok) {
	return Elem::Call(aSpec, aRes);
    }
    TBool args_ok = MVertp::mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Connect") {
	MElem* pair = GetNode(args.at(0));
	if (pair == NULL) {
	    Logger()->Write(EErr, this, "Connecting [%s] - cannot get pair, failed", args.at(0).c_str());
	    throw (runtime_error("Cannot get pair: " + args.at(0)));
	}
	MVertp* vpair = pair->GetObj(vpair);
	if (vpair == NULL) {
	    throw (runtime_error("Pair isn't vertex: " + args.at(0)));
	}
	TBool rr = Connect(vpair);
	aRes = Ifu::FromBool(rr);
    } else if (name == "MVertp_DoGetObj") {
	res = MVertp_DoGetObj(args.at(0).c_str());
    } else if (name == "PairsCount") {
	TInt pc = PairsCount();
	aRes = Ifu::FromInt(pc);
    } else if (name == "IsPair") {
	MElem* earg = GetNode(args.at(0));
	if (earg == NULL) {
	    throw (runtime_error("Cannot get arg: " + args.at(0)));
	}
	MVertp* varg = earg->GetObj(varg);
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
    MVertp* vert = aComp.GetObj(vert);
    if (vert != NULL && vert->PairsCount() > 0) {
	TInt ind = 0;
	while (vert->PairsCount() > 0) {
	    MVertp* pair = vert->GetPair(ind);
	    pair->Disconnect(vert);
	    vert->Disconnect(pair);
	}
    }
    Elem::OnCompDeleting(aComp, aSoft, aModif);
}

void Vertp::DumpCps() const
{
    cout << "== Conn points ==" << endl << "<Pair>  <Conn point>" << endl;
    for (auto elem : mPairToCpReg) {
	MVertp* pair = elem.first;
	MElem* epair = pair->GetObj(epair);
	cout << epair->GetUri() << " : " << elem.second << endl;
    }
}
    
void Vertp::RemovePairFromReg(MVertp* aPair)
{
    TCpsEr er = mPairToCpReg.equal_range(aPair);
    for (TCpsIter it = er.first; it != er.second; it++) {
	mCpToPairReg.erase(it->second);
    }
    mPairToCpReg.erase(aPair);
}
