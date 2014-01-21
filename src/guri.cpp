#include "base.h"
#include "guri.h"

// URI
// Uri conforms to RFC 3986 
// Query syntax: (attr_name '=' attr_value) *( ('&' | '|')  (attr_name '=' attr_value))  

map<string, TNodeType> KNodeTypes;
map<TNodeType, string> KNodeTypesNames;
map<TNodeAttr, string> KNodeAttrsNames;
map<string, TNodeAttr> KNodeAttrs;

const string GUriBase::KTypeAny = "*";
const string GUriBase::KTypeAnywhere = "**";
const string KTypeUnknown = "";

const string KSchemeSep = ":";
const char GUriBase::KBaseSep = '#';
const char GUriBase::KParentSep = ':';
const char GUriBase::KNodeSep = '/';
const char KGroupStart = '(';
const char KGroupEnd = ')';

TBool GUriBase::iInit = EFalse;



GUriBase::GUriBase(const string& aUri): iUri(aUri)
{
    Construct();
}

GUriBase::GUriBase(): iUri()
{
    Construct();
}

// TODO [YB] To move all methods for node types/names to chromo
void GUriBase::Construct()
{
    if (KNodeTypes.size() == 0)
    {
	KNodeTypes["node"] = ENt_Node;
	KNodeTypes["log"] = ENt_Log;
	KNodeTypes["move"] = ENt_Move;
	KNodeTypes["rm"] = ENt_Rm;
	KNodeTypes["change"] = ENt_Change;
	KNodeTypes["cont"] = ENt_Cont;
	KNodeTypes["add"] = ENt_Add;

	for (map<string, TNodeType>::const_iterator it = KNodeTypes.begin(); it != KNodeTypes.end(); it++) {
	    KNodeTypesNames[it->second] = it->first;
	}
    }

    if (KNodeAttrsNames.size() == 0)
    {
	KNodeAttrsNames[ENa_Id] = "id";
	KNodeAttrsNames[ENa_Parent] = "parent";
	KNodeAttrsNames[ENa_MutNode] = "node";
	KNodeAttrsNames[ENa_MutAttr] = "attr";
	KNodeAttrsNames[ENa_MutVal] = "val";

	for (map<TNodeAttr, string>::const_iterator it = KNodeAttrsNames.begin(); it != KNodeAttrsNames.end(); it++) {
	    KNodeAttrs[it->second] = it->first;
	}
    }
    iInit = ETrue;
}

const string& GUriBase::NodeAttrName(TNodeAttr aAttr)
{
    if (!iInit) Construct();
    return KNodeAttrsNames[aAttr];
}

const string& GUriBase::NodeTypeName(TNodeType aType)
{
    if (!iInit) Construct();
    return KNodeTypesNames[aType];
}

TNodeAttr GUriBase::NodeAttr(const string& aAttrName)
{
    if (!iInit) Construct();
    return KNodeAttrs.count(aAttrName) > 0 ? KNodeAttrs[aAttrName] : ENa_Unknown;
}

TNodeType GUriBase::NodeType(const string& aTypeName)
{
    if (!iInit) Construct();
    return KNodeTypes.count(aTypeName) > 0 ? KNodeTypes[aTypeName] : ENt_Unknown;
}

const string& GUriBase::GetLoc() const
{
    TInt size = iElems.size();
    return size == 0 ? KTypeUnknown : iElems.at(size -1).first;
}

const string& GUriBase::Scheme() const
{
    return iScheme;
}

string GUriBase::GetName() const
{
    TInt size = iElems.size();
    return size == 0 ? string() : iElems.at(size -1).second.second;
}

void GUriBase::Append(const GUriBase& aUri)
{
    for (const_elem_iter it = aUri.Elems().begin(); it != aUri.Elems().end(); it++) {
	AppendElem(*it);
    }
}

void GUriBase::AppendElem(const string& aType, const string& aName, char aRelType)
{
    iElems.push_back(TElem(aType, TRel(aRelType, aName)));
}

void GUriBase::AppendElem(const TElem& aElem)
{
    iElems.push_back(aElem);
}

void GUriBase::PrependElem(const string& aType, const string& aName, char aRelType)
{
    iElems.insert(iElems.begin(), TElem(aType, TRel(aRelType, aName)));
}

