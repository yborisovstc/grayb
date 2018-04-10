#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sys/time.h>
#include <stdexcept> 
#include <iostream> 
#include "mmod.h"
#include "ifu.h"


TBool Elem::EN_PERF_TRACE = EFalse;
TBool Elem::EN_PERF_METR = EFalse;
TBool Elem::EN_PERF_DBG1 = ETrue;
TBool Elem::EN_MUT_LIM = ETrue;

const string Elem::KCont_About = "About";
const string MElem::KCont_Categories = "Categories";
const string MElem::KCont_Ctg_Readonly = "Readonly";
const string MElem::KCont_Ctg_Debug = "Debug";

string Elem::Fmt::mSepContInp = ";";
string Elem::Fmt::mSepContName = "~";
// TODO [YB] Alternative method via static Ifu derived is used, remaining for info only
//string IfuPars = "Name,0,GetNode,1,Mutate,4";
//Ifu Elem::mIfu(IfuPars);

MAgentObserver::EIfu MAgentObserver::mIfu;

// Ifu static initialisation
MAgentObserver::EIfu::EIfu()
{
    RegMethod("OnCompDeleting", 1);
    RegMethod("OnCompAdding", 1);
    RegMethod("OnCompChanged", 1);
    RegMethod("OnChanged", 1);
    RegMethod("OnCompRenamed", 2);
    RegMethod("OnCompMutated", 1);
}


MElem::EIfu MElem::mIfu;

// Ifu static initialisation
MElem::EIfu::EIfu()
{
    RegMethod("Name", 0);
    RegMethod("GetMan", 0);
    RegMethod("GetCtx", 0);
    RegMethod("GetAttachedMgr", 0);
    RegMethod("GetAowner", 0);
    RegMethod("GetCompAowner", 1);
    RegMethod("GetRoot", 0);
    RegMethod("GetNode", 2);
    RegMethod("GetNode#2", 3);
    RegMethod("GetContent", 2);
    RegMethod("Mutate", 4);
    RegMethod("Mutate#2", 4);
    RegMethod("IsProvided", 0);
    RegMethod("GetParent", 0);
    RegMethod("GetChromoSpec", 0);
    RegMethod("EType", 1);
    RegMethod("GetUri", 1);
    RegMethod("GetRUri", 1);
    RegMethod("DoGetObj", 1);
    RegMethod("GetIfi", 2);
    RegMethod("GetIfind", 3);
    RegMethod("GetSIfi", 2);
    RegMethod("UnregIfReq", 2);
    RegMethod("UnregIfProv", 4);
    RegMethod("SetObserver", 1);
    RegMethod("OnCompAdding", 1);
    RegMethod("OnCompChanged", 2);
    RegMethod("OnCompDeleting", 2);
    RegMethod("OnChanged", 1);
    RegMethod("CompsCount", 0);
    RegMethod("GetComp", 1);
    RegMethod("GetComp#2", 2);
    RegMethod("AppendMutation", 1);
    RegMethod("AppendMutation#2", 1);
    RegMethod("IsRemoved", 0);
    RegMethod("IsHeirOf", 1);
    RegMethod("IsComp", 1);
    RegMethod("OnNodeMutated", 3);
    RegMethod("IsCompAttached", 1);
    RegMethod("GetCompLrank", 1);
    RegMethod("GetCRoot", 0);
    RegMethod("IsChromoAttached", 0);
    RegMethod("GetCompOwning", 1);
    RegMethod("RemoveChild", 1);
    RegMethod("AppendChild", 1);
    RegMethod("AppendComp", 1);
    RegMethod("RemoveComp", 1);
    RegMethod("OnChildDeleting", 1);
    RegMethod("Delete", 0);
    RegMethod("ContentExists", 1);
    RegMethod("ContValueExists", 1);
    RegMethod("GetContCount", 1);
    RegMethod("GetContComp", 2);
    RegMethod("SetParent", 1);
    RegMethod("IsContOfCategory", 2);
}

// TODO [YB] Seems to get obsolete (ref Ifu::Pack<const TICacheRCtx&>. To remove.
void MElem::EIfu::FromCtx(const TICacheRCtx& aCtx, string& aRes)
{
    for (TICacheRCtx::const_iterator it = aCtx.begin(); it != aCtx.end(); it++) {
	Base* rq = *it;
	if (rq != NULL) {
	    MElem* re = rq->GetObj(re);
	    if (it == aCtx.begin()) {
		aRes += KArraySep;
	    }
	    string uri = re->GetUri(NULL, ETrue);
	    aRes += uri;
	}
    }
}

void MElem::EIfu::ToCtx(MElem* aHost, const string& aString, TICacheRCtx& aCtx)
{
    size_t end = 0;
    size_t beg = end + 1;
    do {
	beg = end + 1;
	size_t mid = beg;
	// Find first non-escaped separator
	do {
	    end = aString.find_first_of(KArraySep, mid); 
	    mid = end + 1;
	} while (end != string::npos && aString.at(end - 1) == KEsc);
	string elem = aString.substr(beg, (end == string::npos) ? string::npos : end - beg);
	MElem* rq = aHost->GetNode(elem);
	aCtx.push_back(rq);
    } while (end != string::npos);
}

string MElem::GetContentOwner(const string& aCont)
{
    string res;
    size_t pos = aCont.find_last_of(Elem::KContentSep);
    if (pos != string::npos) {
	res = aCont.substr(0, pos);
    }
    return res;
}

string MElem::GetContentLName(const string& aName)
{
    string res;
    size_t end = aName.find_last_of(Elem::KContentSep);
    if (end != string::npos) {
	res = aName.substr(end + 1);
    } else {
	res = aName;
    }
    return res;
}

TICacheRCtx::TICacheRCtx(): vector<Base*>() {}

TICacheRCtx::TICacheRCtx(const RqContext* aCtx)
{
    TICacheRCtx res;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	res.push_back(rq);
	cct = cct->Ctx();
    }
    TInt size = res.size();
    reserve(size);
    for (TICacheRCtx::const_reverse_iterator it = res.rbegin(); it != res.rend(); it++) {
	Base* rr = *it;
	push_back(rr);
    }
}



string Elem::PEType()
{
    return string() + GUri::KParentSep + Elem::Type();
}

/*
void Elem::ToCacheRCtx(const RqContext* aCtx, TICacheRCtx& aCct) 
{
    TICacheRCtx res;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	res.push_back(rq);
	cct = cct->Ctx();
    }
    TInt size = res.size();
    aCct.reserve(size);
    for (TICacheRCtx::const_reverse_iterator it = res.rbegin(); it != res.rend(); it++) {
	Base* rr = *it;
	aCct.push_back(rr);
    }
}
*/

Elem::IfIter::IfIter(Elem* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd): iHost(aHost), iIName(aIName), iReq(aReq) 
{
    TICacheKeyF query(iIName, iReq);
    iQFRange = iHost->iICacheQF.equal_range(query);
    if (iQFRange.first != iQFRange.second) {
	iQFIter = iQFRange.first;
	if (aToEnd) {
	    TICacheQFIter tmp(iQFIter);
	    TICacheQFIter qfend(iQFIter);
	    while (++tmp != iQFRange.second) qfend = tmp;
	    iQFIter = qfend;
	    iCacheRange = iHost->iICache.equal_range(iQFIter->second);
	    iCacheIter = iCacheRange.second;
	}
	else {
	    for (TICacheQFIter it = iQFRange.first; it != iQFRange.second; it++) {
		iQFIter = it;
		iCacheRange = iHost->iICache.equal_range(iQFIter->second);
		iCacheIter = iCacheRange.first;
		if (iCacheRange.first != iCacheRange.second) {
		    break;
		}
	    }
	}
    }
}

Elem::IfIter::IfIter(const IfIter& aIt): iHost(aIt.iHost), iIName(aIt.iIName), iReq(aIt.iReq), iQFRange(aIt.iQFRange),
    iQFIter(aIt.iQFIter), iCacheRange(aIt.iCacheRange), iCacheIter(aIt.iCacheIter)
{
}

Elem::MIfIter& Elem::IfIter::operator=(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    iHost = it.iHost; 
    iIName = it.iIName; iReq = it.iReq;
    iQFRange = it.iQFRange;
    iQFIter = it.iQFIter;
    iCacheRange = it.iCacheRange;
    iCacheIter = it.iCacheIter;
    return *this;
}

Elem::MIfIter* Elem::IfIter::Clone() const
{
    return new IfIter(*this);
}

Elem::MIfIter& Elem::IfIter::operator++()
{
    if (++iCacheIter != iCacheRange.second) {
    }
    else {
	TICacheQFIter it = iQFIter;
	for (++it; it != iQFRange.second; it++) {
	    iQFIter = it;
	    iCacheRange = iHost->iICache.equal_range(iQFIter->second);
	    iCacheIter = iCacheRange.first;
	    if (iCacheRange.first != iCacheRange.second) {
		break;
	    }
	}
    }
    return *this;
}

TBool Elem::IfIter::operator==(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    TBool res = (iHost == it.iHost && iIName == it.iIName && iReq == it.iReq && iQFRange == it.iQFRange 
	    && iQFIter == it.iQFIter && iCacheRange == it.iCacheRange && iCacheIter == it.iCacheIter);
    return res;
}

void*  Elem::IfIter::operator*()
{
    void* res = iCacheIter->second;
    return res;
}


void Elem::Delay(long us)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int cur_us = tp.tv_sec * 1000000 + tp.tv_usec;
    long int end_us = cur_us + us;
    while (cur_us < end_us) {
	gettimeofday(&tp, NULL);
	cur_us = tp.tv_sec * 1000000 + tp.tv_usec;
    };
}

// Element

Elem::Elem(const string &aName, MElem* aMan, MEnv* aEnv): iName(aName), iMan(aMan), iEnv(aEnv),
    iObserver(NULL), iParent(NULL), isRemoved(EFalse), mContext(NULL)
{
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Constr";
    Logger()->Write(EInfo, this, ss.str().c_str());
    */
    //Logger()->Write(EInfo, this, "Elem constr p0");
    //Delay(100);
    //Logger()->Write(EInfo, this, "Elem constr p1");
    iMut = Provider()->CreateChromo();
    //Logger()->Write(EInfo, this, "Elem constr p2");
    iMut->Init(ENt_Node);
    //Logger()->Write(EInfo, this, "Elem constr p3");
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    //Logger()->Write(EInfo, this, "Elem constr p4");
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetParent(Type());
    InsertContent(KCont_About);
    //InsertContent(KCont_Categories);
    //InsertContent(KCont_Ctg_Readonly);
}

Elem::Elem(Elem* aMan, MEnv* aEnv): iName(Type()), iMan(aMan), iEnv(aEnv),
    iObserver(NULL), iParent(NULL), isRemoved(EFalse), mContext(NULL)
{
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Constr";
    Logger()->Write(EInfo, this, ss.str().c_str());
    */
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetParent(string());
    InsertContent(KCont_About);
    //InsertContent(KCont_Categories);
    //InsertContent(KCont_Ctg_Readonly);
}

void Elem::Delete()
{
    delete this;
}

Elem::~Elem() 
{
    // Notify the man of deleting
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Destr";
    Logger()->Write(EInfo, this, ss.str().c_str());
    */

    if (iParent != NULL) {
	iParent->OnChildDeleting(this);
    }
    // Unregigster ifaces providers
    UnregAllIfRel();
    // Remove the comps, using iterator refresh because the map is updated on each comp deletion
    auto it = iMComps.begin();
    while (it != iMComps.end()) {
	delete it->second;
	it = iMComps.begin();
    }
    iMComps.clear();
    // TODO [YB] OnCompDeleting called only here, i.e. after Elem derivation desctuctor completed
    // This means that the ovner notified will not be able to understand what agent is deleted
    // To consider to add one more notification - before deletions
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, EFalse, ETrue);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(*this, EFalse, ETrue);
    }

    // Disconnect from the childs
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	MElem* child = it->second;
	child->OnParentDeleting(this);
	//child->SetParent(NULL);
    }
    iChilds.clear();
    // Notify env of root deletion, ref ds_daa_rdo
    if (this == GetRoot()) {
	iEnv->OnRootDeleted();
    }
    iEnv = NULL; // Not owned
    if (iMut != NULL) {
	delete iMut;
	iMut = NULL;
    }
    if (iChromo != NULL) {
	// Remove chromo only if it is deattached from parent's chromo. Ref UC_016. Normally the parent does
	// not deattach the childs chromo even if the child is deleted. This allows to keep node creation 
	// mutation in parents chromo even if the child is deleted then. 
	ChromoNode::Iterator pint = iChromo->Root().Parent();
	if (pint == iChromo->Root().End()) {
	    delete iChromo;
	}
	iChromo = NULL;
    }
}

string Elem::PName() const
{
    ChromoNode croot = iChromo->Root();
    return croot.Attr(ENa_Parent);
}

void Elem::SetParent(const string& aParent)
{
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Parent, aParent);
}

void Elem::SetMan(MElem* aMan)
{
    __ASSERT(iMan == NULL && aMan != NULL || aMan == NULL);
    // TODO [YB] To add notifications here
    iMan = aMan;
}

void Elem::SetObserver(MAgentObserver* aObserver)
{
    __ASSERT(iObserver == NULL && aObserver != NULL || iObserver != NULL && aObserver == NULL);
    iObserver = aObserver;
}

MElem* Elem::GetMan()
{
    return iMan;
}

const MElem* Elem::GetMan() const
{
    return iMan;
}

void Elem::GetCRoot(TMut& aMut) const
{
    aMut = iChromo->Root();
}

unique_ptr<MChromo> Elem::GetFullChromo() const
{
    MChromo *spec = Provider()->CreateChromo();
    spec->Set(iChromo->Root());
    ChromoNode& croot = spec->Root();;
    for (ChromoNode::Iterator it = croot.Begin(); it != croot.End(); it++) {
	ChromoNode node = *it;
	// TODO [YB] This will not work for the case of renaming. To redesign.
	const MElem* targ = this;
	if (node.AttrExists(ENa_MutNode)) {
	    GUri targ_uri(node.Attr(ENa_MutNode));
	    // Including removed comps into lookup, ref ds_daa_chrc_rm
	    targ = ((MElem*) this)->GetNode(targ_uri, ETrue);
	    if (targ == NULL) {
		targ = ((MElem*) this)->GetNode(targ_uri, ETrue);
		//__ASSERT(targ != NULL);
		Logger()->Write(EErr, this, "Getting full chromo, cannot find comp [%s]", node.Attr(ENa_MutNode).c_str());
	    }
	}
	if (targ != NULL) {
	    GUri prnu(node.Attr(ENa_Parent));
	    MElem* comp = targ->GetComp(prnu.GetName(), node.Name());
	    if (comp != NULL) {
		unique_ptr<MChromo> cchromo = comp->GetFullChromo();
		ChromoNode comproot = cchromo->Root();
		for (ChromoNode::Iterator itc = comproot.Begin(); itc != comproot.End(); itc++) {
		    node.AddChild(*itc);
		}
	    }
	}
    }
    return unique_ptr<MChromo>(spec);
}

