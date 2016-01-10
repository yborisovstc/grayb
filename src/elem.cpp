#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sys/time.h>
#include <stdexcept> 
#include "mmod.h"
#include "ifu.h"

TBool Elem::EN_PERF_TRACE = EFalse;
TBool Elem::EN_PERF_METR = EFalse;
TBool Elem::EN_PERF_DBG1 = ETrue;
TBool Elem::EN_MUT_LIM = ETrue;

string Elem::Fmt::mSepContInp = ";";
string Elem::Fmt::mSepContName = "~";
// TODO [YB] Alternative method via static Ifu derived is used, remaining for info only
//string IfuPars = "Name,0,GetNode,1,Mutate,4";
//Ifu Elem::mIfu(IfuPars);

MElem::EIfu MElem::mIfu;

// Ifu static initialisation
MElem::EIfu::EIfu()
{
    RegMethod("Name", 0);
    RegMethod("GetMan", 0);
    RegMethod("GetAttachingMgr", 0);
    RegMethod("GetRoot", 0);
    RegMethod("GetNode", 1);
    RegMethod("GetCont", 1);
    RegMethod("Mutate", 4);
}

string Elem::PEType()
{
    return string() + GUri::KParentSep + Elem::Type();
}

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


Elem::IterImplBase::IterImplBase(Elem& aElem, GUri::TElem aId, TBool aToEnd): iElem(aElem), iId(aId), iExt(GUri::KTypeAny)
{
    char rel = SRel();
    if (!iId.first.empty()) {
	iExtsrel = iId.first.at(iId.first.size() - 1);
	if (iExtsrel == GUri::KParentSep || iExtsrel == GUri::KNodeSep) {
	    iExt = iId.first.substr(0, iId.first.size() - 1);
	}
    }
    if (rel == GUri::KNodeSep) {
	if (iId.second.second == GUri::KTypeAny) {
	    iCIterRange = TNMRegItRange(iElem.iMComps.begin(), iElem.iMComps.end());
	} else {
	    iCIterRange = iElem.iMComps.equal_range(iId.second.second);
	}
	if (aToEnd) {
	    iCIter = iCIterRange.second;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KParentSep || iExt.empty() || iExt == GUri::KTypeAny) {
		iCIter = iCIterRange.first;
		for (; iCIter != iCIterRange.second && iCIter->second->IsRemoved(); iCIter++); 
	    } 
	    else {
		for (iCIter = iCIterRange.first; iCIter != iCIterRange.second; iCIter++) {
		    MElem* comp = iCIter->second;
		    if (comp->GetParent()->Name() == iExt && !comp->IsRemoved()) {
			break;
		    }
		}
	    }
	}
    }
    else if (rel == GUri::KParentSep) {
	if (iId.second.second == GUri::KTypeAny) {
	    iChildsRange = TNMRegItRange(iElem.iChilds.begin(), iElem.iChilds.end());
	} else {
	    iChildsRange = iElem.iChilds.equal_range(iId.second.second);
	}
	if (aToEnd) {
	    iChildsIter = iChildsRange.second;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KNodeSep || iExt.empty() || iExt == GUri::KTypeAny) {
		iChildsIter = iChildsRange.first;
		for (; iChildsIter != iChildsRange.second && iChildsIter->second->IsRemoved(); iChildsIter++); 
	    }
	    else {
		for (iChildsIter = iChildsRange.first; iChildsIter != iChildsRange.second; iChildsIter++) {
		    MElem* comp = iChildsIter->second;
		    MElem* cowner = comp->GetMan()->GetObj(cowner);
		    if (cowner->Name() == iExt && !comp->IsRemoved()) {
			break;
		    }
		}
	    }
	}
    }
};

Elem::IterImplBase::IterImplBase(const IterImplBase& aIt): 
    iElem(aIt.iElem), iId(aIt.iId), iCIterRange(aIt.iCIterRange), iCIter(aIt.iCIter), iChildsIter(aIt.iChildsIter), 
    iExtsrel(aIt.iExtsrel), iExt(aIt.iExt)
{
};

Elem::MIterImpl* Elem::IterImplBase::Clone()
{
    return new IterImplBase(*this);
}

void Elem::IterImplBase::Set(const MIterImpl& aImpl)
{
    const IterImplBase& impl = dynamic_cast<const IterImplBase&>(aImpl);
    iElem = impl.iElem;
    iId = impl.iId;
    iCIterRange = impl.iCIterRange;
    iCIter = impl.iCIter; 
    iChildsIter = impl.iChildsIter;
}

char Elem::IterImplBase::SRel() const
{
    return iId.second.first;
}

void Elem::IterImplBase::PostIncr()
{
    if (SRel() == GUri::KNodeSep) {
	iCIter++;
	// Omit removed comps from the look
	for (; iCIter != iCIterRange.second && iCIter->second->IsRemoved(); iCIter++); 
	if (!iId.first.empty() && iExtsrel == GUri::KParentSep && !iExt.empty() && iExt != GUri::KTypeAny) {
	    for (; iCIter != iCIterRange.second; iCIter++) {
		MElem* comp = iCIter->second;
		if (comp->GetParent()->Name() == iExt && !comp->IsRemoved()) {
		    break;
		}
	    }
	}
    }
    else {
	iChildsIter++;
	// Omit removed children from the look
	for (; iChildsIter != iChildsRange.second && iChildsIter->second->IsRemoved(); iChildsIter++); 
	if (!iId.first.empty() && iExtsrel == GUri::KNodeSep && !iExt.empty() && iExt != GUri::KTypeAny) {
	    for (;iChildsIter != iChildsRange.second; iChildsIter++) {
		MElem* comp = iChildsIter->second;
		MElem* cowner = comp->GetMan()->GetObj(cowner);
		if (cowner->Name() == iExt && !comp->IsRemoved()) {
		    break;
		}
	    }
	}
    }
}

TBool Elem::IterImplBase::IsCompatible(const MIterImpl& aImpl) const
{
    return ETrue;
}

TBool Elem::IterImplBase::IsEqual(const MIterImpl& aImplm) const
{
    const IterImplBase& aImpl = dynamic_cast<const IterImplBase&>(aImplm);
    TBool res = EFalse;
    if (IsCompatible(aImpl) && aImpl.IsCompatible(*this)) {
	res = &iElem == &(aImpl.iElem) && iId == aImpl.iId && iCIter == aImpl.iCIter && iChildsIter == aImpl.iChildsIter;
    }
    return res;
}

MElem*  Elem::IterImplBase::GetElem()
{
    MElem* res = NULL;
    if (SRel() == GUri::KNodeSep) {
	if (iCIter != iCIterRange.second) {
	    res = iCIter->second;
	}
    }
    else if (SRel() == GUri::KParentSep) {
	res = iChildsIter->second;
    }
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
    iObserver(NULL), iParent(NULL), isRemoved(EFalse)
{
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Constr";
    Logger()->Write(MLogRec::EInfo, this, ss.str().c_str());
    */
    //Logger()->Write(MLogRec::EInfo, this, "Elem constr p0");
    //Delay(100);
    //Logger()->Write(MLogRec::EInfo, this, "Elem constr p1");
    iMut = Provider()->CreateChromo();
    //Logger()->Write(MLogRec::EInfo, this, "Elem constr p2");
    iMut->Init(ENt_Node);
    //Logger()->Write(MLogRec::EInfo, this, "Elem constr p3");
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    //Logger()->Write(MLogRec::EInfo, this, "Elem constr p4");
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetParent(Type());
    iComps.reserve(100);
}

Elem::Elem(Elem* aMan, MEnv* aEnv): iName(Type()), iMan(aMan), iEnv(aEnv),
    iObserver(NULL), iParent(NULL)
{
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Constr";
    Logger()->Write(MLogRec::EInfo, this, ss.str().c_str());
    */
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetParent(string());
}

Elem::~Elem() 
{
    // Notify the man of deleting
    /*
    stringstream ss;
    long prt = (long) this;
    ss << prt << ";Destr";
    Logger()->Write(MLogRec::EInfo, this, ss.str().c_str());
    */

    // TODO [YB] OnCompDeleting called only here, i.e. after Elem derivation desctuctor completed
    // This means that the ovner notified will not be able to understand what agent is deleted
    // To consider to add one more notification - before deletions
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, EFalse);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(*this, EFalse);
    }
    if (iParent != NULL) {
	iParent->OnChildDeleting(this);
    }
    // Unregigster ifaces providers
    UnregAllIfRel();
    // Remove the comps, using iterator refresh because the map is updated on each comp deletion
    vector<MElem*>::reverse_iterator it = iComps.rbegin();
    while (it != iComps.rend()) {
	delete *it;
	it = iComps.rbegin();
    }
    iComps.clear();
    iMComps.clear();
    // Disconnect from the childs
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	MElem* child = it->second;
	child->OnParentDeleting(this);
	//child->SetParent(NULL);
    }
    iChilds.clear();
    iEnv = NULL; // Not owned
    if (iMut != NULL) {
	delete iMut;
	iMut = NULL;
    }
    // Remove deps
    RmCMDeps();
    RmMCDeps();
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

const vector<MElem*>& Elem::Comps() const
{
    return iComps;
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

void Elem::SetObserver(MCompsObserver* aObserver)
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

void *Elem::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MOwner::Type()) == 0) {
	res = (MOwner*) this;
    } else if (strcmp(aName, MElem::Type()) == 0) {
	res = (MElem*) this;
    }
    return res;
}

