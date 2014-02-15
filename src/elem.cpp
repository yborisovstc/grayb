#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

string Rank::ToString() const
{
    stringstream res;
    TBool start = ETrue;
    for (vector<TInt>::const_iterator it = begin(); it != end(); it++, start = EFalse) {
	if (!start) {
	    res << ".";
	}
	res << *it;
    }
    return res.str();
}

/*
TBool Rank::IsEqual(const Rank& aArg) const
{
    TBool res = size() == aArg.size();
    if (res) {
	for (TInt cnt = 0; cnt < size() && res; cnt++) {
	    res = (at(cnt) == aArg.at(cnt));
	}
    }
    return res;
}
*/

TInt Rank::Compare(const Rank& aArg) const
{
    TInt res = 0;
    TInt minsize = min(size(), aArg.size());
    for (TInt cnt = 0; cnt < minsize && res == 0; cnt++) {
	if (at(cnt) < aArg.at(cnt)) {
	    res = -1;
	}
	else if (at(cnt) > aArg.at(cnt)) {
	    res = 1;
	}
    }
    if (res == 0 && size() != aArg.size()) {
	res = (size() == minsize) ? -1 : 1;
    }
    return res;
}



set<string> Elem::iCompsTypes;
bool Elem::iInit = false;

string Elem::PEType()
{
    return string() + GUri::KParentSep + Elem::Type();
}

Elem::TICacheRCtx Elem::ToCacheRCtx(const RqContext* aCtx) 
{
    TICacheRCtx res;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	res.push_back(rq);
	cct = cct->Ctx();
    }
    return res;
}

Elem::IfIter::IfIter(Elem* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd): iHost(aHost), iIName(aIName), iReq(aReq) 
{
    TICacheKeyF query(iIName, iReq);
    iQFRange = iHost->iICacheQF.equal_range(query);
    if (iQFRange.first != iQFRange.second) {
	iQFIter = iQFRange.first;
	if (aToEnd) {
	    TICacheQFIter tmp(iQFIter);
	    while (++tmp != iQFRange.second) iQFIter = tmp;
	    iCacheRange = iHost->iICache.equal_range(iQFIter->second);
	    iCacheIter = iCacheRange.second;
	}
	else {
	    iCacheRange = iHost->iICache.equal_range(iQFIter->second);
	    iCacheIter = iCacheRange.first;
	}
    }

};

Elem::IfIter::IfIter(const IfIter& aIt): iHost(aIt.iHost), iIName(aIt.iIName), iReq(aIt.iReq), iQFRange(aIt.iQFRange),
    iQFIter(aIt.iQFIter), iCacheRange(aIt.iCacheRange), iCacheIter(aIt.iCacheIter)
{
}
Elem::IfIter& Elem::IfIter::operator=(const IfIter& aIt)
{
    iHost = aIt.iHost; 
    iIName = aIt.iIName; iReq = aIt.iReq;
    iQFRange = aIt.iQFRange;
    iQFIter = aIt.iQFIter;
    iCacheRange = aIt.iCacheRange;
    iCacheIter = aIt.iCacheIter;
    return *this;
}

Elem::IfIter& Elem::IfIter::operator++()
{
    if (++iCacheIter != iCacheRange.second) {
    }
    else {
	TICacheQFIter tmp(iQFIter);
	if (++tmp != iQFRange.second) {
	    iQFIter++;
	    iCacheRange = iHost->iICache.equal_range(iQFIter->second);
	    iCacheIter = iCacheRange.first;
	}
    }
    return *this;
}

TBool Elem::IfIter::operator==(const IfIter& aIt)
{
    TBool res = (iHost == aIt.iHost && iIName == aIt.iIName && iReq == aIt.iReq && iQFRange == aIt.iQFRange 
	&& iQFIter == aIt.iQFIter && iCacheRange == aIt.iCacheRange && iCacheIter == aIt.iCacheIter);
    return res;
}

void*  Elem::IfIter::operator*()
{
    void* res = iCacheIter->second;
    return res;
}


Elem::IterImplBase::IterImplBase(Elem& aElem, GUri::TElem aId, TBool aToEnd): iElem(aElem), iId(aId)
{
    string ext(GUri::KTypeAny);
    char rel = SRel();
    if (!iId.first.empty()) {
	char extsrel = iId.first.at(iId.first.size() - 1);
	if (extsrel == GUri::KParentSep || extsrel == GUri::KNodeSep) {
	    ext = iId.first.substr(0, iId.first.size() - 1);
	}
    }
    if (rel == GUri::KNodeSep) {
	iCIterRange = iElem.iMComps.equal_range(Elem::TCkey(iId.second.second, ext));
	iCIter = aToEnd ? iCIterRange.second : iCIterRange.first;
    }
    else {
	if (iId.second.second == GUri::KTypeAny) {
	    iChildsRange = pair<TNMReg::iterator, TNMReg::iterator>(iElem.iChilds.begin(), iElem.iChilds.end());
	}
	else {
	    iChildsRange = iElem.iChilds.equal_range(iId.second.second);
	}
	iChildsIter = aToEnd ? iChildsRange.second : iChildsRange.first;
    }
};