void *Elem::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MOwner::Type()) == 0) {
	res = (MOwner*) this;
    } else if (strcmp(aName, MElem::Type()) == 0) {
	void* vv = (MElem*) this;
	MElem* me = (MElem*) this;
	MIface* mei = (MIface*) me;
	MElem* mev = (MElem*) vv;
	MIface* meiv = (MIface*) vv;
	res = mei;
	//res = (MElem*) this;
    }
    return res;
}

Elem::TIfRange Elem::GetIfi(const string& aName, const RqContext* aCtx)
{
    // Get from cache first
    // Add NULL to context in case of anonymous context
    // This is required to avoid problem with If relations invalidating
    // The matter is that the initial requestor doesnt cache relation into its cache
    // So on invalidation provider checks if context is initial (size == 1), if so it ignore relation
    // But it is possible that size == 1 for second requestor if the initial makes anonimous request
    // To avoid this inconsistance we need to explicitly register even anonymous requestor
    RqContext newctx(NULL);
    const RqContext* ctx(aCtx == NULL ? &newctx: aCtx);
    TICacheRCtx req(ctx);
    //ToCacheRCtx(ctx, req);
    IfIter beg(this, aName, req);
    IfIter end(this, aName, req, ETrue);
    if (beg == end) {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: -->", aName.c_str());
	}
	// Invalid cache, update cache
	UpdateIfi(aName, ctx);
	beg = IfIter(this, aName, req);
	end = IfIter(this, aName, req, ETrue);
    } else {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: resolved from cache", aName.c_str());
	}
    }
    return TIfRange(TIfIter(beg), TIfIter(end));
}

void* Elem::GetSIfiC(const string& aName, Base* aRequestor)
{
    RqContext ctx(aRequestor);
    return GetSIfi(aName, &ctx);
}

void* Elem::GetSIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rg = GetIfi(aName, aCtx);
    if (rg.first != rg.second) {
	res = *(rg.first);
    }
    return res;
}

void* Elem::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert) 
{
    void* res = NULL;
    RqContext ctx(this);
    MElem* req = GetNode(aReqUri);
    __ASSERT(aReqAssert || (req != NULL));
    if (req != NULL) {
	TIfRange rg = req->GetIfi(aName, &ctx);
	if (rg.first != rg.second) {
	    res = *(rg.first);
	}
    }
    return res;
}

void Elem::UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx)
{
    TICacheKeyF query(aIfName, aCtx);
    TICacheQFRange rg = iICacheQF.equal_range(query);
    __ASSERT (rg.first != rg.second);

    for (TICacheQFIter it = rg.first; it != rg.second; it++) {
	// Unregister itself on next provider
	Base* prov = it->second.second;
	MElem* prove = prov->GetObj(prove);
	if (prove != this) {
	    const TICacheRCtx& ctx = it->first.second;
	    TICacheRCtx rctx(ctx);
	    rctx.push_back(this);
	    prove->UnregIfReq(it->first.first, rctx);
	}
	iICache.erase(it->second);
    }
    iICacheQF.erase(rg.first, rg.second);
}

// TODO [YB] To consider redesign. Currently rough mechanism of invalidation is utilized.
// For invalidation we unreg the requests not only from given provider but all of them with ctx
// same as for provider related records. This simplifies the whole iface requesting (no need to check
// invalidity on requesting phase, all records are gone) but requres to rerequest ifaces thru all
// potential providers again after invalidatiio from only one particular provider.
void Elem::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv)
{
    TICacheKeyF query(aIfName, aCtx);
    TICacheQFRange rg = iICacheQF.equal_range(query);
    // Unfortunatelly we cannot check the conditions of record here. This is because
    // the track of providers is not stored in requests cache so it is possible
    // that for single request here, i.e. resolved by one provider, this provider can 
    // have more that one requests, i.e multiple providers. So it is possible that
    // provider requests unreg request but it is already unreged
    TICacheQFIter it = rg.first; 
    while (it != rg.second) {
	Base* cnd = it->second.second;
	MElem* cnde = cnd->GetObj(cnde);
	if (it->second.second == aProv) {
	    // Unregister next requestor
	    // TODO [YB] Do we need to unregister this in requestor. In fact this
	    // is not invalidated but caller of UnregIfProv does
	    const TICacheRCtx& ctx = it->first.second;
	    if (ctx.size() > 1) {
		TICacheRCtx rctx(ctx);
		rctx.pop_back();
		Base* reqb = ctx.back();
		MElem* reqe = reqb->GetObj(reqe);
		reqe->UnregIfProv(it->first.first, rctx, this, aInv);
	    }
	    iICache.erase(it->second);
	    iICacheQF.erase(it);
	    rg = iICacheQF.equal_range(query);
	    it = rg.first; 
	}
	else {
	    it++; 
	}
    }
    if (aInv) {
	// Invalidate the interface, ref ds_u_20180408
	InvalidateIfCache(aIfName);
    }
}

void Elem::InvalidateIfCache(const string& aIfName)
{
    auto it = iICacheQF.begin();
    while (it != iICacheQF.end()) {
	if (it->first.first == aIfName) {
	    // Unreg self as provider
	    const TICacheRCtx& ctx = it->first.second;
	    if (ctx.size() > 1) {
		TICacheRCtx rctx(ctx);
		rctx.pop_back();
		Base* reqb = ctx.back();
		MElem* reqe = reqb->GetObj(reqe);
		reqe->UnregIfProv(it->first.first, rctx, this, true);
	    }
	    // Unreg self as requestor
	    Base* prov = it->second.second;
	    MElem* prove = prov->GetObj(prove);
	    if (prove != this) {
		const TICacheRCtx& ctx = it->first.second;
		TICacheRCtx rctx(ctx);
		rctx.push_back(this);
		prove->UnregIfReq(it->first.first, rctx);
	    }
	    iICache.erase(it->second);
	    iICacheQF.erase(it);
	    it = iICacheQF.begin();
	} else {
	    it++;
	}
    }
}

void Elem::UnregAllIfRel(TBool aInv)
{
    TICacheQFIter it = iICacheQF.begin();
    for (TICacheQFIter it = iICacheQF.begin(); it != iICacheQF.end(); it++) {
	// Unregister itself on requestor
	const TICacheRCtx& ctx = it->first.second;
	if (ctx.size() > 1) {
	    TICacheRCtx rctx(ctx);
	    rctx.pop_back();
	    Base* reqb = ctx.back();
	    MElem* reqe = reqb->GetObj(reqe);
	    reqe->UnregIfProv(it->first.first, rctx, this, aInv);
	}
	// Unregister itself on provider
	Base* prov = it->second.second;
	MElem* prove = prov->GetObj(prove);
	if (prove != this) {
	    TICacheRCtx rctx(ctx);
	    rctx.push_back(this);
	    prove->UnregIfReq(it->first.first, rctx);
	}
    }
    iICache.clear();
    iICacheQF.clear();
}

void Elem::RmIfCache(IfIter& aIt)
{
    iICache.erase(aIt.iCacheIter);
}

void Elem::InvalidateIfCache()
{
    UnregAllIfRel(ETrue);
}

void Elem::InsertIfQm(const string& aName, const TICacheRCtx& aReq, Base* aProv)
{
    TICacheKeyF keyf(aName, aReq);
    TICacheKey key(keyf, aProv);
    // Push to query map if not already exists
    bool exists = false;
    TICacheQFRange frange = iICacheQF.equal_range(keyf);  
    for (TICacheQFIter it = frange.first; it != frange.second && !exists; it++) {
	if (it->second.second == aProv) {
	    exists = ETrue;
	}
    }
    if (!exists) {
	pair<TICacheKeyF, TICacheKey> qr(keyf, key);
	iICacheQF.insert(qr);
    }
}

void Elem::InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, void* aVal)
{
    TICacheKeyF keyf(aName, aReq);
    TICacheKey key(keyf, aProv);
    if (aVal != NULL) {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: resolved [%x], caching", aName.c_str(), aVal);
	}
	pair<TICacheKey, void*> val(key, aVal);
	TICacheCIter res = iICache.insert(val);
    }
    InsertIfQm(aName, aReq, aProv);
}

void Elem::InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, void* aVal)
{
    TICacheRCtx req(aCtx);
    //ToCacheRCtx(aCtx, req);
    InsertIfCache(aName, req, aProv, aVal);
}

void Elem::InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg)
{
    for (TIfIter it = aRg.first; it != aRg.second; it++) {
	InsertIfCache(aName, aReq, aProv, *it);
    }
    // Register the request in the map even if result is empty, ref ds_ifcache_refr_fpt
    if (aRg.first == aRg.second) {
	InsertIfQm(aName, aReq, aProv);
    }
}

void Elem::InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, TIfRange aRg)
{
    TICacheRCtx req(aCtx);
    //ToCacheRCtx(aCtx, req);
    InsertIfCache(aName, req, aProv, aRg);
}

void Elem::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = DoGetObj(aName.c_str());
    InsertIfCache(aName, aCtx, this, res);
}

void Elem::LogIfReqs()
{
    Logger()->Write(EInfo, this, "[%x], Ifaces requests: START", this);
    for (TICacheQFIter it = iICacheQF.begin(); it != iICacheQF.end(); it++) {
	const TICacheRCtx& ctx = it->first.second;
	Base* provb = it->second.second;
	Elem* prov = provb->GetObj(prov);
	if (!ctx.empty()) {
	    Base* reqb = ctx.back();
	    Elem* reqe = reqb == NULL ? NULL : reqb->GetObj(reqe);
	    Logger()->Write(EInfo, NULL, "If: [%s], [%i][%x: %s] - [%x: %s]", it->first.first.c_str(),
		    ctx.size(), reqe, reqe == NULL ? "NULL" : reqe->GetUri(NULL, ETrue).c_str(), prov, prov->GetUri(NULL, ETrue).c_str());
	}
	else {
	    Logger()->Write(EInfo, NULL, "If: [%s], [none] - [%x: %s]", it->first.first.c_str(),
		    prov, prov->GetUri(NULL, ETrue).c_str());
	}
    }
    Logger()->Write(EInfo, this, "Ifaces requests: END");
}

void Elem::DumpIfReqs() const
{
    cout << "<< Ifaces requests: START >>" << endl;
    for (auto it : iICacheQF) {
	const TICacheRCtx& ctx = it.first.second;
	Base* provb = it.second.second;
	Elem* prov = provb->GetObj(prov);
	if (!ctx.empty()) {
	    cout << "[" << it.first.first << "], [" << ctx.size() << "]:" << endl;
	    for (auto* reqb : ctx) {
		Elem* reqe = reqb == NULL ? NULL : reqb->GetObj(reqe);
		cout << reqb << "~" << reqe << ": " << (reqe == NULL ? "NULL" : reqe->GetUri(NULL, ETrue)) << endl;
	    }
	    cout << "==> [" << provb << "~" << prov << "," << prov->GetUri(NULL, ETrue) << "]" << endl << endl;
	}
	else {
	    cout << "If: [" << it.first.first << "], [none] - [" << provb << "~" << prov << "," << prov->GetUri(NULL, ETrue) << "]" << endl;
	}
    }
    cout << "<< Ifaces requests: END >>" << endl;
}

string Elem::EType(TBool aShort) const
{
    if (iParent == NULL) {
	return string();
    }
    if (aShort) {
	return iParent->Name();
    }
    else {
	return iParent->EType(aShort) + GUri::KParentSep + iParent->Name();
    }
}

// Stated restriction: name to be unique, ref. ds_mut_nm

#if 0
MElem* Elem::GetComp(const string& aParent, const string& aName) const
{
    MElem* res = NULL;
    if (iMComps.count(TNMKey(aName)) > 0) {
	res = iMComps.find(TNMKey(aName))->second;
    }
    return res;
}
#endif


MElem* Elem::GetComp(const string& aParent, const string& aName) const
{
    MElem* res = NULL;
    pair<TNMReg::const_iterator, TNMReg::const_iterator> range = iMComps.equal_range(TNMKey(aName));
    for (TNMReg::const_iterator it = range.first; it != range.second && res == NULL; it++) {
	MElem* node = it->second;
	if (node->EType() == aParent) {
	    res = node;
	}
    }
    return res;
}

TBool Elem::RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes)
{
    __ASSERT(aUri.IsAbsolute());
    TBool res = EFalse;
    if (aUri.IsErr()) return res;
    GUri::const_elem_iter it = aUri.Elems().begin();
    if (it != aUri.Elems().end()) {
	TBool anywhere = it->name() == GUri::KTypeAnywhere;
	__ASSERT(!anywhere);
	if (it->name().empty()) {
	    MElem* root = NULL;
	    if (it->rel() == GUri::EComp) {
		root = GetRoot();
	    } else if (it->rel() == GUri::EChild) {
		root = GetInhRoot();
	    }
	    it++;
	    GUri::TElem elem = *it;
	    anywhere = elem.name() == GUri::KTypeAnywhere;
	    TBool any = elem.name() == GUri::KTypeAny;
	    if (!anywhere && !any && root->Name() != elem.name()) {
		root = NULL;
	    }
	    if (root == aBase) {
		aRes.AppendTail(aUri, ++it);
		return ETrue;
	    }
	    if (root != NULL && ++it != aUri.Elems().end()) {
		res = root->RebaseUri(aUri, it, anywhere, aBase, aRes);
	    }
	} else {
	    res = RebaseUri(aUri, it, EFalse, aBase, aRes);
	}
    }
    return res;
}

