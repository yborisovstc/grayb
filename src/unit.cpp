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
#include "profiler_events.h"


TBool Unit::EN_MUT_LIM = ETrue;

const string Unit::KCont_About = "About";
const string MUnit::KCont_Categories = "Categories";
const string MUnit::KCont_Ctg_Readonly = "Readonly";
const string MUnit::KCont_Ctg_Debug = "Debug";

string Unit::Fmt::mSepContInp = ";";
string Unit::Fmt::mSepContName = "~";
// TODO [YB] Alternative method via static Ifu derived is used, remaining for info only
//string IfuPars = "Name,0,GetNode,1,Mutate,4";
//Ifu Unit::mIfu(IfuPars);

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


MUnit::EIfu MUnit::mIfu;

// Ifu static initialisation
MUnit::EIfu::EIfu()
{
    RegMethod("Name", 0);
    RegMethod("GetMan", 0);
    RegMethod("GetCtx", 0);
    RegMethod("GetAowner", 0);
    RegMethod("GetCompAowner", 1);
    RegMethod("GetRoot", 0);
    RegMethod("GetNode", 2);
    RegMethod("GetNode#2", 3);
    RegMethod("GetContent", 2);
    RegMethod("Mutate", 4);
    RegMethod("Mutate#2", 4);
    RegMethod("IsProvided", 0);
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
    RegMethod("IsComp", 1);
    RegMethod("GetCompLrank", 1);
    RegMethod("GetCRoot", 0);
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
    RegMethod("IsContOfCategory", 2);
}

void MUnit::EIfu::ToCtx(MUnit* aHost, const string& aString, TICacheRCtx& aCtx)
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
	MUnit* rq = aHost->GetNode(elem);
	aCtx.push_back(rq);
    } while (end != string::npos);
}

// TODO To move to elem. The method exposes knowledge of cont internals
string MUnit::GetContentOwner(const string& aCont)
{
    string res;
    size_t pos = aCont.find_last_of(Unit::KContentSep);
    if (pos != string::npos) {
	res = aCont.substr(0, pos);
    }
    return res;
}

string MUnit::GetContentLName(const string& aName)
{
    string res;
    size_t end = aName.find_last_of(Unit::KContentSep);
    if (end != string::npos) {
	res = aName.substr(end + 1);
    } else {
	res = aName;
    }
    return res;
}




string Unit::PEType()
{
    return string() + GUri::KParentSep + Unit::Type();
}

Unit::IfIter::IfIter(Unit* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd): iHost(aHost), iIName(aIName), iReq(aReq) 
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

Unit::IfIter::IfIter(const IfIter& aIt): iHost(aIt.iHost), iIName(aIt.iIName), iReq(aIt.iReq), iQFRange(aIt.iQFRange),
    iQFIter(aIt.iQFIter), iCacheRange(aIt.iCacheRange), iCacheIter(aIt.iCacheIter)
{
}

Unit::MIfIter& Unit::IfIter::operator=(const MIfIter& aIt)
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

Unit::MIfIter* Unit::IfIter::Clone() const
{
    return new IfIter(*this);
}

Unit::MIfIter& Unit::IfIter::operator++()
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

TBool Unit::IfIter::operator==(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    TBool res = (iHost == it.iHost && iIName == it.iIName && iReq == it.iReq && iQFRange == it.iQFRange 
	    && iQFIter == it.iQFIter && iCacheRange == it.iCacheRange && iCacheIter == it.iCacheIter);
    return res;
}

MIface*  Unit::IfIter::operator*()
{
    MIface* res = iCacheIter->second;
    return res;
}


// Unitent

/** Constructor creating inherited agent: name is defined, parent is Type() */
Unit::Unit(const string &aName, MUnit* aMan, MEnv* aEnv): iName(aName), iMan(aMan), iEnv(aEnv), isRemoved(EFalse), mContext(NULL)
{
    Pdur(PEvents::Dur_Elem_Constr_Start, this);
    Pdstat(PEvents::DurStat_Elem_Constr_Chromo, true);
    Pdstat(PEvents::DurStat_Elem_Constr_Chromo, false);
    if (aName.empty()) {
	// Native base agent, its name is cpp class type
	iName = GetType(PEType());
    } else {
	// Inherited native agent, its name is given, type is class extended type
	iName = aName;
    }
    InsertContent(KCont_About);
    Pdstat(PEvents::DurStat_Elem_Constr, false);
    //InsertContent(KCont_Categories);
    //InsertContent(KCont_Ctg_Readonly);
}

string Unit::GetType(const string& aEType) const
{
    string res;
    size_t pos = aEType.find_last_of(GUri::KParentSep);
    res = aEType.substr(pos + 1);
    return res;
}

string Unit::GetPType(const string& aEType) const
{
    string res;
    size_t pos = aEType.find_last_of(GUri::KParentSep);
    res = aEType.substr(0, pos);
    return res;
}

void Unit::Delete()
{
    delete this;
}

