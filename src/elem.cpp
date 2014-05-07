#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>


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
	iCIterRange = iElem.iMComps.equal_range(Elem::TNMKey(iId.second.second));
	if (aToEnd) {
	    iCIter = iCIterRange.second;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KParentSep || iExt.empty() || iExt == GUri::KTypeAny) {
		iCIter = iCIterRange.first;
	    } 
	    else {
		for (iCIter = iCIterRange.first; iCIter != iCIterRange.second; iCIter++) {
		    Elem* comp = iCIter->second;
		    if (comp->GetParent()->Name() == iExt) {
			break;
		    }
		}
	    }
	}
    }
    else {
	if (iId.second.second == GUri::KTypeAny) {
	    iChildsRange = pair<TNMReg::iterator, TNMReg::iterator>(iElem.iChilds.begin(), iElem.iChilds.end());
	}
	else {
	    iChildsRange = iElem.iChilds.equal_range(iId.second.second);
	}
	if (aToEnd) {
	    iChildsIter = iChildsRange.second;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KNodeSep || iExt.empty() || iExt == GUri::KTypeAny) {
		iChildsIter = iChildsRange.first;
	    }
	    else {
		for (iChildsIter = iChildsRange.first; iChildsIter != iChildsRange.second; iChildsIter++) {
		    Elem* comp = iChildsIter->second;
		    if (comp->GetMan()->Name() == iExt) {
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
	if (!iId.first.empty() && iExtsrel == GUri::KParentSep && !iExt.empty() && iExt != GUri::KTypeAny) {
	    for (; iCIter != iCIterRange.second; iCIter++) {
		Elem* comp = iCIter->second;
		if (comp->GetParent()->Name() == iExt) {
		    break;
		}
	    }
	}
    }
    else {
	iChildsIter++;
	if (!iId.first.empty() && iExtsrel == GUri::KNodeSep && !iExt.empty() && iExt != GUri::KTypeAny) {
	    for (;iChildsIter != iChildsRange.second; iChildsIter++) {
		Elem* comp = iChildsIter->second;
		if (comp->GetMan()->Name() == iExt) {
		    break;
		}
	    }
	}
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
    iObserver(NULL), iParent(NULL), isRemoved(EFalse)
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
	child->OnParentDeleting(this);
	//child->SetParent(NULL);
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

/*
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
*/

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

TBool Elem::AddNode(const ChromoNode& aSpec, TBool aRunTime)
{
    TBool res = ETrue;
    string snode = aSpec.Attr(ENa_MutNode);
    GUri unode(snode);
    Elem* node = GetNode(unode);
    if (node != NULL) {
	if (IsMutSafe(node)) {
	    ChromoNode::Iterator itn = iChromo->Root().Begin();
	    if (!aRunTime) {
		ChromoNode chn = iChromo->Root().AddChild(aSpec);
		node->AddMDep(this, chn, ENa_MutNode);
		itn = chn.Begin();
	    }
	    for (ChromoNode::Const_Iterator mit = aSpec.Begin(); mit != aSpec.End() && res; mit++) {
		const ChromoNode& mno = (*mit);
		Elem* rnode = node->AddElem(mno, aRunTime);
		if (rnode == NULL) {
		    Logger()->Write(MLogRec::EErr, this, "Adding node into [%s] - failure", snode.c_str());
		}
		else {
		    // Adding dependency to object of change
		    if (!aRunTime) {
			const ChromoNode& chno = (*itn);
			rnode->AddMDep(this, chno, ENa_Id);
		    }
		    if (IsLogeventCreOn()) {
			Logger()->Write(MLogRec::EInfo, this, "Added node [%s] into [%s]", mno.Name().c_str(), snode.c_str());
		    }
		}
		if (!aRunTime) {
		    itn++;
		}
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Adding comp to node [%s] - unsafe, used in: [%s]", snode.c_str(), node->GetMajorDep().first.first->GetUri().c_str());
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Adding comp to node: cannot find [%s]", snode.c_str());
    }
    return res;
}

Elem* Elem::GetComp(const string& aParent, const string& aName)
{
    Elem* res = NULL;
    pair<TNMReg::iterator, TNMReg::iterator> range = iMComps.equal_range(TNMKey(aName));
    for (TNMReg::iterator it = range.first; it != range.second && res == NULL; it++) {
	Elem* node = it->second;
	if (node->EType() == aParent) {
	    res = node;
	}
    }
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
    else {
	res = this;
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
	    Elem* node = AddElem(rno, aRunTime);
	    if (node != NULL) {
		if (!aRunTime) {
		    // Attach comp chromo
		   // chrroot.AddChild(node->iChromo->Root(), EFalse);
		}
	    }
	    else {
		string pname = rno.Attr(ENa_Parent);
		Logger()->Write(MLogRec::EErr, this, "Adding node with parent [%s] failed", pname.c_str());
	    }
	}
	else if (rnotype == ENt_Add) {
	    AddNode(rno, aRunTime);
	}
	else if (rnotype == ENt_Change) {
	    ChangeAttr(rno, aRunTime);
	}
	else if (rnotype == ENt_Cont) {
	    DoMutChangeCont(rno, aRunTime);
	}
	else if (rnotype == ENt_Move) {
	    MoveNode(rno, aRunTime);
	}
	else {
	    if (rnotype == ENt_Rm) {
		string snode = rno.Attr(ENa_MutNode);
		GUri unode(snode);
		RmNode(unode);
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

void Elem::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime)
{
    string snode = aSpec.Attr(ENa_MutNode);
    string mattrs = aSpec.Attr(ENa_MutAttr);
    string mval = aSpec.Attr(ENa_MutVal);
    Elem* node = GetNode(snode);
    if (node != NULL) {
	if (IsMutSafe(node)) {
	    TBool res = node->ChangeAttr(GUri::NodeAttr(mattrs), mval);
	    if (!res) {
		Logger()->WriteFormat("ERROR: Changing [%s] - failure", snode.c_str());
	    }
	    else {
		// Adding dependency to object of change
		if (!aRunTime) {
		    ChromoNode chn = iChromo->Root().AddChild(aSpec);
		    //node->AddMDep(this, chn, ENa_MutNode);
		    AddCMDep(chn, ENa_MutNode, node);
		}
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Renaming elem [%s] - unsafe, used in: [%s]", snode.c_str(), node->GetMajorDep().first.first->GetUri().c_str());
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

TBool Elem::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime)
{
    TBool res = ETrue;
    string snode = aSpec.Attr(ENa_MutNode);
    TBool refex = aSpec.AttrExists(ENa_Ref);
    string mval = aSpec.Attr(refex ? ENa_Ref :ENa_MutVal);
    Elem* node = NULL;
    Elem* rnode = NULL;
    if (snode.length() == 0) {
	node = this; 
    }
    else {
	GUri unode(snode);
	node = GetNode(unode);
    }
    if (node != NULL) {
	if (refex) {
	    // For -ref- attr the value is the ref relative to mutated node context
	    rnode = node->GetNode(mval);
	    //mval = rnode->GetRUri(node);
	    if (rnode == NULL) {
		Logger()->Write(MLogRec::EErr, this, "Changing [%s] - cannot find ref", mval.c_str());
		res = EFalse;
	    }
	}
	if (res) {
	    res = node->ChangeCont(mval, EFalse);
	    if (res) {
		if (!aRunTime) {
		    ChromoNode chn = iChromo->Root().AddChild(aSpec);
		    AddCMDep(chn, ENa_MutNode, node);
		    if (refex) {
			AddCMDep(chn, ENa_MutVal, rnode);
		    }
		}
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Changing [%s] - failure", snode.c_str());
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Changing [%s] - cannot find node", snode.c_str());
	}
    }
    return res;
}

Elem* Elem::AddElem(const ChromoNode& aNode, TBool aRunTime)
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
		//parent->AppendChild(elem);
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
    if (elem != NULL) {
	if (!aRunTime) {
	    ChromoNode chn = iChromo->Root().AddChild(elem->iChromo->Root(), EFalse);
	    AddCMDep(chn, ENa_Id, elem);
	    AddCMDep(chn, ENa_Parent, parent);
	}
    }
    return elem;
}

const set<string>& Elem::CompsTypes()
{
    return iCompsTypes;
}

// Reimplemented to keep system consistency. New version is based on run-time model only
// Ref uc_031, ds_rn_prnt
Elem* Elem::CreateHeir(const string& aName, Elem* aMan /*, const GUri& aInitCont */)
{
    Elem* heir = NULL;
    __ASSERT(iParent != NULL);
    if (Provider()->IsProvided(iParent)) {
	// Parent is Agent - native element. Create via provider
	heir = Provider()->CreateNode(EType(), aName, iMan, iEnv);
    }
    else {
	heir = iParent->CreateHeir(aName, iMan);
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
    iParent->RemoveChild(heir);
    AppendChild(heir);

    return heir;
}

#if 0
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
		//parent->AppendChild(heir);
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
#endif

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
    if (GetComp(aComp->EType(), aComp->Name()) == NULL) {
	iMComps.insert(TNMVal(TNMKey(aComp->Name()), aComp));
	iMComps.insert(TNMVal(TNMKey("*"), aComp));
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Registering component [%s] - already exists", aComp->Name().c_str());
	res = EFalse;
    }
    return res;
}

TBool Elem::RegisterChild(Elem* aChild)
{
    TBool res = ETrue;
    iChilds.insert(TNMVal(aChild->Name(), aChild));
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

TBool Elem::IsCompRegistered(Elem* aComp)
{
    Elem* comp = aComp;
    Elem* man = comp->GetMan();
    while (man != this) {
	comp = man;
	man = comp->GetMan();	
    }
    return (GetComp(comp->EType(), comp->Name()) != NULL);
}

TBool Elem::UnregisterComp(Elem* aComp, const string& aName)
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
    __ASSERT(found);
    found = EFalse;
    range = iMComps.equal_range(TNMKey("*"));
    for (TNMReg::iterator it = range.first; it != range.second && !found; it++) {
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

Elem* Elem::GetCommonOwner(Elem* aElem)
{
    Elem* res = NULL;
    Elem* owner = this;
    while (owner != NULL && res == NULL) 
    {
	if (owner->IsComp(aElem)) {
	    res = owner;
	}
	owner = owner->GetMan();
    }
    return res;
}

void Elem::GetRank(Rank& aRank) const
{
    if (iMan != NULL) {
	TInt res = 0;
	for (vector<Elem*>::const_iterator it = iMan->Comps().begin(); it != iMan->Comps().end() && *it != this; it++, res++);
	//aRank.push_back(res);
	aRank.insert(aRank.begin(), res);
	iMan->GetRank(aRank);
    }
}

TInt Elem::GetLocalRank() const
{
    TInt res = -1;
    if (iMan != NULL) {
	res = 0;
	for (vector<Elem*>::const_iterator it = iMan->Comps().begin(); it != iMan->Comps().end() && *it != this; it++, res++);
    }
    return res;
}

void Elem::GetRUri(GUri& aUri, Elem* aTop)
{
    Elem* cowner = GetCommonOwner(aTop);
    GetUri(aUri, cowner);
    Elem* owner = aTop;
    while (owner != cowner) {
	aUri.PrependElem("", GUri::KOwner);
	owner = owner->GetMan();
    }
}

string Elem::GetRUri(Elem* aTop)
{
    GUri uri;
    GetRUri(uri, aTop);
    return uri.GetUri();
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

TBool Elem::IsMutSafe(Elem* aRef)
{
    TBool safemut = ETrue;
    TMDep dep = aRef->GetMajorDep();
    if (dep.first.first != NULL) {
	Rank rank;
	GetRank(rank);
	Rank deprank;
	ChromoNode depnode = iChromo->CreateNode(dep.first.second);
	depnode.GetRank(deprank);
	if (deprank > rank && !deprank.IsRankOf(rank)) {
	    safemut = EFalse;
	}
    }
    return safemut;
}

TBool Elem::RmNode(const GUri& aUri)
{
    TBool res = EFalse;
    Elem* node = GetNode(aUri);
    if (node != NULL) {
	// Check dependent mutations
	if (IsMutSafe(node)) {
	    res = ETrue;
	    // If node has children then just mark it as removed but not remove actually
	    if (node->HasInherDeps()) {
		node->SetRemoved();
	    }
	    else {
		delete node;
	    }
	    if (IsLogeventCreOn()) {
		Logger()->Write(MLogRec::EInfo, this, "Removed elem [%s]", aUri.GetUri().c_str());
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Removing elem [%s] - unsafe, used in: [%s]", aUri.GetUri().c_str(), node->GetMajorDep().first.first->GetUri().c_str());
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Removing elem [%s] - not found", aUri.GetUri().c_str());
    }
    return res;
}

TBool Elem::MoveNode(const ChromoNode& aSpec, TBool aRunTime)
{
    TBool res = EFalse;
    string srcs = aSpec.Attr(ENa_Id);
    TBool dest_spec = aSpec.AttrExists(ENa_MutNode);
    string dests = dest_spec ? aSpec.Attr(ENa_MutNode): string();
    Elem* dnode = dest_spec ? GetNode(dests) : this;
    if (dnode != NULL) {
	GUri srcsuri(srcs);
	if (srcsuri.Scheme().empty()) {
	    // Local node
	    Elem* snode = GetNode(srcs);
	    Elem* sowner = snode->GetMan();
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
		    if (IsMutSafe(snode)) {
			// It is rather complicated to re-create snode in new context because all snode
			// mutation are related to original snode context, so use trick, ref "ds_mv_local"
			// Create heir of source node in destination context
			Elem* heir = snode->CreateHeir(snode->Name(), dnode);
			// Re-adopt back to source parent
			snode->RemoveChild(heir);
			snode->GetParent()->AppendChild(heir);
			res = dnode->AppendComp(heir);
			if (res) {
			    // Remove source node
			    // If node has children then just mark it as removed but not remove actually
			    if (snode->HasInherDeps()) {
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
			Logger()->Write(MLogRec::EErr, this, "Moving elem [%s] - unsafe, used in: [%s]", snode->GetUri().c_str(), snode->GetMajorDep().first.first->GetUri().c_str());
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
		Elem* nnode = NULL;
		if (dnode != NULL) {
		    nnode= dnode->AddElem(root, ETrue);
		}
		else {
		    nnode = AddElem(root, ETrue);
		}
		delete spec;
		res = nnode != NULL;
	    }
	}
	if (!aRunTime && res) {
	    // Adding dependency to object of change
	    ChromoNode chn = iChromo->Root().AddChild(aSpec);
	    AddCMDep(chn, ENa_MutNode, dnode);
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Moving to node [%s] - not found", dests.c_str());
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
    int pos = EType(EFalse).find(aParent);
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

Elem* Elem::GetMajorChild(Rank& rr)
{
    Elem* res = NULL;
    // Child theyself
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	Rank rd;
	it->second->GetRank(rd);
	if (rd > rr) {
	    rr = rd;
	    res = it->second;
	}
    }
    // Childs of childs
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	Elem* res1 = it->second->GetMajorChild(rr);
	if (res1 != NULL) {
	    res = res1;
	}
    }
    return res;
}


void Elem::GetMajorChild(Elem*& aElem, Rank& rr)
{
    // Child theyself
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	Rank rd;
	it->second->GetRank(rd);
	if (rd > rr) {
	    rr = rd;
	    aElem = it->second;
	}
    }
    // Childs of childs
    for (TNMReg::iterator it = iChilds.begin(); it != iChilds.end(); it++) {
	Elem* res1 = it->second->GetMajorChild(rr);
	if (res1 != NULL) {
	    aElem = res1;
	}
    }
}

Elem::TMDep Elem::GetMajorDep()
{
    TMDep res(TMutRef(this, iChromo->Root().Handle()), ENa_Unknown);
    GetMajorDep(res);
    return res;
}

void Elem::GetMajorDep(TMDep& aDep)
{
    // Ref to theses ds_mut_unappr_rt_ths1 for rules of searching deps
    Rank rc;
    ChromoNode mut = iChromo->CreateNode(aDep.first.second == NULL ? iChromo->Root().Handle() : aDep.first.second);
    mut.GetRank(rc);
    // Childs
    Elem* mc = GetMajorChild(rc);
    if (mc != NULL) {
	aDep.first.first = mc;
	aDep.first.second = mc->Chromos().Root().Handle();
    }
    // Other deps
    for (TMDeps::const_iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	TMDep dep = *it;
	Rank rd;
	ChromoNode dcn = iChromo->CreateNode(dep.first.second);
	dcn.GetRank(rd);
	if (rd > rc) {
	    rc = rd;
	    aDep = dep;
	    // Dependence childs
	    Elem* dc = dep.first.first->GetMajorChild(rc);
	    if (dc != NULL) {
		aDep.first.first = dc;
		aDep.first.second = dc->Chromos().Root().Handle();
	    }
	}
    }
    // Components
    for (TNMReg::const_iterator it = iMComps.begin(); it != iMComps.end(); it++) {
	Elem* comp = it->second;
	comp->GetMajorDep(aDep);
    }
}


// Handles parent deleting, ref uc_029
void Elem::OnParentDeleting(Elem* aParent)
{
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
    isRemoved = ETrue;
}

TBool Elem::HasChilds() const
{
    return !iChilds.empty();
}

TBool Elem::HasInherDeps() const
{
    TBool res = !iChilds.empty();
    for (TNMReg::const_iterator it = iMComps.begin(); it != iMComps.end() && !res; it++) {
	Elem* comp = it->second;
	res = comp->HasInherDeps();
    }
    return res;
}

void Elem::CompactChromo()
{
    // Going thru mutations in chromo
    ChromoNode croot = iChromo->Root();
    ChromoNode::Iterator mit = croot.Begin();
    while (mit != croot.End()) {
	TBool mut_removed = EFalse;
	ChromoNode gmut = (*mit);
	Rank grank;
	gmut.GetRank(grank);
	TNodeType muttype = gmut.Type();
	if (muttype == ENt_Change) {
	    // Get node this mutation relates to
	    TCMRelFrom key = TCMRelFrom(gmut.Handle(), ENa_MutNode);
	    if (iCMRelReg.count(key) > 0) {
		Elem* node = iCMRelReg.at(key);
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
		Logger()->Write(MLogRec::EErr, this, "Cannot find related node for mutation of rank [%i]", gmut.GetLocalRank());
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
		Elem* node = iCMRelReg.at(key);
		string new_val(gmut.Attr(ENa_Ref));
		// Get mutations that depends on this node, and have lower rank
		for (TMDeps::iterator it = node->GetMDeps().begin(); it != node->GetMDeps().end(); it++) {
		    TMDep dep = *it;
		    ChromoNode mut = iChromo->CreateNode(dep.first.second);
		    Rank rank;
		    mut.GetRank(rank);
		    if (rank >= grank) break;
		    // Correct mutation
		    if (mut.Type() == ENt_Cont && dep.second == ENa_MutNode) {
			mut.SetAttr(ENa_Ref, new_val);
		    }
		}
		// Remove mutation
		gmut.Rm();
		RmCMDep(gmut, ENa_MutNode);
		mut_removed = ETrue;
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Cannot find related node for mutation of rank [%i]", gmut.GetLocalRank());
	    }
	}

	if (mut_removed) {
	    // Start from the beginning to have valid iterator
	    mit = croot.Begin();
	}
	else {
	    mit++;
	}
    }
}


void Elem::AddMDep(Elem* aNode, const ChromoNode& aMut, TNodeAttr aAttr)
{
    iMDeps.push_back(TMDep(TMutRef(aNode, (void*) aMut.Handle()), aAttr));
}

void Elem::RemoveMDep(const TMDep& aDep)
{
    for (TMDeps::iterator it = iMDeps.begin(); it != iMDeps.end(); it++) {
	if (*it == aDep) {
	    iMDeps.erase(it);
	    break;
	}
    }
}

void Elem::AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, Elem* aNode)
{
    // Node to chromo
    aNode->AddMDep(this, aMut, aAttr);
    // Chromo to node
    iCMRelReg.insert(TCMRel(TCMRelFrom((void*) aMut.Handle(), aAttr), aNode));
}

void Elem::RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr)
{
    TCMRelFrom key((void*) aMut.Handle(), aAttr);
    __ASSERT(iCMRelReg.count(key) > 0);
    Elem* node = iCMRelReg.at(key);
    iCMRelReg.erase(key);
    node->RemoveMDep(TMDep(TMutRef(this, (void*) aMut.Handle()), aAttr));
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