Elem::IterImplBase::IterImplBase(const IterImplBase& aIt): 
    iElem(aIt.iElem), iId(aIt.iId), iCIterRange(aIt.iCIterRange), iCIter(aIt.iCIter), iChildsIter(aIt.iChildsIter)
{
};

void Elem::IterImplBase::Set(const IterImplBase& aImpl)
{
    iElem = aImpl.iElem;
    iId = aImpl.iId;
    iCIterRange = aImpl.iCIterRange;
    iCIter = aImpl.iCIter; 
    iChildsIter = aImpl.iChildsIter;
}

char Elem::IterImplBase::SRel() const
{
    return iId.second.first;
}

void Elem::IterImplBase::PostIncr()
{
    if (SRel() == GUri::KNodeSep) {
	iCIter++;
    }
    else {
	iChildsIter++;
    }
}

TBool Elem::IterImplBase::IsCompatible(const IterImplBase& aImpl) const
{
    return ETrue;
}

TBool Elem::IterImplBase::IsEqual(const IterImplBase& aImpl) const
{
    TBool res = EFalse;
    if (IsCompatible(aImpl) && aImpl.IsCompatible(*this)) {
	res = &iElem == &(aImpl.iElem) && iId == aImpl.iId && iCIter == aImpl.iCIter && iChildsIter == aImpl.iChildsIter;
    }
    return res;
}

void *Elem::IterImplBase::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    return res;
}

const void *Elem::IterImplBase::DoGetObj(const char *aName) const
{
    const void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    return res;
}

Elem*  Elem::IterImplBase::GetElem()
{
    Elem* res = NULL;
    if (SRel() == GUri::KNodeSep) {
	if (iCIter != iCIterRange.second) {
	    res = iCIter->second;
	}
    }
    else {
	res = iChildsIter->second;
    }
    return res;
}


// Element
void Elem::Init()
{
    iCompsTypes.insert("Elem");
    iInit = true;
}

Elem::Elem(const string &aName, Elem* aMan, MEnv* aEnv): Base(aName), iMan(aMan), iEnv(aEnv),
    iObserver(NULL), iParent(NULL)
{
    if (!iInit) 
	Init();
    // Set elem type (parent) by default
    //    iEType = Type();
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetEType(Type());
    SetParent(Type());
}

Elem::Elem(Elem* aMan, MEnv* aEnv): Base(Type()), iMan(aMan), iEnv(aEnv),
    iObserver(NULL), iParent(NULL)
{
    if (!iInit) 
	Init();
    iMut = Provider()->CreateChromo();
    iMut->Init(ENt_Node);
    iChromo = Provider()->CreateChromo();
    iChromo->Init(ENt_Node);
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Id, iName);
    SetEType(string());
    SetParent(string());
}

Elem::~Elem() 
{
    // Notify the man of deleting
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(*this);
    }
    if (iParent != NULL) {
	iParent->OnChildDeleting(this);
    }
    // Remove the comps, using iterator refresh because the map is updated on each comp deletion
    vector<Elem*>::reverse_iterator it = iComps.rbegin();
    while (it != iComps.rend()) {
	delete *it;
	it = iComps.rbegin();
    }
    iComps.clear();
    iMComps.clear();
    // Disconnect from the childs
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	Elem* child = it->second;
	child->SetParent(NULL);
    }
    iChilds.clear();
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

const vector<Elem*>& Elem::Comps() const
{
    return iComps;
}

void Elem::SetParent(const string& aParent)
{
    ChromoNode croot = iChromo->Root();
    croot.SetAttr(ENa_Parent, aParent);
}

void Elem::SetMan(Elem* aMan)
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

Elem* Elem::GetMan()
{
    return iMan;
}

const Elem* Elem::GetMan() const
{
    return iMan;
}

// TODO [YB] To remove aIncUpHier - it isn't used anymore
void *Elem::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    RqContext ctx(this, aCtx);
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompsObserver::Type()) == 0) {
	res = (MCompsObserver*) this;
    }
    return res;
}