Unit::~Unit() 
{
    // Unregigster ifaces providers
    UnregAllIfRel();
    // Remove the comps, using iterator refresh because the map is updated on each comp deletion
    auto it = iMComps.begin();
    while (it != iMComps.end()) {
	delete it->second;
	it = iMComps.begin();
    }
    iMComps.clear();
    // TODO [YB] OnCompDeleting called only here, i.e. after Unit derivation desctuctor completed
    // This means that the ovner notified will not be able to understand what agent is deleted
    // To consider to add one more notification - before deletions
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, EFalse, ETrue);
    }
    iEnv->Observer()->OnCompDeleting(*this, EFalse, ETrue);

    // Notify env of root deletion, ref ds_daa_rdo
    if (this == GetRoot()) {
	iEnv->OnRootDeleted();
    }
    iEnv = NULL; // Not owned
}

string Unit::PName() const
{
    // TODO to implement
    return string();
}

void Unit::SetMan(MUnit* aMan)
{
    __ASSERT(iMan == NULL && aMan != NULL || aMan == NULL);
    // TODO [YB] To add notifications here
    iMan = aMan;
}


MUnit* Unit::GetMan()
{
    return iMan;
}

const MUnit* Unit::GetMan() const
{
    return iMan;
}

MIface *Unit::DoGetObj(const char *aName)
{
    MIface* res = NULL;
    if (strcmp(aName, MUnit::Type()) == 0) {
	res = dynamic_cast<MUnit*>(this);
    }
    return res;
}

Unit::TIfRange Unit::GetIfi(const string& aName, const TICacheRCtx& aCtx)
{
    // Get from cache first
    // Add NULL to context in case of anonymous context
    // This is required to avoid problem with If relations invalidating
    // The matter is that the initial requestor doesnt cache relation into its cache
    // So on invalidation provider checks if context is initial (size == 1), if so it ignore relation
    // But it is possible that size == 1 for second requestor if the initial makes anonimous request
    // To avoid this inconsistance we need to explicitly register even anonymous requestor
    Pdstat(PEvents::DurStat_GetIfi, true);
    TICacheRCtx ctx(aCtx);
    if (ctx.empty()) {
	// TODO Why don't add this to context? This seems to be default behavior
	ctx.push_back(this);
    }
    IfIter beg(this, aName, ctx);
    IfIter end(this, aName, ctx, ETrue);
    if (beg == end) {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: -->", aName.c_str());
	}
	// Invalid cache, update cache
	UpdateIfi(aName, ctx);
	beg = IfIter(this, aName, ctx);
	end = IfIter(this, aName, ctx, ETrue);
	// Register the request with local provider in case of no iface resolved (in case of iface 
	// resolved, the request got already registered).
	// This creates relation chain anycase, registering all potential requesters for the providers
	// This allows to do correct invalidation of iface cache from any provider to its requestors
	if (beg == end) {
	    InsertIfQm(aName, ctx, this);
	}
    } else {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: resolved from cache", aName.c_str());
	}
    }
    Pdstat(PEvents::DurStat_GetIfi, false);
    return TIfRange(TIfIter(beg), TIfIter(end));
}

MIface* Unit::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert) 
{
    MIface* res = NULL;
    MUnit* req = GetNode(aReqUri);
    __ASSERT(aReqAssert || (req != NULL));
    if (req != NULL) {
	TIfRange rg = req->GetIfi(aName, this);
	if (rg.first != rg.second) {
	    res = *(rg.first);
	}
    }
    return res;
}

