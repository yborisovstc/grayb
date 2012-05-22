#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>


set<string> Elem::iCompsTypes;
bool Elem::iInit = false;

Elem::IterImplBase::IterImplBase(Elem& aElem, GUri::TElem aId, TBool aToEnd): iElem(aElem), iId(aId)
{
    iCIterRange = iElem.iMComps.equal_range(Elem::TCkey(iId.second, iId.first));
    iCIter = aToEnd ? iCIterRange.second : iCIterRange.first;
};

Elem::IterImplBase::IterImplBase(const IterImplBase& aIt): 
    iElem(aIt.iElem), iId(aIt.iId), iCIterRange(aIt.iCIterRange), iCIter(aIt.iCIter) 
{
};

void Elem::IterImplBase::Set(const IterImplBase& aImpl)
{
    iElem = aImpl.iElem;
    iId = aImpl.iId;
    iCIterRange = aImpl.iCIterRange;
    iCIter = aImpl.iCIter; 
}

void Elem::IterImplBase::PostIncr()
{
    iCIter++;
}

TBool Elem::IterImplBase::IsCompatible(const IterImplBase& aImpl) const
{
    return ETrue;
}

TBool Elem::IterImplBase::IsEqual(const IterImplBase& aImpl) const
{
    TBool res = EFalse;
    if (IsCompatible(aImpl) && aImpl.IsCompatible(*this)) {
	res = &iElem == &(aImpl.iElem) && iId == aImpl.iId && iCIter == aImpl.iCIter;
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
    if (iCIter != iCIterRange.second) {
	res = iCIter->second;
    }
    return res;
}

/*
// Local nodes iterator
Elem::Iterator::Iterator(Elem& aElem, GUri::TElem aId): iElem(aElem), iId(aId)
{
    iCIterRange = iElem.iMComps.equal_range(Elem::TCkey(iId.second, iId.first));
    iCIter = iCIterRange.first;
};

Elem::Iterator::Iterator(const Iterator& aIt): iElem(aIt.iElem), iCIter(aIt.iCIter) 
{
};

Elem::Iterator& Elem::Iterator::operator=(const Elem::Iterator& aIt) 
{ 
    iElem = aIt.iElem; 
    iId = aIt.iId;
    iCIterRange = aIt.iCIterRange;
    iCIter = aIt.iCIter; 
    return *this; 
};

Elem::Iterator& Elem::Iterator::operator++()
{
    iCIter++;
    return *this;
}

Elem::Iterator Elem::Iterator::Begin()
{
    return Iterator(iElem, iId);
}

Elem::Iterator Elem::Iterator::End()
{
    Iterator res = Iterator(iElem, iId);
    res.iCIter = iCIterRange.second;
    return res;
}

TBool Elem::Iterator::operator==(const Iterator& aIt)
{
    TBool res = &iElem == &(aIt.iElem) && iId == aIt.iId && iCIter == aIt.iCIter;
}

Elem*  Elem::Iterator::operator*()
{
    return iCIter->second;
}
*/

// Element
void Elem::Init()
{
    iCompsTypes.insert("Elem");
    iInit = true;
}

Elem::Elem(const string &aName, Elem* aMan, MEnv* aEnv): Base(aName), iMan(aMan), iEnv(aEnv)
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

Elem::~Elem() 
{
    // Notify the man of deleting
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this);
    }
    // Remove the comps, using iterator refresh because the map is updated on each comp deletion
    vector<Elem*>::reverse_iterator it = iComps.rbegin();
    while (it != iComps.rend()) {
	delete *it;
	it = iComps.rbegin();
    }
    iComps.clear();
    iMComps.clear();
    iEnv = NULL; // Not owned
    if (iMut != NULL) {
	delete iMut;
	iMut = NULL;
    }
    if (iChromo != NULL) {
	delete iChromo;
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
    __ASSERT(iMan == NULL && aMan != NULL || iMan != NULL && aMan == NULL);
    // TODO [YB] To add notifications here
    iMan = aMan;
}

Elem* Elem::GetMan()
{
    return iMan;
}

void *Elem::DoGetObj(const char *aName, TBool aIncUpHier)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName, MCompsObserver::Type()) == 0) {
	res = (MCompsObserver*) this;
    }
    /*
    else {
	Elem* agents = GetComp("Elem", "Agents");
	if (agents != NULL) {
	    for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && res == NULL; it++) {
		res = (*it)->DoGetObj(aName, EFalse);
	    }
	}
    }
    */
    if (res == NULL && aIncUpHier && iMan != NULL) {
	res = iMan->DoGetObj(aName);
    }

    return res;
}