Elem::TIfRange Elem::GetIfi(const string& aName, const RqContext* aCtx)
{
    // Get from cache first
    TICacheRCtx req = ToCacheRCtx(aCtx);
    IfIter beg(this, aName, req);
    IfIter end(this, aName, req, ETrue);
    if (beg == end) {
	// Invalid cache, update cache
	UpdateIfi(aName, aCtx);
	beg = IfIter(this, aName, req);
	end = IfIter(this, aName, req, ETrue);
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
    Elem* req = GetNode(aReqUri);
    __ASSERT(aReqAssert || (req != NULL));
    if (req != NULL) {
	TIfRange rg = req->GetIfi(aName, &ctx);
	if (rg.first != rg.second) {
	    res = *(rg.first);
	}
    }
    return res;
}

void Elem::RmIfCache(IfIter& aIt)
{
    iICache.erase(aIt.iCacheIter);
}

void Elem::InvalidateIfCache()
{
    iICache.clear();
    iICacheQF.clear();
}

void Elem::InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, void* aVal)
{
    TICacheKeyF keyf(aName, aReq);
    pair<TICacheKey, void*> val(TICacheKey(keyf, aProv), aVal);
    TICacheCIter res = iICache.insert(val);
    // Push to query map if not already exists
    bool exists = false;
    TICacheQFRange frange = iICacheQF.equal_range(keyf);  
    for (TICacheQFIter it = frange.first; it != frange.second && !exists; it++) {
	if (it->second == res->first) {
	    exists = ETrue;
	}
    }
    if (!exists) {
	pair<TICacheKeyF, TICacheKey> qr(keyf, res->first);
	iICacheQF.insert(qr);
    }
}

void Elem::InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg)
{
    for (IfIter it = aRg.first; it != aRg.second; it++) {
	InsertIfCache(aName, aReq, aProv, *it);
    }
}

void Elem::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = DoGetObj(aName.c_str(), aCtx);
    if (res != NULL) {
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
    }
}

const string Elem::EType(TBool aShort) const
{
    if (aShort) {
	size_t pos = iEType.find_last_of(GUri::KParentSep);
	   return iEType.substr(pos + 1);
    }
    else  {
	return iEType;
    }
}

void Elem::SetEType(const string& aPName, const string& aPEType)
{
    iEType = aPEType + GUri::KParentSep + aPName;
}

// TODO [YB] Is it redundant? Actually only one type of node allowed -elem
/*
   TBool Elem::AddNode(const ChromoNode& aSpec)
   {
   TBool res = EFalse;
   TNodeType ntype = aSpec.Type();
   if (ntype == ENt_Node) {
   Elem* elem = AddElem(aSpec);
   if (elem != NULL) {
   res = ETrue;
   Logger()->WriteFormat("Node [%s] - added node [%s] of type [%s]", Name().c_str(), elem->Name().c_str(), elem->EType().c_str());
   }
   }
   return res;
   }
   */