void GUriBase::AppendQueryElem(TQueryOpr aOpr, TNodeAttr aAttr, const string& aValue)
{
    iQueryElems.push_back(TQueryElem(aOpr, TQueryCnd(aAttr, aValue)));
}

string GUriBase::GetUri(vector<TElem>::const_iterator aStart, TBool aShort) const
{
    return DoGetUri(aStart, aShort);
}

string GUriBase::SelectGroup(const string& aData, int aEndPos)
{
    char tag = KGroupEnd;    
    int count = 0;
    int endpos = aEndPos;
    string tags(KGroupStart, 1); tags.append(KGroupEnd, 1);
    while (endpos != string::npos && tag != KGroupStart) {
	endpos = aData.find_last_of(tags, endpos);
	tag = aData.at(endpos);
	count++;
    }
    while (endpos != string::npos && count-- > 0) {
	endpos = aData.find_last_of(KGroupStart, endpos);
    }
    return aData.substr(endpos, aEndPos);
}

GUriBase::TElem GUriBase::Elem(char aRelType, const string& aName, const string& aExt)
{
    return TElem(aExt, GUri::TRel(aRelType, aName));
}



GUri::GUri(const string& aUri): GUriBase(aUri)
{
    Parse();
}

GUri::GUri(): GUriBase()
{
}


void GUri::Parse()
{
    TBool fin = EFalse;
    string frag;
    size_t base_end = iUri.find_first_of('#', 0);
    if (base_end != string::npos) {
	// Base part is presented
	iBase = iUri.substr(0, base_end);
	frag = iUri.substr(0, base_end + 1);
	size_t scheme_end = iUri.find_first_of(':', 0);
	if (scheme_end != string::npos) {
	    iScheme = iUri.substr(0, scheme_end);
	}
    }
    else {
	// Base part is missing
	frag = iUri;
    }
    size_t query_beg = frag.find_first_of('?', 0);
    string hier = frag.substr(0, query_beg);
    size_t elem_beg = 0;
    // Path
    size_t elem_end = hier.find_first_of('/', 0);
    string elem = hier.substr(0, elem_end);
    while (!(elem.empty() && elem_end == string::npos)) {
	size_t type_end = elem.find_first_of(':');
	size_t name_beg = 0;
	string type;
	if (type_end == string::npos) {
	    type = KTypeAny;
	}
	else {
	    type = elem.substr(0, type_end);
	    name_beg = type_end + 1;
	}
	string name = elem.substr(name_beg);
	AppendElem(type, name);
	if (elem_end == string::npos) {
	    elem.clear();
	} 
	else {
	    elem_beg = elem_end + 1;
	    elem_end = hier.find_first_of('/', elem_beg);
	    elem = hier.substr(elem_beg, elem_end == string::npos ? elem_end : elem_end - elem_beg);
	}
    }
    // Query
    if (query_beg != string::npos) {
	// Just one condition for now
	string query = frag.substr(query_beg+1);
	size_t cond_beg = 0;
	size_t cond_end = query.find_first_of("&", cond_beg);
	string cond = query.substr(cond_beg, cond_end);
	TQueryOpr op = EQop_Unknown;
	while (cond_beg != string::npos && !cond.empty()) {
	    size_t attr_beg = 0;
	    size_t attr_end = cond.find_first_of('=', 0);
	    size_t val_beg = attr_end + 1;
	    size_t val_end = string::npos;
	    string attrs = cond.substr(attr_beg, attr_end);
	    string val = cond.substr(val_beg, val_end);
	    TNodeAttr attr = KNodeAttrs.count(attrs) > 0 ? KNodeAttrs[attrs] : ENa_Unknown;
	    AppendQueryElem(op, attr, val);
	    char ops = cond_end == string::npos ? ' ' : query.at(cond_end);
	    op = EQop_Unknown;
	    if (ops == '&') 
		op = EQop_And;
	    cond_beg = cond_end == string::npos ? string::npos : cond_end + 1;
	    if (cond_beg != string::npos) {
		cond_end = query.find_first_of("&", cond_beg);
		cond = query.substr(cond_beg, cond_end);
	    }
	}
    }
}

