#include "elem.h"
#include "chromo.h"
#include "mprov.h"
#include <stdio.h>
#include <stdlib.h>


set<string> Elem::iCompsTypes;
bool Elem::iInit = false;

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
//    croot.SetAttr(ENa_Parent, iEType);
    SetEType(Type());
}

void *Elem::DoGetObj(const char *aName)
{
    return (strcmp(aName, Type()) == 0) ? this : NULL;
}

const string& Elem::EType() const
{
    return iEType;
}

Elem* Elem::Clone(const string& aName, Elem* aMan, MEnv* aEnv) const
{
    return new Elem(aName, aMan, aEnv);
}

void Elem::SetEType(const string& aEType)
{
    iEType = aEType;
    iChromo->Root().SetAttr(ENa_Parent, iEType);
}

// TODO [YB] Is it redundant? Actually only one type of node allowed -elem
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

Elem* Elem::GetComp(const string& aParent, const string& aName)
{
    return iComps.count(TCkey(aName, aParent)) > 0 ? iComps[TCkey(aName, aParent)] : NULL;
}

Elem* Elem::GetComp(TInt aInd)
{
    Elem *res = NULL;
    map<TCkey, Elem*>::iterator it = iComps.begin();
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
	res = GetComp(elem.first, elem.second);
	if (res != NULL && aPathBase + 1 != aUri.Elems().end()) {
	    res = res->GetNode(aUri, ++aPathBase);
	}
    }
    return res;
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
		// Attach comp chromo
		chrroot.AddChild(node->iChromo->Root(), EFalse);
		Logger()->WriteFormat("Node [%s] - added node [%s] of type [%s]", Name().c_str(), node->Name().c_str(), node->EType().c_str());
	    }
	    else {
		string pname = rno.Attr(ENa_Parent);
		Logger()->WriteFormat("ERROR: Node [%s] - adding node of type [%s] failed", Name().c_str(), pname.c_str());
	    }
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
    }
    return elem;
}

const set<string>& Elem::CompsTypes()
{
    return iCompsTypes;
}

// TODO [YB] It will not work with grandparent. To support.
Elem* Elem::CreateHeir(const string& aName, Elem* iMan)
{
    Elem* heir = Clone(aName, iMan, iEnv);
    heir->SetEType(Name());
    // Set parent
    // TODO [YB] The context is missed here, just name set. To consider, ref discussion in md#sec_desg_chromo_full
    ChromoNode hroot = heir->Chromos().Root();
    hroot.SetAttr(ENa_Parent, Name());
    // Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
    ChromoNode root = iChromo->Root();
    heir->SetMutation(root);
    heir->Mutate(ETrue);
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
    if (iComps.count(TCkey(aComp->Name(), aComp->EType())) == 0)
    {
	iComps.insert(pair<TCkey, Elem*>(TCkey(aComp->Name(), aComp->EType()), aComp));
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
}

void Elem::OnCompAdding(Elem& aComp)
{
}

void Elem::OnCompChanged(Elem& aComp)
{
    if (iMan != NULL) {
	iMan->OnCompChanged(*this);
    }
}


