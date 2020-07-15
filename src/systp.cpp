#include "systp.h"


/** \brief Edge
 *    {P1:'Uri, proposed point 1' P2:'Uri, proposed point 2Uri'
 *         CP1:'Uri, Connected point 1' CP2:'Uri, Connected point 2'}
 *         CP1 and CP2 are internal and cannot be changed from out of the agent
 *         In contradiction to vertex the Values of paremeters are URI to systems connpoint.
 */

/** Simple connection point: stated provided and required ifaces only
 * {ConnPoint:{Provided:'Iface1' Required:'Iface2'}}"
 */


const string KCp_SimpleCp = "ConnPoint";
const string KCp_SimpleCp_P = "Provided";
const string KCp_SimpleCp_R = "Required";
const string KCp_Extender = "Extender";
const string KCp_Extender_Int = "Int";
const string KCp_Socket = "Socket";

// System
string Systp::PEType()
{
    return Vertp::PEType() + GUri::KParentSep + Type();
}

Systp::Systp(const string& aName, MUnit* aMan, MEnv* aEnv): Vertp(aName, aMan, aEnv)
{
    SetCrAttr(PEType(), aName);
}

void Systp::OnCompDeleting(const MUnit* aComp, TBool aSoft, TBool aModif)
{
    Vertp::OnCompDeleting(aComp, aSoft, aModif);
}

// TODO To consider separating of handling of content change on local level and on owner level
TBool Systp::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    TBool res = true;
    if (aComp == this) {
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
			    MUnit* cea = GetNode(cpuria);
			    assert(cea != NULL);
			    MVertp* cva = cea->GetObj(cva);
			    assert(cva != NULL);
			    MUnit* ceb = GetNode(cpurib);
			    assert(ceb != NULL);
			    MVertp* cvb = ceb->GetObj(cvb);
			    assert(cvb != NULL);
			    cva->Disconnect(cvb);
			    SetContentValue(ContentKey(edgeName, nameCPA), "");
			    SetContentValue(ContentKey(edgeName, nameCPB), "");
			}
			// Establishing new connection
			string purib = GetContentValue(ContentKey(edgeName, namePB));
			if (!puria.empty() && !purib.empty()) {
			    // Checking if specified conn points are valid
			    GUri uria(puria);
			    GUri urib(purib);
			    MUnit* ea = GetNode(uria);
			    MUnit* eb = GetNode(urib);
			    string cna = uria.GetContent();
			    string cnb = urib.GetContent();
			    string cva = ea->GetContent(cna, true);
			    string cvb = eb->GetContent(cnb, true);
			    if (cva.empty() || cvb.empty()) {
				Logger()->Write(EErr, this, "Connecting [%s - %s] - connpoint [%s] doesn't exist", puria.c_str(), purib.c_str(),
					cva.empty() ? puria.c_str() : purib.c_str());
				res = false;
			    } else {
				// Verifying compatibility
				bool compatible = AreCpsCompatible(ea, cna, eb, cnb);
				if (compatible) {
				    MUnit* ea = GetNode(puria);
				    assert(ea != NULL);
				    MVertp* va = ea->GetObj(va);
				    assert(va != NULL);
				    MUnit* eb = GetNode(purib);
				    assert(eb != NULL);
				    MVertp* vb = eb->GetObj(vb);
				    assert(vb != NULL);
				    string cna = GUri(puria).GetContent();
				    string cnb = GUri(purib).GetContent();
				    res = va->Connect(vb, cna);
				    res = res && vb->Connect(va, cnb);
				    if (res) {
					SetContentValue(ContentKey(edgeName, nameCPA), puria);
					SetContentValue(ContentKey(edgeName, nameCPB), purib);
				    }
				} else {
				    Logger()->Write(EErr, this, "Connecting [%s - %s] - incompatible", puria.c_str(), purib.c_str());
				    res = false;
				}
			    }
			}
		    }
		} else {
		    // Wrong compoment
		    res = false;
		}
	    } // cont isn't edge itself
	} // isedge
	if (res) {
	    res= Vertp::OnCompChanged(aComp, aContName, aModif);
	}
    } // aComp == this
    return res;
}