void Unit::UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx)
{
    TICacheKeyF query(aIfName, aCtx);
    TICacheQFRange rg = iICacheQF.equal_range(query);
    __ASSERT (rg.first != rg.second);

    for (TICacheQFIter it = rg.first; it != rg.second; it++) {
	// Unregister itself on next provider
	Base* prov = it->second.second;
	MUnit* prove = prov->GetObj(prove);
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
void Unit::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv, TBool aInv)
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
	MUnit* cnde = cnd->GetObj(cnde);
	if (it->second.second == aProv) {
	    // Unregister next requestor
	    // TODO [YB] Do we need to unregister this in requestor. In fact this
	    // is not invalidated but caller of UnregIfProv does
	    const TICacheRCtx& ctx = it->first.second;
	    if (ctx.size() > 1) {
		TICacheRCtx rctx(ctx);
		rctx.pop_back();
		Base* reqb = ctx.back();
		MUnit* reqe = reqb->GetObj(reqe);
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

void Unit::InvalidateIfCache(const string& aIfName)
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
		MUnit* reqe = reqb->GetObj(reqe);
		reqe->UnregIfProv(it->first.first, rctx, this, true);
	    }
	    // Unreg self as requestor
	    Base* prov = it->second.second;
	    MUnit* prove = prov->GetObj(prove);
	    if (prove == NULL) {
		prove = prov->GetObj(prove);
	    }
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

void Unit::UnregAllIfRel(TBool aInv)
{
    TICacheQFIter it = iICacheQF.begin();
    for (TICacheQFIter it = iICacheQF.begin(); it != iICacheQF.end(); it++) {
	// Unregister itself on requestor
	const TICacheRCtx& ctx = it->first.second;
	if (ctx.size() > 1) {
	    TICacheRCtx rctx(ctx);
	    rctx.pop_back();
	    Base* reqb = ctx.back();
	    MUnit* reqe = reqb->GetObj(reqe);
	    reqe->UnregIfProv(it->first.first, rctx, this, aInv);
	}
	// Unregister itself on provider
	Base* prov = it->second.second;
	MUnit* prove = prov->GetObj(prove);
	if (prove != this) {
	    TICacheRCtx rctx(ctx);
	    rctx.push_back(this);
	    prove->UnregIfReq(it->first.first, rctx);
	}
    }
    iICache.clear();
    iICacheQF.clear();
}

void Unit::RmIfCache(IfIter& aIt)
{
    iICache.erase(aIt.iCacheIter);
}

void Unit::InvalidateIfCache()
{
    UnregAllIfRel(ETrue);
}

void Unit::InsertIfQm(const string& aName, const TICacheRCtx& aReq, MUnit* aProv)
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

void Unit::InsertIfCache(const string& aName, const TICacheRCtx& aReq, MUnit* aProv, MIface* aVal)
{
    TICacheKeyF keyf(aName, aReq);
    TICacheKey key(keyf, aProv);
    if (aVal != NULL) {
	if (IsIftEnabled()) {
	    Logger()->Write(EInfo, this, "Iface [%s]: resolved [%x], caching", aName.c_str(), aVal);
	}
	pair<TICacheKey, MIface*> val(key, aVal);
	TICacheCIter res = iICache.insert(val);
    }
    InsertIfQm(aName, aReq, aProv);
}

void Unit::InsertIfCache(const string& aName, const TICacheRCtx& aReq, MUnit* aProv, TIfRange aRg)
{
    for (TIfIter it = aRg.first; it != aRg.second; it++) {
	InsertIfCache(aName, aReq, aProv, *it);
    }
    // Register the request in the map even if result is empty, ref ds_ifcache_refr_fpt
    if (aRg.first == aRg.second) {
	InsertIfQm(aName, aReq, aProv);
    }
}

void Unit::UpdateIfiLocal(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TICacheRCtx ctx(aCtx);
    ctx.push_back(this);
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
}

MIface* Unit::getLocalIface(const string& aName, const TICacheRCtx& aCtx)
{
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
    return res;
}

void Unit::UpdateIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TIfRange rr;
    TICacheRCtx ctx(aCtx);
    ctx.push_back(this);
    // Try local ifaces first
    MIface* res = (MIface*) DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    } else if (aName == MAgent::Type()) {
	for (auto it : iMComps) {
	    MUnit* comp = it.second;
	    res = comp->getLocalIface(aName, ctx);
	    if (res != NULL) {
		InsertIfCache(aName, aCtx, comp, res);
	    }
	}
    } else {
	TIfRange rg = GetIfi(MAgent::Type(), this);
	// Using cache of agents because iterators will be broken
	vector<MUnit*> ca;
	for (TIfIter it = rg.first; it != rg.second; it++) {
	    MAgent* ait = (MAgent*) (*it);
	    MIface* iit = ait->DoGetIface(MUnit::Type());
	    MUnit* eit = dynamic_cast<MUnit*>(iit);
	    ca.push_back(eit);
	}
	for (MUnit* agent : ca) {
	    if (agent != NULL && !ctx.IsInContext(agent)) {
		rr = agent->GetIfi(aName, ctx);
		InsertIfCache(aName, aCtx, agent, rr);
	    }
	}
    }
}

