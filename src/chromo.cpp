#include "chromo.h"
#include <stdlib.h>
#include <stdio.h>

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

TBool Rank::IsRankOf(const Rank& aArg) const
{
    TBool res = size() >= aArg.size();
    if (res) {
	for (TInt cnt = 0; cnt < aArg.size() && res; cnt++) {
	    res = (at(cnt) == aArg.at(cnt));
	}
    }
    return res;
}

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

Rank& Rank::operator+=(const Rank& aArg)
{
    for (vector<TInt>::const_iterator it = aArg.begin(); it != aArg.end(); it++) {
	push_back(*it);
    }
    return *this;
}


// Chromo

TBool Chromo::mInitiated = EFalse;
Chromo::TDeps Chromo::mDeps;

Chromo::Chromo()
{
    // TODO [YB] This scheme of deps is not complete. The idea was to get the map
    // pair<mutation_being_applied_to_node, dependency_this_node_have> to level of dep
    // but currently instead of dependency_this_node_have the only muts arg is used
    // but not the whole dep (muts type, muts arg). To improve.
    if (!mInitiated) {
	mDeps.insert(TDepsElm(TDep(ENt_Node, ENa_Id, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Node, ENa_Parent, EDp_Direct), EDl_Affecting));
	mDeps.insert(TDepsElm(TDep(ENt_Node, ENa_Id, EDp_Child), EDl_Affecting));

	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Id, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Id, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Parent, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Parent, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Ref, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_Ref, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_MutNode, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Change, ENa_MutNode, EDp_Comps), EDl_Critical));

	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_Id, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_Id, EDp_Comps), EDl_Affecting));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_Parent, EDp_Direct), EDl_Affecting));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_Ref, EDp_Direct), EDl_Affecting));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_Ref, EDp_Comps), EDl_Affecting));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_MutNode, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Cont, ENa_MutNode, EDp_Comps), EDl_Affecting));

	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Id, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Id, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Parent, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Parent, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Ref, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_Ref, EDp_Comps), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_MutNode, EDp_Direct), EDl_Critical));
	mDeps.insert(TDepsElm(TDep(ENt_Rm, ENa_MutNode, EDp_Comps), EDl_Critical));

	mInitiated = ETrue;
    }
}

TBool Chromo::IsDepOfLevel(const TDep& aDep, TDepsLevel aLevel)
{ 
    TDepsLevel lev = EDl_Harmless;
    if (mDeps.count(aDep) > 0) {
	lev = mDeps.at(aDep);
    }
    return (lev & aLevel) > 0;
};

void Chromo::GetPath(const string& aUri, string& aPath)
{
    size_t scheme_end = aUri.find_first_of(':');
    size_t hier_beg = (scheme_end != string::npos) ? scheme_end+1 : 0; 
    size_t frag_beg = aUri.find_first_of("#");
    string hier = aUri.substr(hier_beg, frag_beg - hier_beg);
    size_t pos = hier.find("//"); 
    if (pos == 0) {
	// There is authority
    }
    else {
	aPath = hier;
    }
}

void Chromo::GetFrag(const string& aUri, string& aFrag)
{
    size_t frag_beg = aUri.find_first_of('#');
    if (frag_beg != string::npos) {
	aFrag = aUri.substr(frag_beg + 1);
    }
}


// Node of chromo spec

ChromoNode::ChromoNode(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle)
{
}

ChromoNode& ChromoNode::operator=(const ChromoNode& aNode) 
{ 
    // TODO [YB] Wrong to assign model because its base is not assignable (Base). To redesign.
//    iMdl = aNode.iMdl; iHandle = aNode.iHandle;
    iHandle = aNode.iHandle;
    return *this; 
}

TBool ChromoNode::operator==(const ChromoNode& aNode) 
{ 
    return iHandle == aNode.iHandle;
}

const string ChromoNode::Attr(TNodeAttr aAttr) 
{
    char* sattr = iMdl.GetAttr(iHandle, aAttr); 
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
};

const string ChromoNode::Attr(TNodeAttr aAttr) const
{
    char* sattr = iMdl.GetAttr(iHandle, aAttr); 
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
};

TInt ChromoNode::AttrInt(TNodeAttr aAttr) const 
{
    string attr = Attr(aAttr); 
    __ASSERT(!attr.empty()); 
    return atoi(attr.c_str());
};

TBool ChromoNode::AttrBool(TNodeAttr aAttr) const 
{ 
    string attr = Attr(aAttr); 
    __ASSERT(!attr.empty() || attr == "yes" || attr == "no"); 
    return (attr == "yes"); 
};