Elem::TIfRange Elem::GetIfi(const string& aName, const RqContext* aCtx)
{
    // Get from cache first
    TICacheRCtx req;
    // Add NULL to context in case of anonymous context
    // This is required to avoid problem with If relations invalidating
    // The matter is that the initial requestor doesnt cache relation into its cache
    // So on invalidation provider checks if context is initial (size == 1), if so it ignore relation
    // But it is possible that size == 1 for second requestor if the initial makes anonimous request
    // To avoid this inconsistance we need to explicitly register even anonymous requestor
    RqContext newctx(NULL);
    const RqContext* ctx(aCtx == NULL ? &newctx: aCtx);
    ToCacheRCtx(ctx, req);
    IfIter beg(this, aName, req);
    IfIter end(this, aName, req, ETrue);
    if (beg == end) {
	if (IsIftEnabled()) {
	    Logger()->Write(MLogRec::EInfo, this, "Iface [%s]: -->", aName.c_str());
	}
	// Invalid cache, update cache
	// Register the request with local provider anycase to create relation. 
	// This is required for correct invalidation.
	InsertIfQm(aName, req, this);
	UpdateIfi(aName, ctx);
	beg = IfIter(this, aName, req);
	end = IfIter(this, aName, req, ETrue);
    } else {
	if (IsIftEnabled()) {
	    Logger()->Write(MLogRec::EInfo, this, "Iface [%s]: resolved from cache", aName.c_str());
	}
    }
    return TIfRange(beg, end);
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
	Elem* prove = prov->GetObj(prove);
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
void Elem::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, Elem* aProv, TBool aInv)
{
    TICacheKeyF query(aIfName, aCtx);
    TICacheQFRange rg = iICacheQF.equal_range(query);
    // Unfortunatelly we cannot check the conditions of record here. This is because
    // the track of providers is not stored in requests cache so it is possible
    // that for single request here, i.e. resolved by one provider, this provider can 
    // have more that one requests, i.e multiple providers. So it is possible that
    // provider requests unreg request but it is already unreged

    //for (TICacheQFIter it = rg.first; it != rg.second; it++) {
    TICacheQFIter it = rg.first; 
    while (it != rg.second) {
	Base* cnd = it->second.second;
	Elem* cnde = cnd->GetObj(cnde);
	if (it->second.second == aProv || aInv) {
	    // Unregister next requestor
	    // TODO [YB] Do we need to unregister this in requestor. In fact this
	    // is not invalidated but caller of UnregIfProv does
	    const TICacheRCtx& ctx = it->first.second;
	    if (ctx.size() > 1) {
		TICacheRCtx rctx(ctx);
		rctx.pop_back();
		Base* reqb = ctx.back();
		Elem* reqe = reqb->GetObj(reqe);
		reqe->UnregIfProv(it->first.first, rctx, this, aInv);
	    }
	    iICache.erase(it->second);
	    iICacheQF.erase(it);
	    if (!aInv) break;
	    rg = iICacheQF.equal_range(query);
	    it = rg.first; 
	}
	else {
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
	    Elem* reqe = reqb->GetObj(reqe);
	    reqe->UnregIfProv(it->first.first, rctx, this, aInv);
	}
	// Unregister itself on provider
	Base* prov = it->second.second;
	Elem* prove = prov->GetObj(prove);
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
	    Logger()->Write(MLogRec::EInfo, this, "Iface [%s]: resolved [%x], caching", aName.c_str(), aVal);
	}
	pair<TICacheKey, void*> val(key, aVal);
	TICacheCIter res = iICache.insert(val);
    }
    InsertIfQm(aName, aReq, aProv);
}

void Elem::InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, void* aVal)
{
    TICacheRCtx req;
    ToCacheRCtx(aCtx, req);
    InsertIfCache(aName, req, aProv, aVal);
}

void Elem::InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg)
{
    for (TIfIter it = aRg.first; it != aRg.second; it++) {
	InsertIfCache(aName, aReq, aProv, *it);
    }
    // Register the request in the map even if result is empty
    if (aRg.first == aRg.second) {
	InsertIfQm(aName, aReq, aProv);
    }
}

void Elem::InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, TIfRange aRg)
{
    TICacheRCtx req;
    ToCacheRCtx(aCtx, req);
    InsertIfCache(aName, req, aProv, aRg);
}

Elem* Elem::GetIcCtxComp(const TICacheRCtx& aCtx, TInt aInd) 
{
    Elem* res = NULL;
    if (aInd < aCtx.size()) {
	Base* cc = aCtx.at(aInd);
	res = cc->GetObj(res);
    }
    return res;
}

void Elem::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = DoGetObj(aName.c_str());
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
}

void Elem::LogIfReqs()
{
    Logger()->Write(MLogRec::EInfo, this, "[%x], Ifaces requests: START", this);
    for (TICacheQFIter it = iICacheQF.begin(); it != iICacheQF.end(); it++) {
	const TICacheRCtx& ctx = it->first.second;
	Base* provb = it->second.second;
	Elem* prov = provb->GetObj(prov);
	if (!ctx.empty()) {
	    Base* reqb = ctx.back();
	    Elem* reqe = reqb == NULL ? NULL : reqb->GetObj(reqe);
	    Logger()->Write(MLogRec::EInfo, NULL, "If: [%s], [%i][%x: %s] - [%x: %s]", it->first.first.c_str(),
		    ctx.size(), reqe, reqe == NULL ? "NULL" : reqe->GetUri(NULL, ETrue).c_str(), prov, prov->GetUri(NULL, ETrue).c_str());
	}
	else {
	    Logger()->Write(MLogRec::EInfo, NULL, "If: [%s], [none] - [%x: %s]", it->first.first.c_str(),
		    prov, prov->GetUri(NULL, ETrue).c_str());
	}
    }
    Logger()->Write(MLogRec::EInfo, this, "Ifaces requests: END");
}

const string Elem::EType(TBool aShort) const
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

MElem* Elem::GetComp(const string& aParent, const string& aName)
{
    MElem* res = NULL;
    pair<TNMReg::iterator, TNMReg::iterator> range = iMComps.equal_range(TNMKey(aName));
    for (TNMReg::iterator it = range.first; it != range.second && res == NULL; it++) {
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
	TBool anywhere = (*it).second.second == GUri::KTypeAnywhere;
	if (it->second.second.empty() || anywhere) {
	    MElem* root = NULL;
	    if (it->second.first == GUri::KNodeSep) {
		root = GetRoot();
	    } else if (it->second.first == GUri::KParentSep) {
		root = GetInhRoot();
	    }
	    if (!anywhere) {
		it++;
		GUri::TElem elem = *it;
		anywhere = elem.second.second == GUri::KTypeAnywhere;
		TBool any = elem.second.second == GUri::KTypeAny;
		if (!anywhere && !any && root->Name() != elem.second.second) {
		    root = NULL;
		}
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
    TBool res = EFalse;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    TBool anywhere = aAnywhere;
    if (elem.second.second == "..") {
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
	    Logger()->Write(MLogRec::EErr, this, "Revasing uri [%s] - path to top of root", aUri.GetUri().c_str());
	}
    } else {
	if (!anywhere && elem.second.second == GUri::KTypeAnywhere) {
	    elem = GUri::Elem(GUri::KParentSep, GUri::KTypeAny, GUri::KTypeAny);
	    anywhere = ETrue;
	}
	TBool isnative = elem.second.first == GUri::KSepNone;
	if (isnative) {
	    // Native node, not embedded to hier, to get from environment, ref uc_045
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		Elem* node = Provider()->GetNode(elem.second.second);
		if (node == aBase) {
		    aRes.AppendTail(aUri, ++uripos);
		    return ETrue;
		}
		if (node != NULL) {
		    res = node->RebaseUri(aUri, uripos, EFalse, aBase, aRes);
		}
	    }
	} else {
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    if (it != itend) {
		uripos++;
		if (uripos != aUri.Elems().end()) {
		    for (; it != itend && !res; it++) {
			MElem* node = *it;
			if (node == aBase) {
			    aRes.AppendTail(aUri, uripos);
			    return ETrue;
			}
			res = node->RebaseUri(aUri, uripos, EFalse, aBase, aRes);
		    }
		}
	    }
	}
	if (!res && anywhere && uripos != aUri.Elems().end()) {
	    // Try to search in all nodes
	    elem = GUri::Elem(GUri::KNodeSep, GUri::KTypeAny, GUri::KTypeAny);
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    for (; it != itend && !res; it++) {
		MElem* node = *it;
		if (node == aBase) {
		    aRes.AppendTail(aUri, uripos);
		    return ETrue;
		}
		res = node->RebaseUri(aUri, uripos, anywhere, aBase, aRes);
	    }
	}
    }
    return res;
}

MElem* Elem::GetNode(const GUri& aUri) 
{ 
    if (aUri.IsErr()) return NULL;
    MElem* res = NULL;
    GUri::const_elem_iter it = aUri.Elems().begin(); 
    if (it != aUri.Elems().end()) {
	TBool anywhere = (*it).second.second == GUri::KTypeAnywhere; 
	if (it->second.second.empty() || anywhere) {
	    MElem* root = NULL;
	    if (it->second.first == GUri::KNodeSep) {
		root = GetRoot();
	    }
	    else if (it->second.first == GUri::KParentSep) {
		root = GetInhRoot();
	    }
	    if (!anywhere) {
		it++;
		GUri::TElem elem = *it;
		anywhere = elem.second.second == GUri::KTypeAnywhere; 
		TBool any = elem.second.second == GUri::KTypeAny;
		if (!anywhere && !any && root->Name() != elem.second.second) {
		    root = NULL;
		}
	    }
	    if (root != NULL && ++it != aUri.Elems().end()) {
		res = root->GetNode(aUri, it, anywhere);
	    }
	    else {
		res = root;
	    }
	}
	else {
	    res = GetNode(aUri, it); 
	}
    }
    else {
	res = this;
    }
    return res;
}

Elem* Elem::GetNodeS(const char* aUri)
{
    MElem* mel = GetNode(aUri);
    Elem* res = (mel == NULL) ? NULL: mel->GetObj(res);
    return res;
}

Elem::Iterator Elem::NodesLoc_Begin(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId));
}

Elem::Iterator Elem::NodesLoc_End(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId, ETrue));
}

MElem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere) 
{
    MElem* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    TBool anywhere = aAnywhere;
    if (elem.second.second == "..") {
	if (iMan != NULL) {
	    if (++uripos != aUri.Elems().end()) {
		res = iMan->GetNode(aUri, uripos);
	    }
	    else {
		res = iMan;
	    }
	}
	else {
	    __ASSERT(EFalse);
	    Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - path to top of root", aUri.GetUri().c_str());
	}
    }
    else {
	if (!anywhere && elem.second.second == GUri::KTypeAnywhere) {
	    elem = GUri::Elem(GUri::KParentSep, GUri::KTypeAny, GUri::KTypeAny);
	    anywhere = ETrue;
	}
	TBool isnative = elem.second.first == GUri::KSepNone;
	if (isnative) {
	    // Native node, not embedded to hier, to get from environment, ref uc_045
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		Elem* node = Provider()->GetNode(elem.second.second);
		if (node != NULL) {
		    res = node->GetNode(aUri, uripos);
		}
	    }
	    else {
		res = Provider()->GetNode(elem.second.second);
	    }
	}
	else {
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    if (it != itend) {
		uripos++;
		if (uripos != aUri.Elems().end()) {
		    for (; it != itend; it++) {
			MElem* node = *it;
			MElem* res1 = node->GetNode(aUri, uripos);
			if (res1 != NULL) {
			    if (res == NULL) {
				res = res1;
			    }
			    else {
				res = NULL;
				Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - multiple choice", aUri.GetUri().c_str());
				break;
			    }
			}
		    }
		}
		else {
		    res = *it;
		    if (++it != itend) {
			MElem* dup = *it;
			Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - multiple choice", aUri.GetUri().c_str());
			res = NULL;
		    }
		}
	    }
	}
	if (res == NULL && anywhere && uripos != aUri.Elems().end()) {
	    // Try to search in all nodes
	    elem = GUri::Elem(GUri::KNodeSep, GUri::KTypeAny, GUri::KTypeAny);
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    for (; it != itend; it++) {
		MElem* node = *it;
		MElem* res1 = node->GetNode(aUri, uripos, anywhere);
		if (res1 != NULL) {
		    if (res == NULL) {
			res = res1;
			}
			else {
			res = NULL;
			Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - multiple choice", aUri.GetUri().c_str());
			break;
		    }
		}
	    }
	}
    }
    return res;
}