void Unit::LogIfReqs()
{
    Logger()->Write(EInfo, this, "[%x], Ifaces requests: START", this);
    for (TICacheQFIter it = iICacheQF.begin(); it != iICacheQF.end(); it++) {
	const TICacheRCtx& ctx = it->first.second;
	Base* provb = it->second.second;
	Unit* prov = provb->GetObj(prov);
	if (!ctx.empty()) {
	    Base* reqb = ctx.back();
	    Unit* reqe = reqb == NULL ? NULL : reqb->GetObj(reqe);
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

void Unit::DumpIfReqs() const
{
    cout << "<< Ifaces requests: START >>" << endl;
    for (auto it : iICacheQF) {
	const TICacheRCtx& ctx = it.first.second;
	Base* provb = it.second.second;
	MUnit* prov = provb->GetObj(prov);
	if (!ctx.empty()) {
	    cout << "[" << it.first.first << "], [" << ctx.size() << "]:" << endl;
	    for (auto* reqe : ctx) {
		cout << reqe << ": " << (reqe == NULL ? "NULL" : reqe->GetUri(NULL, ETrue)) << endl;
	    }
	    cout << "==> [" << prov << "," << prov->GetUri(NULL, ETrue) << "]" << endl << endl;
	}
	else {
	    cout << "If: [" << it.first.first << "], [none] - [" << prov << "," << prov->GetUri(NULL, ETrue) << "]" << endl;
	}
    }
    cout << "<< Ifaces requests: END >>" << endl;
}

void Unit::DumpIfCache() const
{
    cout << "<< Ifaces cache: START >>" << endl;
    for (auto it : iICache) {
	const TICacheRCtx& ctx = it.first.first.second;
	MIface* iface = it.second;
	MUnit* prov = it.first.second;
	if (!ctx.empty()) {
	    cout << "[" << it.first.first.first << "], [" << ctx.size() << "]:" << endl;
	    for (auto* reqe : ctx) {
		cout << reqe << ": " << (reqe == NULL ? "NULL" : reqe->GetUri(NULL, ETrue)) << endl;
	    }
	    cout << "==> [" << prov << "," << prov->GetUri(NULL, ETrue) << "] -> [" << iface << ": " << iface->Uid() << "]" << endl << endl;
	}
	else {
	    cout << "If: [" << it.first.first.first << "], [none] - [" << prov << "," << prov->GetUri(NULL, ETrue) << "] -> [" << iface << "]" << endl;
	}
    }
    cout << "<< Ifaces cache: END >>" << endl;
}


string Unit::EType(TBool aShort) const
{
    return string();
}

// Stated restriction: name to be unique, ref. ds_mut_nm

MUnit* Unit::GetComp(const string& aParent, const string& aName) const
{
    MUnit* res = NULL;
    if (iMComps.count(TNMKey(aName)) > 0) {
	res = iMComps.find(TNMKey(aName))->second;
    }
    return res;
}

TBool Unit::RebaseUri(const GUri& aUri, const MUnit* aBase, GUri& aRes)
{
    __ASSERT(aUri.IsAbsolute());
    TBool res = EFalse;
    if (aUri.IsErr()) return res;
    GUri::const_elem_iter it = aUri.Elems().begin();
    if (it != aUri.Elems().end()) {
	TBool anywhere = it->name() == GUri::KTypeAnywhere;
	__ASSERT(!anywhere);
	if (it->name().empty()) {
	    MUnit* root = NULL;
	    if (it->rel() == GUri::EComp) {
		root = GetRoot();
	    } else if (it->rel() == GUri::EChild) {
		// Unit doesn't support inher hier, error
	    }
	    if (root != NULL) {
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
	    }
	} else {
	    res = RebaseUri(aUri, it, EFalse, aBase, aRes);
	}
    }
    return res;
}

TBool Unit::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MUnit* aBase, GUri& aRes)
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
		Unit* node = Provider()->GetNode(elem.name());
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
		MUnit* node = iMComps.at(elem.name());
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

MUnit* Unit::GetNode(const GUri& aUri, TBool aInclRm) 
{ 
    if (aUri.IsErr()) return NULL;
    MUnit* res = NULL;
    GUri::const_elem_iter it = aUri.Elems().begin(); 
    if (it != aUri.Elems().end()) {
	TBool anywhere = (*it).name() == GUri::KTypeAnywhere; 
	if (it->name().empty() || anywhere) {
	    MUnit* root = NULL;
	    if (it->rel() == GUri::EComp) {
		// Using env's root to get root, because of in case of native agent
		// GetRoot() doesn't work, ref ds_di_nacgna
		root = iEnv->Root()->GetRoot();
	    }
	    else if (it->rel() == GUri::EChild) {
		// Unit doesn't support inher hier, error
	    }
	    if (root != NULL) {
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

MUnit* Unit::GetNodeS(const char* aUri)
{
    MUnit* mel = GetNode(aUri, ETrue);
    return mel;
}

MUnit* Unit::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm) 
{
    Pdstat(PEvents::DurStat_GetNode, true);
    MUnit* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    __ASSERT(elem.rel() != GUri::EChild);
    __ASSERT(!aAnywhere);
    if (elem.name() == "..") {
	// Checking if mutation context is defined. Using context instead of owner if so, ref ds_daa_itn_sfo
	MUnit* top = (mContext == NULL ? iMan : mContext);
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
		Unit* node = Provider()->GetNode(elem.name());
		if (node != NULL) {
		    res = node->GetNode(aUri, uripos, EFalse, aInclRm);
		}
	    } else {
		res = Provider()->GetNode(elem.name());
	    }
	} else {
	    if (iMComps.count(elem.name()) > 0) {
		MUnit* node = iMComps.at(elem.name());
		uripos++;
		if (uripos != aUri.Elems().end()) {
		    res = node->GetNode(aUri, uripos, EFalse, aInclRm);
		} else {
		    res = node;
		}
	    }
	}
    }
    Pdstat(PEvents::DurStat_GetNode, false);
    return res;
}

#ifdef UNICONTENT

TBool Unit::IsContOfCategory(const string& aName, const string& aCategory) const
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
void Unit::InsertContCompsRec(const string& aName, const string& aComp)
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

void Unit::InsertContent(const string& aName)
{
    if (!aName.empty()) {
	string oname = GetContentOwner(aName);
	InsertContCompsRec(oname, aName);
	InsertContent(oname);
    }
}

void Unit::RemoveContent(const string& aName)
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

void Unit::RemoveContentComp(const string& aOwner, const string& aComp)
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

void Unit::SetContentValue(const string& aName, const string& aValue)
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

void Unit::SetContentCategory(const string& aName, const string& aCategory)
{
    TContentKey key(aName, ECrl_Category);
    mContent.insert(TContentRec(key, aCategory));
}

TBool Unit::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) 
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
	iEnv->Observer()->OnCompChanged(*this, aName, aRtOnly);
    }
    return res;
}