TBool Elem::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes)
{
    __ASSERT(!aAnywhere);
    TBool res = EFalse;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    TBool anywhere = aAnywhere;
    if (elem.name() == "..") {
	if (iMan == aBase) {
	    aRes.AppendTail(aUri, ++uripos);
	    return ETrue;
	}
	if (iMan != NULL) {
	    if (++uripos != aUri.Elems().end()) {
		res = iMan->RebaseUri(aUri, uripos, EFalse, aBase, aRes);
	    }
	} else {
	    __ASSERT(EFalse);
	    Logger()->Write(EErr, this, "Rebasing uri [%s] - path to top of root", aUri.toString().c_str());
	}
    } else {
	if (!anywhere && elem.name() == GUri::KTypeAnywhere) {
	    elem = GUri::TElem(GUri::KTypeAny, GUri::EChild, GUri::KTypeAny);
	    anywhere = ETrue;
	    __ASSERT(!anywhere);
	}
	TBool isnative = elem.rel() == GUri::ENone;
	if (isnative) {
	    // Native node, not embedded to hier, to get from environment, ref uc_045
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		Elem* node = Provider()->GetNode(elem.name());
		if (node == aBase) {
		    aRes.AppendTail(aUri, ++uripos);
		    return ETrue;
		}
		if (node != NULL) {
		    res = node->RebaseUri(aUri, uripos, EFalse, aBase, aRes);
		}
	    }
	} else {
	    if (iMComps.count(elem.name()) > 0) {
		MElem* node = iMComps.at(elem.name());
		uripos++;
		if (uripos != aUri.Elems().end()) {
		    if (node == aBase) {
			aRes.AppendTail(aUri, uripos);
			return ETrue;
		    }
		    res = node->RebaseUri(aUri, uripos, EFalse, aBase, aRes);
		}
	    }
	}
    }
    return res;
}

MElem* Elem::GetNode(const GUri& aUri, TBool aInclRm) 
{ 
    if (aUri.IsErr()) return NULL;
    MElem* res = NULL;
    GUri::const_elem_iter it = aUri.Elems().begin(); 
    if (it != aUri.Elems().end()) {
	TBool anywhere = (*it).name() == GUri::KTypeAnywhere; 
	if (it->name().empty() || anywhere) {
	    MElem* root = NULL;
	    if (it->rel() == GUri::EComp) {
		// Using env's root to get root, because of in case of native agent
		// GetRoot() doesn't work, ref ds_di_nacgna
		root = iEnv->Root()->GetRoot();
	    }
	    else if (it->rel() == GUri::EChild) {
		root = GetInhRoot();
	    }
	    if (!anywhere) {
		it++;
		GUri::TElem elem = *it;
		anywhere = elem.name() == GUri::KTypeAnywhere; 
		TBool any = elem.name() == GUri::KTypeAny;
		if (!anywhere && !any && root->Name() != elem.name()) {
		    root = NULL;
		}
	    }
	    if (root != NULL && ++it != aUri.Elems().end()) {
		res = root->GetNode(aUri, it, anywhere, aInclRm);
	    }
	    else {
		res = root;
	    }
	}
	else {
	    res = GetNode(aUri, it, EFalse, aInclRm); 
	}
    }
    else {
	res = this;
    }
    return res;
}

MElem* Elem::GetNodeS(const char* aUri)
{
    MElem* mel = GetNode(aUri, ETrue);
    return mel;
}

MElem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm) 
{
    MElem* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    __ASSERT(elem.rel() != GUri::EChild);
    __ASSERT(!aAnywhere);
    if (elem.name() == "..") {
	// Checking if mutation context is defined. Using context instead of owner if so, ref ds_daa_itn_sfo
	MElem* top = (mContext == NULL ? iMan : mContext);
	if (top != NULL) {
	    if (++uripos != aUri.Elems().end()) {
		res = top->GetNode(aUri, uripos, EFalse, aInclRm);
	    } else {
		res = top;
	    }
	} else {
	    Logger()->Write(EErr, this, "Getting node [%s] - path to top of root", aUri.toString().c_str());
	}
    }
    else {
	TBool isnative = elem.rel() == GUri::ENone;
	if (isnative) {
	    // Native node, not embedded to hier, to get from environment, ref uc_045
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		Elem* node = Provider()->GetNode(elem.name());
		if (node != NULL) {
		    res = node->GetNode(aUri, uripos, EFalse, aInclRm);
		}
	    } else {
		res = Provider()->GetNode(elem.name());
	    }
	} else {
	    if (iMComps.count(elem.name()) > 0) {
		MElem* node = iMComps.at(elem.name());
		uripos++;
		if (uripos != aUri.Elems().end()) {
		    res = node->GetNode(aUri, uripos, EFalse, aInclRm);
		} else {
		    res = node;
		}
	    }
	}
    }
    return res;
}

void Elem::SetMutation(const ChromoNode& aMuta)
{
    iMut->Set(aMuta);
}

void Elem::SetMutation(const string& aMutSpec)
{
    iMut->SetFromSpec(aMutSpec);
}

ChromoNode Elem::AppendMutation(const ChromoNode& aMuta)
{
    return iMut->Root().AddChild(aMuta);
}

ChromoNode Elem::AppendMutation(TNodeType aType)
{
    return iMut->Root().AddChild(aType);
}

TBool Elem::AppendMutation(const string& aFileName)
{
    TBool res = EFalse;
    Chromo *spec = Provider()->CreateChromo();
    res = spec->Set(aFileName);
    if (res) {
	iMut->Root().AddChild(spec->Root(), ETrue);
	res = ETrue;
    }
    delete spec;
    return res;
}

void Elem::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    ChromoNode& root = iMut->Root();
    DoMutation(root, aRunTimeOnly, aCheckSafety, aTrialMode, aCtx == NULL ? this: aCtx);
    // Clear mutation
    for (ChromoNode::Iterator mit = root.Begin(); mit != root.End();)
    {
	ChromoNode node = *mit;
	mit++; // It is required because removing node by iterator breakes iterator itself
	root.RmChild(node);
    }
}

void Elem::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    DoMutation(aMutsRoot, aRunTimeOnly, aCheckSafety, aTrialMode, aCtx == NULL ? this: aCtx);
}