const string& Elem::EType() const
{
    return iEType;
}

void Elem::SetEType(const string& aEType)
{
    iEType = aEType;
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
		Logger()->WriteFormat("ERROR: Adding node into [%s] - failure", snode.c_str());
	    }
	    else {
		Logger()->WriteFormat("[%s]: added node [%s] into [%s]", Name().c_str(), mno.Name().c_str(), snode.c_str());
	    }
	}
    }
    else {
	Logger()->WriteFormat("ERROR: Adding node: cannot find [%s]", snode.c_str());
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
    GUri::const_elem_iter it = aUri.Elems().begin(); 
    return GetNode(aUri, it); 
}

/*
Elem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase)
{
    Elem* res = NULL;
    GUri::TElem elem = *aPathBase;
    if (elem.first == GUri::KTypeElem) {
	if (elem.second == "..") {
	    res = iMan->GetNode(aUri, ++aPathBase);
	}
	else {
	    res = GetComp(elem.first, elem.second);
	}
    }
    else  {
	res = GetNode(aUri, aPathBase);
    }
    if (res != NULL && aPathBase + 1 != aUri.Elems().end()) {
	res = res->GetNode(aUri, ++aPathBase);
    }
    return res;
}
*/

Elem* Elem::GetNodeLoc(const GUri::TElem& aElem)
{
    return GetComp(aElem.first, aElem.second);
}
/*
Elem::Iterator Elem::GetNodesLoc(const GUri::TElem& aId)
{
    return Iterator(*this, aId);
}
*/

Elem::Iterator Elem::NodesLoc_Begin(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId));
}

Elem::Iterator Elem::NodesLoc_End(const GUri::TElem& aId)
{
    return Iterator(new IterImplBase(*this, aId, ETrue));
}

#if 0
Elem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase)
{
    Elem* res = NULL;
    GUri::TElem elem = *aPathBase;
    if (elem.second == "..") {
	if (iMan != NULL) {
	    res = iMan->GetNode(aUri, ++aPathBase);
	}
	else {
	    Logger()->WriteFormat("[%s]: getting node [%s] - path to top of root", Name().c_str(), aUri.GetUri().c_str());
	}
    }
    else {
	/*
	res = GetNodeLoc(elem);
	if (res != NULL && aPathBase + 1 != aUri.Elems().end()) {
	    res = res->GetNode(aUri, ++aPathBase);
	}
	*/
	Iterator it = GetNodesLoc(elem);
	if (it != it.End()) {
	    if (aPathBase + 1 != aUri.Elems().end()) {
		for (; it != it.End(); it++) {
		    Elem* node = *it;
		    if (res == NULL) {
			res = node->GetNode(aUri, ++aPathBase);
		    }
		    else {
			res = NULL;
			Logger()->WriteFormat("ERR: [%s]: getting node [%s] - mutliple choice", Name().c_str(), aUri.GetUri().c_str());
			break;
		    }
		}
	    }
	    else {
		res = *it;
		if (++it != it.End()) {
		    res = NULL;
		    Logger()->WriteFormat("ERR: [%s]: getting node [%s] - mutliple choice", Name().c_str(), aUri.GetUri().c_str());
		}
	    }
	}
    }
    return res;
}
#endif

Elem* Elem::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase)
{
    Elem* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    if (elem.second == "..") {
	if (iMan != NULL) {
	    res = iMan->GetNode(aUri, ++aPathBase);
	}
	else {
	    Logger()->WriteFormat("[%s]: getting node [%s] - path to top of root", Name().c_str(), aUri.GetUri().c_str());
	}
    }
    else {
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
			    Logger()->WriteFormat("ERR: [%s]: getting node [%s] - multiple choice", Name().c_str(), aUri.GetUri().c_str());
			    break;
			}
		    }
		}
	    }
	    else {
		res = *it;
		if (++it != itend) {
		    res = NULL;
		    Logger()->WriteFormat("ERR: [%s]: getting node [%s] - multiple choice", Name().c_str(), aUri.GetUri().c_str());
		}
	    }
	}
    }
    return res;
}

const vector<Elem*>& Elem::Comps()
{
    return iComps;
}