string ChromoNode::GetName(const string& aTname)
{
    size_t tpos = aTname.find("%");
    return (tpos != string::npos) ? aTname.substr(tpos+1) : aTname;
}

ChromoNode::Iterator ChromoNode::Parent()
{
    void* parent = iMdl.Parent(iHandle);
    return  (parent == NULL) ?  End() : Iterator(ChromoNode(iMdl, parent));
}

ChromoNode::Iterator ChromoNode::Root()
{
    void* root = iMdl.Root(iHandle);
    return  (root == NULL) ?  End() : Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Root() const 
{
    void* root = iMdl.Root(iHandle);
    return  (root == NULL) ?  End() : Const_Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Parent() const
{
    void* parent = iMdl.Parent(iHandle);
    return  (parent == NULL) ?  End() : Const_Iterator(ChromoNode(iMdl, parent));
}

ChromoNode::Iterator ChromoNode::Find(TNodeType aType, const string& aName) 
{ 
    ChromoNode::Iterator res = End();
    size_t pos = aName.find("/");
    string name = aName.substr(0, pos);
    for (ChromoNode::Iterator it = Begin(); it != End(); it++) {
	if (((*it).Type() == aType) && (name.compare((*it).Name()) == 0)) {
	    res = it;  break;
	}
    }
    if ((res != End()) &&  (pos != string::npos)) {
	res = (*res).Find(aType, aName.substr(pos + 1));	
    }
    return res;
};

ChromoNode::Const_Iterator ChromoNode::Find(TNodeType aType, const string& aName) const
{ 
    ChromoNode::Const_Iterator res = End();
    for (ChromoNode::Const_Iterator it = Begin(); it != End(); it++) {
	if (((*it).Type() == aType) && (aName.compare((*it).Name()) == 0)) {
	    res = it;  break;
	}
    }
    return res;
};

ChromoNode::Iterator ChromoNode::Find(TNodeType aType, const string& aName, TNodeAttr aAttr, const string& aAttrVal)
{
    ChromoNode::Iterator res = End();
    for (ChromoNode::Iterator it = Begin(); it != End(); it++) {
	const ChromoNode& node = (*it);
	if ((node.Type() == aType)  && (aName.compare(node.Name()) == 0) && node.AttrExists(aAttr) && (aAttrVal.compare(node.Attr(aAttr)) == 0)) {
	    res = it;  break;
	}
    }
    return res;
};

TInt ChromoNode::GetLocalRank()
{
    TInt res = 0;
    TBool found = false;
    ChromoNode prnt = *Parent();
    for (Iterator it = prnt.Begin(); it != prnt.End() && !found; it++, res++) {
	if (*it == *this) {
	    found = ETrue;
	}
    }
    return res;
}

TInt ChromoNode::GetLocalSize()
{
    TInt res = 0;
    for (Iterator it = Begin(); it != End(); it++, res++);
    return res;
}

TInt ChromoNode::Count() const
{
    TInt res = 0;
    for (Const_Iterator it = Begin(); it != End(); it++, res++);
    return res;
}

ChromoNode ChromoNode::At(TInt aInd) const
{
    ChromoNode res = *End();
    Const_Iterator it = Begin();
    for (TInt ind = 0; it != End() && ind != aInd; it++, ind++);
    if (it != End()) {
	res = *it;
    }
    return res;
}

ChromoNode& ChromoNode::GetNode(const GUri& aUri) const
{
}

void ChromoNode::GetRank(Rank& aRank) const
{
    ChromoNode prnt = *Parent();
    if (prnt.iHandle != NULL) {
	TInt res = 0;
	for (Iterator it = prnt.Begin(); it != prnt.End() && !(*it == *this); it++, res++);
	aRank.insert(aRank.begin(), res);
	prnt.GetRank(aRank);
    }
}

void ChromoNode::GetRank(Rank& aRank, const ChromoNode& aBase) const
{
    ChromoNode prnt = *Parent();
    if (prnt.iHandle != NULL && iHandle != aBase.iHandle) {
	TInt res = 0;
	for (Iterator it = prnt.Begin(); it != prnt.End() && !(*it == *this); it++, res++);
	aRank.insert(aRank.begin(), res);
	prnt.GetRank(aRank, aBase);
    }
}

ChromoNode ChromoNode::AddChild(TNodeType aType) 
{ 
    return ChromoNode(iMdl, iMdl.AddChild(iHandle, aType)); 
}

ChromoNode ChromoNode::AddChild(const ChromoNode& aNode, TBool aCopy) 
{ 
    return ChromoNode(iMdl, iMdl.AddChild(iHandle, aNode.Handle(), aCopy)); 
}