void Elem::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    const ChromoNode& mroot = aMutSpec;
    if (mroot.Begin() == mroot.End()) return;
    ChromoNode sroot = *(mroot.Root());
    TInt tord = 0;
    TInt lim = 0;
    TInt order = 0;
    TBool isattached = EFalse;
    if (EN_MUT_LIM && this == GetRoot()) {
	tord = iEnv->ChMgr()->GetSpecMaxOrder();
	lim = iEnv->ChMgr()->GetLim();
	isattached = IsChromoAttached();
    }
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++, order++)
    {
	ChromoNode rno = (*rit);
	// Omit inactive mutations
	if (iEnv->ChMgr()->EnableOptimization() && rno.AttrExists(ENa_Inactive)) {
	    if (!aRunTime) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	Logger()->SetContextMutId(rno.LineId());
	if (EN_MUT_LIM && this == GetRoot()) {
	    // Avoiding mutations above limit. Taking into account only attached chromos.
	    if (EN_MUT_LIM && isattached && tord > 0 && order > tord - lim) {
		if (!aRunTime && !aTrialMode) {
		    iChromo->Root().AddChild(rno);
		}
		continue;
	    }
	}
	if (rno.AttrExists(ENa_MutNode)) {
	    // Transform DHC mutation to OSM mutation
	    // Transform ENa_Targ: enlarge to ENa_MutNode
	    MElem* targ = rno.AttrExists(ENa_Targ) ? GetNode(rno.Attr(ENa_Targ)) : this;
	    if (targ == NULL) {
		Logger()->Write(EErr, this, "Cannot find node [%s]", rno.Attr(ENa_Targ).c_str());
		continue;
	    }
	    MElem* iftarg = targ->GetNode(rno.Attr(ENa_MutNode));
	    if (iftarg == NULL) {
		Logger()->Write(EErr, this, "Cannot find target node [%s]", rno.Attr(ENa_MutNode).c_str());
		continue;
	    }
	    // Extract "comp" attr for component related mutations
	    MElem* ftarg = iftarg;
	    TNodeType mtype = rno.Type();
	    if (mtype == ENt_Rm || mtype == ENt_Change) {
		// For components related muts correct targ to owner
		ftarg = iftarg->GetMan();
		string sturi = iftarg->GetUri(ftarg, ETrue);
		rno.SetAttr(ENa_Comp, sturi);
	    }
	    if (ftarg != this) {
		string sturi = ftarg->GetUri(this, ETrue);
		rno.SetAttr(ENa_Targ, sturi);
	    }
	    rno.RmAttr(ENa_MutNode);
	    // Transform refs
	    if (rno.AttrExists(ENa_Parent)) {
		string prnturi = rno.Attr(ENa_Parent);
		MElem* parent = targ->GetNode(prnturi);
		if (parent == NULL) {
		    // Probably external node not imported yet - ask env for resolving uri
		    GUri pruri(prnturi);
		    MImportMgr* impmgr = iEnv->ImpsMgr();
		    parent = impmgr->OnUriNotResolved(this, pruri);
		}
		if (parent == NULL) {
		    Logger()->Write(EErr, this, "Cannot find parent [%s]", prnturi.c_str());
		    continue;
		}
		string spuri = parent->GetUri(ftarg, ETrue);
		rno.SetAttr(ENa_Parent, spuri);
	    }
	}
	if (rno.AttrExists(ENa_Targ)) {
	    // Targeted mutation, propagate downward, i.e redirect to comp owning the target
	    // ref ds_mut_osm_linchr_lce
	    MElem* ftarg = GetNode(rno.Attr(ENa_Targ));
	    // Mutation is not local, propagate downward
	    if (ftarg != NULL) {
		//TMut madd(rno);
		rno.RmAttr(ENa_Targ);
		ChromoNode madd = ftarg->AppendMutation(rno);
		//madd.RmAttr(ENa_Targ);
		//ftarg->AppendMutation(madd);
		// Redirect the mut to target: no run-time to keep the mut in internal nodes
		// Propagate till target owning comp if run-time to keep hidden all muts from parent 
		ftarg->Mutate(EFalse, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
		//ftarg->Mutate(aRunTime, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
	    } else {
		Logger()->Write(EErr, this, "Cannot find target node [%s]", rno.Attr(ENa_Targ).c_str());
	    }
	} else {
	    // Local mutation
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		AddElem(rno, aRunTime, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Change) {
		ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Cont) {
		DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
	    }
	    else if (rnotype == ENt_Move) {
		MoveNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Import) {
		ImportNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Rm) {
		RmNode(rno, aRunTime, aCheckSafety, aTrialMode, aCtx);
	    }
	    else {
		Logger()->Write(EErr, this, "Mutating - unknown mutation type [%d]", rnotype);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

#ifdef UNICONTENT

TBool Elem::IsContOfCategory(const string& aName, const string& aCategory) const
{
    TBool res = EFalse;
    pair<TContent::const_iterator, TContent::const_iterator> range =
       	mContent.equal_range(TContentKey(aName, ECrl_Category));
    for (TContent::const_iterator it = range.first; it != range.second; it++) {
	if (it->second == aCategory) {
	    res = ETrue; break;
	}
    }
    return res;
}

// TODO To add handling of comp already exists
void Elem::InsertContCompsRec(const string& aName, const string& aComp)
{
    TContentKey key(aName, ECrl_Component);
    pair<TContent::const_iterator, TContent::const_iterator> range = mContent.equal_range(key);
    TBool exists = EFalse;
    for (TContent::const_iterator it = range.first; it != range.second; it++) {
	if (it->second == aComp) {
	    exists = ETrue;
	    break;
	}
    }
    if (!exists) {
	mContent.insert(TContentRec(key, aComp));
    }
}

void Elem::InsertContent(const string& aName)
{
    if (!aName.empty()) {
	string oname = GetContentOwner(aName);
	InsertContCompsRec(oname, aName);
	InsertContent(oname);
    }
}

void Elem::RemoveContent(const string& aName)
{
    // Value
    TContent::iterator it = mContent.find(TContentKey(aName, ECrl_Value));
    while (it != mContent.end()) {
	mContent.erase(it);
	it = mContent.find(TContentKey(aName, ECrl_Value));
    }
    // Components
    it = mContent.find(TContentKey(aName, ECrl_Component));
    while (it != mContent.end()) {
	RemoveContent(it->second);
	it = mContent.find(TContentKey(aName, ECrl_Component));
    }
}

void Elem::RemoveContentComp(const string& aOwner, const string& aComp)
{
    string fname = ContentCompId(aOwner, aComp);
    // Remove comps record
    pair<TContent::iterator, TContent::iterator> range = mContent.equal_range(TContentKey(aOwner, ECrl_Component));
    for (TContent::iterator it = range.first; it != range.second; it++) {
	if (it->second == fname) {
	    mContent.erase(it); break;
	}
    }
    // Remove comps content
    RemoveContent(fname);
}

void Elem::SetContentValue(const string& aName, const string& aValue)
{
    TContentKey key(aName, ECrl_Value);
    TInt count = mContent.count(key);
    if (count == 0) { // New value
	mContent.insert(TContentRec(key, aValue));
    } else if (count == 1) { // Existing value
	TContent::iterator it = mContent.find(key);
	it->second = aValue;
    } else { // Error
	__ASSERT(EFalse);
    }
}

void Elem::SetContentCategory(const string& aName, const string& aCategory)
{
    TContentKey key(aName, ECrl_Category);
    mContent.insert(TContentRec(key, aCategory));
}

TBool Elem::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) 
{
    TBool res = ETrue;
    size_t pos_beg = 0, pos = 0;
    if (!aVal.empty() && aVal.at(0) == KContentStart) { // Complex content
	string delims(1, KContentCompsSep); delims += KContentValQuote; delims += KContentEnd;
	size_t pos_beg = 1;
	// Parsing elements of content (value | comps | category)
	TBool end = EFalse;
	do { // By elements
	    // Passing thru elements separator
	    pos = aVal.find_first_not_of(KContentCompsSep, pos_beg);
	    //pos = Ifu::FindFirstCtrl(aVal, delims, pos_beg);
	    if (pos != string::npos) {
		char dl = aVal.at(pos);
		if (dl == KContentEnd) { // Complex content closing delimiter
		    end = ETrue;
		} else if (dl == KContentValQuote) { // Value
		    pos_beg = pos + 1;
		    pos = Ifu::FindFirstCtrl(aVal, KContentValQuote, pos_beg);
		    if (pos == string::npos) { // Error: missing value closing quote
			res = EFalse;
		    } else {
			string value = aVal.substr(pos_beg, pos - pos_beg);
			InsertContent(aName);
			SetContentValue(aName, value);
			pos_beg = pos + 1;
		    }
		} else if (dl == KContentPref_Category) { // Category
		    pos_beg = pos + 1;
		    delims = string(1, KContentCompsSep); delims += KContentEnd;
		    pos = aVal.find_first_of(delims, pos_beg);
		    if (pos != string::npos) {
			string category = aVal.substr(pos_beg, pos - pos_beg);
			SetContentCategory(aName, category);
			pos_beg = pos + 1;
		    } else { // Error: undelimited category
			__ASSERT(EFalse);
			res = EFalse;
		    }
		} else if (isalpha(dl)) { // Component
		    pos_beg = pos;
		    // Getting comp name
		    pos = aVal.find_first_of(KContentValSep, pos_beg);
		    if (pos == string::npos) { // Error: comps name separator is missing
			__ASSERT(EFalse);
			res = EFalse;
		    }
		    string sname = aVal.substr(pos_beg, pos - pos_beg);
		    pos_beg = pos + 1;
		    if (pos_beg >= aVal.size()) { // Error: Uncomplete comp
			__ASSERT(EFalse);
			res = EFalse;
		    } else {
			if (aVal.at(pos_beg) == KContentStart) { // Comps value is full content
			    pos = Ifu::FindRightDelim(aVal, KContentStart, KContentEnd, pos_beg + 1);
			    string value = aVal.substr(pos_beg, pos - pos_beg + 1);
			    ChangeCont(value, aRtOnly, ContentCompId(aName, sname));
			} else if (aVal.at(pos_beg) == KContentValQuote) { // Quoted value
			    pos_beg += 1;
			    pos = Ifu::FindFirstCtrl(aVal, KContentValQuote, pos_beg);
			    string value = aVal.substr(pos_beg, pos - pos_beg);
			    ChangeCont(value, aRtOnly, ContentCompId(aName, sname));
			} else if (aVal.at(pos_beg) == KContentDeletion) { // Deletion
			    RemoveContentComp(aName, sname);
			    pos = pos_beg;
			} else { // Error: Incorrect comp value start delimiter
			    res = EFalse;
			    __ASSERT(EFalse);
			}
		    }
		    if (pos == string::npos) { // Error: missing comp end delimiter
			res = EFalse;
			__ASSERT(EFalse);
		    }
		    pos_beg = pos + 1;
		} else { // Error: incorrect element
		    __ASSERT(EFalse);
		    res = EFalse;
		}
	    } else { // Error: missing closing conent delimiter
		__ASSERT(EFalse);
		res = EFalse;
	    }
	} while (!end && pos != string::npos && res); // By elements
    } else { // Bare value
	InsertContent(aName);
	SetContentValue(aName, aVal);
    }
    if (res) {
	res = OnCompChanged(*this, aName, aRtOnly);
    }
    return res;
}

TBool Elem::ContentHasComps(const string& aContName) const
{
    return (GetContCount(aContName) > 0);
}

string Elem::GetContent(const string& aId, TBool aFull) const
{
    string res;
    TContentKey valkey(aId, ECrl_Value);
    TBool valexists = ContValueExists(aId);
    if (!aFull) { // Simple content
	res = valexists ? GetContentValue(aId) : string();
    } else { // Full content
	TContentKey ctgkey(aId, ECrl_Category);
	TBool hascomps = ContentHasComps(aId);
	TBool ctgexist = mContent.count(ctgkey) > 0;
	if (valexists || hascomps || mContent.count(ctgkey) > 0) { // Not empty
	    res = KContentStart;
	    // Quoted value
	    if (valexists) {
		string val = GetContentValue(aId);
		res += KContentValQuote + val + KContentValQuote ;
	    }
	    if (ctgexist) { // Categories
		pair<TContent::const_iterator, TContent::const_iterator> range = 
		    mContent.equal_range(ctgkey);
		for (TContent::const_iterator it = range.first; it != range.second; it++) {
		    string ctg = it->second;
		    res += KContentCompsSep; res += KContentPref_Category + ctg;
		}
	    }
	    if (hascomps) { //  Components
		res += " ";
		for (TInt ind = 0; ind < GetContCount(aId); ind++) {
		    string comp = GetContComp(aId, ind);
		    if (ind != 0) {
			res += " ";
		    }
		    if (ContentHasComps(comp)) { // Full content of comp
			string compcnt = GetContent(comp, ETrue);
			res += GetContentLName(comp) + KContentValSep + compcnt;
		    } else { // Simple content
			string compcnt = GetContent(comp, EFalse);
			res += GetContentLName(comp) + KContentValSep + KContentValQuote + compcnt + KContentValQuote;
		    }
		}
	    }
	    res += KContentEnd;
	}
    }
    return res;
}

TBool Elem::ContentExists(const string& aName) const
{
    return (ContValueExists(aName) || ContentHasComps(aName));
}

TBool Elem::ContValueExists(const string& aName) const
{
    return (mContent.count(TContentKey(aName, ECrl_Value)) > 0);
}

string Elem::GetContComp(const string& aOwnerName, TInt aInd) const
{
    __ASSERT(aInd < GetContCount(aOwnerName));
    TContentKey compkey(aOwnerName, ECrl_Component);
    pair<TContent::const_iterator, TContent::const_iterator> range = mContent.equal_range(compkey);
    TContent::const_iterator it = range.first;
    for (TInt cnt = aInd; it != range.second && cnt > 0; it++, cnt--);
    return it->second;
}

TInt Elem::GetContCount(const string& aName) const
{
    return mContent.count(TContentKey(aName, ECrl_Component));
}

string Elem::GetContentValue(const string& aName) const
{
    string res;
    TContent::const_iterator it = mContent.find(TContentKey(aName, ECrl_Value));
    if (it != mContent.end()) {
	res = it->second;
    }
    return res;
}


void Elem::DumpContent() const
{
    cout << "<< Content >>" << endl;
    for (TContent::const_iterator it = mContent.begin(); it != mContent.end(); it++) {
	TCntRel rel = it->first.second;
	string srel;
	if (rel == ECrl_Component) srel = "[Comp]";
	else if (rel == ECrl_Category) srel = "[Category]";
	else if (rel == ECrl_Value) srel = "[Value]";
	else srel = "[???]";
	cout << it->first.first << " " << srel << " : " << it->second << endl;
    }
}

#else // UNICONTENT

void Elem::SetContentValue(const string& aName, const string& aValue)
{
}

// TODO Not used?
void Elem::InsertContentComp(const string& aContName, const string& aCompName)
{
}

TBool Elem::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) 
{
    TBool res = ETrue;
    size_t pos_beg = 0, pos = 0;
    if (aVal.empty()) { // Empty conent
	mCntVals[aName] = aVal;
	InsertContent(aName);
    } else if (aVal.at(0) == KContentStart) { // Complex content
	string delims(1, KContentValQuote); delims += KContentValSep;
	size_t pos_beg = 1;
	size_t pos = aVal.find_first_of(delims, pos_beg);
	if (pos == string::npos) { // Empty content
	} else { // Non-empty content
	    if (aVal.at(pos) == KContentValQuote) { // Value
		pos_beg = pos + 1;
		pos = Ifu::FindFirstCtrl(aVal, KContentValQuote, pos_beg);
		if (pos == string::npos) { // Error: missing value closing quote
		    res = EFalse;
		} else {
		    string value = aVal.substr(pos_beg, pos - pos_beg);
		    mCntVals[aName] = value;
		    InsertContent(aName);
		    pos_beg = pos + 1;
		}
	    } else { // Value is omitted
	    }
	    // Parsing comps
	    while (pos != string::npos && res) { // By comps
		pos = aVal.find_first_not_of(' ', pos_beg);
		__ASSERT(pos != string::npos);
		pos_beg = pos;
		// Getting comp name
		pos = Ifu::FindFirstCtrl(aVal, KContentValSep, pos_beg);
		if (pos == string::npos) break;
		string sname = aVal.substr(pos_beg, pos - pos_beg);
		pos_beg = pos + 1;
		if (pos_beg >= aVal.size()) { // Error: Uncomplete comp
		    res = EFalse; break;
		} else {
		    if (aVal.at(pos_beg) == KContentStart) { // Comps value is full content
			pos_beg += 1;
			pos = Ifu::FindFirstCtrl(aVal, KContentEnd, pos_beg);
		    } else if (aVal.at(pos_beg) == KContentValQuote) { // Quoted value
			pos_beg += 1;
			pos = Ifu::FindFirstCtrl(aVal, KContentValQuote, pos_beg);
		    } else { // Error: Incorrect comp value start delimiter
			res = EFalse; break;
		    }
		}
		if (pos == string::npos) { // Error: missing comp end delimiter
		    res = EFalse; break;
		} else {
		    string value = aVal.substr(pos_beg, pos - pos_beg);
		    ChangeCont(value, aRtOnly, ContentCompId(aName, sname));
		}
		pos_beg = pos + 1;
	    };
	}
    } else if (aVal.at(0) == KContentValQuote) { // Quoted value
	res = EFalse; // Disabled at the moment
	/*
	pos_beg = 1;
	pos = Ifu::FindFirstCtrl(aVal, KContentValQuote, pos_beg);
	if (pos == string::npos) { // Error: missing closing quote
	    res = EFalse;
	} else {
	    string value = aVal.substr(pos_beg, pos - pos_beg);
	    mCntVals[aName] = value;
	    InsertContent(aName);
	}
	*/
    } else { // Bare value
	mCntVals[aName] = aVal;
	InsertContent(aName);
    }
    if (res) {
	OnCompChanged(*this, aName);
    }
    return res;
}

void Elem::InsertContent(const string& aName)
{
    size_t end = aName.find_last_of(KContentSep);
    if (end != string::npos) {
	string oname = aName.substr(0, end);
	InsertContCompsRec(oname, aName);
	InsertContent(oname);
    } else {
	InsertContCompsRec("", aName);
    }
}

void Elem::InsertContentComp(const string& aContName, const string& aCompName)
{
    InsertContCompsRec(aContName, aCompName);
    InsertContent(aContName);
}

void Elem::InsertContCompsRec(const string& aName, const string& aComp)
{
    pair<TCntComps::const_iterator, TCntComps::const_iterator> range = mCntComps.equal_range(aName);
    TBool exists = EFalse;
    for (TCntComps::const_iterator it = range.first; it != range.second; it++) {
	if (it->second == aComp) {
	    exists = ETrue;
	    break;
	}
    }
    if (!exists) {
	mCntComps.insert(TCntRec(aName, aComp));
    }
}

TBool Elem::ContentHasComps(const string& aContName) const
{
    return (mCntComps.count(aContName) > 0);
}

string Elem::GetContent(const string& aId, TBool aFull) const
{
    string res;
    if (!aFull) { // Simple content
	res = (mCntVals.count(aId) > 0) ? mCntVals.at(aId) : string();
    } else if (mCntVals.count(aId) > 0 || mCntComps.count(aId) > 0) { // Full content, not empty
	res = KContentStart;
	// Quoted value
	if (mCntVals.count(aId) > 0) {
	    string val = mCntVals.at(aId);
	    res += KContentValQuote + mCntVals.at(aId) + KContentValQuote ;
	}
	//  Components
	TInt count = GetContCount(aId);
	/*
	if (count > 0) {
	    res += " ";
	    pair<TCntComps::const_iterator, TCntComps::const_iterator> range = mCntComps.equal_range(aId);
	    for (TCntComps::const_iterator it = range.first; it != range.second; it++) {
		string comp = it->second;
		if (it != range.first) {
		    res += " ";
		}
		if (ContentHasComps(comp)) { // Full content of comp
		    string compcnt = GetContent(comp, ETrue);
		    res += GetContentLName(comp) + KContentValSep + compcnt;
		} else { // Simple content
		    string compcnt = GetContent(comp, EFalse);
		    res += GetContentLName(comp) + KContentValSep + KContentValQuote + compcnt + KContentValQuote;
		}
	    }
	}
	*/

	if (count > 0) {
	    res += " ";
	    for (TInt ind = 0; ind < count; ind++) {
		string comp = GetContComp(aId, ind);
		if (ind != 0) {
		    res += " ";
		}
		if (ContentHasComps(comp)) { // Full content of comp
		    string compcnt = GetContent(comp, ETrue);
		    res += GetContentLName(comp) + KContentValSep + compcnt;
		} else { // Simple content
		    string compcnt = GetContent(comp, EFalse);
		    res += GetContentLName(comp) + KContentValSep + KContentValQuote + compcnt + KContentValQuote;
		}
	    }
	}
	res += KContentEnd;
    }
    return res;
}

TBool Elem::ContentExists(const string& aName) const
{
    return (mCntVals.count(aName) > 0) || (mCntComps.count(aName) > 0);
}

TBool Elem::ContValueExists(const string& aName) const
{
    return (mCntVals.count(aName) > 0);
}

string Elem::GetContComp(const string& aOwnerName, TInt aInd) const
{
    __ASSERT(aInd < GetContCount(aOwnerName));
    pair<TCntComps::const_iterator, TCntComps::const_iterator> range = mCntComps.equal_range(aOwnerName);
    TCntComps::const_iterator it = range.first;
    for (TInt cnt = aInd; it != range.second && cnt > 0; it++, cnt--);
    return it->second;
}

TInt Elem::GetContCount(const string& aName) const
{
    return mCntComps.count(aName);
}

void Elem::DumpCntVal() const
{
    cout << "<< Values >>" << endl;
    for (TCntVals::const_iterator it = mCntVals.begin(); it != mCntVals.end(); it++) {
	cout << it->first << ": " << it->second << endl;
    }
    cout << "<< Components >>" << endl;
    for (TCntComps::const_iterator it = mCntComps.begin(); it != mCntComps.end(); it++) {
	cout << it->first << ": " << it->second << endl;
    }
}

#endif // UNICONTENT

void Elem::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string snode = aSpec.Attr(ENa_Comp);
    string mattrs = aSpec.Attr(ENa_MutAttr);
    string mval = aSpec.Attr(ENa_MutVal);
    MElem* node = aSpec.AttrExists(ENa_Comp) ? GetNode(snode) : this;
    TBool mutadded = EFalse;
    if (node != NULL) {
	if (node != this && (epheno || node->GetAowner() == this  || IsDirectInheritedComp(node))) {
	    TBool res = node->ChangeAttr(TMut::NodeAttr(mattrs), mval);
	    if (!res) {
		Logger()->Write(EErr, this, "Changing node [%s] - failure", snode.c_str());
	    } else {
		// Adding dependency to object of change
		if (!aRunTime) {
		    ChromoNode chn = iChromo->Root().AddChild(aSpec);
		    NotifyNodeMutated(aSpec, aCtx);
		    mutadded = ETrue;
		}
	    }
	} else {
	    Logger()->Write(EErr, this, "Changing node [%s]  - attempt of phenotypic modification - disabled", snode.c_str());
	}
    } else {
	Logger()->Write(EErr, this, "Changing node [%s] - cannot find node or node isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
    }
}

TBool Elem::ChangeAttr(TNodeAttr aAttr, const string& aVal) 
{ 
    TBool res = EFalse;
    if (aAttr == ENa_Id) {
	string sOldName(Name());
	iName = aVal;
	res = iMan->OnCompRenamed(*this, sOldName);
	if (res && iParent != NULL) {
	    res = iParent->OnChildRenamed(this, sOldName);
	}
	if (!res) {
	    // Rollback
	    iName = sOldName;
	}
    }
    return res;
}

TBool Elem::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(!aSpec.AttrExists(ENa_MutNode));
    TBool res = ETrue;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    TBool efix = iEnv->ChMgr()->EnableFixErrors();
    TBool erepos = iEnv->ChMgr()->EnableReposMuts();
    string snode = aSpec.Attr(ENa_MutNode);
    string cname = aSpec.Attr(ENa_Id);
    TBool refex = aSpec.AttrExists(ENa_Ref);
    string mval = aSpec.Attr(refex ? ENa_Ref :ENa_MutVal);
    MElem* node = NULL;
    MElem* rnode = NULL;
    TBool mutadded = EFalse;
    if (snode.length() == 0) {
	node = this; 
    }
    else {
	GUri unode(snode);
	node = GetNode(unode);
    }
    if (node != NULL && (node == this || IsComp(node))) {
	// Only direct inherited comp is available for modif, ref. uc_044_disc_3
	if (epheno || node == this || (!aCheckSafety || IsDirectInheritedComp(node))) {
	    if (refex) {
		// For -ref- attr the value is the ref relative to mutated node context
		rnode = node->GetNode(mval);
		//mval = rnode->GetRUri(node);
		if (rnode == NULL) {
		    rnode = node->GetNode(mval);
		    Logger()->Write(EErr, this, aSpec,
			    "Changing content of node [%s] to ref [%s] - cannot find ref", snode.c_str(), mval.c_str());
		    res = EFalse;
		}
	    }
	    if (res) {
		// Ref ds_mut_osm_linchr_notif. With OSM (Original Sequence of Mutation) approach aRunTime can be set false
		// even if the mutation is parents mutation (inherited mutation) in order to keep all nodes chromo.
		// So we need to re-calculate run-time property: only if the mut is not to be included in root chromo
		// Mark change as persistent (not run-time) only if the change affects model chromo
		// TODO [YB] To cleanup, to remove run-time arg from notifications
		TBool persist = ETrue;
		res = node->ChangeCont(mval, !persist, cname);
		if (res) {
		    if (!aRunTime) {
			ChromoNode chn = iChromo->Root().AddChild(aSpec);
			NotifyNodeMutated(aSpec, aCtx);
			mutadded = ETrue;
		    }
		} else {
		    Logger()->Write(EErr, this, aSpec, "Changing node [%s] - failure", snode.c_str());
		    node->ChangeCont(mval, aRunTime, cname);
		}
	    }
	}
	else  {
	    Logger()->Write(EErr, this, "Changing content of node [%s]  - attempt of phenotypic modification - disabled", 
		    snode.c_str());
	}
    }
    else {
	Logger()->Write(EErr, this, "Changing node [%s] - cannot find node or node isn't comp", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	ChromoNode mut = iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
	TInt mutid = mut.LineId();
    }
    return res;
}

MElem* Elem::AddElem(const ChromoNode& aNode, TBool aRunTime, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(!aNode.AttrExists(ENa_MutNode));
    string snode = aNode.Attr(ENa_MutNode);
    string sparent = aNode.Attr(ENa_Parent);
    string sname = aNode.Name();
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    TBool erepos = iEnv->ChMgr()->EnableReposMuts();
    TBool ecsaf = iEnv->ChMgr()->EnableCheckSafety();
    TBool mutadded = EFalse;
    TBool res = EFalse;
    TBool ptrace = EN_PERF_TRACE;
    TBool ptevent = EFalse;
    //Log(TLog(EInfo, this) + "Adding element [" + sname + "]");
    //if (EN_PERF_METR && iName == "Held1" && sname == "1211685360"/* && sparent == "/(Elem:)Root/(Elem:)Modules/(Elem:)VisComps/(Extender:)DrawingElemExt"*/) {
    //if (EN_PERF_METR && EN_PERF_DBG1 && iName == "DrawingElem" && sname == "Logspec") {
    if (EN_PERF_METR && EN_PERF_DBG1 && iName == "Velocity_L" && sname == "448726636") {
	Logger()->Write(EInfo, this, "Point_1");
	ptrace = ETrue;
	iEnv->SetSBool(MEnv::ESb_EnPerfTrace, ETrue);
	ptevent = ETrue;
    }
    /* Profiling using wall-clock. It doesn't shows time interval within given thread, other threads are affecting
       struct timeval tp;
       gettimeofday(&tp, NULL);
       long int t1 = tp.tv_sec * 1000000 + tp.tv_usec;
       */
    //timespec t1;
    //clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t1);
    if (ptrace) Logger()->Write(EInfo, this, "Adding node [%s], t1", sname.c_str());
    long t1 = GetClock();

    __ASSERT(!sname.empty());
    /*
       if (IsLogeventCreOn()) {
       Logger()->Write(EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
       }
       */
    //Logger()->Write(EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
    MElem* elem = NULL;
    MElem* node = snode.empty() ? this: GetNode(snode); 
    if (node != NULL) {
	// Obtain parent first
	MElem *parent = NULL;
	// Check if the parent is specified
	if (!sparent.empty()) {
	    // Check the parent scheme
	    GUri prnturi(sparent);
	    TBool ext_parent = ETrue;
	    if (prnturi.Scheme().empty()) {
		// Local parent
		parent = GetNode(prnturi);
		if (parent == NULL) {
		    // Probably external node not imported yet - ask env for resolving uri
		    GUri pruri(prnturi);
		    MImportMgr* impmgr = iEnv->ImpsMgr();
		    parent = impmgr->OnUriNotResolved(this, pruri);
		}
		ext_parent = EFalse;
	    }
	    else {
		// TODO [YB] To add seaching the module - it will allow to specify just file of spec wo full uri
		Chromo *spec = Provider()->CreateChromo();
		res = spec->Set(sparent);
		if (res) {
		    const ChromoNode& root = spec->Root();
		    parent = AddElem(root);
		    delete spec;
		}
	    }
	    if (parent != NULL) {
		if (epheno || node == this || !ecsaf || IsDirectInheritedComp(node)) {
		    // Create heir from the parent
		    if (ptrace) { long t1_2 = GetClockElapsed(t1); Logger()->Write(EInfo, this, "Adding node, t1-t2: %d", t1_2);}
		    long t3; if (ptrace) t3 = GetClock();
		    elem = parent->CreateHeir(sname, node, NULL);
		    if (ptrace) { long t3_4 = GetClockElapsed(t3); Logger()->Write(EInfo, this, "Adding node, t3-t4: %d", t3_4);}
		    // Remove external parent from system
		    // [YB] DON'T remove parent, otherwise the inheritance chain will be broken
		    if (ext_parent) {
			// delete parent;
		    }
		    if (elem != NULL) {
			// TODO [YB] Seems to be just temporal solution. To consider using context instead.
			// Make heir based on the parent: re-parent the heir (currently it's of grandparent's parent) and clean the chromo
			ChromoNode hroot = elem->Chromos().Root();
			hroot.SetAttr(ENa_Parent, sparent);
			if (!snode.empty()) {
			    hroot.SetAttr(ENa_MutNode, snode);
			}
			// Heir has been created, now we can establish solid two-ways relations, ref. ds_daa_hunv
			//
			// Mark change as persistent (not run-time) only if the change affects model chromo, ref ds_mut_osm_linchr_notif.
			/*
			TBool persist = ETrue;
			elem->SetMan(NULL);
			res = node->AppendComp(elem, !persist);
			*/
			elem->SetParent(NULL);
			res = parent->AppendChild(elem);
			if (res) {
			    if (!aRunTime) {
				// Copy just top node, not recursivelly, ref ds_daa_chrc_va
				ChromoNode ech = elem->Chromos().Root();
				ChromoNode chn = iChromo->Root().AddChild(ech, ETrue, EFalse);
				NotifyNodeMutated(ech, aCtx);
				mutadded = ETrue;
			    } else {
				mutadded = ETrue;
			    }
			    // Mutate object ignoring run-time option. This is required to keep nodes chromo even for inherited nodes
			    // To avoid this inherited nodes chromo being attached we just don't attach inherited nodes chromo
			    long t5; if (ptrace) t5 = GetClock();
			    elem->SetMutation(aNode);
			    elem->Mutate(EFalse, ecsaf, aTrialMode, aRunTime ? elem : aCtx);
			    if (ptrace) { long t5_6 = GetClockElapsed(t3); Logger()->Write(EInfo, this, "Adding node, t5-t6: %d", t5_6);}
			    //Logger()->Write(EInfo, this, "Added node [%s:%s]", elem->EType().c_str(), elem->Name().c_str());
			    /*
			       if (IsLogeventCreOn()) {
			       Logger()->Write(EInfo, this, "Added node [%s:%s]", elem->EType().c_str(), elem->Name().c_str());
			       }
			       */
			}
			else {
			    Log(TLog(EErr, this) + "Adding node [" + elem->EType() + ":" + elem->Name() + "] failed");
			    delete elem;
			    elem = NULL;
			}
		    }
		    else {
			Logger()->Write(EErr, this, "Creating heir [%s] from parent [%s] - failed", sname.c_str(), sparent.c_str());
		    }
		}
		else  {
		    TBool isi = IsInheritedComp(node);
		    Logger()->Write(EErr, this, "Creating elem [%s] in node [%s] - attempt of phenotypic modification - disabled", 
			    sname.c_str(), snode.c_str());
		}
	    }
	    else {
		Logger()->Write(EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    }
	}
    }
    else  {
	Logger()->Write(EErr, this, "Creating elem [%s] in node [%s] - cannot find node", sname.c_str(), snode.c_str());
    }
    if (!aRunTime && !mutadded && !aTrialMode) {
	ChromoNode chn = iChromo->Root().AddChild(aNode, ETrue, EFalse);
	NotifyNodeMutated(chn, aCtx);
    }
    if (EN_PERF_METR && elem != NULL) {
	/* Profiling using wall-clock. It doesn't shows time interval within given thread, other threads are affecting
	   gettimeofday(&tp, NULL);
	   long int t2 = tp.tv_sec * 1000000 + tp.tv_usec;
	   int nodes = elem->GetCapacity();
	   long tspan = t2-t1;
	   long tspan_n = tspan/(nodes + 1);
	   Logger()->Write(EInfo, this, "Created comp [%s] in node [%s];%d;%d", sname.c_str(), snode.c_str(), tspan, tspan_n);
	   */
	timespec te;
	//long tspan = GetClockElapsed(t1, te);
	long tspan = GetClockElapsed(t1);
	int nodes = elem->GetCapacity();
	long tspan_n = tspan/(nodes + 1);
	Logger()->Write(EInfo, this, "Created comp [%s] in node [%s];%d;%d;%d", sname.c_str(), snode.c_str(), nodes, tspan, tspan_n);
    }
    if (ptevent) {
	iEnv->SetSBool(MEnv::ESb_EnPerfTrace, EFalse);
    }
    return elem;
}

long Elem::GetClock()
{
    timespec tt;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tt);
    return tt.tv_nsec/1000;
}

long Elem::GetClockElapsed(const timespec& aStart, timespec& aEnd)
{
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &aEnd);
    return ((aEnd.tv_nsec-aStart.tv_nsec) < 0) ? (1000000000 + aEnd.tv_nsec - aStart.tv_nsec) : (aEnd.tv_nsec - aStart.tv_nsec);
}

long Elem::GetClockElapsed(long aStart)
{
    timespec aEnd;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &aEnd);
    long end_us = aEnd.tv_nsec/1000;
    return ((end_us - aStart) < 0) ? (1000000 + end_us - aStart) : (end_us - aStart);
}

// Reimplemented to keep system consistency. New version is based on run-time model only
// Ref uc_031, ds_rn_prnt
MElem* Elem::CreateHeir(const string& aName, MElem* aMan, MElem* aContext)
{
    MElem* heir = NULL;
    //Logger()->Write(EInfo, this, "CreateHeir, p1 ");
    if (IsProvided()) {
	heir = Provider()->CreateNode(Name(), aName, aMan, iEnv);
	// Persistently attach heir to final owner, but also set context for mutation, ref ds_daa_itn_sfo
	aMan->AppendComp(heir);
	heir->SetCtx(aContext);
	// TODO To move AppendComp to CreateNode: initially set two-ways ownning relateion ?
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	MElem* hprnt = heir->GetParent();
	hprnt->RemoveChild(heir);
	heir->SetParent(hprnt);
    } else {
	__ASSERT(iParent != NULL);
	if (iParent->IsProvided()) {
	    // Parent is Agent - native element. Create via provider
	    heir = Provider()->CreateNode(EType(), aName, aMan, iEnv);
	    // Persistently attach heir to final owner, but also set context for mutation, ref ds_daa_itn_sfo
	    aMan->AppendComp(heir);
	    heir->SetCtx(iMan);
	    // Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	    MElem* hprnt = heir->GetParent();
	    hprnt->RemoveChild(heir);
	    heir->SetParent(hprnt);
	}
	else {
	    heir = iParent->CreateHeir(aName, aMan, iMan);
	}
	if (EN_PERF_TRACE) Logger()->Write(EInfo, this, "CreateHeir, p2 ");
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	ChromoNode croot = iChromo->Root();
	heir->SetMutation(croot);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	heir->Mutate(ETrue, EFalse, EFalse, heir);
	if (EN_PERF_TRACE) Logger()->Write(EInfo, this, "CreateHeir, p3 ");
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetParent(Name());
	// Relocate heir to hier from which the request of creating heir came
	heir->SetCtx(NULL);
	heir->SetCtx(aContext);
	// Using full comp-owner relation, ref ds_di_cnfr_susl
	//iMan->RemoveComp(heir);
	//aMan->AppendComp(heir);
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	heir->SetParent(NULL);
	heir->SetParent(this);
	if (EN_PERF_TRACE) Logger()->Write(EInfo, this, "CreateHeir, p4 ");
    }
    return heir;
}

TBool Elem::AppendChild(MElem* aChild)
{
    TBool res = RegisterChild(aChild);
    if (res) {
	aChild->SetParent(this);
    }
    return res;
}

void Elem::RemoveChild(MElem* aChild)
{
    UnregisterChild(aChild);
    aChild->SetParent(NULL);
}

TBool Elem::AppendComp(MElem* aComp, TBool aRt)
{
    MElem* cowner = aComp->GetMan();
    __ASSERT(cowner == NULL || cowner == this);
    if (cowner == NULL) {
	aComp->SetMan(this);
    }
    TBool res = RegisterComp(aComp);
    if (res)
    {
	// Propagate notification only if self is the component of owner, ref ds_di_cnfr
	if (iMan != NULL && iMan->GetComp(string(), Name()) == this) {
	    iMan->OnCompAdding(*aComp, aRt);
	}
	if (iObserver != NULL) {
	    iObserver->OnCompAdding(*aComp, aRt);
	}
    }
    return res;
}

void Elem::RemoveComp(MElem* aComp)
{
    MElem* cowner = aComp->GetMan();
    __ASSERT(cowner == this);
    TBool res = UnregisterComp(aComp);
    if (res) {
	aComp->SetMan(NULL);
    }
}

TBool Elem::RegisterComp(MElem* aComp)
{
    TBool res = ETrue;
    __ASSERT(iMComps.count(aComp->Name()) == 0);
    iMComps.insert(TNMVal(aComp->Name(), aComp));
    return res;
}

TBool Elem::RegisterChild(MElem* aChild)
{
    TBool res = ETrue;
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	if (it->second == aChild) {
	    __ASSERT(EFalse);
	}
    }
    iChilds.insert(TNMVal(aChild->Name(), aChild));
    return res;
}