string GUri::DoGetUri(vector<TElem>::const_iterator aStart, TBool aShort) const
{
    string res;
    // Hier
    for (vector<GUri::TElem>::const_iterator it = aStart; it != iElems.end(); it++) {
	GUri::TElem elem = *it;
	if (!aShort) {
	    if (!elem.first.empty() && elem.first != KTypeAny) {
		res.append(elem.first + ":");
	    }
	}
	res.append(elem.second.second);
	if (it + 1 != iElems.end()) {
	    res.append("/");
	}
    }
    // Query
    if (!iQueryElems.empty()) {
	res.append("?");
	for (vector<TQueryElem>::const_iterator it = iQueryElems.begin(); it != iQueryElems.end(); it++) {
	    TQueryOpr opr = it->first;
	    if (opr ==  EQop_And) {
		res.append("&");
	    }
	    res.append(KNodeAttrsNames[it->second.first]);
	    res.append("=");
	    res.append(it->second.second);
	}
    }
    return res;
}

GUri GUri::operator+(const GUri& aUri)
{
    GUri res(*this);
    res.Append(aUri);
    return res;
}

GUri& GUri::operator+=(const GUri& aUri)
{
    Append(aUri);
    return *this;
}

/*
void GUri::ToString(string& aRes)
{
    if (!iScheme.empty()) {
	aRes.append(iScheme + KSchemeSep);
    }
    if (!iBase.empty()) {
	aRes.append(iBase + KBaseSep);
    }
    for (vector<GUri::TElem>::const_iterator it = iElems.begin(); it != iElems.end(); it++) {
	if (!it->second.empty()) {
	    aRes.append(it->first + KParentSep + it->second);
	}
	if (it + 1 != iElems.end()) {
	    aRes.append(KElemSep);
	}
    }
}
*/


#if 0

IUri::IUri(const string& aUri): GUriBase(aUri)
{
    Parse();
}

IUri::IUri(): GUriBase()
{
}


void IUri::Parse()
{
    TBool fin = EFalse;
    size_t elem_beg = 0;
    size_t elem_end = string::npos;
    string frag = iUri;
    string seps;
    seps.append(1, KNodeSep);
    seps.append(1, KParentSep);
    while (!fin) {
	size_t name_sep_pos = frag.find_last_of(seps);
	if (name_sep_pos == string::npos) {
	    // No name sep, just simple name, setup elem and finish
	    string loc = "";
	    string name = frag;
	    iElems.push_back(TElem(loc, name));
	    iElems.push_back(TElem(loc, KTypeAny));
	    fin = ETrue;
	}
	else if (frag.at(name_sep_pos) == KNodeSep) {
	    // No inheritance chain, fill out uri elemaent and finish
	    string loc = frag.substr(elem_beg, name_sep_pos);
	    string name = frag.substr(name_sep_pos + 1, elem_end);
	    iElems.push_back(TElem(loc, name));
	    fin = ETrue;
	}
	else if (frag.at(name_sep_pos) == KParentSep) {
	    // inheritance chain presenting, parse it
	    if (name_sep_pos == 0) {
		// Empty inheritance, empty loc - add elem 
		string loc = "";
		string name = frag;
		iElems.push_back(TElem(loc, name));
		fin = ETrue;
	    }
	    else if (frag.at(name_sep_pos - 1) == KNodeSep) {
		// Empty inheritance but some loc - add elem
		string loc = frag.substr(elem_beg, name_sep_pos - 1);
		string name = frag;
		iElems.push_back(TElem(loc, name));
		fin = ETrue;
	    }
	    else if (frag.at(name_sep_pos - 1) == KGroupEnd) {
		// Grouping in inheritance chain
		frag = SelectGroup(frag, name_sep_pos - 1);
	    }
	}
    }
}

string IUri::DoGetUri(vector<TElem>::const_iterator aStart, TBool aShort) const
{
    string res;
    // Hier
    for (vector<GUri::TElem>::const_iterator it = aStart; it != iElems.end(); it++) {
	GUri::TElem elem = *it;
	if (!aShort) {
	    if (!elem.first.empty() && elem.first != KTypeAny) {
		res.append(elem.first + KNodeSep);
	    }
	}
	res.append(elem.second);
	if (it + 1 != iElems.end()) {
	    res.append(1, KParentSep);
	}
    }
    return res;
}

#endif