void Elem::SetMutation(const ChromoNode& aMuta)
{
    iMut->Set(aMuta);
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

TBool Elem::MergeMutation(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    ChromoNode& chrroot = iChromo->Root();
    TNodeType rnotype = aSpec.Type();
    if (rnotype == ENt_Move) {
	res = MergeMutMove(aSpec);
    }
    else {
	chrroot.AddChild(aSpec);
	res = ETrue;
    }
    return res;
}

TBool Elem::MergeMutMove(const ChromoNode& aSpec)
{
    TBool res = EFalse;
    /*
    DesUri src(aSpec.Attr(ENa_Id));
    TNodeType srctype = src.Elems().at(0).first;
    string srcname = src.Elems().at(0).second;
    DesUri dest(aSpec.Attr(ENa_MutNode));
    string destname = dest.Elems().at(0).second;
    if (srctype == ENt_Object) {
	CAE_ChromoNode& croot = iChromo->Root();
	// Find the dest and src
	CAE_ChromoNode::Iterator nidest = croot.Find(ENt_Object, destname);
	CAE_ChromoNode::Iterator nisrc = croot.Find(ENt_Object, srcname);
	// Move node
	CAE_ChromoNode nsrc = *nisrc;
	nsrc.MoveNextTo(nidest);
	res = ETrue;
    }
    else {
	Logger()->WriteFormat("ERROR: Moving element [%s] - unsupported element type for moving", srcname.c_str());
    }
    */
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
		Logger()->WriteFormat("ERROR: Node [%s] - adding node of type [%s] failed", Name().c_str(), pname.c_str());
	    }
	}
	else {
	    if (rnotype == ENt_Add) {
		AddNode(rno);
	    }
	    else if (rnotype == ENt_Cont) {
		DoMutChangeCont(rno);
	    }
	    /*
	       else if (rnotype == ENt_Rm) {
	       string snode = rno.Attr(ENa_MutNode);
	       DesUri unode(snode);
	       RmNode(unode);
	       }
	       else if (rnotype == ENt_Change) {
	       ChangeAttr_v2(rno);
	       }
	       else if (rnotype == ENt_MutMove) 
	       {
	       MoveElem_v1(rno);
	       }
	       */
	    else {
		Logger()->WriteFormat("ERROR: Mutating node [%s] - unknown mutation type [%d]", Name().c_str(), rnotype);
	    }
	    if (!aRunTime) {
		MergeMutation(rno);
	    }
	}
    }
}

TBool Elem::ChangeCont(const string& aVal) 
{
    return EFalse;
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
	res = node->ChangeCont(mval);
	if (!res) {
	    Logger()->WriteFormat("ERROR: Changing [%s] - failure", snode.c_str());
	}
    }
    else {
	Logger()->WriteFormat("ERROR: Changing [%s] - cannot find node", snode.c_str());
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
    Logger()->WriteFormat("[%s] - start adding node [%s:%s]", Name().c_str(), sparent.c_str(), sname.c_str());
    Elem* elem = NULL;
    // Obtain parent first
    Elem *parent = NULL;
    // Check the scheme
    GUri prnturi(sparent);
    TBool ext_parent = ETrue;
    if (prnturi.Scheme().empty()) {
	// Local parent
	parent = GetNode(prnturi);
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
	elem = Provider()->CreateNode(sparent, sname, this, iEnv);
	if (elem == NULL) {
	    Logger()->WriteFormat("ERROR: [%s] - creating elem [%s] - parent [%s] not found", Name().c_str(), sname.c_str(), sparent.c_str());
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
	if (ext_parent) {
	    delete parent;
	}
    }
    if (elem == NULL) {
	Logger()->WriteFormat("ERROR: [%s] - creating elem [%s] - failed", Name().c_str(), sname.c_str());
    }
    else {
	TBool res = AppendComp(elem);
	if (!res) {
	    delete elem;
	    elem = NULL;
	}
	else {
	    // Mutate object 
	    elem->SetMutation(aNode);
	    elem->Mutate();
	}
	Logger()->WriteFormat("[%s] - added node [%s:%s]", Name().c_str(), elem->EType().c_str(), elem->Name().c_str());
    }
    return elem;
}

const set<string>& Elem::CompsTypes()
{
    return iCompsTypes;
}

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
	    // Correct uri because parent uri is relative to the man context
	    prnturi.PrependElem("node", "..");
	    parent = GetNode(prnturi);
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
    }
    if (parent == NULL) {
	// No parents found, create from embedded parent
	heir = Provider()->CreateNode(sparent, aName, this, iEnv);
	if (heir == NULL) {
	    Logger()->WriteFormat("ERROR: [%s] - creating elem [%s] - parent [%s] not found", Name().c_str(), aName.c_str(), sparent.c_str());
	}
    }
    else {
	// Create heir from the parent, the mutation context set to the current (the man)
	heir = parent->CreateHeir(aName, iMan);
    }

    // Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
    ChromoNode root = iChromo->Root();
    heir->SetMutation(root);
    // Mutate run-time only - don't update chromo
    heir->Mutate(ETrue);
    // Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
    heir->SetEType(Name());
    heir->SetParent(Name());
    heir->SetMan(NULL);
    heir->SetMan(aMan);
    // Remove external parent from system
    if (parent != NULL && ext_parent) {
	delete parent;
    }
    return heir;
}

