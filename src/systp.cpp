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

// System
string Systp::PEType()
{
    return Vertp::PEType() + GUri::KParentSep + Type();
}

Systp::Systp(const string& aName, MElem* aMan, MEnv* aEnv): Vertp(aName, aMan, aEnv)
{
    SetParent(Type());
}

Systp::Systp(MElem* aMan, MEnv* aEnv): Vertp(Type(), aMan, aEnv)
{
    SetParent(Vertp::PEType());
}

void Systp::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    Vertp::OnCompDeleting(aComp, aSoft, aModif);
}

// TODO To consider separating of handling of content change on local level and on owner level
TBool Systp::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool res = true;
    if (&aComp == this) {
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
			if (!puria.empty() && !purib.empty()) {
			    // Verifying compatibility first
			    bool compatible = AreCpsCompatible(puria, purib);
			    if (compatible) {
				MElem* ea = GetNode(puria);
				assert(ea != NULL);
				MVertp* va = ea->GetObj(va);
				assert(va != NULL);
				MElem* eb = GetNode(purib);
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

bool Systp::AreCpsCompatible(const GUri& aCp1, const GUri& aCp2)
{
    bool res = true;
    MElem* ea = GetNode(aCp1);
    MElem* eb = GetNode(aCp2);
    string cna = aCp1.GetContent();
    string cnb = aCp2.GetContent();
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
		string pa = ea->GetContent(ContentKey(cptype_a, KCp_SimpleCp_P));
		string ra = ea->GetContent(ContentKey(cptype_a, KCp_SimpleCp_R));
		string pb = eb->GetContent(ContentKey(cptype_b, KCp_SimpleCp_P));
		string rb = eb->GetContent(ContentKey(cptype_b, KCp_SimpleCp_R));
		if (!inv && pa == rb && ra == pb || inv && pa == pb && ra == rb) {
		} else res = false;
	    }
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
	MElem* rq = aCtx.back();
	MVertp* rqv = rq != nullptr ? rq->GetObj(rqv) : nullptr;
	if (rqv != nullptr && IsPair(rqv)) {
	    // Request from a pair
	    /*
	    string cp = GetPairCp(rqv);
	    string cptype = GetContCount(cp) == 1 ? GetContComp(cp, 0) : string();
	    string cptype_s = GetContentLName(cptype);
	    if (cptype_s == KCp_SimpleCp) {
		UpdateIfiForConnPoint(aName, aCtx, cptype);
	    }
	    */
	    TCpsEr cps = GetCpsForPair(rqv);
	    for (auto it = cps.first; it != cps.second; it++) {
		string& cp = it->second;
		string cptype = GetContCount(cp) == 1 ? GetContComp(cp, 0) : string();
		string cptype_s = GetContentLName(cptype);
		if (cptype_s == KCp_SimpleCp) {
		    UpdateIfiForConnPoint(aName, aCtx, cptype);
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
	// Requested provided iface, use base resolver
	Vertp::UpdateIfi(aIfName, aCtx);
    }
}

bool Systp::GetExtended(MElem* aNode, const string& aExt, string& aInt, bool& aInv)
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