vector<MElem*>& Elem::Comps()
{
    return iComps;
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

void Elem::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode)
{
    ChromoNode& root = iMut->Root();
    DoMutation(root, aRunTimeOnly, aCheckSafety, aTrialMode);
    // Clear mutation
    for (ChromoNode::Iterator mit = root.Begin(); mit != root.End();)
    {
	ChromoNode node = *mit;
	mit++; // It is required because removing node by iterator breakes iterator itself
	root.RmChild(node);
    }
}

void Elem::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode)
{
    DoMutation(aMutsRoot, aRunTimeOnly, aCheckSafety, aTrialMode);
}

// TODO [YB] Is megre the valid idea at all ?
// TODO [YB] Semms not used anymore, to remove
TBool Elem::MergeMutation(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    ChromoNode& chrroot = iChromo->Root();
    TNodeType rnotype = aSpec.Type();
    // TODO Disabling merge because it's not working for non-local movement
    // ref MergeMutMove. To consider rework merge algorithm
    if (EFalse) {
	res = MergeMutMove(aSpec);
    }
    else {
	chrroot.AddChild(aSpec);
	res = ETrue;
    }
    return res;
}

// TODO This merge algorithm doesn't work for movement non local nodes
// for instance movement components in owned node.
// In this case more complicated algorithm is required to merge "add" mutations
TBool Elem::MergeMutMove(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    GUri src(aSpec.Attr(ENa_Id));
    string srcname = src.Elems().at(0).second.second;
    GUri dest(aSpec.Attr(ENa_MutNode));
    string destname = dest.Elems().size() > 0 ? dest.Elems().at(0).second.second : string();
    ChromoNode& croot = iChromo->Root();
    // Find the dest and src
    ChromoNode::Iterator nidest = croot.Find(ENt_Node, destname);
    ChromoNode::Iterator nisrc = croot.Find(ENt_Node, srcname);
    if (nidest == croot.End()) {
	// Move node to end
	ChromoNode nsrc = *nisrc;
	nsrc.MoveToEnd();
	res = ETrue;
    }
    else if (nidest != croot.End() && nisrc != croot.End()) {
	// Move node
	ChromoNode nsrc = *nisrc;
	nsrc.MovePrevTo(nidest);
	res = ETrue;
    }
    return res;
}

void Elem::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode)
{
    const ChromoNode& mroot = aMutSpec;
    if (mroot.Begin() == mroot.End()) return;
    ChromoNode sroot = *(mroot.Root());
    TInt tord = 0;
    TInt lim = 0;
    TBool isattached = EFalse;
    if (EN_MUT_LIM) {
	tord = iEnv->ChMgr()->GetSpecMaxOrder();
	lim = iEnv->ChMgr()->GetLim();
	isattached = IsChromoAttached();
    }
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++)
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
	TInt order = rno.GetOrder();
	// Avoiding mutations above limit. Taking into account only attached chromos.
	if (EN_MUT_LIM && isattached && tord > 0 && order > tord - lim) {
	    if (!aRunTime && !aTrialMode) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	TNodeType rnotype = rno.Type();
	if (rnotype == ENt_Node) {
	    MElem* node = AddElem(rno, aRunTime, aTrialMode);
	    if (node == NULL) {
		string pname = rno.Attr(ENa_Parent);
		Logger()->Write(MLogRec::EErr, this, "Adding node with parent [%s] failed", pname.c_str());
	    }
	}
	else if (rnotype == ENt_Change) {
	    ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode);
	}
	else if (rnotype == ENt_Cont) {
	    DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode);
	}
	else if (rnotype == ENt_Move) {
	    MoveNode(rno, aRunTime, aTrialMode);
	}
	else if (rnotype == ENt_Import) {
	    ImportNode(rno, aRunTime, aTrialMode);
	}
	else if (rnotype == ENt_Rm) {
	    RmNode(rno, aRunTime, aCheckSafety, aTrialMode);
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Mutating - unknown mutation type [%d]", rnotype);
	}
	Logger()->SetContextMutId();
    }
}

TBool Elem::IsContChangeable(const string& aName) const
{
    return EFalse;
}

TBool Elem::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) 
{
    return EFalse;
}

void Elem::GetCont(string& aCont, const string& aName)
{
}

TBool Elem::GetCont(TInt aInd, string& aName, string& aCont) const
{
    return EFalse;
}

TInt Elem::GetContCount() const
{
    return 0;
}