TBool Unit::ContentHasComps(const string& aContName) const
{
    return (GetContCount(aContName) > 0);
}

string Unit::GetContent(const string& aId, TBool aFull) const
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

TBool Unit::ContentExists(const string& aName) const
{
    return (ContValueExists(aName) || ContentHasComps(aName));
}

TBool Unit::ContValueExists(const string& aName) const
{
    return (mContent.count(TContentKey(aName, ECrl_Value)) > 0);
}

string Unit::GetContComp(const string& aOwnerName, TInt aInd) const
{
    __ASSERT(aInd < GetContCount(aOwnerName));
    TContentKey compkey(aOwnerName, ECrl_Component);
    pair<TContent::const_iterator, TContent::const_iterator> range = mContent.equal_range(compkey);
    TContent::const_iterator it = range.first;
    for (TInt cnt = aInd; it != range.second && cnt > 0; it++, cnt--);
    return it->second;
}

TInt Unit::GetContCount(const string& aName) const
{
    return mContent.count(TContentKey(aName, ECrl_Component));
}

string Unit::GetContentValue(const string& aName) const
{
    string res;
    TContent::const_iterator it = mContent.find(TContentKey(aName, ECrl_Value));
    if (it != mContent.end()) {
	res = it->second;
    }
    return res;
}


void Unit::DumpContent() const
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

void Unit::SetContentValue(const string& aName, const string& aValue)
{
}

// TODO Not used?
void Unit::InsertContentComp(const string& aContName, const string& aCompName)
{
}

TBool Unit::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) 
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

void Unit::InsertContent(const string& aName)
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

void Unit::InsertContentComp(const string& aContName, const string& aCompName)
{
    InsertContCompsRec(aContName, aCompName);
    InsertContent(aContName);
}

void Unit::InsertContCompsRec(const string& aName, const string& aComp)
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

TBool Unit::ContentHasComps(const string& aContName) const
{
    return (mCntComps.count(aContName) > 0);
}

string Unit::GetContent(const string& aId, TBool aFull) const
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

TBool Unit::ContentExists(const string& aName) const
{
    return (mCntVals.count(aName) > 0) || (mCntComps.count(aName) > 0);
}

TBool Unit::ContValueExists(const string& aName) const
{
    return (mCntVals.count(aName) > 0);
}

string Unit::GetContComp(const string& aOwnerName, TInt aInd) const
{
    __ASSERT(aInd < GetContCount(aOwnerName));
    pair<TCntComps::const_iterator, TCntComps::const_iterator> range = mCntComps.equal_range(aOwnerName);
    TCntComps::const_iterator it = range.first;
    for (TInt cnt = aInd; it != range.second && cnt > 0; it++, cnt--);
    return it->second;
}

TInt Unit::GetContCount(const string& aName) const
{
    return mCntComps.count(aName);
}

void Unit::DumpCntVal() const
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

TBool Unit::ChangeAttr(TNodeAttr aAttr, const string& aVal) 
{ 
    TBool res = EFalse;
    if (aAttr == ENa_Id) {
	string sOldName(Name());
	iName = aVal;
	res = iMan->OnCompRenamed(*this, sOldName);
	iEnv->Observer()->OnCompRenamed(*this, sOldName);
	if (!res) {
	    // Rollback
	    iName = sOldName;
	}
    }
    return res;
}


TBool Unit::AppendComp(MUnit* aComp, TBool aRt)
{
    MUnit* cowner = aComp->GetMan();
    __ASSERT(cowner == NULL || cowner == this);
    if (cowner == NULL) {
	aComp->SetMan(this);
    }
    TBool res = RegisterComp(aComp);
    if (res)
    {
	// Invalidate iface cache in case if agents has been deleted
	MAgent* iagt = aComp->GetObj(iagt);
	if (iagt != NULL) {
	    InvalidateIfCache(MAgent::Type());
	}

	// Propagate notification only if self is the component of owner, ref ds_di_cnfr
	if (iMan != NULL && iMan->GetComp(string(), Name()) == this) {
	    Pdstat(PEvents::DurStat_OnCompAdd, true);
	    iMan->OnCompAdding(*aComp, aRt);
	    Pdstat(PEvents::DurStat_OnCompAdd, false);
	    iEnv->Observer()->OnCompAdding(*aComp, aRt);
	}
    }
    return res;
}

void Unit::RemoveComp(MUnit* aComp)
{
    MUnit* cowner = aComp->GetMan();
    __ASSERT(cowner == this);
    MAgent* iagt = aComp->GetObj(iagt);
    TBool res = UnregisterComp(aComp);
    if (res) {
	aComp->SetMan(NULL);
    }
    // Invalidate iface cache in case if agents has been added
    if (iagt != NULL) {
	InvalidateIfCache(MAgent::Type());
    }

}

TBool Unit::RegisterComp(MUnit* aComp)
{
    TBool res = ETrue;
    __ASSERT(iMComps.count(aComp->Name()) == 0);
    iMComps.insert(TNMVal(aComp->Name(), aComp));
    return res;
}

