#include "chromo.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept> 


using namespace std;

const string TMut::KCtrls = string(TMut::KSep, TMut::KAttrSep);

map<TNodeAttr, string> KNodeAttrsNames_Init()
{
    map<TNodeAttr, string> res;
    res[ENa_Id] = "id";
    res[ENa_Parent] = "parent";
    res[ENa_Ref] = "ref";
    res[ENa_MutNode] = "node";
    res[ENa_MutAttr] = "attr";
    res[ENa_MutVal] = "val";
    res[ENa_Order] = "ord";
    res[ENa_TOrder] = "tord";
    res[ENa_Inactive] = "na";
    res[ENa_Targ] = "targ";
    res[ENa_Comp] = "comp";
    res[ENa_NS] = "ns";
    return res;
}

map<TNodeType, string> KNodeTypesNames_Init()
{
    map<TNodeType, string> res;
    res[ENt_Node] = "node";
    res[ENt_Move] = "move";
    res[ENt_Rm] = "rm";
    res[ENt_Change] = "change";
    res[ENt_Cont] = "cont";
    res[ENt_Import] = "import";
    res[ENt_Seg] = "seg";
    return res;
}

map<string, TNodeType> TMut::KNodeTypes_Init()
{
    map<string, TNodeType> res;
    for (map<TNodeType, string>::const_iterator it = KNodeTypesNames.begin(); it != KNodeTypesNames.end(); it++) {
	res[it->second] = it->first;
    }
    return res;
}

map<string, TNodeAttr> TMut::KNodeAttrs_Init()
{
    map<string, TNodeAttr> res;
    for (map<TNodeAttr, string>::const_iterator it = KNodeAttrsNames.begin(); it != KNodeAttrsNames.end(); it++) {
	res[it->second] = it->first;
    }
    return res;
}

map<TNodeType, string> TMut::KNodeTypesNames = KNodeTypesNames_Init();
map<TNodeAttr, string> TMut::KNodeAttrsNames = KNodeAttrsNames_Init();
map<string, TNodeType> TMut::KNodeTypes = TMut::KNodeTypes_Init();
map<string, TNodeAttr> TMut::KNodeAttrs = TMut::KNodeAttrs_Init();


const string& TMut::NodeAttrName(TNodeAttr aAttr)
{
    return KNodeAttrsNames[aAttr];
}

const string& TMut::NodeTypeName(TNodeType aType)
{
    return KNodeTypesNames[aType];
}

TNodeAttr TMut::NodeAttr(const string& aAttrName)
{
    return KNodeAttrs.count(aAttrName) > 0 ? KNodeAttrs[aAttrName] : ENa_Unknown;
}

TNodeType TMut::NodeType(const string& aTypeName)
{
    return KNodeTypes.count(aTypeName) > 0 ? KNodeTypes[aTypeName] : ENt_Unknown;
}

TMut::TMut(): mType(ENt_Unknown)
{
}

TMut::TMut(TNodeType aType): mType(aType)
{
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val):
    mType(aType)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val):
    mType(aType)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
    mAttrs.insert(TElem(aAttr1, aAttr1Val));
}

TMut::TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val,
	TNodeAttr aAttr2, const string& aAttr2Val): mType(aType)
{
    mAttrs.insert(TElem(aAttr0, aAttr0Val));
    mAttrs.insert(TElem(aAttr1, aAttr1Val));
    mAttrs.insert(TElem(aAttr2, aAttr2Val));
}

TMut::TMut(const ChromoNode& aCnode): mType(aCnode.Type())
{
    for (map<TNodeAttr, string>::const_iterator it = KNodeAttrsNames.begin(); it != KNodeAttrsNames.end(); it++) {
	TNodeAttr attr = it->first;
	if (aCnode.AttrExists(attr)) {
	    SetAttr(attr, aCnode.Attr(attr));
	}
    }
}

TBool TMut::AttrExists(TNodeAttr aId) const
{
    return mAttrs.count(aId) > 0;
}

string TMut::Attr(TNodeAttr aId) const
{
    return mAttrs.count(aId) > 0 ? mAttrs.at(aId) : string();
}

void TMut::SetAttr(TNodeAttr aAttr, const string& aAttrVal)
{
    mAttrs[aAttr] = aAttrVal;
}

void TMut::RmAttr(TNodeAttr aAttr)
{
    mAttrs.erase(aAttr);
}