TBool Elem::UnregisterChild(MElem* aChild, const string& aName)
{
    TBool res = EFalse;
    const string& name = aName.empty() ? aChild->Name() : aName;
    assert (iChilds.count(name) > 0); 
    pair<TNMReg::iterator, TNMReg::iterator> range = iChilds.equal_range(name);
    TBool found = EFalse;
    for (TNMReg::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aChild) {
	    iChilds.erase(it);
	    found = ETrue;
	}
    }
    __ASSERT(found);
    res = ETrue;
    return res;
}

TBool Elem::UnregisterComp(MElem* aComp, const string& aName)
{
    TBool res = EFalse;
    const string& name = aName.empty() ? aComp->Name() : aName;
    assert (iMComps.count(name) > 0); 
    iMComps.erase(name);

    res = ETrue;
    return res;
}

MElem* Elem::GetNode(const string& aUri, TBool aInclRm)
{
    MElem* res = NULL;
    GUri uri(aUri);
    if (!uri.IsErr()) {
	res = GetNode(uri, aInclRm);
    }
    else  {
	Logger()->Write(EErr, this, "Incorrect URI [%s]", aUri.c_str());
    }
    return res;
}

void Elem::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    // Translate to hier
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	iMan->OnCompDeleting(aComp, aSoft, aModif);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(aComp, aSoft, aModif);
    }
    // Handle for direct child
    if (aComp.GetMan() == this) {
	// Don't deattach the childs chromo. Ref UC_016. The childs chromo (childs creation mutation) needs
	// to be kept in parents chromo.
	/*
	// Deattach the comp's chromo
	iChromo->Root().RmChild(aComp.Chromos().Root(), ETrue);
	*/
	// Don't unregister comp in case if notif is for "soft" removal, ref ds_mut_rm_appr2
	if (!aSoft) {
	    // Unregister first
	    UnregisterComp(&aComp);
	}
    }
}