void Elem::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode)
{
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string snode = aSpec.Attr(ENa_MutNode);
    string mattrs = aSpec.Attr(ENa_MutAttr);
    string mval = aSpec.Attr(ENa_MutVal);
    MElem* node = GetNode(snode);
    TBool mutadded = EFalse;
    if (node != NULL && IsComp(node)) {
	if (epheno || node->GetAowner() == this  || IsDirectInheritedComp(node)) {
	    if (!aCheckSafety || IsMutSafe(node)) {
		Elem* enode = ToElem(node);
		TBool res = enode->ChangeAttr(GUri::NodeAttr(mattrs), mval);
		if (!res) {
		    Logger()->Write(MLogRec::EErr, this, "Changing node [%s] - failure", snode.c_str());
		}
		else {
		    // Adding dependency to object of change
		    if (!aRunTime) {
			ChromoNode chn = iChromo->Root().AddChild(aSpec);
			mutadded = ETrue;
			AddCMDep(chn, ENa_MutNode, enode);
		    }
		}
	    }
	    else {
		Elem* enode = ToElem(node);
		Logger()->Write(MLogRec::EErr, this, "Renaming elem [%s] - unsafe, used in: [%s]", 
			snode.c_str(), enode->GetMajorDep().first.first->GetUri().c_str());
	    }
	}
	else {
	    TBool efix = iEnv->ChMgr()->EnableFixErrors();
	    if (efix) {
		// Trying to repair chromo by transforming pheno to mut
		MElem* nowner = node->GetMan();
		MElem* mnode = nowner->GetAttachingMgr();
		ChromoNode mut(aSpec);
		if (mnode == node) {
		    mut.SetAttr(ENa_MutNode, "");
		}
		else {
		    GUri nuri;
		    node->GetRUri(nuri, mnode);
		    mut.SetAttr(ENa_MutNode, nuri.GetUri(EFalse));
		}
		mnode->ChangeAttr(mut, aRunTime, aCheckSafety);
		mutadded = ETrue;
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Changing node [%s]  - attempt of phenotypic modification - disabled", snode.c_str());
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Changing node [%s] - cannot find node or node isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
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

TBool Elem::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode)
{
    TBool res = ETrue;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    TBool efix = iEnv->ChMgr()->EnableFixErrors();
    TBool erepos = iEnv->ChMgr()->EnableReposMuts();
    string snode = aSpec.Attr(ENa_MutNode);
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
	    if (!aCheckSafety || IsMutSafe(node)) {
		if (refex) {
		    // For -ref- attr the value is the ref relative to mutated node context
		    rnode = ToElem(node->GetNode(mval));
		    //mval = rnode->GetRUri(node);
		    if (rnode == NULL) {
			rnode = ToElem(node->GetNode(mval));
			Logger()->Write(MLogRec::EErr, this, aSpec,
				"Changing content of node [%s] to ref [%s] - cannot find ref", snode.c_str(), mval.c_str());
			res = EFalse;
		    }
		    else {
			TMDep dep;
			if (aCheckSafety && !IsRefSafe(rnode, ENa_Ref, node, &dep)) {
			    res = EFalse;
			    if (erepos && iMan != NULL) {
				// Forward ref found, keep chromo consistent using invriance principle, ref uc_046
				//TMDep dep = GetRefDep(rnode, ENa_Ref, node);
				//Elem* att_owner = GetAowner();
				//res = att_owner->ResolveMutUnsafety(this, dep);
				res = iMan->ResolveMutUnsafety(this, dep);
			    }
			    if (!res) {
				Logger()->Write(MLogRec::EErr, this, "Changing content of node [%s] - unsafe: rank of ref [%s] is too big", 
					snode.c_str(), mval.c_str());
			    }
			}
		    }
		}
		if (res) {
		    res = node->ChangeCont(mval, EFalse);
		    if (res) {
			if (!aRunTime) {
			    ChromoNode chn = iChromo->Root().AddChild(aSpec);
			    mutadded = ETrue;
			    AddCMDep(chn, ENa_MutNode, node);
			    if (refex) {
				AddCMDep(chn, ENa_Ref, rnode);
			    }
			    // Optimize the chromo for the mutation
			    if (aTrialMode) {
				CompactChromo(chn);
			    }
			}
		    }
		    else {
			Logger()->Write(MLogRec::EErr, this, aSpec, "Changing [%s] - failure", snode.c_str());
			node->ChangeCont(mval, EFalse);
		    }
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Changing content of [%s] - unsafe, used in: [%s]", snode.c_str(), 
			node->GetMajorDep().first.first->GetUri().c_str());
	    }
	}
	else  {
	    if (efix) {
		// Trying to repair chromo by transforming pheno to mut
		MElem* mnode = node->GetAttachingMgr();
		ChromoNode mut(aSpec);
		TBool fixok = ETrue;
		if (refex) {
		    rnode = ToElem(node->GetNode(mval));
		    if (rnode != NULL) {
			if (!mnode->IsRefSafe(rnode, ENa_Ref, node)) {
			    // Trying to move mutnode to the last position
			    TMDep dep = rnode->GetMajorDep(ENt_Cont, MChromo::EDl_Critical);
			    //TBool sres = ShiftComp(mnode);
			    TBool sres = ShiftCompOverDep(mnode, dep);
			    if (!sres) {
				Logger()->Write(MLogRec::EWarn, this, 
					"Changing content of node [%s], attempting to repair - cannot repair - rank of ref [%s] is too big", 
					snode.c_str(), mval.c_str());
				fixok = EFalse;
			    }
			}
		    }
		    else {
			Logger()->Write(MLogRec::EWarn, this, "Changing content of node [%s], attempting to repair - cannot find ref [%s]", 
				snode.c_str(), mval.c_str());
			fixok = EFalse;
		    }
		}
		if (fixok) {
		    if (mnode == node) {
			mut.SetAttr(ENa_MutNode, "");
		    }
		    else {
			GUri nuri;
			node->GetRUri(nuri, mnode);
			mut.SetAttr(ENa_MutNode, nuri.GetUri(EFalse));
		    }
		    mnode->DoMutChangeCont(mut, aRunTime, aCheckSafety);
		    mutadded = ETrue;
		}
	    }
	    else {
		IsInheritedComp(node);
		Logger()->Write(MLogRec::EErr, this, "Changing content of node [%s]  - attempt of phenotypic modification - disabled", 
			snode.c_str());
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Changing node [%s] - cannot find node or node isn't comp", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	ChromoNode mut = iChromo->Root().AddChild(aSpec);
	TInt mutid = mut.LineId();
    }
    return res;
}

MElem* Elem::AddElem(const ChromoNode& aNode, TBool aRunTime, TBool aTrialMode)
{
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
    //if (EN_PERF_METR && iName == "Held1" && sname == "1211685360"/* && sparent == "/(Elem:)Root/(Elem:)Modules/(Elem:)VisComps/(Extender:)DrawingElemExt"*/) {
    //if (EN_PERF_METR && EN_PERF_DBG1 && iName == "DrawingElem" && sname == "Logspec") {
    if (EN_PERF_METR && EN_PERF_DBG1 && iName == "Velocity_L" && sname == "448726636") {
	Logger()->Write(MLogRec::EInfo, this, "Point_1");
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
    if (ptrace) Logger()->Write(MLogRec::EInfo, this, "Adding node [%s], t1", sname.c_str());
    long t1 = GetClock();

    __ASSERT(!sname.empty());
    /*
    if (IsLogeventCreOn()) {
	Logger()->Write(MLogRec::EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
    }
    */
    //Logger()->Write(MLogRec::EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
    MElem* elem = NULL;
    MElem* mnode = snode.empty() ? this: GetNode(snode); 
    if (mnode != NULL) {
	Elem* node = ToElem(mnode);
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
		    parent = ToElem(impmgr->OnUriNotResolved(this, pruri));
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
		    if (ecsaf && IsForwardRef(parent)) {
			res = EFalse;
			if (erepos && iMan != NULL) {
			    // Forward ref found, keep chromo consistent using invriance principle, ref uc_046
			    TMDep dep = parent->GetMajorDep(ENt_Change, MChromo::EDl_Critical);
			    res = iMan->ResolveMutUnsafety(this, dep);
			}
			if (!res) {
			    Logger()->Write(MLogRec::EErr, this, 
				    "Creating [%s] - cannot create - rank of parent [%s] is too big", sname.c_str(), sparent.c_str());
			}
		    }
		    // Create heir from the parent
		    if (ptrace) { long t1_2 = GetClockElapsed(t1); Logger()->Write(MLogRec::EInfo, this, "Adding node, t1-t2: %d", t1_2);}
		    long t3; if (ptrace) t3 = GetClock();
		    elem = parent->CreateHeir(sname, node);
		    if (ptrace) { long t3_4 = GetClockElapsed(t3); Logger()->Write(MLogRec::EInfo, this, "Adding node, t3-t4: %d", t3_4);}
		    // TODO [YB] Seems to be just temporal solution. To consider using context instead.
		    // Make heir based on the parent: re-parent the heir (currently it's of grandparent's parent) and clean the chromo
		    ChromoNode hroot = elem->Chromos().Root();
		    hroot.SetAttr(ENa_Parent, sparent);
		    if (!snode.empty()) {
			hroot.SetAttr(ENa_MutNode, snode);
		    }
		    // Remove external parent from system
		    // [YB] DON'T remove parent, otherwise the inheritance chain will be broken
		    if (ext_parent) {
			// delete parent;
		    }
		    if (elem != NULL) {
			res = node->AppendComp(elem);
			if (res) {
			    if (!aRunTime) {
				ChromoNode chn = iChromo->Root();
				if (node == this) {
				    // True mutation
				    chn = iChromo->Root().AddChild(elem->Chromos().Root(), EFalse);
				}
				else {
				    // Fenothypic modification
				    chn = iChromo->Root().AddChild(elem->Chromos().Root(), EFalse);
				    AddCMDep(chn, ENa_MutNode, node);
				}
				AddCMDep(chn, ENa_Id, elem);
				AddCMDep(chn, ENa_Parent, parent);
				mutadded = ETrue;
			    }
			    // Mutate object 
			    long t5; if (ptrace) t5 = GetClock();
			    elem->SetMutation(aNode);
			    elem->Mutate(EFalse, ecsaf, aTrialMode);
			    if (ptrace) { long t5_6 = GetClockElapsed(t3); Logger()->Write(MLogRec::EInfo, this, "Adding node, t5-t6: %d", t5_6);}
			    //Logger()->Write(MLogRec::EInfo, this, "Added node [%s:%s]", elem->EType().c_str(), elem->Name().c_str());
			    /*
			    if (IsLogeventCreOn()) {
				Logger()->Write(MLogRec::EInfo, this, "Added node [%s:%s]", elem->EType().c_str(), elem->Name().c_str());
			    }
			    */
			}
			else {
			    Logger()->Write(MLogRec::EErr, this, "Adding node [%s:%s] failed", elem->EType().c_str(), elem->Name().c_str());
			    delete elem;
			    elem = NULL;
			}
		    }
		    else {
			Logger()->Write(MLogRec::EErr, this, "Creating elem [%s] - failed", sname.c_str());
		    }
		}
		else  {
		    TBool efix = iEnv->ChMgr()->EnableFixErrors();
		    if (efix) {
			// Trying to repair chromo by transforming pheno to mut
			MElem* mnode = node->GetAttachingMgr();
			ChromoNode mut(aNode);
			TBool fixok = ETrue;
			if (!mnode->IsRefSafe(parent, ENa_Parent)) {
			    // Trying to move mutnode to the last position
			    TBool sres = ShiftComp(mnode);
			    if (!sres) {
				Logger()->Write(MLogRec::EWarn, this, 
					"Creating [%s], attempting to repair - cannot repair - rank of parent [%s] is too big", 
					sname.c_str(), sparent.c_str());
				fixok = EFalse;
			    }
			}
			if (fixok) {
			    if (mnode == node) {
				mut.SetAttr(ENa_MutNode, "");
			    }
			    else {
				GUri nuri;
				node->GetRUri(nuri, mnode);
				mut.SetAttr(ENa_MutNode, nuri.GetUri(EFalse));
			    }
			    elem = mnode->AddElem(mut, aRunTime);
			    mutadded = ETrue;
			}
		    }
		    else {
			TBool isi = IsInheritedComp(node);
			Logger()->Write(MLogRec::EErr, this, "Creating elem [%s] in node [%s] - attempt of phenotypic modification - disabled", 
				sname.c_str(), snode.c_str());
		    }
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    }
	}
    }
    else  {
	Logger()->Write(MLogRec::EErr, this, "Creating elem [%s] in node [%s] - cannot find node", sname.c_str(), snode.c_str());
    }
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aNode);
    }
    if (EN_PERF_METR && elem != NULL) {
	/* Profiling using wall-clock. It doesn't shows time interval within given thread, other threads are affecting
	gettimeofday(&tp, NULL);
	long int t2 = tp.tv_sec * 1000000 + tp.tv_usec;
	int nodes = elem->GetCapacity();
	long tspan = t2-t1;
	long tspan_n = tspan/(nodes + 1);
	Logger()->Write(MLogRec::EInfo, this, "Created comp [%s] in node [%s];%d;%d", sname.c_str(), snode.c_str(), tspan, tspan_n);
	*/
	timespec te;
	//long tspan = GetClockElapsed(t1, te);
	long tspan = GetClockElapsed(t1);
	int nodes = elem->GetCapacity();
	long tspan_n = tspan/(nodes + 1);
	Logger()->Write(MLogRec::EInfo, this, "Created comp [%s] in node [%s];%d;%d;%d", sname.c_str(), snode.c_str(), nodes, tspan, tspan_n);
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
MElem* Elem::CreateHeir(const string& aName, MElem* aMan)
{
    MElem* heir = NULL;
    //Logger()->Write(MLogRec::EInfo, this, "CreateHeir, p1 ");
    if (Provider()->IsProvided(this)) {
	heir = Provider()->CreateNode(Name(), aName, ToElem(aMan), iEnv);
    }
    else {
	__ASSERT(iParent != NULL);
	if (Provider()->IsProvided(ToElem(iParent))) {
	    // Parent is Agent - native element. Create via provider
	    heir = Provider()->CreateNode(EType(), aName, ToElem(iMan), iEnv);
	}
	else {
	    heir = iParent->CreateHeir(aName, iMan);
	}
	if (EN_PERF_TRACE) Logger()->Write(MLogRec::EInfo, this, "CreateHeir, p2 ");
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	ChromoNode root = iChromo->Root();
	heir->SetMutation(root);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	heir->Mutate(ETrue, ETrue, EFalse);
	if (EN_PERF_TRACE) Logger()->Write(MLogRec::EInfo, this, "CreateHeir, p3 ");
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetParent(Name());
	// Relocate heir to hier from which the request of creating heir came
	heir->SetMan(NULL);
	heir->SetMan(aMan);
	// Re-adopte the child
	iParent->RemoveChild(heir);
	AppendChild(heir);
	if (EN_PERF_TRACE) Logger()->Write(MLogRec::EInfo, this, "CreateHeir, p4 ");
    }
    return heir;
}

TBool Elem::AppendChild(MElem* aChild)
{
    TBool res = RegisterChild(aChild);
    if (res)
    {
	aChild->SetParent(this);
    }
    return res;
}

void Elem::RemoveChild(MElem* aChild)
{
    UnregisterChild(aChild);
    aChild->SetParent(NULL);
}

TBool Elem::AppendComp(MElem* aComp)
{
    TBool res = RegisterComp(aComp);
    if (res)
    {
	iComps.push_back(aComp);
	if (iMan != NULL) {
	    iMan->OnCompAdding(*aComp);
	}
	if (iObserver != NULL) {
	    iObserver->OnCompAdding(*aComp);
	}
    }
    return res;
}

TBool Elem::RegisterComp(MElem* aComp)
{
    TBool res = ETrue;
    MElem* node = GetComp(aComp->EType(), aComp->Name());
    if (node == NULL || node->IsRemoved()) {
	iMComps.insert(TNMVal(TNMKey(aComp->Name()), aComp));
    } else {
	Logger()->Write(MLogRec::EErr, this, "Registering component [%s] - already exists", aComp->Name().c_str());
	res = EFalse;
    }
    return res;
}

TBool Elem::RegisterChild(MElem* aChild)
{
    TBool res = ETrue;
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
    assert (GetComp(aComp->EType(), name) != NULL); 
    // Removing old name related records in register
    TBool found = EFalse;
    pair<TNMReg::iterator, TNMReg::iterator> range = iMComps.equal_range(TNMKey(name));
    for (TNMReg::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aComp) {
	    iMComps.erase(it);
	    found = ETrue;
	}
    }
    //__ASSERT(found); /* To avoid panic when deleting comp that hasn't been registered yet */
    res = ETrue;
    return res;
}

TBool Elem::MoveComp(MElem* aComp, MElem* aDest)
{
    TBool res = EFalse;
    vector<MElem*>::iterator it;
    for (it = iComps.begin(); it != iComps.end() && *it != aComp; it++);
    __ASSERT(it != iComps.end());
    iComps.erase(it);
    if (aDest != NULL) {
	for (it = iComps.begin(); it != iComps.end() && *it != aDest; it++);
	iComps.insert(it, aComp);
    }
    else {
	iComps.push_back(aComp);
    }
    res = ETrue;
    return res;
}

TBool Elem::MoveComp(MElem* aComp, const ChromoNode& aDest)
{
    TBool res = EFalse;
    vector<MElem*>::iterator it;
    for (it = iComps.begin(); it != iComps.end() && *it != aComp; it++);
    __ASSERT(it != iComps.end());
    if (aDest.Handle() != NULL) {
	// Check first if dest exists 
	vector<MElem*>::iterator dit;
	for (dit = iComps.begin(); dit != iComps.end(); dit++) {
	    Elem* ee = ToElem(*dit);
	    if (ee->Chromos().Root().Handle() == aDest.Handle()) break;
	}
	__ASSERT(dit != iComps.end());
    }
    iComps.erase(it);
    if (aDest.Handle() != NULL) {
	for (it = iComps.begin(); it != iComps.end(); it++) {
	    Elem* ee = ToElem(*it);
	    if (ee->Chromos().Root().Handle() == aDest.Handle()) break;
	}
	__ASSERT(it != iComps.end());
	iComps.insert(it, aComp);
    }
    else {
	iComps.push_back(aComp);
    }
    res = ETrue;
    return res;
}

TBool Elem::ShiftCompOverDep(MElem* aComp, const TMDep& aDep)
{
    TBool res = ETrue;
    MElem* owner = aComp->GetAowner();
    MElem* comp = owner->GetCompOwning(aComp);
    ChromoNode mch = owner->Chromos().Root();
    ChromoNode cch = aComp->Chromos().Root();
    ChromoNode::Iterator depit(mch.Mdl(), aDep.first.second);
    MElem* depcomp = owner->GetCompOwning(aDep.first.first);
    if (aDep.first.first == owner) { }
    else if (depcomp != NULL) {
	depit = ChromoNode::Iterator(depcomp->Chromos().Root());
    }
    else {
	res = EFalse;
    }
    if (res) {
	ChromoNode depmut = (*depit);
	ChromoNode::Iterator nextit = depmut.FindNextSibling(ENt_Node);
	cch.MoveNextTo(depit);
	ChromoNode nextmut = *nextit;
	res = owner->MoveComp(comp, nextmut);
    }
    return res;
}

// TODO [YB] This method seems doesn't make sense, consider to remove
TBool Elem::ShiftComp(MElem* aComp, MElem* aDest)
{
    TBool res = EFalse;
    ChromoNode cch = aComp->Chromos().Root();
    if (aDest != NULL) {
	ChromoNode dch = aComp->Chromos().Root();
	if (dch.Root() == cch.Root()) {
	    cch.MovePrevTo(dch);
	    res = ETrue;
	}
    }
    else {
	cch.MoveToEnd();
	res = ETrue;
    }
    if (res) {
	res = MoveComp(aComp, aDest);
    }
    return res;
}

MElem* Elem::GetNode(const string& aUri)
{
    MElem* res = NULL;
    GUri uri(aUri);
    if (!uri.IsErr()) {
	res = GetNode(uri);
    }
    else  {
	Logger()->Write(MLogRec::EErr, this, "Incorrect URI [%s]", aUri.c_str());
    }
    return res;
}

void Elem::OnCompDeleting(MElem& aComp, TBool aSoft)
{
    // Translate to hier
    if (iMan != NULL) {
	iMan->OnCompDeleting(aComp, aSoft);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(aComp, aSoft);
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
	    // Then remove from comps
	    for (vector<MElem*>::iterator oit = iComps.begin(); oit != iComps.end(); oit++) {
		if (*oit == &aComp) {
		    iComps.erase(oit); break;
		}
	    }
	}
    }
}