/*
   Elem* Elem::GetComp(const string& aName, Elem* aRequestor)
   {
// Search local
CAE_Object* res = iComps.count(aName) > 0 ? iComps[aName] : NULL;
// Then down
if (res == NULL && aRequestor != NULL) {
for (map<string, CAE_Object*>::iterator it = iComps.begin(); it != iComps.end() && res == NULL; it++) {
CAE_Object* comp = it->second;
if (comp != aRequestor) {
res = comp->GetComp(aName, this);
}
}
}
// Then up
if (res == NULL && iMan != aRequestor) {
res = iMan->GetComp(aName, this);
}
return res;
}
*/

TBool Elem::AppendComp(Elem* aComp)
{
    TBool res = ETrue;
    if (iMComps.count(TCkey(aComp->Name(), aComp->EType())) == 0)
    {
	iComps.push_back(aComp);
	iMComps.insert(pair<TCkey, Elem*>(TCkey(aComp->Name(), aComp->EType()), aComp));
	iMComps.insert(pair<TCkey, Elem*>(TCkey(aComp->Name(), "*"), aComp));
	iMComps.insert(pair<TCkey, Elem*>(TCkey("*", "*"), aComp));
    }
    else {
	Logger()->WriteFormat("ERROR: [%s] - Adding elem [%s] - name already exists", Name().c_str(), aComp->Name().c_str());
	res = EFalse;
    }

    return res;
}

Elem* Elem::GetNode(const string& aUri)
{
    GUri uri(aUri);
    return GetNode(uri);
}

void Elem::OnCompDeleting(Elem& aComp)
{
    // Deattach the comp's chromo
    iChromo->Root().RmChild(aComp.Chromos().Root(), ETrue);
    // Remove from comps
    map<TCkey, Elem*>::iterator it = iMComps.find(TCkey(aComp.Name(), aComp.EType()));
    __ASSERT(it != iMComps.end());
    // TODO [YB] the item (name,*) isn't removed - to add
    it = iMComps.find(TCkey(aComp.Name(), aComp.EType()));
    iMComps.erase(it);
    for (vector<Elem*>::iterator oit = iComps.begin(); oit != iComps.end(); oit++) {
	if (*oit == &aComp) {
	    iComps.erase(oit); break;
	}	
    }
}

void Elem::OnCompAdding(Elem& aComp)
{
}

void Elem::OnCompChanged(Elem& aComp)
{
    Elem* agents = GetComp("Elem", "Agents");
    if (agents != NULL) {
	TBool res = false;
	for (vector<Elem*>::const_iterator it = agents->Comps().begin(); it != agents->Comps().end() && !res; it++) {
	    MACompsObserver* iagent = (*it)->GetObj(iagent);
	    if (iagent != NULL) {
		res = iagent->HandleCompChanged(*this, aComp);
	    }
	}
    }
    else {
	DoOnCompChanged(aComp);
    }
}

/*
   void Elem::OnCompChanged(Elem& aComp)
   {
   Elem* agents = GetComp("Elem", "Agents");
    if (agents != NULL) {
	Elem* eagent = agents->GetComp("*", "MACompsObserver");
	if (eagent != NULL) {
	    MACompsObserver* iagent = eagent->GetObj(iagent);
	    __ASSERT(iagent != NULL);
	    iagent->HandleCompChanged(*this, aComp);
	}
    }
    else {
	DoOnCompChanged(aComp);
    }
}
*/

void Elem::DoOnCompChanged(Elem& aComp)
{
    if (iMan != NULL) {
	iMan->OnCompChanged(aComp);
    }
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

void Elem::GetUri(Elem* aTop, GUri& aUri)
{
    aUri.PrependElem(EType(), Name());
    if (iMan != aTop) {
	iMan->GetUri(aTop, aUri);
    }
}



Agent::Agent(const string &aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetEType(Type());
    SetParent(Type());
}

void *Agent::DoGetObj(const char *aName, TBool aIncUpHier)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