TBool Unit::UnregisterComp(MUnit* aComp, const string& aName)
{
    TBool res = EFalse;
    const string& name = aName.empty() ? aComp->Name() : aName;
    assert (iMComps.count(name) > 0); 
    iMComps.erase(name);

    res = ETrue;
    return res;
}

MUnit* Unit::GetNode(const string& aUri, TBool aInclRm)
{
    MUnit* res = NULL;
    GUri uri(aUri);
    if (!uri.IsErr()) {
	res = GetNode(uri, aInclRm);
    }
    else  {
	Logger()->Write(EErr, this, "Incorrect URI [%s]", aUri.c_str());
    }
    return res;
}

void Unit::OnCompDeleting(MUnit& aComp, TBool aSoft, TBool aModif)
{
    // Translate to hier
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    /*
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	iMan->OnCompDeleting(aComp, aSoft, aModif);
    }
    */
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
void Unit::OnCompAdding(MUnit& aComp, TBool aModif)
{
    //Pdstat(PEvents::DurStat_OnCompAdd, true);
#ifdef __EXT_ASSERTS__
    string url = aComp.GetUri(NULL, ETrue);
    MUnit* comp = GetNode(url);
    __ASSERT(comp != NULL);
#endif
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    /*
    if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	iMan->OnCompAdding(aComp, aModif);
    }
    */
    //Pdstat(PEvents::DurStat_OnCompAdd, false);
}

TEhr Unit::ProcessCompChanged(MUnit& aComp, const string& aContName)
{
    Pdstat(PEvents::DurStat_OnCompChanged, true);
    TEhr res = EEHR_Ignored;
    TBool hres = EFalse;
    TIfRange rg = GetIfi(MAgent::Type(), this);
    // Using cache of agents because iterators will be broken
    vector<MUnit*> ca;
    for (TIfIter it = rg.first; it != rg.second; it++) {
	MAgent* ait = (MAgent*) (*it);
	MIface* iit = ait->DoGetIface(MUnit::Type());
	MUnit* eit = dynamic_cast<MUnit*>(iit);
	ca.push_back(eit);
    }
    for (MUnit* agent : ca) {
	MACompsObserver* iagent = agent == NULL ? NULL : agent->GetObj(iagent);
	if (iagent != NULL) {
	    hres = iagent->HandleCompChanged(*this, aComp, aContName);
	    res = hres ? EEHR_Accepted : EEHR_Denied;
	    if (!hres) {
		iagent->HandleCompChanged(*this, aComp, aContName);
	    }
	}
    }
    return res;
}

TEhr Unit::ProcessChanged(MUnit& aComp)
{
    Pdstat(PEvents::DurStat_OnCompChanged, true);
    TEhr res = EEHR_Ignored;
    TBool hres = EFalse;
    TIfRange rg = GetIfi(MAgent::Type(), this);
    // Using cache of agents because iterators will be broken
    vector<MUnit*> ca;
    for (TIfIter it = rg.first; it != rg.second; it++) {
	MAgent* ait = (MAgent*) (*it);
	MIface* iit = ait->DoGetIface(MUnit::Type());
	MUnit* eit = dynamic_cast<MUnit*>(iit);
	ca.push_back(eit);
    }
    for (MUnit* agent : ca) {
	MACompsObserver* iagent = agent->GetObj(iagent);
	if (iagent != NULL) {
	    hres = iagent->HandleChanged(*this, aComp);
	    res = hres ? EEHR_Accepted : EEHR_Denied;
	}
    }
    return res;
}

// TODO [YB] To include agents as member of elem. This will be more effective
// TODO [YB] The current scheme doesn't allow element to get notification of related node change.
// The only notification thru hier can be get via MOwner, but not relation thru edges.
// Thus if node A is related to node B via the chain of edges, the change of B or even the change
// of the relation chain cannot be observed by node A. This is the serious lack. This is because the 
// If providing mechanism is used instead of full relations tracking in Vert. So node A has Ifaces cache
// that includes ifaces from node B but there is no mechanism of the changes in the cache. To consider
// to implement cache update notification. Ref UC_010 
TBool Unit::OnCompChanged(MUnit& aComp, const string& aContName, TBool aModif)
{
    Pdstat(PEvents::DurStat_OnCompChanged, true);
    TBool res = EFalse;
    TEhr pres = ProcessCompChanged(aComp, aContName);

    // Propagate to upper layer if the notification wasn't denied
    // TODO To consider if the event is to be propagated to upper level even if it has been already handled
    // Enable propagation from attached node only, ref ds_di_cnfr_snpn
    if (pres == EEHR_Ignored) {
	if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	    res = iMan->OnCompChanged(aComp, aContName, aModif);
	}
    } else {
	res = (pres == EEHR_Accepted);
    }
    Pdstat(PEvents::DurStat_OnCompChanged, false);
    return res;
}

TBool Unit::OnCompRenamed(MUnit& aComp, const string& aOldName)
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
    /*
       if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
       iMan->OnCompRenamed(aComp, aOldName);
       }
       */
    return res;
}

