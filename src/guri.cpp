#include "base.h"
#include "guri.h"

// URI
// Uri conforms to RFC 3986 
// Query syntax: (attr_name '=' attr_value) *( ('&' | '|')  (attr_name '=' attr_value))  

map<string, TNodeType> KNodeTypes;
map<TNodeType, string> KNodeTypesNames;
map<TNodeAttr, string> KNodeAttrsNames;
map<string, TNodeAttr> KNodeAttrs;

TBool GUri::iInit = EFalse;

const string& GUri::NodeAttrName(TNodeAttr aAttr)
{
    if (!iInit) Construct();
    return KNodeAttrsNames[aAttr];
}

const string& GUri::NodeTypeName(TNodeType aType)
{
    if (!iInit) Construct();
    return KNodeTypesNames[aType];
}

TNodeAttr GUri::NodeAttr(const string& aAttrName)
{
    if (!iInit) Construct();
    return KNodeAttrs.count(aAttrName) > 0 ? KNodeAttrs[aAttrName] : ENa_Unknown;
}

TNodeType GUri::NodeType(const string& aTypeName)
{
    if (!iInit) Construct();
    return KNodeTypes.count(aTypeName) > 0 ? KNodeTypes[aTypeName] : ENt_Unknown;
}

GUri::GUri(const string& aUri): iUri(aUri)
{
    Construct();
    Parse();
}

GUri::GUri(): iUri()
{
    Construct();
}

void GUri::Construct()
{
    if (KNodeTypes.size() == 0)
    {
	KNodeTypes["graph"] = ENt_Graph;
	KNodeTypes["vert"] = ENt_Vert;
	KNodeTypes["edge"] = ENt_Edge;
	KNodeTypes["log"] = ENt_Log;
	KNodeTypes["add"] = ENt_Add;
	KNodeTypes["move"] = ENt_Move;
	KNodeTypes["rm"] = ENt_Rm;
	KNodeTypes["change"] = ENt_Change;

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

void GUri::Parse()
{
    TBool fin = EFalse;
    size_t query_beg = iUri.find_first_of('?', 0);
    string hier = iUri.substr(0, query_beg);
    size_t elem_beg = 0;
    size_t elem_end = hier.find_first_of('/', 0);
    string elem = hier.substr(0, elem_end);
    // Hier
    while (!elem.empty()) {
	size_t type_end = elem.find_first_of(':');
	size_t name_beg = 0;
	TNodeType type;
	if (type_end == string::npos) {
	    type = ENt_Graph;
	}
	else {
	    string tname = elem.substr(0, type_end);
	    type = KNodeTypes[tname];
	    name_beg = type_end + 1;
	}
	string name = elem.substr(name_beg);
	iElems.push_back(TElem(type, name));
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
	string query = iUri.substr(query_beg+1);
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

string GUri::GetUri(vector<TElem>::const_iterator aStart) const
{
    string res;
    // Hier
    for (vector<GUri::TElem>::const_iterator it = aStart; it != iElems.end(); it++) {
	GUri::TElem elem = *it;
	if (elem.first != ENt_Vert) {
	    res.append(KNodeTypesNames[elem.first] + ":");
	}
	res.append(elem.second);
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

TNodeType GUri::GetType() const
{
    TInt size = iElems.size();
    return size == 0 ? ENt_Unknown : iElems.at(size -1).first;
}

string GUri::GetName() const
{
    TInt size = iElems.size();
    return size == 0 ? string() : iElems.at(size -1).second;
}

void GUri::AppendElem(TNodeType aType, const string& aName)
{
    iElems.push_back(TElem(aType, aName));
}

void GUri::PrependElem(TNodeType aType, const string& aName)
{
    iElems.insert(iElems.begin(), TElem(aType, aName));
}

void GUri::AppendQueryElem(TQueryOpr aOpr, TNodeAttr aAttr, const string& aValue)
{
    iQueryElems.push_back(TQueryElem(aOpr, TQueryCnd(aAttr, aValue)));
}