// TODO [YB] To avoid propagation thru the whole model. It is temporarily remaining because of
// missing proper mechanism of the whole model observer.
void Elem::OnCompAdding(MElem& aComp, TBool aModif)
{
#ifdef __EXT_ASSERTS__
    string url = aComp.GetUri(NULL, ETrue);
    MElem* comp = GetNode(url);
    __ASSERT(comp != NULL);
#endif
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	iMan->OnCompAdding(aComp, aModif);
    }
    if (iObserver != NULL) {
	iObserver->OnCompAdding(aComp, aModif);
    }
}

// TODO [YB] To include agents as member of elem. This will be more effective
// TODO [YB] The current scheme doesn't allow element to get notification of related node change.
// The only notification thru hier can be get via MOwner, but not relation thru edges.
// Thus if node A is related to node B via the chain of edges, the change of B or even the change
// of the relation chain cannot be observed by node A. This is the serious lack. This is because the 
// If providing mechanism is used instead of full relations tracking in Vert. So node A has Ifaces cache
// that includes ifaces from node B but there is no mechanism of the changes in the cache. To consider
// to implement cache update notification. Ref UC_010 
TBool Elem::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    MElem* agents = GetComp("Elem", "Agents");
    TBool res = ETrue;
    if (agents != NULL) {
	for (TInt ci = 0; ci < agents->CompsCount() && res; ci++) {
	    MElem* acomp = agents->GetComp(ci);
	    MACompsObserver* iagent = acomp->GetObj(iagent);
	    if (iagent != NULL) {
		res = iagent->HandleCompChanged(*this, aComp, aContName);
		if (!res) {
		    iagent->HandleCompChanged(*this, aComp, aContName);
		}
	    }
	}
    }
    // Propagate to upper layer if the notification wasn't denied
    // TODO To consider if the event is to be propagated to upper level even if it has been already handled
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	res = iMan->OnCompChanged(aComp, aContName, aModif);
    }
    // Notify observer
    if (iObserver != NULL) {
	iObserver->OnCompChanged(aComp, aContName, aModif);
    }
    return res;
}

TBool Elem::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    TBool res = EFalse;
    if (aComp.GetMan() == this) {
	// Unregister the comp with its old name
	res = UnregisterComp(&aComp, aOldName);
	if (res) {
	    // Register the comp again with its current name
	    res = RegisterComp(&aComp);
	}
    }
    // Propagate the notification
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	iMan->OnCompRenamed(aComp, aOldName);
    }
    if (iObserver != NULL) {
	iObserver->OnCompRenamed(aComp, aOldName);
    }
    return res;
}

TBool Elem::OnChanged(MElem& aComp)
{
    TBool res = ETrue;
    MElem* agents = GetComp("Elem", "Agents");
    if (agents != NULL) {
	for (TInt ci = 0; ci < agents->CompsCount() && res; ci++) {
	    MElem* acomp = agents->GetComp(ci);
	    MACompsObserver* iagent = acomp->GetObj(iagent);
	    if (iagent != NULL) {
		res = iagent->HandleCompChanged(*this, aComp);
	    }
	}
    }
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	res = iMan->OnChanged(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnChanged(aComp);
    }
    return res;
}

MElem* Elem::GetCompOwning(const string& aParent, MElem* aElem)
{
    MElem* res = NULL;
    MElem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this && node->EType() == aParent)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