TBool Elem::AddNode(const ChromoNode& aSpec)
{
    TBool res = ETrue;
    string snode = aSpec.Attr(ENa_MutNode);
    GUri unode(snode);
    Elem* node = GetNode(unode);
    if (node != NULL) {
	for (ChromoNode::Const_Iterator mit = aSpec.Begin(); mit != aSpec.End() && res; mit++)
	{
	    const ChromoNode& mno = (*mit);
	    res = node->AddElem(mno);
	    if (!res) {
		Logger()->Write(MLogRec::EErr, this, "Adding node into [%s] - failure", snode.c_str());
	    }
	    else {
		if (IsLogeventCreOn()) {
		    Logger()->Write(MLogRec::EInfo, this, "Added node [%s] into [%s]", mno.Name().c_str(), snode.c_str());
		}
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Adding node: cannot find [%s]", snode.c_str());
    }
    return res;
}

Elem* Elem::GetComp(const string& aParent, const string& aName)
{
    TMElem::iterator it = iMComps.find(TCkey(aName, aParent));
    return (it != iMComps.end()) ? it->second : NULL;
    //    return iMComps.count(TCkey(aName, aParent)) > 0 ? iMComps[TCkey(aName, aParent)] : NULL;
}

Elem* Elem::GetComp(TInt aInd)
{
    Elem *res = NULL;
    map<TCkey, Elem*>::iterator it = iMComps.begin();
    for (TInt cnt = 0; cnt < aInd; cnt++) { 
	it++;
    }
    res = it->second;
    return res;
}

Elem* Elem::GetNode(const GUri& aUri) 
{ 
    Elem* res = NULL;
    GUri::const_elem_iter it = aUri.Elems().begin(); 
    if (it != aUri.Elems().end()) {
	TBool anywhere = (*it).second.second == GUri::KTypeAnywhere; 
	if (it->second.second.empty() || anywhere) {
	    Elem* root = NULL;
	    if (it->second.first == GUri::KNodeSep) {
		root = GetRoot();
	    }
	    else {
		root = GetInhRoot();
	    }
	    if (!anywhere) {
		it++;
		GUri::TElem elem = *it;
		anywhere = elem.second.second == GUri::KTypeAnywhere; 
	    }
	    if (++it != aUri.Elems().end()) {
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
    return res;
}

Elem* Elem::GetNodeS(const char* aUri)
{
    return GetNode(aUri);
}

Elem* Elem::GetNodeLoc(const GUri::TElem& aElem)
{
    return GetComp(aElem.first, aElem.second.second);
}

Elem::Iterator Elem::NodesLoc_Begin(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId));
}

Elem::Iterator Elem::NodesLoc_End(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId, ETrue));
}

Elem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere) 
{
    Elem* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    TBool anywhere = aAnywhere;
    if (elem.second.second == "..") {
	if (iMan != NULL) {
	    res = iMan->GetNode(aUri, ++aPathBase);
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
	Iterator it = NodesLoc_Begin(elem);
	Iterator itend = NodesLoc_End(elem);
	if (it != itend) {
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		for (; it != itend; it++) {
		    Elem* node = *it;
		    Elem* res1 = node->GetNode(aUri, uripos);
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
		    res = NULL;
		    Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - multiple choice", aUri.GetUri().c_str());
		}
	    }
	}
	if (res == NULL && anywhere && uripos != aUri.Elems().end()) {
	    // Try to search in all nodes
	    elem = GUri::Elem(GUri::KNodeSep, GUri::KTypeAny, GUri::KTypeAny);
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    for (; it != itend; it++) {
		Elem* node = *it;
		Elem* res1 = node->GetNode(aUri, uripos, anywhere);
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

vector<Elem*>& Elem::Comps()
{
    return iComps;
}

void Elem::SetMutation(const ChromoNode& aMuta)
{
    iMut->Set(aMuta);
}

void Elem::AppendMutation(const ChromoNode& aMuta)
{
    iMut->Root().AddChild(aMuta);
}

void Elem::Mutate(TBool aRunTimeOnly)
{
    ChromoNode& root = iMut->Root();
    DoMutation(root, aRunTimeOnly);
    // Clear mutation
    for (ChromoNode::Iterator mit = root.Begin(); mit != root.End();)
    {
	ChromoNode node = *mit;
	mit++; // It is required because removing node by iterator breakes iterator itself
	root.RmChild(node);
    }
}

// TODO [YB] Is megre the valid idea at all ?
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

void Elem::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime)
{
    const ChromoNode& mroot = aMutSpec;
    ChromoNode& chrroot = iChromo->Root();
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++)
    {
	TBool res = EFalse;
	ChromoNode rno = (*rit);
	TNodeType rnotype = rno.Type();
	if (rnotype == ENt_Node) {
	    Elem* node = AddElem(rno);
	    if (node != NULL) {
		if (!aRunTime) {
		    // Attach comp chromo
		    chrroot.AddChild(node->iChromo->Root(), EFalse);
		}
	    }
	    else {
		string pname = rno.Attr(ENa_Parent);
		Logger()->Write(MLogRec::EErr, this, "Adding node with parent [%s] failed", pname.c_str());
	    }
	}
	else {
	    if (rnotype == ENt_Add) {
		AddNode(rno);
	    }
	    else if (rnotype == ENt_Cont) {
		DoMutChangeCont(rno);
	    }
	    else if (rnotype == ENt_Rm) {
		string snode = rno.Attr(ENa_MutNode);
		GUri unode(snode);
		RmNode(unode);
	    }
	    else if (rnotype == ENt_Change) {
		ChangeAttr(rno);
	    }
	    else if (rnotype == ENt_Move) 
	    {
		MoveNode(rno);
	    }
	    else {
		Logger()->WriteFormat("ERROR: Mutating node [%s] - unknown mutation type [%d]", Name().c_str(), rnotype);
	    }
	    if (!aRunTime) {
		MergeMutation(rno);
	    }
	}
    }
}

TBool Elem::ChangeCont(const string& aVal, TBool aRtOnly) 
{
    return EFalse;
}

void Elem::GetCont(string& aCont)
{
}

void Elem::ChangeAttr(const ChromoNode& aSpec)
{
    string snode = aSpec.Attr(ENa_MutNode);
    string mattrs = aSpec.Attr(ENa_MutAttr);
    string mval = aSpec.Attr(ENa_MutVal);
    Elem* node = GetNode(snode);
    if (node != NULL) {
	TBool res = node->ChangeAttr(GUri::NodeAttr(mattrs), mval);
	if (!res) {
	    Logger()->WriteFormat("ERROR: Changing [%s] - failure", snode.c_str());
	}
    }
    else {
	Logger()->WriteFormat("ERROR: Changing [%s] - cannot find node", snode.c_str());
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

TBool Elem::DoMutChangeCont(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    string snode = aSpec.Attr(ENa_MutNode);
    string mval = aSpec.Attr(ENa_MutVal);
    Elem* node = NULL;
    if (snode.length() == 0) {
	node = this; 
    }
    else {
	GUri unode(snode);
	node = GetNode(unode);
    }
    if (node != NULL) {
	res = node->ChangeCont(mval, EFalse);
	if (!res) {
	    Logger()->Write(MLogRec::EErr, this, "Changing [%s] - failure", snode.c_str());
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Changing [%s] - cannot find node", snode.c_str());
    }
    return res;
}

Elem* Elem::AddElem(const ChromoNode& aNode)
{
    string sparent = aNode.Attr(ENa_Parent);
    string sname = aNode.Name();
    if (sname.length() == 0) {
	char *name = (char*) malloc(100);
	sprintf(name, "%d", rand());
	sname = name;
	free(name);
    }
    if (IsLogeventCreOn()) {
	Logger()->Write(MLogRec::EInfo, this, "Start adding node [%s:%s]", sparent.c_str(), sname.c_str());
    }
    Elem* elem = NULL;
    // Obtain parent first
    Elem *parent = NULL;
    // Check if the parent is specified
    if (!sparent.empty()) {
	// Check the parent scheme
	GUri prnturi(sparent);
	TBool ext_parent = ETrue;
	if (prnturi.Scheme().empty()) {
	    // Local parent
	    parent = GetNode(prnturi);
	    /*
	       if (parent == NULL) {
	    // No parents found, request provider for native one
	    parent = Provider()->GetNode(sparent);
	    }
	    */
	    ext_parent = EFalse;
	}
	else {
	    // TODO [YB] To add seaching the module - it will allow to specify just file of spec wo full uri
	    Chromo *spec = Provider()->CreateChromo();
	    TBool res = spec->Set(sparent);
	    if (res) {
		const ChromoNode& root = spec->Root();
		parent = AddElem(root);
		delete spec;
	    }
	}
	if (parent == NULL) {
	    // No parents found, create from embedded parent
	    parent = Provider()->GetNode(sparent);
	    elem = Provider()->CreateNode(sparent, sname, this, iEnv);
	    if (parent != NULL) {
		parent->AppendChild(elem);
	    }
	    else  {
		Logger()->Write(MLogRec::EErr, this, "Creating [%s] - parent [%s] not found", sname.c_str(), sparent.c_str());
	    }
	}
	else {
	    // Create heir from the parent
	    elem = parent->CreateHeir(sname, this);
	    // TODO [YB] Seems to be just temporal solution. To consider using context instead.
	    // Make heir based on the parent: re-parent the heir (currently it's of grandparent's parent) and clean the chromo
	    //elem->SetEType(sparent); // The type is set when creating heir
	    ChromoNode hroot = elem->Chromos().Root();
	    hroot.SetAttr(ENa_Parent, sparent);
	    // Remove external parent from system
	    // [YB] DON'T remove parent, otherwise the inheritance chain will be broken
	    if (ext_parent) {
		// delete parent;
	    }
	}
	if (elem == NULL) {
	    Logger()->Write(MLogRec::EErr, this, "Creating elem [%s] - failed", sname.c_str());
	}
	else {
	    TBool res = AppendComp(elem);
	    if (!res) {
		Logger()->Write(MLogRec::EErr, this, "Adding node [%s:%s] failed", elem->EType().c_str(), elem->Name().c_str());
		delete elem;
		elem = NULL;
	    }
	    else {
		// Mutate object 
		elem->SetMutation(aNode);
		elem->Mutate();
		if (IsLogeventCreOn()) {
		    Logger()->Write(MLogRec::EInfo, this, "Added node [%s:%s]", elem->EType().c_str(), elem->Name().c_str());
		}
	    }
	}
    }
return elem;
}

const set<string>& Elem::CompsTypes()
{
    return iCompsTypes;
}

// Note that parents chromo is not copied to heirs chromo, ref UC_019
Elem* Elem::CreateHeir(const string& aName, Elem* aMan /*, const GUri& aInitCont */)
{
    Elem* heir = NULL;
    // Obtain parent first
    string sparent = iChromo->Root().Attr(ENa_Parent);
    // Check the scheme
    GUri prnturi(sparent);
    TBool ext_parent = EFalse;
    Elem *parent = NULL;
    if (!sparent.empty()) {
	if (prnturi.Scheme().empty()) {
	    // Local parent
	    // If uri is not absolute then correct uri because parent uri is relative to the man context
	    const string& firstelem = prnturi.Elems().begin()->second.second;
	    if (!firstelem.empty() && !(firstelem == GUri::KTypeAnywhere)) {
		prnturi.PrependElem("node", "..");
	    }
	    parent = GetNode(prnturi);
	    /*
	       if (parent == NULL) {
	    // No parents found, request provider for native one
	    parent = Provider()->GetNode(sparent);
	    }
	    */
	}
	else {
	    // TODO [YB] To add seaching the module - it will allow to specify just file of spec wo full uri
	    Chromo *spec = Provider()->CreateChromo();
	    TBool res = spec->Set(sparent);
	    if (res) {
		const ChromoNode& root = spec->Root();
		parent = AddElem(root);
		delete spec;
		ext_parent = ETrue;
	    }
	}
	if (parent == NULL) {
	    // No parents found, create from embedded parent
	    parent = Provider()->GetNode(sparent);
	    heir = Provider()->CreateNode(sparent, aName, iMan, iEnv);
	    if (parent != NULL) {
		parent->AppendChild(heir);
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Creating child [%s] - parent [%s] not found", aName.c_str(), sparent.c_str());
	    }
	}
	else {
	    // Create heir from the parent, the mutation context set to the current (the man)
	    heir = parent->CreateHeir(aName, iMan);
	}
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	ChromoNode root = iChromo->Root();
	heir->SetMutation(root);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	heir->Mutate(ETrue);
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetEType(Name(), EType(EFalse));
	heir->SetParent(Name());
	heir->SetMan(NULL);
	heir->SetMan(aMan);
	// Re-adopte the child
	parent->RemoveChild(heir);
	AppendChild(heir);
    }
    else {
	// Inheritance root - create native element
	heir = new Elem(aName, aMan, iEnv);
	AppendChild(heir);
    }
    // Remove external parent from system
    if (parent != NULL && ext_parent) {
	//delete parent;
    }
    return heir;
}

TBool Elem::AppendChild(Elem* aChild)
{
    TBool res = RegisterChild(aChild);
    if (res)
    {
	aChild->SetParent(this);
    }
    return res;
}

void Elem::RemoveChild(Elem* aChild)
{
    UnregisterChild(aChild);
    aChild->SetParent(NULL);
}

TBool Elem::AppendComp(Elem* aComp)
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

TBool Elem::RegisterComp(Elem* aComp)
{
    TBool res = ETrue;
    if (iMComps.count(TCkey(aComp->Name(), aComp->EType())) == 0)
    {
	iMComps.insert(pair<TCkey, Elem*>(TCkey(aComp->Name(), aComp->EType()), aComp));
	iMComps.insert(pair<TCkey, Elem*>(TCkey(aComp->Name(), "*"), aComp));
	iMComps.insert(pair<TCkey, Elem*>(TCkey("*", aComp->EType()), aComp));
	iMComps.insert(pair<TCkey, Elem*>(TCkey("*", "*"), aComp));
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Adding elem [%s] - name already exists", aComp->Name().c_str());
	res = EFalse;
    }
    return res;
}

TBool Elem::RegisterChild(Elem* aChild)
{
    TBool res = ETrue;
    iChilds.insert(TNKey(aChild->Name(), aChild));
    return res;
}

TBool Elem::UnregisterChild(Elem* aChild, const string& aName)
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

TBool Elem::IsCompRegistered(Elem* aComp) const
{
    Elem* comp = aComp;
    Elem* man = comp->GetMan();
    while (man != this) {
	comp = man;
	man = comp->GetMan();	
    }
    return (iMComps.count(TCkey(comp->Name(), comp->EType())) > 0);
}

TBool Elem::UnregisterComp(Elem* aComp, const string& aName)
{
    TBool res = EFalse;
    const string& name = aName.empty() ? aComp->Name() : aName;
    assert (iMComps.count(TCkey(name, aComp->EType())) > 0); 
    // Removing old name related records in register
    //   Name-Type is unique record, erasing it
    iMComps.erase(TCkey(name, aComp->EType()));
    //   Name-AnyType is not unique, erasing only records for this comp
    TBool found = EFalse;
    pair<TMElem::iterator, TMElem::iterator> range = iMComps.equal_range(TCkey(name, "*"));
    for (TMElem::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aComp) {
	    iMComps.erase(it);
	    found = ETrue;
	}
    }
    __ASSERT(found);
    //   AnyName-Type is not unique, erasing only records for this comp
    found = EFalse;
    range = iMComps.equal_range(TCkey("*", aComp->EType()));
    for (TMElem::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aComp) {
	    iMComps.erase(it);
	    found = ETrue;
	}
    }
    __ASSERT(found);
    //   AnyName-AnyType is not unique, erasing only records for this comp
    range = iMComps.equal_range(TCkey("*", "*"));
    found = EFalse;
    for (TMElem::iterator it = range.first; it != range.second && !found; it++) {
	if (it->second == aComp) {
	    iMComps.erase(it);
	    found = ETrue;
	}
    }
    __ASSERT(found);
    res = ETrue;
    return res;
}

TBool Elem::MoveComp(Elem* aComp, Elem* aDest)
{
    TBool res = EFalse;
    vector<Elem*>::iterator it;
    for (it = iComps.begin(); it != iComps.end() && *it != aComp; it++);
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

Elem* Elem::GetNode(const string& aUri)
{
    Elem* res = NULL;
    GUri uri(aUri);
    if (!uri.IsErr()) {
	res = GetNode(uri);
    }
    else  {
	Logger()->Write(MLogRec::EErr, this, "Incorrect URI [%s]", aUri.c_str());
    }
    return res;
}

void Elem::OnCompDeleting(Elem& aComp)
{
    // Translate to hier
    if (iMan != NULL) {
	iMan->OnCompDeleting(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnCompDeleting(aComp);
    }
    // Handle for direct child
    if (aComp.GetMan() == this) {
	// Don't deattach the childs chromo. Ref UC_016. The childs chromo (childs creation mutation) needs
	// to be kept in parents chromo.
	/*
	// Deattach the comp's chromo
	iChromo->Root().RmChild(aComp.Chromos().Root(), ETrue);
	*/
	// Unregister first
	UnregisterComp(&aComp);
	// Then remove from comps
	for (vector<Elem*>::iterator oit = iComps.begin(); oit != iComps.end(); oit++) {
	    if (*oit == &aComp) {
		iComps.erase(oit); break;
	    }	
	}
    }
}

void Elem::OnCompAdding(Elem& aComp)
{
    // If the comp is already registered then propagate the event
    if (IsCompRegistered(&aComp)) {
	if (iMan != NULL) {
	    iMan->OnCompAdding(aComp);
	}
	if (iObserver != NULL) {
	    iObserver->OnCompAdding(aComp);
	}
    }
}

// TODO [YB] To include agents as member of elem. This will be more effective
// TODO [YB] The current scheme doesn't allow element to get notification of related node change.
// The only notification thry hier can be get via MCompsObserver, but not relation thru edges.
// Thus is node A is related to node B via the chain of edges, the change of B or even the change
// of the relation chain cannot be observed by node A. This is the serious lack. This is because the 
// If providing mechanism is used instead of full relations tracking in Vert. So node A has Ifaces cache
// that includes ifaces from node B but there is no mechanism of the changes in the cache. To consider
// to implement cache update notification. Ref UC_010 
void Elem::OnCompChanged(Elem& aComp)
{
    Elem* agents = GetComp("Elem", "Agents");
    TBool res = false;
    if (agents != NULL) {
	for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && !res; it++) {
	    MACompsObserver* iagent = (*it)->GetObj(iagent);
	    if (iagent != NULL) {
		res = iagent->HandleCompChanged(*this, aComp);
	    }
	}
    }
    /*
       DoOnCompChanged(aComp);
       */
    if (!res) {
	DoOnCompChanged(aComp);
    }
    // Propagate notification to upper level
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnCompChanged(aComp);
    }
}

TBool Elem::OnCompRenamed(Elem& aComp, const string& aOldName)
{
    TBool res = EFalse;
    // Unregister the comp with its old name
    res = UnregisterComp(&aComp, aOldName);
    if (res) {
	// Register the comp againg with its current name
	res = RegisterComp(&aComp);
    }
    return res;
}

void Elem::OnContentChanged(Elem& aComp)
{
    if (iMan != NULL) {
	iMan->OnContentChanged(aComp);
    }
    if (iObserver != NULL) {
	iObserver->OnContentChanged(aComp);
    }
}

void Elem::DoOnCompChanged(Elem& aComp)
{
}

Elem* Elem::GetCompOwning(const string& aParent, Elem* aElem)
{
    Elem* res = NULL;
    Elem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this && node->EType() == aParent)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

// TODO [YB] to use the generic mathod that uses uri instead of having specific func for type and name
Elem* Elem::GetCompOwningN(const string& aName, Elem* aElem)
{
    Elem* res = NULL;
    Elem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this && node->Name() == aName)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

Elem* Elem::GetCompOwning(Elem* aElem)
{
    Elem* res = NULL;
    Elem* node = aElem;
    while (node->GetMan() != NULL && !(node->GetMan() == this)) {
	node = node->GetMan();	
    }
    if (node->GetMan() != NULL) {
	res = node;
    }
    return res;
}

TBool Elem::IsComp(Elem* aElem)
{
    Elem* man = aElem->GetMan();
    while (man != NULL && man != this) {
	man = man->GetMan();	
    }
    return man == this;
}

void Elem::GetRank(Rank& aRank) 
{
    if (iMan != NULL) {
	TInt res = 0;
	for (vector<Elem*>::const_iterator it = iMan->Comps().begin(); it != iMan->Comps().end() && *it != this; it++, res++);
	//aRank.push_back(res);
	aRank.insert(aRank.begin(), res);
	iMan->GetRank(aRank);
    }
}

void Elem::GetUri(GUri& aUri, Elem* aTop)
{
    if (aTop != this) {
	string ext(EType());
	ext.append(1, GUri::KParentSep);
	aUri.PrependElem(ext, Name());
	if (iMan != NULL) {
	    if (iMan != aTop) {
		iMan->GetUri(aUri, aTop);
	    }
	}
	else {
	    aUri.PrependElem("", "");
	}
    }
}

string Elem::GetUri(Elem* aTop)
{
    GUri uri;
    GetUri(uri, aTop);
    return uri.GetUri();
}

Elem* Elem::GetRoot() const
{
    Elem* res = (Elem*) this;
    if (iMan != NULL) {
	res = iMan->GetRoot();
    }
    return res;
}

Elem* Elem::GetInhRoot() const
{
    Elem* res = (Elem*) this;
    if (iParent != NULL) {
	res = iParent->GetInhRoot();
    }
    return res;

}

TBool Elem::RmNode(const GUri& aUri)
{
    Elem* node = GetNode(aUri);
    if (node != NULL) {
	delete node;
	if (IsLogeventCreOn()) {
	    Logger()->Write(MLogRec::EInfo, this, "Removed elem [%s]", aUri.GetUri().c_str());
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Removing elem [%s] - not found", aUri.GetUri().c_str());
    }
}

TBool Elem::MoveNode(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    TBool dest_spec = aSpec.AttrExists(ENa_MutNode);
    string dests = dest_spec ? aSpec.Attr(ENa_MutNode): string();
    Elem* dnode = dest_spec ? GetNode(dests) : NULL;
    GUri srcsuri(srcs);
    if (srcsuri.Scheme().empty()) {
	// Local node
	Elem* snode = GetNode(srcs);
	Elem* owner = snode->GetMan();
	if (snode != NULL) {
	    if (owner != NULL && (dnode == NULL || owner == dnode->GetMan())) {
		// Source and dest has one owner - shifting local node
		res = owner->MoveComp(snode, dnode);
		if (!res) {
		    Logger()->Write(MLogRec::EErr, this, "Moving element [%s] - failure", srcs.c_str());
		}
	    }
	    else {
		// Inter-nodes movement
		const ChromoNode& root = snode->Chromos().Root();
		Elem* res = NULL;
		if (dnode != NULL) {
		    res = dnode->AddElem(root);
		}
		else {
		    res = AddElem(root);
		}
		if (res != NULL) {
		    delete snode;
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
	TBool res = spec->Set(srcs);
	if (res) {
	    const ChromoNode& root = spec->Root();
	    if (dnode != NULL) {
		dnode->AddElem(root);
	    }
	    else {
		AddElem(root);
	    }
	    delete spec;
	}
    }
    return res;
}

TBool Elem::IsName(const char* aName)
{
    return iName == aName;
}

TBool Elem::IsLogeventCreOn() 
{
    Elem* node = GetNode("Elem:Logspec/Elem:Creation");
    return node != NULL;
}

// TODO [YB] To implement with usage of URI but not just string
// TODO [YB] The problem is more serious: we need to check the full type
// (all parents chain) to detect inheritance.
TBool Elem::IsHeirOf(const string& aParent) const
{
    int pos = iEType.find(aParent);
    return pos != string::npos;
}

// Checks if elements chromo is attached. Ref UC_019 for details
// The whole path to root needs to be checked because it is possible
// that the comps chromo is attached but some its mgr is deattached
TBool Elem::IsChromoAttached() const
{
    bool res = true;
    if (iMan != NULL) {
	ChromoNode root = iChromo->Root();
	ChromoNode::Iterator rparentit = root.Parent();
	if (rparentit != root.End()) {
	    ChromoNode rparent = *rparentit;
	    res = (rparent.Type() == ENt_Node);
	}
	if (res) {
	    res = iMan->IsChromoAttached();
	}
    }
    return res;
}

Elem* Elem::GetAttachingMgr()
{
    Elem* res = NULL;
    Elem* cand = this;
    while (res == NULL && cand != NULL) {
	if (cand->IsChromoAttached()) {
	    res = cand;
	}
	else {
	    cand = cand->GetMan();
	}
    }
    return res;
}

Elem* Elem::GetParent()
{
    return iParent;
}

const Elem* Elem::GetParent() const
{
    return iParent;
}

void Elem::SetParent(Elem* aParent)
{
    __ASSERT(aParent != NULL && iParent == NULL || aParent == NULL && iParent != NULL);
    iParent = aParent;
}

void Elem::OnChildDeleting(Elem* aChild)
{
    // Unregister the child
    UnregisterChild(aChild);
}

TBool Elem::OnChildRenamed(Elem* aChild, const string& aOldName)
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

Agent::Agent(const string &aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *Agent::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