// TODO not used, why?
TBool Unit::OnChanged(MUnit& aComp)
{
    TBool res = EFalse;
    // TODO Single CompObserver API is used both for handling CompChange
    // and Change events. 
    TEhr pres = ProcessCompChanged(aComp, string());

    if (pres == EEHR_Ignored) {
	if (iMan != NULL && (mContext == NULL || mContext == iMan)) {
	    res = iMan->OnChanged(aComp);
	}
    } else {
	res = (pres == EEHR_Accepted);
    }
    return res;
}

MUnit* Unit::GetCompOwning(const string& aParent, MUnit* aElem)
{
    MUnit* res = NULL;
    MUnit* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this && node->EType() == aParent)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

MUnit* Unit::GetCompOwning(MUnit* aElem)
{
    MUnit* res = NULL;
    MUnit* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

const MUnit* Unit::GetCompOwning(const MUnit* aElem) const
{
    const MUnit* res = NULL;
    const MUnit* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

TBool Unit::IsComp(const MUnit* aElem) const
{
    const MUnit* man = aElem->GetMan();
    while (man != NULL && man != this) {
	man = man->GetMan();	
    }
    return man == this;
}

MUnit* Unit::GetCommonOwner(MUnit* aElem)
{
    MUnit* res = NULL;
    MUnit* owner = this;
    while (owner != NULL && res == NULL) 
    {
	if (owner->IsComp(aElem) || owner == aElem) {
	    res = owner;
	}
	owner = owner->GetMan();
    }
    return res;
}

void Unit::GetRUri(GUri& aUri, MUnit* aTop)
{
    if (IsProvided()) {
	// Native agent
	aUri.PrependElem(EType(), Name(), GUri::KSepNone);
    } else {
	// Not native agent, process
	MUnit* cowner = GetCommonOwner(aTop);
	GetUri(aUri, cowner);
	MUnit* owner = aTop;
	while (owner != cowner) {
	    aUri.PrependElem("", GUri::KUpperLevel);
	    owner = owner->GetMan();
	}
    }
}

string Unit::GetRUri(MUnit* aTop)
{
    GUri uri;
    GetRUri(uri, aTop);
    return uri.toString();
}

void Unit::GetUri(GUri& aUri, MUnit* aTop) const
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

string Unit::GetUri(MUnit* aTop, TBool aShort) const
{
    GUri uri;
    GetUri(uri, aTop);
    return uri.toString(aShort);
}

// TODO [YB] To consider if GetRoot can be moved from MUnit to Unit
// Actually it is using for Unit impl only and can be replaced by
// sequentual steps to owners, see example below. The problem is
// that seq steps approach is not optimal in distributed async models
// because it requires intermodels exchange for each step
/*
   MUnit* Unit::GetRoot() const
   {
   MUnit* res = (MUnit*) this;
   MUnit* owner = res->GetMan();
   while (owner != NULL) {
   res = owner;
   owner = res->GetMan();
   }
   return res;
   }
   */
MUnit* Unit::GetRoot() const
{
    MUnit* res = (MUnit*) this;
    if (iMan != NULL) {
	res = iMan->GetRoot();
    }
    return res;
}

TBool Unit::IsLogeventCreOn() 
{
    MUnit* node = GetNode("./Unit:Logspec/Unit:Creation");
    string ls = GetContent("Debug.Creation");
    return node != NULL || ls == "y";
}

TBool Unit::IsRemoved() const
{
    return isRemoved;
}

void Unit::SetRemoved(TBool aModif)
{
    // Remove node from native hier but keep it in inher hier
    // Notify the man of deleting
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, ETrue, aModif);
	iEnv->Observer()->OnCompDeleting(*this, ETrue, aModif);
    }
    for (auto& it : iMComps) {
	MUnit* comp = it.second;
	comp->SetRemoved(ETrue);
    }
    // Mark node as removed from hative hier
    isRemoved = ETrue;
}

TInt Unit::GetCapacity() const
{
    TInt res = 0;
    for (auto& it : iMComps) {
	res++;
	MUnit* comp = it.second;
	res += comp->GetCapacity();
    }
    return res;
}

TInt Unit::CompsCount() const
{
    return iMComps.size();
}

MUnit* Unit::GetComp(TInt aInd)
{
    auto it = iMComps.begin();
    for (TInt i = 0; i < aInd; i++, it++);
    return it->second;
}

