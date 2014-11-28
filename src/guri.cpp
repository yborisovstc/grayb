#include "base.h"
#include "guri.h"


// URI
// Uri conforms to RFC 3986 
// Query syntax: (attr_name '=' attr_value) *( ('&' | '|')  (attr_name '=' attr_value))  

map<string, TNodeType> KNodeTypes;
map<TNodeType, string> KNodeTypesNames;
map<TNodeAttr, string> GUriBase::KNodeAttrsNames;
map<string, TNodeAttr> KNodeAttrs;

const string GUriBase::KTypeAny = "*";
const string GUriBase::KTypeAnywhere = "**";
const string KTypeUnknown = "";
const string GUriBase::KUpperLevel = "..";

const string KSchemeSep = ":";
const char GUriBase::KBaseSep = '#';
const char GUriBase::KParentSep = ':';
const char GUriBase::KSepNone = ' ';
const char GUriBase::KNodeSep = '/';
const char KGroupStart = '(';
const char KGroupEnd = ')';
const char KCurUnit = '.';
const string KGroupMark = "()";
const string KRelTypesMarks = "/:";

TBool GUriBase::iInit = EFalse;



GUriBase::GUriBase(const string& aUri): iUri(aUri), iErr(EFalse)
{
    Construct();
}

GUriBase::GUriBase(): iUri(), iErr(EFalse)
{
    Construct();
}

// TODO [YB] To move all methods for node types/names to chromo
void GUriBase::Construct()
{
    if (KNodeTypes.size() == 0)
    {
	KNodeTypes["node"] = ENt_Node;
	KNodeTypes["move"] = ENt_Move;
	KNodeTypes["rm"] = ENt_Rm;
	KNodeTypes["change"] = ENt_Change;
	KNodeTypes["cont"] = ENt_Cont;
	KNodeTypes["add"] = ENt_Add;
	KNodeTypes["order"] = ENt_Order;

	for (map<string, TNodeType>::const_iterator it = KNodeTypes.begin(); it != KNodeTypes.end(); it++) {
	    KNodeTypesNames[it->second] = it->first;
	}
    }

    if (KNodeAttrsNames.size() == 0)
    {
	KNodeAttrsNames[ENa_Id] = "id";
	KNodeAttrsNames[ENa_Parent] = "parent";
	KNodeAttrsNames[ENa_Ref] = "ref";
	KNodeAttrsNames[ENa_MutNode] = "node";
	KNodeAttrsNames[ENa_MutAttr] = "attr";
	KNodeAttrsNames[ENa_MutVal] = "val";
	KNodeAttrsNames[ENa_Order] = "ord";
	KNodeAttrsNames[ENa_TOrder] = "tord";
	KNodeAttrsNames[ENa_Pos] = "pos";

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

void GUriBase::AppendElem(const string& aExt, const char aExtRel, const string& aName, char aRelType)
{
    AppendElem(aExt + string(1, aExtRel), aName, aRelType);

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

char GUriBase::GetExtFirstPart(const string& aExt, string& aPart)
{
    aPart.append(GUri::KTypeAny);
    char rel = GUri::KParentSep;
    if (!aExt.empty()) {
	char srel = aExt.at(aExt.size() - 1);
	if (srel == GUri::KParentSep || srel == GUri::KNodeSep) {
	    rel = srel;
	    aPart = aExt.substr(0, aExt.size() - 1);
	}
    }
    return rel;
}



GUri::GUri(const string& aUri): GUriBase(aUri)
{
    Parse();
}

GUri::GUri(): GUriBase()
{
}

// Finds the end of group. Nested group are taken into account
size_t GUriBase::FindGroup(const string& aStr, size_t aPos) 
{
    size_t res = string::npos;
    size_t pos = aStr.find_first_of(KGroupStart, aPos); 
    TInt count = 1;
    if (pos == aPos) {
	pos += 1;
	while (count > 0) {
	    pos = aStr.find_first_of(KGroupMark, pos); 
	    if (pos == string::npos) {
		res = aStr.size() + 1;
		break;
	    }
	    else {
		count += (aStr.at(pos) == KGroupStart) ? 1 : -1;
		res = pos;
	    }
	}
    }
    return res;
}

void GUri::Parse()
{
    TBool fin = EFalse;
    TBool err = EFalse;
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
    // Path
    // Check the relation symbol, if omitted that assume it's hier
    if (!hier.empty()) {
	size_t pos = 0;
	char rtype = KSepNone;
	char rs = hier.at(pos);
	TBool curunit = EFalse;
	if (rs == KCurUnit) {
	    curunit = ETrue;
	}
	if (hier.size() > 1) {
	    char rs1 = hier.at(pos + 1);
	    if (rs == KCurUnit && rs1 != KCurUnit) {
		curunit = ETrue;
		pos += 1;
	    }
	    rs = hier.at(pos);
	    size_t rs_pos = KRelTypesMarks.find_first_of(rs, 0);
	    if (rs_pos != string::npos) {
		rtype = rs;
		pos += 1;
	    }
	    else if (!curunit) {
		curunit = ETrue;
	    }
	    if (!curunit) {
		// Root
		AppendElem("", "", rtype);
	    }
	    do {
		// Selecting next unit
		string ext;
		size_t ext_end = FindGroup(hier, pos);
		size_t name_beg = 0;
		if (ext_end != string::npos) {
		    if (ext_end >= hier.size()) {
			err = ETrue;
		    }
		    else {
			ext = hier.substr(pos + 1, ext_end - pos - 1);
			pos = ext_end + 1;
		    }
		}
		if (!err) {
		    size_t name_end = hier.find_first_of(KRelTypesMarks, pos);
		    string name = hier.substr(pos, name_end - pos);
		    if (name == KUpperLevel && rtype == KSepNone) {
			err = ETrue; break;
		    }
		    AppendElem(ext, name, rtype);
		    pos = name_end;
		    if (pos != string::npos) {
			rtype = hier.at(pos);
			pos += 1;
		    }
		}
	    } while (!err && pos != string::npos && pos < hier.size());
	}
	if (!iErr && err) {
	    iErr = err;
	}
    }
}

string GUri::DoGetUri(vector<TElem>::const_iterator aStart, TBool aShort) const
{
    string res;
    // Hier
    for (vector<GUri::TElem>::const_iterator it = aStart; it != iElems.end(); it++) {
	GUri::TElem elem = *it;
	if (it == aStart && (!elem.first.empty() || !elem.second.second.empty()) && elem.second.first != KSepNone) {
	    // Current node
	    res.append(1, KCurUnit);
	    res.append(1, elem.second.first);
	}
	if (!aShort) {
	    if (!elem.first.empty() && elem.first != KTypeAny) {
		res.append(1, KGroupStart);
		res.append(elem.first);
		res.append(1, KGroupEnd);
	    }
	}
	res.append(elem.second.second);
	if (it + 1 != iElems.end()) {
	    res.append(1, elem.second.first);
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