TMut::TMut(const string& aSpec)
{
    size_t type_beg = 0, type_end = 0;
    type_end = aSpec.find_first_of(KSep, type_beg); 
    string types = aSpec.substr(type_beg, (type_end == string::npos) ? string::npos : type_end - type_beg);
    mType = NodeType(types);
    if (mType == ENt_Unknown) {
	throw (runtime_error("Incorrect TMut type: " + types));
    }
    if (type_end != string::npos) {
	size_t attr_end = type_end;
	size_t attr_beg;
	do {
	    attr_beg = attr_end + 1;
	    size_t attr_mid = attr_beg;
	    // Find first non-escaped separator
	    do {
		attr_end = aSpec.find_first_of(KSep, attr_mid); 
		attr_mid = attr_end + 1;
	    } while (attr_end != string::npos && aSpec.at(attr_end - 1) == KEsc);
	    string attr = aSpec.substr(attr_beg, (attr_end == string::npos) ? string::npos : attr_end - attr_beg);
	    // Parse arg
		size_t attrtype_end = attr.find_first_of(KAttrSep); 
		if (attrtype_end != string::npos) {
		    string attrtype = attr.substr(0, attrtype_end);
		    TNodeAttr atype = NodeAttr(attrtype);
		    if (atype == ENa_Unknown) {
			throw (runtime_error("Incorrect TMut attr type: " + atype));
		    }
		    size_t attrval_beg = attrtype_end + 1;
		    string attrval = attr.substr(attrval_beg, string::npos);
		    SetAttr(atype, DeEscCtrls(attrval));
		} else {
		    throw (runtime_error("Incorrect TMut attr: " + attr));
		}
	} while (attr_end != string::npos);
    }
}

TMut::operator string() const
{
    string res(NodeTypeName(mType));
    for (TAttrs::const_iterator it = mAttrs.begin(); it != mAttrs.end(); it++) {
	res += KSep + NodeAttrName(it->first) + KAttrSep + EscapeCtrls(it->second);
    }
    return res;
}

/*
const TMut::TElem& TMut::AttrAt(TInt aInd) const
{
    __ASSERT(aInd < mAttrs.size());
    return mAttrs.at(aInd);
}
*/

string TMut::EscapeCtrls(const string& aInp)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	if (cc == KSep || cc == KAttrSep || cc == KEsc) {
	    res.push_back(KEsc);
	}
	res.push_back(cc);
    }
    return res;
}

string TMut::DeEscCtrls(const string& aInp)
{
    string res;
    for (string::const_iterator it = aInp.begin(); it != aInp.end(); it++) {
	const char cc = *it;
	string::const_iterator itn = it + 1;
	if (itn != aInp.end()) {
	    const char cn = *itn;
	    if (cc == KEsc && IsCtrlSymb(cn)) {
		res.push_back(cn);
		it++;
		continue;
	    }
	}
	res.push_back(cc);
    }
    return res;
}


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


// Chromo utilities

void ChromoUtils::GetPath(const string& aUri, string& aPath)
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

void ChromoUtils::GetFrag(const string& aUri, string& aFrag)
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
    __ASSERT(&iMdl == &aNode.iMdl);
    iHandle = aNode.iHandle;
    return *this;
}

TBool ChromoNode::operator==(const ChromoNode& aNode)
{
    return iHandle == aNode.iHandle;
}

TBool ChromoNode::operator==(const ChromoNode& aNode) const
{ 
    return iHandle == aNode.iHandle;
}

const string ChromoNode::Attr(TNodeAttr aAttr)
{
    return  iMdl.GetAttr(iHandle, aAttr); 
    /*
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
    */
};

const string ChromoNode::Attr(TNodeAttr aAttr) const
{
    return iMdl.GetAttr(iHandle, aAttr); 
    /*
    string res; 
    if (sattr != NULL)
	res.assign(sattr);
    free(sattr); 
    return res; 
    */
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
    THandle parent = iMdl.Parent(iHandle);
    return  (parent == THandle()) ?  End() : Iterator(ChromoNode(iMdl, parent));
}