MIface* Unit::Call(const string& aSpec, string& aRes)
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
    } else if (name == "EType") {
	aRes = EType(Ifu::ToBool(args.at(0)));
    } else if (name == "AppendComp") {
	MUnit* comp = GetNode(args.at(0));
	TBool rr = AppendComp(comp);
	aRes = Ifu::Pack(rr);
    } else if (name == "RemoveComp") {
	MUnit* comp = GetNode(args.at(0));
	RemoveComp(comp);
    } else if (name == "GetUri") {
	MUnit* base = GetNode(args.at(0));
	aRes = GetUri(base, ETrue);
    } else if (name == "GetRUri") {
	MUnit* base = GetNode(args.at(0));
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
	TICacheRCtx ctx;
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MUnit* elem = GetNode(suri);
	    ctx.push_back(elem);
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
	TICacheRCtx ctx;
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MUnit* elem = GetNode(suri);
	    ctx.push_back(elem);
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
	TICacheRCtx ctx;
	for (vector<string>::iterator it = ctxe.begin(); it != ctxe.end(); it++) {
	    string suri = *it;
	    MUnit* elem = GetNode(suri);
	    ctx.push_back(elem);
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
	MUnit* prov = GetNode(args.at(2));
	TBool inv = Ifu::ToBool(args.at(3));	
	UnregIfProv(name, ctx, prov, inv);
    } else if (name == "OnCompAdding") {
	MUnit* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	OnCompAdding(*comp);
    } else if (name == "OnCompChanged") {
	MUnit* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool rr = OnCompChanged(*comp, args.at(1));
	aRes = Ifu::Pack(rr);
    } else if (name == "OnChanged") {
	MUnit* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool res = OnCompChanged(*comp);
	aRes = Ifu::Pack(res);
    } else if (name == "OnCompDeleting") {
	MUnit* comp = GetNode(args.at(0), ETrue);
	if (comp == NULL) {
	    throw (runtime_error("Cannot get node " + args.at(0)));
	}
	TBool soft = Ifu::ToBool(args.at(1));
	OnCompDeleting(*comp, soft);
    } else if (name == "CompsCount") {
	TInt cnt = CompsCount();
	aRes = Ifu::Pack(cnt);
    } else if (name == "GetComp") {
	TInt ind = Ifu::ToInt(args.at(0));
	res = GetComp(ind);
    } else if (name == "GetComp#2") {
	res = GetComp(args.at(0), args.at(1));
    } else if (name == "IsRemoved") {
	TBool rr = IsRemoved();
	aRes = Ifu::Pack(rr);
    } else if (name == "IsComp") {
	MUnit* comp = GetNode(args.at(0));
	if (comp == NULL) {
	    throw (runtime_error("Cannot get agent" + args.at(0)));
	} else {
	    TBool rr = IsComp(comp);
	    aRes = Ifu::FromBool(rr);
	}
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

TInt Unit::IfRangeSize(const TIfRange& aRange) const
{
    TInt res = 0;
    for (TIfIter it = aRange.first; it != aRange.second; it++, res++);
    return res;
}

void* Unit::GetIfind(TIfRange& aRange, TInt aInd)
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

void Unit::DumpComps(TBool aRecurs) const
{
    for (TNMReg::const_iterator it = iMComps.begin(); it != iMComps.end(); it++) {
	MUnit* comp = it->second;
	cout << comp->GetUri(NULL, ETrue) << ": " << (void*) comp << endl;
	if (aRecurs)
	    comp->DumpComps(aRecurs);
    }
}

TBool Unit::IsProvided() const
{
    return Provider()->IsProvided(this);
}

string Unit::ContentCompId(const string& aOwnerName, const string& aCompName)
{
    return  aOwnerName.empty() ? aCompName : aOwnerName + KContentSep + aCompName;
}

// TODO Dup of ContentCompId
string Unit::ContentKey(const string& aBase, const string& aSuffix)
{
    return  aBase + KContentSep + aSuffix;
}

string Unit::ContentValueKey(const string& aId)
{
    return  aId + KContentSep + KContentKey_Value;
}

string Unit::ContentCompsKey(const string& aId)
{
    return  aId + KContentSep + KContentKey_Comps;
}

TBool Unit::IsContentCompOf(const string& aContName, const string& aContOwner)
{
    TBool res = aContName.find(aContOwner) == 0;
    return res;
}

TBool Unit::ContentHasComp(const string& aContFname, const string& aCompName) const
{
    return ContentExists(ContentKey(aContFname, aCompName));
}

string Unit::GetAssociatedData(const string& aUri) const
{
    return string();
}

MUnit* Unit::GetCtx()
{
    return mContext;
}

void Unit::SetCtx(MUnit* aContext)
{
    __ASSERT(mContext == NULL && aContext != NULL || aContext == NULL);
    mContext = aContext;
}

void Unit::DumpIfCtx(const TICacheRCtx& aCtx)
{
    cout << "<< Dump of Ifaces request context: START >>" << endl;
    for (auto item: aCtx) {
	MUnit* eitem = item->GetObj(eitem);
	string uri = eitem->GetUri(NULL, true);
	cout << uri << endl;
    }
    cout << "<< Dump of Ifaces request context: END >>" << endl;
}

void Unit::DumpIfRange(const TIfRange& aRange)
{
    cout << "<< Dump of Ifaces range: START >>" << endl;
    for (auto it = aRange.first; it != aRange.second; it++) {
	const MIface* item = *it;
	string uri = item->Uid();
	cout << uri << endl;
    }
    cout << "<< Dump of Ifaces range: END >>" << endl;
}

MUnit* Unit::CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext)
{
    Pdstat(PEvents::DurStat_CreateHeir, true);
    MUnit* uheir = NULL;
    if (IsProvided()) {
	uheir = Provider()->CreateNode(Name(), aName, aMan, iEnv);
	aMan->AppendComp(uheir);
	uheir->SetCtx(aContext);
    } else {
	Logger()->Write(EErr, this, "The parent is not provided: [%s]: -->", aName.c_str());
    }
    Pdstat(PEvents::DurStat_CreateHeir, false);
    return uheir;
}