bool Systp::AreCpsCompatible(MUnit* aNode1, const string& aCp1, MUnit* aNode2, const string& aCp2)
{
    bool res = true;
    MUnit* ea = aNode1;
    MUnit* eb = aNode2;
    const string& cna = aCp1;
    const string& cnb = aCp2;
    string cva = ea->GetContent(cna, true);
    string cvb = eb->GetContent(cnb, true);
    if (ea->GetContCount(cna) == 1 && eb->GetContCount(cnb)) {
	string cptype_a = ea->GetContComp(cna, 0);
	string cptype_b = eb->GetContComp(cnb, 0);
	string cptype_as = GetContentLName(cptype_a);
	string cptype_bs = GetContentLName(cptype_b);

	bool inv = false;

	if (cptype_as == KCp_Extender) {
	    res = GetExtended(ea, cptype_a, cptype_a, inv);
	    cptype_as = GetContentLName(cptype_a);
	}

	if (cptype_bs == KCp_Extender) {
	    res = GetExtended(eb, cptype_b, cptype_b, inv);
	    cptype_bs = GetContentLName(cptype_b);
	}

	if (res && cptype_as == cptype_bs) {
	    if (cptype_as == KCp_SimpleCp) {
		// Simple connection points
		string pa = ea->GetContent(ContentKey(cptype_a, KCp_SimpleCp_P));
		string ra = ea->GetContent(ContentKey(cptype_a, KCp_SimpleCp_R));
		string pb = eb->GetContent(ContentKey(cptype_b, KCp_SimpleCp_P));
		string rb = eb->GetContent(ContentKey(cptype_b, KCp_SimpleCp_R));
		if (!inv && pa == rb && ra == pb || inv && pa == pb && ra == rb) {
		} else res = false;
	    } else if (cptype_as == KCp_Socket) {
		// Sockets
		// Validate pins
		int a_pins_count = ea->GetContCount(cptype_a);
		int b_pins_count = eb->GetContCount(cptype_b);
		if (a_pins_count == b_pins_count) {
		    for (int cnt = 0; cnt < a_pins_count; cnt++) {
			string pina = ea->GetContComp(cptype_a, cnt);
			string pinb = eb->GetContComp(cptype_b, cnt);
			bool pin_cptbl = AreCpsCompatible(ea, pina, eb, pinb);
			if (! pin_cptbl) {
			    res = false; break;
			}
		    }
		} else res = false;
	    } else res = false;
	} else res = false;
    } else res = false;
    return res;
}

void Systp::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TBool resg = EFalse;
    TICacheRCtx ctx(aCtx); ctx.push_back(this);
    // Don't provide local ifaces, the only contract based request is handled
    if (!aCtx.empty()) {
	MUnit* rq = aCtx.back();
	MVertp* rqv = rq != nullptr ? rq->GetObj(rqv) : nullptr;
	if (rqv != nullptr && IsPair(rqv)) {
	    // Request from a pair
	    TCpsEr cps = GetCpsForPair(rqv);
	    for (auto it = cps.first; it != cps.second; it++) {
		string& cp = it->second;
		string cptype = GetContCount(cp) == 1 ? GetContComp(cp, 0) : string();
		string cptype_s = GetContentLName(cptype);
		if (cptype_s == KCp_SimpleCp) {
		    UpdateIfiForConnPoint(aName, aCtx, cptype);
		} else if (cptype_s == KCp_Extender) {
		    UpdateIfiForExtender(aName, aCtx, cptype);
		} else if (cptype_s == KCp_Socket) {
		    UpdateIfiForSocket(aName, aCtx, cptype);
		}
	    }
	} else {
	    // Request from non-pair, handling as vert
	    // TODO The access aside CPs should be disabled
	    Vertp::UpdateIfi(aName, aCtx);
	}
    }
}