ChromoNode::Iterator ChromoNode::Root()
{
    THandle root = iMdl.Root(iHandle);
    return  (root == THandle()) ?  End() : Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Root() const 
{
    THandle root = iMdl.Root(iHandle);
    return  (root == THandle()) ?  End() : Const_Iterator(ChromoNode(iMdl, root));
}

ChromoNode::Const_Iterator ChromoNode::Parent() const
{
    THandle parent = iMdl.Parent(iHandle);
    return  (parent == THandle()) ?  End() : Const_Iterator(ChromoNode(iMdl, parent));
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

ChromoNode::Iterator ChromoNode::FindNodeInMhUri(const GUri& aMhUri, const GUri::const_elem_iter& aMhUriPos, GUri::const_elem_iter& aResPos)
{
    ChromoNode::Iterator res = End();
    for (ChromoNode::Iterator it = Begin(); it != End(); it++) {
	ChromoNode node = *it;
	if (node.Type() == ENt_Node) {
	    GUri nuri;
	    if (node.AttrExists(ENa_MutNode)) {
		GUri duri(node.Attr(ENa_MutNode));
		nuri.Append(duri);
	    }
	    nuri.AppendElem(string(), node.Name());
	    GUri::const_elem_iter rpos;
	    TBool matched = aMhUri.Compare(aMhUriPos, nuri, rpos);
	    if (matched) {
		aResPos = rpos;
		return it;
	    }
	}
    }
    return res;

}

TInt ChromoNode::GetLocalSize()
{
    TInt res = 0;
    //for (Iterator it = Begin(); it != End(); it++, res++);
    Iterator it = Begin();
    while (it != End()) {
	it++;
	res++;
    }
    return res;
}

TInt ChromoNode::GetLocalSize(TNodeType aType) const
{
    TInt res = 0;
    for (Const_Iterator it = Begin(); it != End(); it++) {
	if ((*it).Type() == aType) {
	    res++;
	}
    }
    return res;
}

TInt ChromoNode::Count() const
{
    TInt res = 0;
    for (Const_Iterator it = Begin(); it != End(); it++, res++);
    return res;
}

ChromoNode ChromoNode::GetNodeByMhUri(const GUri& aUri)
{
    if (aUri.IsErr()) return ChromoNode(iMdl, THandle());
    GUri::const_elem_iter it = aUri.Elems().begin();
    if (it != aUri.Elems().end()) {
	if (it->name().empty()) {
	    Iterator root = End();
	    if (it->rel() == GUri::EComp) {
		root = Root();
	    } else {
		__ASSERT(false);
	    }
	    it++;
	    GUri::TElem elem = *it;
	    if ((*root).Name() != elem.name()) {
		root = End();
	    }
	    if (root != End() && ++it != aUri.Elems().end()) {
		return (*root).GetNodeByMhUri(aUri, it);
	    } else {
		return *root;
	    }
	} else {
	    return GetNodeByMhUri(aUri, it);
	}
    } else {
	return *this;
    }
}

ChromoNode ChromoNode::GetNodeByMhUri(const GUri& aUri, GUri::const_elem_iter& aPathBase)
{
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    if (elem.name() == GUri::KUpperLevel) {
	// Handle nodes distination path
	// Get number of hops
	TInt hops = 0;
	if (AttrExists(ENa_MutNode)) {
	    string destpath = Attr(ENa_MutNode);
	    GUri dp(destpath);
	    hops = dp.Elems().size();
	}
	Iterator it = Parent();
	TBool hopsok = ETrue;
	for (TInt ct = 0; ct < hops; ct++) {
	    uripos++;
	    if (uripos->name() != GUri::KUpperLevel) {
		hopsok = EFalse;
		break;
	    }
	}
	if (!hopsok) return *(End());
	if (it != End()) {
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		ChromoNode node = *it;
		return node.GetNodeByMhUri(aUri, uripos);
	    } else {
		return *it;
	    }
	} else {
	    return *it;
	}
    } else {
	GUri::const_elem_iter path_res;
	Iterator it = FindNodeInMhUri(aUri, aPathBase, path_res);
	if (it != End()) {
	    uripos = path_res;
	    if (uripos != aUri.Elems().end()) {
		ChromoNode node = *it;
		return node.GetNodeByMhUri(aUri, uripos);
	    } else {
		return *it;
	    }
	} else {
	    return *it;
	}
    }
}

ChromoNode ChromoNode::GetNode(const GUri& aUri)
{
    if (aUri.IsErr()) return ChromoNode(iMdl, THandle());
    GUri::const_elem_iter it = aUri.Elems().begin();
    if (it != aUri.Elems().end()) {
	if (it->name().empty()) {
	    Iterator root = End();
	    if (it->rel() == GUri::EComp) {
		root = Root();
	    } else {
		__ASSERT(false);
	    }
	    it++;
	    GUri::TElem elem = *it;
	    if ((*root).Name() != elem.name()) {
		root = End();
	    }
	    if (root != End() && ++it != aUri.Elems().end()) {
		return (*root).GetNode(aUri, it);
	    } else {
		return *root;
	    }
	} else {
	    return GetNode(aUri, it);
	}
    } else {
	return *this;
    }
}

ChromoNode ChromoNode::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase)
{
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    if (elem.name() == GUri::KUpperLevel) {
	// Get number of hops
	TInt hops = 0;
	if (AttrExists(ENa_MutNode)) {
	    string destpath = Attr(ENa_MutNode);
	    GUri dp(destpath);
	    hops = dp.Elems().size();
	}
	Iterator it = Parent();
	TBool hopsok = ETrue;
	for (TInt ct = 0; ct < hops; ct++) {
	    uripos++;
	    if (uripos->name() != GUri::KUpperLevel) {
		hopsok = EFalse;
		break;
	    }
	}
	if (!hopsok) return *(End());
	if (it != End()) {
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		ChromoNode node = *it;
		return node.GetNode(aUri, uripos);
	    } else {
		return *it;
	    }
	} else {
	    return *it;
	}
    } else {
	Iterator it = Find(ENt_Node, elem.name());
	if (it != End()) {
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		ChromoNode node = *it;
		return node.GetNode(aUri, uripos);
	    } else {
		return *it;
	    }
	} else {
	    return *it;
	}
    }
}