MElem* Elem::GetCompOwning(MElem* aElem)
{
    MElem* res = NULL;
    MElem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

const MElem* Elem::GetCompOwning(const MElem* aElem) const
{
    const MElem* res = NULL;
    const MElem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

MElem* Elem::GetUpperAowner() {
    MElem* cand = GetAowner();
    MElem* res = NULL;
    while (cand != NULL) {
	res = cand;
	cand = cand->GetAowner();
    }
    return res;
}

TBool Elem::IsComp(const MElem* aElem) const
{
    const MElem* man = aElem->GetMan();
    while (man != NULL && man != this) {
	man = man->GetMan();	
    }
    return man == this;
}

TBool Elem::IsAownerOf(const MElem* aElem) const
{
    const MElem* aowner = aElem->GetAowner();
    while (aowner != NULL && aowner != this) {
	aowner = aowner->GetAowner();	
    }
    return aowner == this;
}

MElem* Elem::GetCommonOwner(MElem* aElem)
{
    MElem* res = NULL;
    MElem* owner = this;
    while (owner != NULL && res == NULL) 
    {
	if (owner->IsComp(aElem) || owner == aElem) {
	    res = owner;
	}
	owner = owner->GetMan();
    }
    return res;
}

// Using combined model/chromo calculation, ref ds_daa_chrc_va
void Elem::GetRank(Rank& aRank, const ChromoNode& aMut) const
{
    const MElem* att = GetAttachedMgr();
    if (att == this) {
	// Get models node rank first
	GetRank(aRank);
	if (aMut != Chromos().Root()) {
	    // Add mutations rank
	    TInt lr = aMut.GetLocalRank();
	    aRank.push_back(lr);
	}
    } else if (att != NULL) {
	att->GetRank(aRank);
    } else {
	aRank.push_back(-1);
    }
}

void Elem::GetCompRank(Rank& aRank, const MElem* aComp) const
{
    TInt lrank = GetCompLrank(aComp);
    if (lrank > -1)  {
	aRank.insert(aRank.begin(), lrank);
    }
    if (iMan != NULL) {
	iMan->GetCompRank(aRank, this);
    }
}

TInt Elem::GetCompLrank(const MElem* aComp) const
{
    TInt res = -1;
    const ChromoNode& croot = Chromos().Root();
    //const ChromoNode& comproot = aComp->Chromos().Root();
    TMut comproot;
    aComp->GetCRoot(comproot);
    //string name = comproot.Name();
    string name = comproot.Attr(ENa_Id);
    //TBool ia = comproot.IsActive();
    TBool found = EFalse;
    for (ChromoNode::Const_Iterator it = croot.Begin(); it != croot.End() && !found; it++, res++) {
	ChromoNode mut = *it;
	//found = (mut.Type() == ENt_Node) && mut.IsActive() == ia && (mut.Name() == name);
	found = (mut.Type() == ENt_Node) && mut.IsActive() && (mut.Name() == name);
	if (found) {
	    const MElem* targ = this;
	    if (mut.AttrExists(ENa_Targ)) {
	       	targ = ((MElem*) this)->GetNode(mut.Attr(ENa_Targ), ETrue);
	    }
	    const MElem* cowner = aComp->GetMan();
	    if (cowner != targ) {
		found = EFalse;
	    }
	}
    }
    if (!found)  {
	res = -1;
    }
    return res;
}

MElem* Elem::GetCompAowner(const MElem* aComp)
{
    MElem* res = NULL;
    TInt lr = GetCompLrank(aComp);
    if (lr == -1) {
	if (iMan != NULL) {
	    res = iMan->GetCompAowner(aComp);
	} else {
	    res = NULL;
	}
    } else {
	res = this;
    }
    return res;
}

const MElem* Elem::GetCompAowner(const MElem* aComp) const
{
    const MElem* res = NULL;
    TInt lr = GetCompLrank(aComp);
    if (lr == -1) {
	if (iMan != NULL) {
	    res = iMan->GetCompAowner(aComp);
	} else {
	    res = NULL;
	}
    } else {
	res = this;
    }
    return res;
}

// Using only attached nodes to make model and chromo rank equivalent, ref ds_daa_chrc_va
void Elem::GetRank(Rank& aRank) const
{
    if (iMan != NULL) {
	iMan->GetCompRank(aRank, this);
    }
}

TInt Elem::GetLocalRank() const
{
    TInt res = -1;
    if (iMan != NULL) {
	res = 0;
	for (TInt ci = 0; ci < iMan->CompsCount() && iMan->GetComp(ci) != this; ci++, res++);
    }
    return res;
}

void Elem::GetRUri(GUri& aUri, MElem* aTop)
{
    if (IsProvided()) {
	// Native agent
	aUri.PrependElem(EType(), Name(), GUri::KSepNone);
    } else {
	// Not native agent, process
	MElem* cowner = GetCommonOwner(aTop);
	GetUri(aUri, cowner);
	MElem* owner = aTop;
	while (owner != cowner) {
	    aUri.PrependElem("", GUri::KUpperLevel);
	    owner = owner->GetMan();
	}
    }
}

string Elem::GetRUri(MElem* aTop)
{
    GUri uri;
    GetRUri(uri, aTop);
    return uri.toString();
}

void Elem::GetUri(GUri& aUri, MElem* aTop) const
{
    if (aTop != this) {
	string ext(EType());
	ext.append(1, GUri::KParentSep);
	if (iMan != NULL) {
	    aUri.PrependElem(ext, Name());
	    if (iMan != aTop) {
		iMan->GetUri(aUri, aTop);
	    }
	}
	else {
	    if (this == iEnv->Root()) {
		// Root agent
		aUri.PrependElem(ext, Name());
		aUri.PrependElem("", "");
	    }
	    else {
		// Native agent, no separator
		aUri.PrependElem(ext, Name(), GUri::KSepNone);
	    }
	}
    }
}

string Elem::GetUri(MElem* aTop, TBool aShort) const
{
    GUri uri;
    GetUri(uri, aTop);
    return uri.toString(aShort);
}

// TODO [YB] To consider if GetRoot can be moved from MElem to Elem
// Actually it is using for Elem impl only and can be replaced by
// sequentual steps to owners, see example below. The problem is
// that seq steps approach is not optimal in distributed async models
// because it requires intermodels exchange for each step
/*
   MElem* Elem::GetRoot() const
   {
   MElem* res = (MElem*) this;
   MElem* owner = res->GetMan();
   while (owner != NULL) {
   res = owner;
   owner = res->GetMan();
   }
   return res;
   }
   */
MElem* Elem::GetRoot() const
{
    MElem* res = (MElem*) this;
    if (iMan != NULL) {
	res = iMan->GetRoot();
    }
    return res;
}

MElem* Elem::GetInhRoot() const
{
    MElem* res = (MElem*) this;
    if (iParent != NULL) {
	res = iParent->GetInhRoot();
    }
    return res;

}

TBool Elem::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    TBool res = EFalse;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string snode = aSpec.Attr(ENa_Comp);
    MElem* node = aSpec.AttrExists(ENa_Comp) ? GetNode(snode) : this;
    TBool mutadded = EFalse;
    if (node != NULL) {
	if ((node != this) && (epheno || node->GetMan() == this || IsCompOfInheritedComp(node))) {
	    // Check dependent mutations
	    res = ETrue;
	    // Just mark node as removed but not remove actually, ref ds_mut_rm_appr
	    // Refuse removing if here are heirs, needs to unparent them first, ref ds_mut_rm_deps
	    if (!node->HasInherDeps(node)) {
		// Refuse removing if there are other critical deps (e.g. -ref-)
		//if (dep.first.first == NULL || dep.second == ENa_Id) {
		// TODO [YB] To add checking refs
		if (ETrue) {
		    node->SetRemoved(aRunTime);
		} else {
		    Logger()->Write(EErr, this,
			    "Removing node [%s], refused, there is ref [%s] to the node, release first",
			    snode.c_str(), "some ref");
		}
	    } else {
		Logger()->Write(EErr, this,
			"Removing node [%s], refused, there are heirs of the node, unparent first", snode.c_str());
		node->HasInherDeps(node);
	    }
	    if (IsLogeventCreOn()) {
		Logger()->Write(EInfo, this, "Removed elem [%s]", snode.c_str());
	    }
	}
	else  {
	    Logger()->Write(EErr, this, "Removing node [%s] - attempt of phenotypic modification - disabled", snode.c_str());
	    IsCompOfInheritedComp(node);
	}
    }
    else {
	Logger()->Write(EErr, this, "Removing node [%s] - not found or isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, aCtx);
    }
    return res;
}

TBool Elem::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode)
{
    TBool res = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    TBool dest_spec = aSpec.AttrExists(ENa_MutNode);
    string dests = dest_spec ? aSpec.Attr(ENa_MutNode): string();
    MElem* dnode = dest_spec ? GetNode(dests) : this;
    TBool ecsaf = iEnv->ChMgr()->EnableCheckSafety();
    TBool mutadded = EFalse;
    if (dnode != NULL) {
	GUri srcsuri(srcs);
	if (srcsuri.Scheme().empty()) {
	    // Local node
	    MElem* snode = GetNode(srcs);
	    if (snode != NULL) {
		MElem* sowner = snode->GetMan();
		if (sowner != NULL && dnode == sowner) {
		    // Attempt of moving node to the current contect 
		    Logger()->Write(EErr, this, "Moving node [%s] to it's current owner - disabled", snode->GetUri().c_str());
		}
		else if (dnode == snode) {
		    // Attempt of moving node to itself
		    Logger()->Write(EErr, this, "Moving node [%s] into itself - disabled", snode->GetUri().c_str());
		}
		else if (dnode != this) {
		    Logger()->Write(EErr, this, "Moving to another destination");
		}
		else {
		    // Inter-nodes movement
		    // TODO [YB] To consider if this movement does make sense. It is not movement of chunk of chromo, but 
		    // run-time only.
		    // It is rather complicated to re-create snode in new context because all snode
		    // mutation are related to original snode context, so use trick, ref "ds_mv_local"
		    // Create heir of source node in destination context
		    MElem* heir = snode->CreateHeir(snode->Name(), dnode, NULL);
		    // Re-adopt back to source parent
		    heir->SetParent(NULL);
		    snode->GetParent()->AppendChild(heir);
		    //res = dnode->AppendComp(heir);
		    res = ETrue;
		    if (res) {
			// Remove source node
			// If node has children then just mark it as removed but not remove actually
			if (snode->HasInherDeps(snode)) {
			    //TODO [YB] Consider to handle correctly
			    snode->SetRemoved(aRunTime);
			}
			else {
			    delete snode;
			}
		    }
		    else {
			Logger()->Write(EErr, this, "Moving node [%s] failed", snode->GetUri().c_str());
			delete heir;
		    }
		}
	    }
	    else {
		Logger()->Write(EErr, this, "Moving node [%s] - not found", srcs.c_str());
	    }
	}
	else {
	    // Remote node
	    Chromo *spec = Provider()->CreateChromo();
	    TBool res1 = spec->Set(srcs);
	    if (res1) {
		const ChromoNode& root = spec->Root();
		MElem* nnode = NULL;
		if (dnode != NULL) {
		    nnode= dnode->AddElem(root, ETrue);
		}
		else {
		    nnode = AddElem(root, ETrue);
		}
		delete spec;
		res = nnode != NULL;
	    }
	    else {
		Logger()->Write(EErr, this, "Moving [%s] to [%s]: source node not found", srcs.c_str(), dests.c_str());
	    }
	}
	if (!aRunTime && res) {
	    // Adding dependency to object of change
	    ChromoNode chn = iChromo->Root().AddChild(aSpec);
	    NotifyNodeMutated(aSpec, NULL);
	    mutadded = ETrue;
	}
    }
    else {
	Logger()->Write(EErr, this, "Moving to node [%s] - not found", dests.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, NULL);
    }
    return res;
}

TBool Elem::IsName(const char* aName)
{
    return iName == aName;
}

TBool Elem::IsLogeventCreOn() 
{
    MElem* node = GetNode("./Elem:Logspec/Elem:Creation");
    string ls = GetContent("Debug.Creation");
    return node != NULL || ls == "y";
}

// TODO [YB] To implement with usage of URI but not just string
// TODO [YB] The problem is more serious: we need to check the full type
// (all parents chain) to detect inheritance.
TBool Elem::IsHeirOf(const string& aParent) const
{
    TBool res = EFalse;
    string et = EType(EFalse);
    int pos = et.find(aParent);
    if (pos == 0) {
	res = (aParent.size() == et.size() || et.at(aParent.size()) == GUri::KParentSep);
    }
    return res;
    //return pos == 0 && (aParent.size() == et.size() || et.at(aParent.size()) == GUri::KParentSep);
}

// Checks if elements chromo is attached. Ref UC_019 for details
// Using model based calculation, ref ds_daa_chrc_va
TBool Elem::IsChromoAttached() const
{
    const MElem* atm = GetAttachedMgr();
    return atm == this;
}

TBool Elem::IsMutAttached(const ChromoNode& aMut) const
{
    return IsChromoAttached();
}

// Getting owner that is attached. It can differ from owner that attaches the node
// For owner attaching the node, use
#if 0
MElem* Elem::GetAttachedMgr() 
{
    MElem* res = NULL;
    MElem* cand = this;
    while (res == NULL && cand != NULL) {
	if (cand->IsChromoAttached()) {
	    res = cand;
	} else {
	    cand = cand->GetMan();
	}
    }
    return res;
}

const MElem* Elem::GetAttachedMgr() const
{
    const MElem* res = NULL;
    const MElem* cand = this;
    while (res == NULL && cand != NULL) {
	// Attached or root
	if (cand->IsChromoAttached() || iMan == NULL) {
	    res = cand;
	} else {
	    cand = cand->GetMan();
	}
    }
    return res;
}
#endif

MElem* Elem::GetAttachedMgr() 
{
    MElem* res = GetRoot();
    if (this != res) {
	MElem* comp = res->GetCompOwning(this);
	while (res != this && res->GetCompLrank(comp) > -1) {
	    res = comp;
	    comp = res->GetCompOwning(this);
	}
    }
    return res;
}

const MElem* Elem::GetAttachedMgr() const
{
    const MElem* res = GetRoot();
    if (this != res) {
	const MElem* comp = res->GetCompOwning(this);
	while (res != this && res->GetCompLrank(comp) > -1) {
	    res = comp;
	    comp = res->GetCompOwning(this);
	}
    }
    return res;
}

MElem* Elem::GetAcompOwning(MElem* aComp)
{
    MElem* res = NULL;
    MElem* cand = aComp;
    while (res == NULL && cand != NULL) {
	if (IsAownerOf(cand)) {
	    res = cand;
	} else {
	    cand = cand->GetMan();
	}
    }
    return res;
}

// Get the node that attaches this node. Don't confuse with the nearest attached
// owner, for that ref to GetAttachedMgr
MElem* Elem::GetAowner()
{
    MElem* res = NULL;
    if (iMan != NULL) {
	res = iMan->GetCompAowner(this);
    }
    return res;
}

const MElem* Elem::GetAowner() const
{
    const MElem* res = NULL;
    if (iMan != NULL) {
	res = iMan->GetCompAowner(this);
    }
    return res;
}

TBool Elem::IsInheritedComp(const MElem* aNode) const
{
    MElem* uaowner = ((MElem*) aNode)->GetUpperAowner();
    TBool res = uaowner == NULL || IsComp(uaowner) && !uaowner->IsComp(this);
    return res;
}

TBool Elem::IsDirectInheritedComp(const MElem* aNode) const
{
    MElem* uaowner = ((MElem*) aNode)->GetUpperAowner();
    MElem* acompo = ((MElem*) this)->GetAcompOwning((Elem*) aNode);
    TBool isinher = uaowner == NULL || IsComp(uaowner) && !uaowner->IsComp(this);
    TBool res = isinher && acompo == NULL;
    return res;
}

TBool Elem::IsCompOfInheritedComp(const MElem* aNode) const
{
    TBool res = IsInheritedComp(aNode);
    if (!res) {
	const MElem* owner = aNode->GetMan();
	while (owner != this && !res) {
	    res = IsInheritedComp(owner);
	    owner = owner->GetMan();
	}
    }
    return res;
}

MElem* Elem::GetParent()
{
    return iParent;
}

const MElem* Elem::GetParent() const
{
    return iParent;
}

void Elem::SetParent(MElem* aParent)
{
    __ASSERT(aParent != NULL && iParent == NULL || aParent == NULL && iParent != NULL);
    iParent = aParent;
}

void Elem::OnChildDeleting(MElem* aChild)
{
    // Unregister the child
    UnregisterChild(aChild);
}

TBool Elem::OnChildRenamed(MElem* aChild, const string& aOldName)
{
    TBool res = EFalse;
    // Unregister the child with its old name
    res = UnregisterChild(aChild, aOldName);
    if (res) {
	// Register the comp againg with its current name
	res = RegisterChild(aChild);
    }
    return res;
}

// Handles parent deleting, ref uc_029
void Elem::OnParentDeleting(MElem* aParent)
{
    iParent = NULL;
    /* TODO [YB] this solution contradicts to principles of incrementing creations
     * to redesign
    // Only local parent deletion is handled for now
    __ASSERT(aParent == iParent);
    // Copy parents choromo and reparent to grandparent
    ChromoNode phroot = iParent->Chromos().Root();
    ChromoNode hroot = iChromo->Root();
    ChromoNode fchild = *(hroot.Begin());
    for (ChromoNode::Iterator it = phroot.Begin(); it != phroot.End(); it++) {
    ChromoNode cn = *it;
    hroot.AddPrev(fchild, cn);
    }
    hroot.SetAttr(ENa_Parent, phroot.Attr(ENa_Parent));
    Elem* gparent = iParent->GetParent();
    gparent->RegisterChild(this);
    iParent = gparent;
    */
}

ChromoNode Elem::GetChNode(const GUri& aUri) const
{
}

TBool Elem::IsRemoved() const
{
    return isRemoved;
}

void Elem::SetRemoved(TBool aModif)
{
    // Remove node from native hier but keep it in inher hier
    // Notify the man of deleting
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, ETrue, aModif);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(*this, ETrue, aModif);
    }
    for (auto& it : iMComps) {
	MElem* comp = it.second;
	comp->SetRemoved(ETrue);
    }
    // Mark node as removed from hative hier
    isRemoved = ETrue;
}

TBool Elem::HasChilds() const
{
    return !iChilds.empty();
}

TBool Elem::HasInherDeps(const MElem* aScope) const
{
    TBool res = EFalse;
    for (TNMReg::const_iterator it = iChilds.begin(); it != iChilds.end() && !res; it++) {
	MElem* comp = it->second;
	res = !comp->IsRemoved() && !aScope->IsComp(comp);
    }
    for (TNMReg::const_iterator it = iMComps.begin(); it != iMComps.end() && !res; it++) {
	MElem* comp = it->second;
	res = !comp->IsRemoved() && comp->HasInherDeps(aScope);
    }
    return res;
}

TInt Elem::GetCapacity() const
{
    TInt res = 0;
    for (auto& it : iMComps) {
	res++;
	MElem* comp = it.second;
	res += comp->GetCapacity();
    }
    return res;
}

// This variant is for using env imports manager
TBool Elem::ImportNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode)
{
    MImportMgr* impmgr = iEnv->ImpsMgr();
    __ASSERT(impmgr != NULL);
    TBool res = EFalse;
    TBool mutadded = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    res = impmgr->Import(srcs);
    if (!aRunTime && (res || !res && !aTrialMode)) {
	iChromo->Root().AddChild(aSpec);
	NotifyNodeMutated(aSpec, NULL);
    }
    return ETrue;
}

TInt Elem::CompsCount() const
{
    return iMComps.size();
}

MElem* Elem::GetComp(TInt aInd)
{
    auto it = iMComps.begin();
    for (TInt i = 0; i < aInd; i++, it++);
    return it->second;
}

string Elem::Mid() const
{
    // Generating Mid as relative UID from local root, ref ds_daa_pxdup_birc
    return GetUri(iEnv->Root(), ETrue);
}