void Systp::UpdateIfiForConnPoint(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId)
{
    string prov = GetContent(ContentKey(aCpId, KCp_SimpleCp_P));
    if (aIfName == prov) {
	// Checking if connpint is socket pin
	string pinname = GetContentOwner(aCpId);
	string socktype = GetContentOwner(pinname);
	string socktype_s = GetContentLName(socktype);
	if (socktype_s == KCp_Socket) {
	    // Get pairs
	    string sockname = GetContentOwner(socktype);
	    TPairsEr pairs = Systp::GetPairsForCp(sockname);
	    for (auto it =pairs.first; it != pairs.second; it++) {
		MUnit* pair = it->second->MVertp::GetObj(pair);
	    }
	} else {
	    // Requested provided iface, use base resolver
	    Vertp::UpdateIfi(aIfName, aCtx);
	}
    }
}

void Systp::UpdateIfiForExtender(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId)
{
    string sIntKey = ContentKey(aCpId, KCp_Extender_Int);
    string inttype = GetContCount(sIntKey) == 1 ? GetContComp(sIntKey, 0) : string();
    string inttype_s = GetContentLName(inttype);
    if (inttype_s == KCp_SimpleCp) {
	TPairsEr pairs = Systp::GetPairsForCp(sIntKey);
	for (auto it =pairs.first; it != pairs.second; it++) {
	    MUnit* pair = it->second->MVertp::GetObj(pair);
	    TICacheRCtx ctx(aCtx); ctx.push_back(this);
	    TIfRange rr = pair->GetIfi(aIfName, ctx);
	    InsertIfCache(aIfName, aCtx, pair, rr);
	}
	//UpdateIfiForConnPoint(aIfName, aCtx, inttype);
    }
}

void Systp::UpdateIfiForSocket(const string& aIfName, const TICacheRCtx& aCtx, const string& aCpId)
{
}

bool Systp::GetExtended(MUnit* aNode, const string& aExt, string& aInt, bool& aInv)
{
    bool res = true;
    aInv = ~aInv;
    if (aNode->GetContCount(aExt) == 1) {
	string ext_int = aNode->GetContComp(aExt, 0); // Int
	if (aNode->GetContCount(ext_int) == 1) {
	    string ext_int_type = aNode->GetContComp(ext_int, 0); // Type of Int
	    string ext_int_types = GetContentLName(ext_int_type);
	    if (ext_int_types == KCp_Extender) {
		res = GetExtended(aNode, ext_int_type, aInt, aInv);
	    } else {
		aInt = ext_int_type;
	    }
	} else res = false;
    } else res = false;
    return res;
}

MIface *Systp::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MSyst::Type()) == 0) {
	res = dynamic_cast<MSyst*>(this);
    } else {
	res = Vertp::DoGetObj(aName);
    }
    return res;
}

MVertp::TPairsEr Systp::GetPairsForCp(const string& aCp)
{
    TPairsEr res = mCpToPairReg.equal_range(aCp);
    return res;
}

MVertp::TCpsEr Systp::GetCpsForPair(MVertp* aPair)
{
    TCpsEr res = mPairToCpReg.equal_range(aPair);
    return res;
}

MIfProv::TIfRange Systp::GetIfiCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx)
{
}

#if 0
TIfRange Systp::GetIfiForCp(const string& aName, const string& aCp, const TICacheRCtx& aCtx)
{
    string kk = mHost.ContentKey(Vertp::KContent_Connpoints, aName);
    MVertp::TPairsEr pairs = GetPairsForCp(kk);
    if (pairs.first != pairs.second) {
	MVertp* pair = pairs.first->second;
	MUnit* epair = pair->GetObj(epair);
	TICacheRCtx ctx(aCtx); ctx.push_back(this);
	TIfRange rr = pair->GetIfi(aName, ctx);
	// TODO To consider extending context with conn point. In this case the ifaced from CP
	// can be cached locally. Otherwise we need to collect them from pairs
	// InsertIfCache(aIfName, aCtx, pair, rr);
    }
}
#endif 