ChromoNode ChromoNode::AddChild(TNodeType aType) 
{ 
    return ChromoNode(iMdl, iMdl.AddChild(iHandle, aType)); 
}

ChromoNode ChromoNode::AddChild(const TMut& aMut)
{
    ChromoNode mut(iMdl, iMdl.AddChild(iHandle, aMut.Type()));
    for (TMut::TAttrs::const_iterator it = aMut.Attrs().begin(); it != aMut.Attrs().end(); it++) {
	mut.SetAttr(it->first, it->second);
    }
    return mut;
}

ChromoNode ChromoNode::AddChild(const ChromoNode& aNode, TBool aCopy, TBool aRecursively) 
{ 
    return ChromoNode(iMdl, iMdl.AddChild(iHandle, aNode.Handle(), aCopy, aRecursively)); 
}

ChromoNode::Iterator ChromoNode::GetChildOwning(const ChromoNode& aNode) const
{
    ChromoNode res(aNode);
    ChromoNode prnt = *res.Parent();
    while (prnt != *this && prnt != ChromoNode(iMdl, THandle())) {
	res = prnt;
    }
    return Iterator(res);
}

void ChromoNode::ReduceToSelection(const ChromoNode& aSelNode)
{
    __ASSERT(*aSelNode.Parent() == *this);
    Iterator it = Begin();
    while (it != End() && !(*it == aSelNode)) {
	ChromoNode node = *it;
	if (node.Type() == ENt_Import) {
	    it++;
	} else {
	    RmChild(node);
	    it = Begin();
	}
    }
    Reverse_Iterator itr = Rbegin();
    while (itr != Rend() && !(*itr == aSelNode)) {
	RmChild(*itr);
	itr = Rbegin();
    }
}

void ChromoNode::GetUri(GUri& aUri) const
{
    Const_Iterator parent = Parent();
    if (parent != End()) {
	aUri.PrependElem(string(), Name());
	(*parent).GetUri(aUri);
    }
    else {
	aUri.PrependElem(string(), Name());
	aUri.PrependElem("", "");
    }
}

void ChromoNode::GetUri(GUri& aUri, const ChromoNode& aBase) const
{
    Const_Iterator parent = Parent();
    if (parent != End()) {
	if (parent == aBase) {
	    return;
	}
	aUri.PrependElem(string(), Name());
	(*parent).GetUri(aUri);
    }
    else {
	aUri.PrependElem(string(), Name());
	aUri.PrependElem("", "");
    }
}

void ChromoNode::Activate()
{ 
    if (AttrExists(ENa_Inactive)) {
	RmAttr(ENa_Inactive);
    }
    if (Type() == ENt_Node) {
	Iterator it = Begin();
	while (it != End()) {
	    (*it).Activate();
	    it++;
	}
    }
};

ChromoNode::operator string() const
{
    string res;
    ToString(res);
    return res;
}

void ChromoNode::Dump() const { iMdl.Dump(iHandle);}

void ChromoNode::DumpBackTree() const { iMdl.DumpBackTree(iHandle);}