// TODO [YB] To avoid propagation thru the whole model. It is temporarily remaining because of
// missing proper mechanism of the whole model observer.
void Elem::OnCompAdding(MElem& aComp)
{
    // Propagate the event
    if (iMan != NULL) {
	iMan->OnCompAdding(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnCompAdding(aComp);
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
TBool Elem::OnCompChanged(MElem& aComp)
{
    MElem* agents = GetComp("Elem", "Agents");
    TBool res = EFalse;
    if (agents != NULL) {
	for (vector<MElem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && !res; it++) {
	    MACompsObserver* iagent = (*it)->GetObj(iagent);
	    if (iagent != NULL) {
		res = iagent->HandleCompChanged(*this, aComp);
	    }
	}
    }
    // Propagate notification to upper level
    if (res && iMan != NULL) {
	res = iMan->OnCompChanged(aComp);
    }
    if (res && iObserver != NULL) {
	iObserver->OnCompChanged(aComp);
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
    if (iMan != NULL) {
	iMan->OnCompRenamed(aComp, aOldName);
    }
    if (iObserver != NULL) {
	iObserver->OnCompRenamed(aComp, aOldName);
    }
    return res;
}

TBool Elem::OnContentChanged(MElem& aComp)
{
    TBool res = EFalse, res1 = EFalse;
    if (iMan != NULL) {
	res = iMan->OnContentChanged(aComp);
    }
    if (iObserver != NULL) {
	res1 = iObserver->OnContentChanged(aComp);
    }
    return res && res1;
}

void Elem::DoOnCompChanged(Elem& aComp)
{
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

// Gets acomp that attaches the given node or node itself if this is attaching it
// !! Returns only 1st level acomp or this
MElem* Elem::GetAcompAttaching(MElem* aElem)
{
    MElem* res = NULL;
    MElem* node = aElem;
    MElem* aowner = node->GetAowner();
    while (aowner != NULL && aowner != this) {
	node = aowner;	
	aowner = node->GetAowner();
    }
    if (aowner != NULL) {
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

MElem* Elem::GetCommonAowner(MElem* aElem)
{
    MElem* res = NULL;
    MElem* owner = this;
    while (owner != NULL && res == NULL) 
    {
	if (owner->IsAownerOf(aElem) || owner == aElem) {
	    res = owner;
	}
	owner = owner->GetAowner();
    }
    return res;
}

// TODO [YB] After the change in AddElem to attach chromo before mutating the case of deattached non-trivial 
// chromo is not possible. This simplifies calculating rank: actually rank is calculated as chromo node rank
// Run-time rank doesn't make sense now. Probably the node in dep record is also not neede.
// So to consider: 1. removing run-time rank calculation, 2. removing node from dep record
void Elem::GetRank(Rank& aRank, const ChromoNode& aMut) const
{
    const MElem* att = GetAttachingMgr();
    if (att == this) {
	aMut.GetRank(aRank);
    }
    else {
	att->Chromos().Root().GetRank(aRank);
    }
}

void Elem::GetLRank(Rank& aRank, TBool aCur) const
{
    ChromoNode end = *(iChromo->Root().Rbegin());
    if (aCur) {
	GetRank(aRank, end);
    }
    else {
	GetRank(aRank, iChromo->Root());
    }
}


TInt Elem::GetLocalRank() const
{
    TInt res = -1;
    if (iMan != NULL) {
	res = 0;
	for (vector<MElem*>::const_iterator it = iMan->Comps().begin(); it != iMan->Comps().end() && *it != this; it++, res++);
    }
    return res;
}

void Elem::GetRUri(GUri& aUri, MElem* aTop)
{
    MElem* cowner = GetCommonOwner(aTop);
    GetUri(aUri, cowner);
    MElem* owner = aTop;
    while (owner != cowner) {
	aUri.PrependElem("", GUri::KUpperLevel);
	owner = owner->GetMan();
    }
}

string Elem::GetRUri(MElem* aTop)
{
    GUri uri;
    GetRUri(uri, aTop);
    return uri.GetUri();
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
    return uri.GetUri(aShort);
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

TBool Elem::IsRefSafe(MElem* aRef, TNodeAttr aReftype, MElem* aObj, TMDep* aDep)
{
    TBool res = ETrue;
    Rank rank;
    GetRank(rank, iChromo->Root());
    Rank deprank;
    TMDep dep = GetRefDep(aRef, aReftype, aObj);
    MElem* depnode = dep.first.first;
    if (depnode != NULL) {
	ChromoNode depmut = iChromo->CreateNode(dep.first.second);
	depnode->GetRank(deprank, depmut);
    }
    if (deprank > rank && !deprank.IsRankOf(rank)) {
	res = EFalse;
	if (aDep != NULL) {
	    *aDep = dep;
	}
    }
    return res;
}

TBool Elem::IsMutSafe(MElem* aRef)
{
    TBool safemut = ETrue;
    TMDep dep = aRef->GetMajorDep();
    if (dep.first.first != NULL) {
	Rank rank;
	GetRank(rank, iChromo->Root());
	Rank deprank;
	ChromoNode depnode = iChromo->CreateNode(dep.first.second);
	// Using combined calc of rank because of possibility that mut can be deattached
	dep.first.first->GetRank(deprank, depnode);
	if (deprank > rank && !deprank.IsRankOf(rank)) {
	    safemut = EFalse;
	}
    }
    return safemut;
}

TBool Elem::IsForwardRef(MElem* aRef)
{
    TBool res = EFalse;
    Rank crank;
    GetLRank(crank);
    Rank rrank;
    aRef->GetLRank(rrank);
    if (rrank > crank && !rrank.IsRankOf(crank)) {
	res = ETrue;
    }
    return res;
}

TBool Elem::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode)
{
    TBool res = EFalse;
    TBool epheno = iEnv->ChMgr()->EnablePhenoModif();
    string snode = aSpec.Attr(ENa_MutNode);
    MElem* node = GetNode(snode);
    TBool mutadded = EFalse;
    if (node != NULL && IsComp(node)) {
	if (epheno || node->GetMan() == this || IsCompOfInheritedComp(node)) {
	    // Check dependent mutations
	    if (!aCheckSafety || IsMutSafe(node)) {
		res = ETrue;
		// Just mark node as removed but not remove actually, ref ds_mut_rm_appr
		// Refuse removing if here are heirs, needs to unparent them first, ref ds_mut_rm_deps
		if (!node->HasInherDeps(node)) {
		    // Refuse removing if there are other critical deps (e.g. -ref-)
		    // TODO [YB] To consider -refs- to be automatically optimized out (ref. ds_mut_sqeezing_rm)
		    TMDep dep = node->GetMajorDep(ENt_Rm, MChromo::EDl_Critical);
		    if (dep.first.first == NULL || dep.second == ENa_Id) {
			node->SetRemoved();
			if (!aRunTime) {
			    // Adding dependency to object of change
			    ChromoNode chn = iChromo->Root().AddChild(aSpec);
			    AddCMDep(chn, ENa_MutNode, node);
			    mutadded = ETrue;
			    // Optimize the chromo for the mutation
			    if (aTrialMode) {
				CompactChromo(chn);
			    }
			}
		    } else {
			Logger()->Write(MLogRec::EErr, this,
				"Removing node [%s], refused, there is ref [%s] to the node, release first",
				snode.c_str(), dep.first.first->GetUri(0, ETrue).c_str());
		    }
		} else {
		    Logger()->Write(MLogRec::EErr, this,
			    "Removing node [%s], refused, there are heirs of the node, unparent first", snode.c_str());
		    node->HasInherDeps(node);
		}
		if (IsLogeventCreOn()) {
		    Logger()->Write(MLogRec::EInfo, this, "Removed elem [%s]", snode.c_str());
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Removing elem [%s] - unsafe, used in: [%s]", snode.c_str(), 
			node->GetMajorDep().first.first->GetUri().c_str());
	    }
	}
	else  {
	    TBool efix = iEnv->ChMgr()->EnableFixErrors();
	    if (efix) {
		// Trying to repair chromo by transforming pheno to mut
		MElem* mnode = node->GetMan()->GetAttachingMgr();
		ChromoNode mut(aSpec);
		if (mnode == node) {
		    mut.SetAttr(ENa_MutNode, "");
		}
		else {
		    GUri nuri;
		    node->GetRUri(nuri, mnode);
		    mut.SetAttr(ENa_MutNode, nuri.GetUri(EFalse));
		}
		mnode->RmNode(mut, aRunTime, ETrue);
		mutadded = ETrue;
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Removing node [%s] - attempt of phenotypic modification - disabled", snode.c_str());
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Removing node [%s] - not found or isn't component", snode.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
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
    TBool mutadded = EFalse;
    if (dnode != NULL) {
	GUri srcsuri(srcs);
	if (srcsuri.Scheme().empty()) {
	    // Local node
	    MElem* snode = GetNode(srcs);
	    MElem* sowner = snode->GetMan();
	    if (snode != NULL) {
		if (sowner != NULL && dnode == sowner) {
		    // Attempt of moving node to the current contect 
		    Logger()->Write(MLogRec::EErr, this, "Moving node [%s] to it's current owner - disabled", snode->GetUri().c_str());
		}
		else if (dnode == snode) {
		    // Attempt of moving node to itself
		    Logger()->Write(MLogRec::EErr, this, "Moving node [%s] into itself - disabled", snode->GetUri().c_str());
		}
		else if (!((dnode == this || IsComp(dnode)) && (snode == this || IsComp(snode)))) {
		    // Incremantal mutation approach rule - mut refs shall be comps
		    Logger()->Write(MLogRec::EErr, this, "Moving node - refs are not comps");
		}
		else {
		    // Inter-nodes movement
		    // TODO [YB] To consider if this movement does make sense. It is not movement of chunk of chromo, but 
		    // run-time only.
		    if (IsMutSafe(snode)) {
			// It is rather complicated to re-create snode in new context because all snode
			// mutation are related to original snode context, so use trick, ref "ds_mv_local"
			// Create heir of source node in destination context
			MElem* heir = snode->CreateHeir(snode->Name(), dnode);
			// Re-adopt back to source parent
			snode->RemoveChild(heir);
			snode->GetParent()->AppendChild(heir);
			res = dnode->AppendComp(heir);
			if (res) {
			    // Remove source node
			    // If node has children then just mark it as removed but not remove actually
			    if (snode->HasInherDeps(snode)) {
				//TODO [YB] Consider to handle correctly
				snode->SetRemoved();
			    }
			    else {
				delete snode;
			    }
			}
			else {
			    Logger()->Write(MLogRec::EErr, this, "Moving node [%s] failed", snode->GetUri().c_str());
			    delete heir;
			}
		    }
		    else {
			Logger()->Write(MLogRec::EErr, this, "Moving elem [%s] - unsafe, used in: [%s]", 
				snode->GetUri().c_str(), snode->GetMajorDep().first.first->GetUri().c_str());
		    }
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Moving node [%s] - not found", srcs.c_str());
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
		Logger()->Write(MLogRec::EErr, this, "Moving [%s] to [%s]: source node not found", srcs.c_str(), dests.c_str());
	    }
	}
	if (!aRunTime && res) {
	    // Adding dependency to object of change
	    ChromoNode chn = iChromo->Root().AddChild(aSpec);
	    if (dnode != this) {
		AddCMDep(chn, ENa_MutNode, dnode);
	    }
	    mutadded = ETrue;
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Moving to node [%s] - not found", dests.c_str());
    }
    // Append mutation to chromo anytype, ref uc_043
    if (!aRunTime && !mutadded && !aTrialMode) {
	iChromo->Root().AddChild(aSpec);
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
    return node != NULL;
}

// TODO [YB] To implement with usage of URI but not just string
// TODO [YB] The problem is more serious: we need to check the full type
// (all parents chain) to detect inheritance.
TBool Elem::IsHeirOf(const string& aParent) const
{
    int pos = EType(EFalse).find(aParent);
    return pos != string::npos;
}

// Checks if elements chromo is attached. Ref UC_019 for details
// Using chromo based calculation, ref ds_mut_attach_calc
TBool Elem::IsChromoAttached() const
{
    return iChromo->Root().Root() == GetRoot()->Chromos().Root();
}

TBool Elem::IsMutAttached(const ChromoNode& aMut) const
{
    return aMut.Root() == GetRoot()->Chromos().Root();
}

// Getting owner that is attached. It can differ from owner that attaches the node
// For owner attaching the node, use
MElem* Elem::GetAttachingMgr() 
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

const MElem* Elem::GetAttachingMgr() const
{
    const MElem* res = NULL;
    const MElem* cand = this;
    while (res == NULL && cand != NULL) {
	if (cand->IsChromoAttached()) {
	    res = cand;
	} else {
	    cand = cand->GetMan();
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
// owner, for that ref to GetAttachingMgr
MElem* Elem::GetAowner()
{
    MElem* res = NULL;
    ChromoNode::Iterator prnt = iChromo->Root().Parent();
    if (prnt != iChromo->Root().End()) {
	MElem* cand = GetMan();
	while (res == NULL && cand != NULL) {
	    if (cand->Chromos().Root() == *prnt) {
		res = cand;
	    }
	    else {
		cand = cand->GetMan();
	    }
	}
    }
    return res;
}

const MElem* Elem::GetAowner() const
{
    const MElem* res = NULL;
    ChromoNode::Iterator prnt = iChromo->Root().Parent();
    if (prnt != iChromo->Root().End()) {
	const MElem* cand = GetMan();
	while (res == NULL && cand != NULL) {
	    if (cand->Chromos().Root() == *prnt) {
		res = cand;
	    }
	    else {
		cand = cand->GetMan();
	    }
	}
    }
    return res;
}

TBool Elem::IsPhenoModif() const
{
    TBool res = !IsChromoAttached();
    if (res) {
	TMDep dep;
	GetDep(dep, ENa_Id, ETrue);
	MElem* depnode = dep.first.first;
	res = depnode != NULL && depnode->IsChromoAttached();
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

void Elem::GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly, TBool aAnyType) const 
{
    if (iMDeps.begin() != iMDeps.end()) {
	for (TMDeps::const_iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	    TMDep dep = *it;
	    if (aAnyType || dep.second == aAttr) {
		aDep = dep;
		break;
	    }
	}
    }
    else {
	// No deps, the node is internal, move to owner
	if (!aLocalOnly && iMan != NULL) {
	    iMan->GetDep(aDep, aAttr);
	}
    }
}

TMDep Elem::GetMajorDep()
{
    // Starting from dep Id considering also deattached nodes, ref uc_038
    TMDep res(TMutRef(NULL, NULL), ENa_Unknown);
    GetDep(res, ENa_Id);
    GetMajorDep(res);
    return res;
}

void Elem::GetDepRank(const TMDep& aDep, Rank& aRank)
{
    ChromoNode mut = aDep.first.first->Chromos().CreateNode(aDep.first.second);
    aDep.first.first->GetRank(aRank, mut);
}

TBool Elem::IsDepActive(const TMDep& aDep)
{
    ChromoNode mut = aDep.first.first->Chromos().CreateNode(aDep.first.second);
    return mut.IsActive();
}

void Elem::GetMajorDep(TMDep& aDep, TBool aUp, TBool aDown)
{
    //Logger()->Write(MLogRec::EInfo, this, "Gmd");
    // Ref to theses ds_mut_unappr_rt_ths1 for rules of searching deps
    Rank rc;
    if (aDep.first.first == NULL || aDep.first.second == NULL || aDep.second == ENa_Unknown) {
	// Starting from dep Id considering also deattached nodes, ref uc_038
	GetDep(aDep, ENa_Id);
    }
    if (aDep.first.first == NULL || aDep.first.second == NULL || aDep.second == ENa_Unknown) {
	aDep = TMDep(TMutRef(this, iChromo->Root().Handle()), ENa_Id);
    }
    ChromoNode mut = iChromo->CreateNode(aDep.first.second);
    aDep.first.first->GetRank(rc, mut);
    // Direct deps
    for (TMDeps::const_iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	TMDep dep = *it;
	Rank rd;
	GetDepRank(dep, rd);
	if (IsDepActive(dep) && rd > rc) {
	    rc = rd;
	    aDep = dep;
	}
    }
    // Owner, ref ds_mut_unappr_rt_ths2
    if (iMan != NULL && aUp) {
	iMan->GetMajorDep(aDep, ETrue, EFalse);
    }
    if (aDown) {
	// Childs
	for (TNMReg::const_iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	    MElem* child = it->second;
	    child->GetMajorDep(aDep, EFalse, ETrue);
	}
	// Components
	for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end(); it++) {
	    MElem* comp = *it;
	    comp->GetMajorDep(aDep, EFalse, ETrue);
	}
    }
}

TMDep Elem::GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel)
{
    // Starting from dep Id considering also deattached nodes, ref uc_038
    TMDep res(TMutRef(NULL, NULL), ENa_Unknown);
    GetDep(res, ENa_Id);
    GetMajorDep(res, aMut, MChromo::EDp_Direct, aLevel);
    return res;
}

void Elem::GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp, TBool aDown)
{
    // Ref to theses ds_mut_unappr_rt_ths1 for rules of searching deps
    Rank rc;
    if (aDep.first.first == NULL || aDep.first.second == NULL || aDep.second == ENa_Unknown) {
	// Starting from dep Id considering also deattached nodes, ref uc_038
	GetDep(aDep, ENa_Id);
    }
    if (aDep.first.first == NULL || aDep.first.second == NULL || aDep.second == ENa_Unknown) {
	aDep = TMDep(TMutRef(this, iChromo->Root().Handle()), ENa_Id);
    }
    ChromoNode mut = iChromo->CreateNode(aDep.first.second);
    aDep.first.first->GetRank(rc, mut);
    // Registered deps
    for (TMDeps::const_iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	TMDep dep = *it;
	if (Chromo::IsDepOfLevel(Chromo::TDep(aMut, dep.second, aDpath), aLevel)) {
	    Rank rd;
	    ChromoNode dcn = iChromo->CreateNode(dep.first.second);
	    dep.first.first->GetRank(rd, dcn);
	    if (IsDepActive(dep) && rd > rc) {
		rc = rd;
		aDep = dep;
	    }
	}
    }
    // Owner, ref ds_mut_unappr_rt_ths2
    if (iMan != NULL && aUp) {
	iMan->GetMajorDep(aDep, aMut, MChromo::EDp_Owner, aLevel, ETrue, EFalse);
    }
    if (aDown) {
	// Childs
	for (TNMReg::const_iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	    MElem* child = it->second;
	    child->GetMajorDep(aDep, aMut, MChromo::EDp_Child, aLevel, EFalse, ETrue);
	}
	// Components
	for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end(); it++) {
	    MElem* comp = *it;
	    comp->GetMajorDep(aDep, aMut, MChromo::EDp_Comps, aLevel, EFalse, ETrue);
	}
    }
}

void Elem::GetDepRank(Rank& aRank, TNodeAttr aAttr) 
{
    TMDep dep(TMutRef(NULL, NULL), ENa_Unknown);
    GetDep(dep, aAttr);
    MElem* depnode = dep.first.first;
    if (depnode != NULL) {
	ChromoNode depmut = iChromo->CreateNode(dep.first.second);
	// Using combined calc of rank because of possibility that mut can be deattached
	depnode->GetRank(aRank, depmut);
    }
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

void Elem::SetRemoved()
{
    // Remove node from native hier but keep it in inher hier
    // Notify the man of deleting
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(*this);
    }
#if 0 // Regular iteration is to be used because no real removal happens, but just "soft", ref ds_mut_rm_appr2
    // Mark the comps as removed, using iterator refresh because the map is updated on each comp deletion
    vector<Elem*>::reverse_iterator it = iComps.rbegin();
    while (it != iComps.rend()) {
	Elem* comp = *it;
	comp->SetRemoved();
	it = iComps.rbegin();
    }
#else
    // Removing comps in reverce order
    for (vector<MElem*>::reverse_iterator it = iComps.rbegin(); it != iComps.rend(); it++) {
	MElem* comp = *it;
	comp->SetRemoved();
    }
#endif
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

TBool Elem::CompactChromo()
{
    TBool corrected = EFalse;
    // Going thru mutations in chromo
    ChromoNode croot = iChromo->Root();
    ChromoNode::Iterator mit = croot.Begin();
    while (mit != croot.End()) {
	ChromoNode gmut = (*mit);
	// Avoid optimizing out deattached mutations, ref ds_mut_sqeezing_so_prnc_att
	if (IsMutAttached(gmut)) {
	    corrected |= CompactChromo(gmut);
	}
	mit++;
    }
    return corrected;
}

TBool Elem::CompactChromo(const ChromoNode& aNode)
{
    TBool mut_removed = EFalse;
    TBool corrected = EFalse;
    ChromoNode gmut = aNode;
    Rank grank;
    gmut.GetRank(grank);
    TNodeType muttype = gmut.Type();
    if (muttype == ENt_Node) {
	// Get node this mutation relates to
	TCMRelFrom key = TCMRelFrom(gmut.Handle(), ENa_Id);
	if (iCMRelReg.count(key) > 0) {
	    MElem* node = iCMRelReg.at(key);
	    // Compact the node
	    corrected = node->CompactChromo();
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Chromo squeezing: cannot find related node for mutation of rank [%i]", gmut.GetLocalRank());
	}
    }
    else if (false && muttype == ENt_Change) {
	// Get node this mutation relates to
	TCMRelFrom key = TCMRelFrom(gmut.Handle(), ENa_MutNode);
	if (iCMRelReg.count(key) > 0) {
	    MElem* node = iCMRelReg.at(key);
	    string new_val(gmut.Attr(ENa_MutVal));
	    // Get mutations that depends on this node, and have lower rank
	    for (TMDeps::iterator it = node->GetMDeps().begin(); it != node->GetMDeps().end(); it++) {
		TMDep dep = *it;
		ChromoNode mut = iChromo->CreateNode(dep.first.second);
		Rank rank;
		mut.GetRank(rank);
		if (rank >= grank) break;
		// Correct mutation
		if (dep.second == ENa_Id) {
		    mut.SetAttr(dep.second, new_val);
		}
		else if (dep.second == ENa_Parent) {
		    mut.SetAttr(dep.second, node->GetUri(dep.first.first));
		}
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Chromo squeezing: cannot find related node for mutation of rank [%i]", gmut.GetLocalRank());
	}
	// Remove mutation
	gmut.Rm();
	RmCMDep(gmut, ENa_MutNode);
	mut_removed = ETrue;
    }
    else if (muttype == ENt_Cont) {
	// Get node this mutation relates to
	TCMRelFrom key = TCMRelFrom(gmut.Handle(), ENa_MutNode);
	if (iCMRelReg.count(key) > 0) {
	    MElem* node = iCMRelReg.at(key);
	    // Get -cont- mutations that depends on this node, and have lower rank
	    for (TMDeps::iterator it = node->GetMDeps().begin(); it != node->GetMDeps().end(); it++) {
		TMDep dep = *it;
		ChromoNode mut = iChromo->CreateNode(dep.first.second);
		Rank rank;
		mut.GetRank(rank);
		if (rank >= grank) continue;
		// Deactivate mutation
		if (mut.Type() == ENt_Cont && dep.second == ENa_MutNode) {
		    // Avoid optimizing out deattached mutations, ref ds_mut_sqeezing_so_prnc_att
		    if (IsMutAttached(mut)) {
			mut.Deactivate();
			corrected = ETrue;
		    }
		}
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Chromo squeezing: cannot find related node for mutation of rank [%i]", gmut.GetLocalRank());
	}
    }
    else if (muttype == ENt_Rm) {
	// Get node this mutation relates to
	TCMRelFrom key = TCMRelFrom(gmut.Handle(), ENa_MutNode);
	// Optimize -rm- out only if there is original -node- mut, ref ds_mut_sqeezing_so_prnc_cnd
	if (iCMRelReg.count(key) > 0) {
	    // Deactivate mutation of creation of deleted node
	    MElem* node = iCMRelReg.at(key);
	    for (TMDeps::iterator it = node->GetMDeps().begin(); it != node->GetMDeps().end(); it++) {
		TMDep dep = *it;
		ChromoNode mut = iChromo->CreateNode(dep.first.second);
		Rank rank;
		mut.GetRank(rank);
		if (mut.Type() == ENt_Node && (dep.second == ENa_MutNode) || dep.second == ENa_Id) {
		    // Avoid optimizing out deattached mutations, ref ds_mut_sqeezing_so_prnc_att
		    if (IsMutAttached(mut)) {
			mut.Deactivate();
			corrected = ETrue;
		    }
		}
	    }
	    // Deativete -rm- mutation only is origin mutation of creating node was deactivated
	    if (corrected) {
		gmut.Deactivate();
	    }
	}
    }
    return corrected;
}

void Elem::UndoCompactChromo()
{
    ChromoNode croot = iChromo->Root();
    croot.Activate();
}

void Elem::AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr)
{
    iMDeps.push_back(TMDep(TMutRef(aNode, (void*) aMut.Handle()), aAttr));
}

void Elem::RemoveMDep(const TMDep& aDep, const MElem* aContext)
{
    if (aContext == this) return;
    for (TMDeps::iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	if (*it == aDep) {
	    MElem* node = aDep.first.first;
	    ChromoNode mut = iChromo->CreateNode(aDep.first.second);
	    node->RmCMDep(mut, aDep.second, aContext != NULL ? aContext: this);
	    iMDeps.erase(it);
	    break;
	}
    }
}

void Elem::RmMCDeps()
{
    while (iMDeps.begin() != iMDeps.end()) {
	TMDep dep = *(iMDeps.begin());
	MElem* node = dep.first.first;
	ChromoNode mut = iChromo->CreateNode(dep.first.second);
	node->RmCMDep(mut, dep.second, this);
	iMDeps.erase(iMDeps.begin());
    }
}

void Elem::AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode)
{
    // Node to chromo
    aNode->AddMDep(this, aMut, aAttr);
    // Chromo to node
    iCMRelReg.insert(TCMRel(TCMRelFrom((void*) aMut.Handle(), aAttr), aNode));
}

void Elem::RmCMDeps()
{
    while (iCMRelReg.begin() != iCMRelReg.end()) {
	TCMRelReg::iterator it = iCMRelReg.begin();
	ChromoNode mut = iChromo->CreateNode(it->first.first);
	RmCMDep(mut, it->first.second);
    }
}

TBool Elem::RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext)
{
    TBool res = EFalse;
    if (aContext == this) return res;
    TCMRelFrom key((void*) aMut.Handle(), aAttr);
    if (iCMRelReg.count(key) > 0) {
	MElem* node = iCMRelReg.at(key);
	iCMRelReg.erase(key);
	node->RemoveMDep(TMDep(TMutRef(this, (void*) aMut.Handle()), aAttr), aContext != NULL ? aContext: this);
	res = ETrue;
    }
    else {
	for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end() && !res; it++) {
	    MElem* comp = *it;
	    res = comp->RmCMDep(aMut, aAttr);
	}
    }
    return res;
}

ChromoNode Elem::GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const
{
    ChromoNode res(Chromos().Root().Mdl(), NULL);
    TBool smut = aMut.Handle() != NULL;

    const ChromoNode& root = Chromos().Root();
    for (ChromoNode::Const_Iterator mit = root.Begin(); mit != root.End(); mit++) {
	ChromoNode mut = smut ? aMut: *mit;
	Rank mutrank;
	GetRank(mutrank, mut);
	typedef map<TNodeAttr, string> TAttrs;
	// Looking thru all types of dependencies
	for (TAttrs::const_iterator it = GUriBase::KNodeAttrsNames.begin(); it != GUriBase::KNodeAttrsNames.end(); it++) {
	    TNodeAttr attr = it->first;
	    TCMRelFrom key((void*) mut.Handle(), attr);
	    if (iCMRelReg.count(key) > 0) {
		MElem* mnode = iCMRelReg.at(key);
		// Check if this is forward dep, i.e. model node rank is greater that mut rank
		Rank mdeprank;
		mnode->GetRank(mdeprank, mnode->Chromos().Root());
		if (mdeprank > mutrank) {
		    MElem* comp = aOwner->GetCompOwning(mnode);
		    if (comp != NULL) {
			res = comp->Chromos().Root();
			break;
		    }
		}
	    }
	}
	// If dep isn't found and mut is the node, look into its deep
	if (res.Handle() == NULL && mut.Type() == ENt_Node) {
	    TCMRelFrom key((void*) mut.Handle(), ENa_Id);
	    __ASSERT(iCMRelReg.count(key) != 0);
	    MElem* mnode = iCMRelReg.at(key);
	    res = mnode->GetLocalForwardCCDep(aOwner, ChromoNode());
	    if (res.Handle() != NULL) break;
	}
	if (res.Handle() != NULL) break;
	if (smut) break;
    }
    return res;
}

// TODO [YB] To consider reimplementation using GetCommonAowner instead of
// searching common Aowner here
TBool Elem::ResolveMutUnsafety(MElem* aMutated, const TMDep& aDep)
{
    TBool res = EFalse;
    // Get dependency on dependent node
    // TODO [YB] Mut type is used incorrectly here! It is not mut of aDepOn, so there is no
    // sense to pass it to aDepOn->GetMajorDep
//    TMDep dep = aRef->GetMajorDep(aMutType, MChromo::EDl_Critical);
    // Check if both mutated and dependency are owned
    MElem* mcomp = GetAcompAttaching(aMutated);
    if (mcomp != NULL) {
	// Target point for shifting is dep mutation if dcomp is this and dcomp creation mut if not
	MElem* dcomp = this;
	ChromoNode targmut(iChromo->CreateNode(aDep.first.second));
	if (aDep.first.first != this) {
	    //dcomp = GetAcompAttaching(aDep.first.first);
	    dcomp = GetAcompOwning(aDep.first.first);
	    if (dcomp != NULL) {
		targmut = dcomp->Chromos().Root();
	    }
	}
	if (dcomp != NULL) {
	    // Owned, shifting the mutated over the dependency
	    mcomp->Chromos().Root().MoveNextTo(targmut);
	    // Shifting changes deps scheme, resolve forward deps
	    ResolveMutsUnsafety();
	    res = ETrue;
	}
    }
    else {
	// Not owned, redirect to owner
	if (iMan != NULL) 
	    res = iMan->ResolveMutUnsafety(aMutated, aDep);
    }
    return res;
}

TBool Elem::ResolveMutsUnsafety()
{
    // Use simple direct algorithm: find out the node with forward dep and move it
    // Repeat the search from the beginning
    ChromoNode& root = Chromos().Root();
    ChromoNode::Iterator mit = root.Begin();
    while (mit != root.End()) {
	ChromoNode node = *mit;
	ChromoNode dep = GetLocalForwardCCDep(this, node);
	if (dep.Handle() != NULL) {
	    // Dep found, shifting the current mutation over the dependency
	    node.MoveNextTo(dep);
	    // Restart searching dep
	    mit = root.Begin();
	}
	else {
	    mit++;
	}
    }
}

void Elem::GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef)
{
}

TMDep Elem::GetRefDep(MElem* aRef, TNodeAttr aReftype, MElem* aObj)
{
    // Get regular dep first
    // TODO [YB] Using ENt_Cont at the moment, because it seems to be nearest
    // to evolve dependencies we looking at. To consider more solid solution
    TMDep dep = aRef->GetMajorDep(ENt_Cont, MChromo::EDl_Critical);
    // Try to check also implicit deps, ref ds_indp_mutord_impl
    if (aReftype == ENa_Ref && iMan != NULL && aObj != NULL) {
	iMan->GetImplicitDep(dep, aObj, aRef);
    }
    return dep;
}

void Elem::GetContactsData(vector<string, string>& aData) const
{
}

TBool Elem::HasParentModifs() const
{
    return iParent->HasModifs(iParent);
}

void Elem::CopyModifsFromParent()
{
    MElem* aowner = GetAowner();
    if (aowner != NULL) {
	aowner->CopyParentModifsToComp(this);
    }
}

// Checking if agent has modifs made outside of owner/self aBase
TBool Elem::HasModifs(const MElem* aBase) const
{
    TBool res = false;
    for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end() && !res; it++) {
	MElem* comp = *it;
	res = comp->HasModifs(aBase);
    }
    for (TMDeps::const_iterator it = iMDeps.begin(); it != iMDeps.end() && !res; it++) {
	TMDep dep = *it;
	if (dep.second == ENa_MutNode && dep.first.first->IsComp(aBase)) res = true;
    }
    return res;
}

/* Transforms modif to comps parent to comps mutations, ref ds_transf_prntmodif
 * Modifs just are rebased from parents aowner to this (owner of comp)
 * Then during the mutation the modifs are transromed to muts because of phenomdf disabled
 * Rebasing scheme is as:
 *                parents_owner - parent - tart_in_parent - ref_in_parent
 *              /
 * common_owner
 *              \
 *                compons_owner - comp   -  targ_in_comp  - ref_in_comp
 *
 * Where parent is parent of comp
 * aBase - parents_owner
 * aMut - mut of aBase
 */
void Elem::CopyParentModifsToComp(MElem* aComp)
{
    TBool efix = iEnv->ChMgr()->EnableFixErrors();
    iEnv->ChMgr()->SetEnableFixErrors(ETrue);
    MElem* cparent = aComp->GetParent();
    MElem* paowner = cparent->GetAttachingMgr();
    // Go thru muts in chromo, select comps parents related and copy
    ChromoNode& chrroot = paowner->Chromos().Root();
    for (ChromoNode::Iterator it = chrroot.Begin(); it != chrroot.End(); it++) {
	ChromoNode mut = *it;
	if (mut.AttrExists(ENa_MutNode)) {
	    string tnodeuris = mut.Attr(ENa_MutNode);
	    GUri tnodeuri(tnodeuris);
	    GUri uri_prnt_to_targ;
	    TBool isuriofcomp = paowner->RebaseUriToIntNode(tnodeuri, cparent, uri_prnt_to_targ);
	    if (isuriofcomp) {
		// Rebase target uri from parent to comp
		GUri uri_this_to_ctarg;
		aComp->GetUri(uri_this_to_ctarg, this);
		uri_this_to_ctarg.Append(uri_prnt_to_targ);
		string curi = uri_this_to_ctarg.GetUri();
		ChromoNode newmut = AppendMutation(mut);
		// Rebase parent
		if (newmut.AttrExists(ENa_Parent)) {
		    string cururis = newmut.Attr(ENa_Parent);
		    GUri cururi(cururis);
		    GUri newuri;
		    RebaseUriToOuterNode(paowner, cururi, newuri);
		    newmut.SetAttr(ENa_Parent, newuri.GetUri());
		}
		// No need to rebase refs because it's relative to targ
		newmut.SetAttr(ENa_MutNode, curi);
		Mutate(EFalse, ETrue, EFalse);
	    }
	}
    }
    iEnv->ChMgr()->SetEnableFixErrors(efix);
}

/* Rebases uri to the node that is within uri chain
 * aUri - input uri
 * aComp - new base, the comp of base of input uri
 * aResult - relult uri
 */
TBool Elem::RebaseUriToIntNode(const GUri& aUri, const MElem* aComp, GUri& aResult)
{
    TBool res = EFalse;
    const MElem* node = GetNode(aUri);
    if (node == aComp) {
	res = ETrue;
    } else {
	for (GUriBase::const_elem_iter it = aUri.Elems().begin(); it != aUri.Elems().end(); it++) {
	    string uri = aUri.GetUriBody(it);
	    const MElem* node = GetNode(uri);
	    if (node == aComp) {
		res = ETrue;
		aResult.AppendTail(aUri, it);
		break;
	    };
	}
    }
    return res;
}

/* Rebases uri to this node that is out of uri chain
 * aBase - old base
 * aResult - result node
 */
void Elem::RebaseUriToOuterNode(MElem* aOldBase, const GUri& aUri, GUri& aResult)
{
    aOldBase->GetRUri(aResult, this);
    aResult.Append(aUri);
}

TInt Elem::GetCapacity() const
{
    TInt res = 0;
    for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end(); it++) {
	res++;
	MElem* comp = *it;
	res += comp->GetCapacity();
    }
    return res;
}

void Elem::LogComps() const
{
    for (vector<MElem*>::const_iterator it = iComps.begin(); it != iComps.end(); it++) {
	Elem* comp = ToElem(*it);
	Logger()->Write(MLogRec::EInfo, comp, ">");
	comp->LogComps();
    }
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
    }
    return ETrue;
}

MElem* Elem::GetComp(TInt aInd)
{
    return iComps.at(aInd);
}

string Elem::Uid() const
{
    return GetUri() + GUriBase::KIfaceSepS + MElem::Type();
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
    } else if (name == "GetMan") {
	res = GetMan();
    } else if (name == "GetRoot") {
	res = GetRoot();
    } else if (name == "GetNode") {
	res = GetNode(args.at(0));
    } else if (name == "GetCont") {
	GetCont(aRes, args.at(0));
    } else if (name == "Mutate") {
	TBool rtonly, checksafety, trialmode;
	rtonly = Ifu::ToBool(args.at(1));
	checksafety = Ifu::ToBool(args.at(2));
	trialmode = Ifu::ToBool(args.at(3));
	SetMutation(args.at(0));
	Mutate(rtonly, checksafety, trialmode);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

void Elem::Init()
{
}