MIface* Elem::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	throw (runtime_error("Wrong arguments number"));
    if (name == "Name") {
	aRes = Name();
    } else if (name == "Delete") {
	Delete();
    } else if (name == "GetMan") {
	res = GetMan();
    } else if (name == "GetCtx") {
	res = GetCtx();
    } else if (name == "GetRoot") {
	res = GetRoot();
    } else if (name == "GetParent") {
	res = GetParent();
    } else if (name == "IsProvided") {
	TBool rr = IsProvided();
	//aRes = Ifu::FromBool(rr);
	aRes = Ifu::Pack(rr);
    } else if (name == "GetNode") {
	TBool inclrm = Ifu::ToBool(args.at(1));
	res = GetNode(args.at(0), inclrm);
    } else if (name == "GetNode#2") {
	GUri uri(args.at(0));
	//TBool anywhere = Ifu::ToBool(args.at(1));
	TBool anywhere = Ifu::Unpack(args.at(1), anywhere);
	TBool inclrm = Ifu::ToBool(args.at(2));
	GUri::const_elem_iter it = uri.Begin();
	res = GetNode(uri, it, anywhere, inclrm);
    } else if (name == "ContentExists") {
	TBool rr = ContentExists(args.at(0));
	aRes = Ifu::Pack(rr);
    } else if (name == "ContValueExists") {
	TBool rr = ContValueExists(args.at(0));
	aRes = Ifu::Pack(rr);
    } else if (name == "GetContCount") {
	TInt rr = GetContCount(args.at(0));
	aRes = Ifu::FromInt(rr);
    } else if (name == "GetContent") {
	TBool full = Ifu::ToBool(args.at(1));
	aRes = GetContent(args.at(0), full);
    } else if (name == "GetContComp") {
	TInt ind = Ifu::ToInt(args.at(1));
	aRes = GetContComp(args.at(0), ind);
    } else if (name == "Mutate") {
	TBool rtonly, checksafety, trialmode;
	rtonly = Ifu::ToBool(args.at(1));
	checksafety = Ifu::ToBool(args.at(2));
	trialmode = Ifu::ToBool(args.at(3));
	SetMutation(args.at(0));
	Mutate(rtonly, checksafety, trialmode);
    } else if (name == "Mutate#2") {
	TBool rtonly, checksafety, trialmode;
	rtonly = Ifu::ToBool(args.at(0));
	checksafety = Ifu::ToBool(args.at(1));
	trialmode = Ifu::ToBool(args.at(2));
	MElem* ctx = GetNode(args.at(3));
	Mutate(rtonly, checksafety, trialmode, ctx);
    } else if (name == "GetChromoSpec") {
	aRes = GetChromoSpec();
    } else if (name == "EType") {
	aRes = EType(Ifu::ToBool(args.at(0)));
    } else if (name == "AppendComp") {
	MElem* comp = GetNode(args.at(0));
	TBool rr = AppendComp(comp);
	aRes = Ifu::Pack(rr);
    } else if (name == "RemoveComp") {
	MElem* comp = GetNode(args.at(0));
	RemoveComp(comp);
    } else if (name == "AppendChild") {
	MElem* child = GetNode(args.at(0));
	if (child == NULL) {
	    MElem* nn = GetNode(args.at(0));
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool rr = AppendChild(child);
	aRes = Ifu::Pack(rr);
    } else if (name == "RemoveChild") {
	MElem* child = GetNode(args.at(0));
	RemoveChild(child);
    } else if (name == "SetParent") {
	MElem* parent = GetNode(args.at(0));
	SetParent(parent);
    } else if (name == "GetUri") {
	MElem* base = GetNode(args.at(0));
	aRes = GetUri(base, ETrue);
    } else if (name == "GetRUri") {
	MElem* base = GetNode(args.at(0));
	aRes = GetRUri(base);
    } else if (name == "DoGetObj") {
	// TODO [YB] DoGetObj cannot be used here because it returns just void* prt to given iface
	// Trivial casting this iface to MIface* can cause to wrong result if MIface is not the first
	// parent of given iface. To consider adding MIface method "GetIface" to be used here
	res = (MIface*) DoGetObj(args.at(0).c_str());
    } else if (name == "GetIfi") {
	string name = args.at(0);
	vector<string> ctxe;
	if (!args.at(1).empty()) {
	    Ifu::ToStringArray(args.at(1), ctxe);
	}
	RqContext* ctx(NULL);
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MElem* elem = GetNode(suri);
	}
	TIfRange rg = GetIfi(name, ctx);
	res = NULL;
	TInt isize = IfRangeSize(rg);
	aRes = Ifu::FromInt(isize);
	/*
	if (rg.first != rg.second) {
	    res = (MIface*) *(rg.first);
	}
	*/
    } else if (name == "GetIfind") {
	string name = args.at(0);
	vector<string> ctxe;
	if (!args.at(1).empty()) {
	    Ifu::ToStringArray(args.at(1), ctxe);
	}
	RqContext* ctx(NULL);
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MElem* elem = GetNode(suri);
	}
	TIfRange rg = GetIfi(name, ctx);
	TInt ind = Ifu::ToInt(args.at(2));
	res = (MIface*) GetIfind(rg, ind);
    } else if (name == "GetSIfi") {
	string name = args.at(0);
	vector<string> ctxe;
	if (!args.at(1).empty()) {
	    Ifu::ToStringArray(args.at(1), ctxe);
	}
	RqContext* ctx(NULL);
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MElem* elem = GetNode(suri);
	}
	TIfRange rg = GetIfi(name, ctx);
	TInt isize = IfRangeSize(rg);
	if (isize == 1) {
	    res = (MIface*) *(rg.first);
	}
    } else if (name == "UnregIfReq") {
	string name = args.at(0);
	TICacheRCtx ctx;
	EIfu::ToCtx(this, args.at(1), ctx);
	UnregIfReq(name, ctx);
    } else if (name == "UnregIfProv") {
	string name = args.at(0);
	TICacheRCtx ctx;
	EIfu::ToCtx(this, args.at(1), ctx);
	MElem* prov = GetNode(args.at(2));
	TBool inv = Ifu::ToBool(args.at(3));	
	UnregIfProv(name, ctx, prov, inv);
    } else if (name == "SetObserver") {
	MExtIfProv* prov = iEnv->ExtIfProv();
	if (args.at(0) == GUri::Nil()) {
	    SetObserver(NULL);
	} else {
	    if (prov == NULL) {
		throw (runtime_error("Cannot get ext iface provider"));
	    }
	    MIface* iobs = prov->GetEIface(args.at(0), MAgentObserver::Type());
	    if (iobs == NULL) {
		throw (runtime_error("Cannot get agent observer iface" + args.at(0)));
	    }
	    MAgentObserver* obs = dynamic_cast<MAgentObserver*>(iobs);
	    if (obs == NULL) {
		throw (runtime_error("Cannot get agent observer" + args.at(0)));
	    }
	    SetObserver(obs);
	}
    } else if (name == "OnCompAdding") {
	MElem* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	OnCompAdding(*comp);
    } else if (name == "OnCompChanged") {
	MElem* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool rr = OnCompChanged(*comp, args.at(1));
	aRes = Ifu::Pack(rr);
    } else if (name == "OnChanged") {
	MElem* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool res = OnCompChanged(*comp);
	aRes = Ifu::Pack(res);
    } else if (name == "OnCompDeleting") {
	MElem* comp = GetNode(args.at(0), ETrue);
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool soft = Ifu::ToBool(args.at(1));
	OnCompDeleting(*comp, soft);
    } else if (name == "OnChildDeleting") {
	MElem* child = GetNode(args.at(0));
	OnChildDeleting(child);
    } else if (name == "OnParentDeleting") {
	MElem* prnt = GetNode(args.at(0));
	OnParentDeleting(prnt);
    } else if (name == "CompsCount") {
	TInt cnt = CompsCount();
	aRes = Ifu::Pack(cnt);
    } else if (name == "GetComp") {
	TInt ind = Ifu::ToInt(args.at(0));
	res = GetComp(ind);
    } else if (name == "GetComp#2") {
	res = GetComp(args.at(0), args.at(1));
    } else if (name == "AppendMutation") {
	TMut mut(args.at(0));
	AppendMutation(mut);
    } else if (name == "AppendMutation#2") {
	Chromo* mut = iEnv->Provider()->CreateChromo();
	mut->SetFromSpec(args.at(0));
	AppendMutation(mut->Root());
	delete mut;
    } else if (name == "IsRemoved") {
	TBool rr = IsRemoved();
	aRes = Ifu::Pack(rr);
    } else if (name == "IsHeirOf") {
	TBool rr = IsHeirOf(args.at(0));
	aRes = Ifu::FromBool(rr);
    } else if (name == "IsComp") {
	MElem* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get agent" + args.at(0)));
	} else {
	    TBool rr = IsComp(comp);
	    aRes = Ifu::FromBool(rr);
	}
    } else if (name == "OnNodeMutated") {
	MElem* node = GetNode(args.at(0));
	TMut mut(args.at(1));
	MElem* ctx = GetNode(args.at(2));
	OnNodeMutated(node, mut, ctx);
    } else if (name == "IsCompAttached") {
	MElem* node = GetNode(args.at(0));
	TBool isatt = IsCompAttached(node);
	aRes = Ifu::FromBool(isatt);
    } else if (name == "GetCompLrank") {
	MElem* node = GetNode(args.at(0));
	TInt rank = GetCompLrank(node);
	aRes = Ifu::FromInt(rank);
    } else if (name == "GetCRoot") {
	TMut croot;
	GetCRoot(croot);
	aRes = croot;
    } else if (name == "IsChromoAttached") {
	TBool isatt = IsChromoAttached();
	aRes = Ifu::FromBool(isatt);
    } else if (name == "GetCompOwning") {
	MElem* node = GetNode(args.at(0));
	res = GetCompOwning(node);
    } else if (name == "GetAowner") {
	res = GetAowner();
    } else if (name == "GetCompOwning") {
	TBool rr = IsContOfCategory(args.at(0), args.at(1));
	aRes = Ifu::FromBool(rr);
    } else if (name == "GetCompAowner") {
	MElem* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	res = GetCompAowner(comp);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

TInt Elem::IfRangeSize(const TIfRange& aRange) const
{
    TInt res = 0;
    for (TIfIter it = aRange.first; it != aRange.second; it++, res++);
    return res;
}

void* Elem::GetIfind(TIfRange& aRange, TInt aInd)
{
    void* res = NULL;
    TIfIter it = aRange.first;
    for (; it != aRange.second && aInd >= 0; it++, aInd--) {
	if (aInd == 0)  {
	    res = *it;
	    break;
	}
    }
    if (aInd != 0)  {
	__ASSERT(EFalse);
    }
    return res;
}

void Elem::DumpChilds() const
{
    for (TNMReg::const_iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	MElem* child = it->second;
	cout << "ptr: " << (void*) child << ", name: " << child->Name() << endl;
    }
}

void Elem::DumpComps() const
{
    cout << endl << "<< Named register of components >>" << endl << endl;
    for (TNMReg::const_iterator it = iMComps.begin(); it != iMComps.end(); it++) {
	MElem* comp = it->second;
	cout << "name: " << it->first << ", ptr: " << (void*) comp << endl;
    }
}

void Elem::NotifyNodeMutated(const ChromoNode& aMut, const MElem* aCtx)
{
    if (this != GetRoot()) {
	MElem* owner = GetMan();
	if (aCtx != NULL && (aCtx == owner || aCtx->IsComp(owner)) || owner->IsCompAttached(this)) {
	    owner->OnNodeMutated(this, aMut, aCtx);
	}
	if (HasChilds()) {
	    NotifyParentMutated(aMut);
	}
    } 
}

void Elem::OnNodeMutated(const MElem* aNode, const TMut& aMut, const MElem* aCtx)
{
    MElem* root = GetRoot();
    // Copy mutation to the chromo in case of root or first non-attached owner in he owners chain
    // ref ds_mut_osm_linchr_comdp_mp
    //if (this == root || !IsCompAttached(GetCompOwning(aNode))) {
    // Accept propagation from attached comp only
    if (true) {
	string nuri = aNode->GetUri(this, ETrue);
	ChromoNode anode = iChromo->Root().AddChild(aMut);
	anode.SetAttr(ENa_Targ, nuri);
	if (iObserver != NULL) {
	    iObserver->OnCompMutated(aNode);
	}
    }
    if (this != root) {
	// Propagate till upper node of attaching chain, ref ds_mut_osm_linchr_lce
	// i.e to the owner if owner is attaching owner or is under mutated node attaching owner
	MElem* owner = GetMan();
	if (aCtx != NULL && (aCtx == owner || aCtx->IsComp(owner)) || owner->IsCompAttached(this)) {
	    owner->OnNodeMutated(aNode, aMut, aCtx);
	}
	if (HasChilds()) {
	    string nuri = aNode->GetUri(this, ETrue);
	    TMut mut(aMut);
	    mut.SetAttr(ENa_Targ, nuri);
	    NotifyParentMutated(mut);
	}
    }
}

void Elem::NotifyParentMutated(const TMut& aMut)
{
    if (this != GetRoot()) {
	for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	    MElem* child = it->second;
	    child->OnParentMutated(this, aMut);
	}
    }
}

void Elem::OnParentMutated(MElem* aParent, const TMut& aMut)
{
    TBool rres = ETrue;
    // Rebase and apply parent's mutation
    ChromoNode rno = iMut->Root().AddChild(aMut);
    if (rno.AttrExists(ENa_Ref)) {
	MElem* ref = aParent->GetNode(rno.Attr(ENa_Ref));
	if (ref != NULL) {
	    string ruri = ref->GetRUri(this);
	    rno.SetAttr(ENa_Ref, ruri);
	} else {
	    Logger()->Write(EErr, this, "Cannot find ref [%s]", rno.Attr(ENa_Ref).c_str());
	    rres = EFalse;
	}
    }
    if (rno.AttrExists(ENa_Parent)) {
	MElem* prn = aParent->GetNode(rno.Attr(ENa_Parent));
	if (prn != NULL) {
	    string puri = prn->GetRUri(this);
	    rno.SetAttr(ENa_Parent, puri);
	} else {
	    Logger()->Write(EErr, this, "Cannot find parent [%s]", rno.Attr(ENa_Parent).c_str());
	    rres = EFalse;
	}
    }
    if (rres) {
	Mutate(ETrue, EFalse, ETrue, EFalse);
    }
}

TBool Elem::IsCompAttached(const MElem* aComp) const
{
    return (GetCompLrank(aComp) != -1);
}

void Elem::SaveChromo(const char* aPath) const
{
    iChromo->Save(aPath);
}

TBool Elem::IsProvided() const
{
    return Provider()->IsProvided(this);
}

string Elem::GetChromoSpec() const
{
    string res;
    iChromo->GetSpec(res);
    return res;
}

void Elem::AppendMutation(const TMut& aMut)
{
    ChromoNode mut = iMut->Root().AddChild(aMut);
}

string Elem::ContentCompId(const string& aOwnerName, const string& aCompName)
{
    return  aOwnerName.empty() ? aCompName : aOwnerName + KContentSep + aCompName;
}

string Elem::ContentKey(const string& aBase, const string& aSuffix)
{
    return  aBase + KContentSep + aSuffix;
}

string Elem::ContentValueKey(const string& aId)
{
    return  aId + KContentSep + KContentKey_Value;
}

string Elem::ContentCompsKey(const string& aId)
{
    return  aId + KContentSep + KContentKey_Comps;
}

string Elem::GetAssociatedData(const string& aUri) const
{
    return string();
}

MElem* Elem::GetCtx()
{
    return mContext;
}

void Elem::SetCtx(MElem* aContext)
{
    __ASSERT(mContext == NULL && aContext != NULL || aContext == NULL);
    mContext = aContext;
}
