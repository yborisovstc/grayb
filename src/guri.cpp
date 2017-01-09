#include "base.h"
#include "guri.h"
#include "chromo.h"


// URI
// Uri conforms to RFC 3986 
// Query syntax: (attr_name '=' attr_value) *( ('&' | '|')  (attr_name '=' attr_value))  

const string GUriBase::KTypeAny = "*";
const string GUriBase::KTypeAnywhere = "**";
const string KTypeUnknown = "";
const string GUriBase::KUpperLevel = "..";

const char GUriBase:: KSchemeSep = ':';
const char GUriBase::KPathDelim = '/';
const char GUriBase::KQueryDelim = '?';
const char GUriBase::KBaseSep = '#';
const char GUriBase::KIfaceSep = '%';
const string GUriBase::KIfaceSepS = "%";
const char GUriBase::KParentSep = ':';
const char GUriBase::KSepNone = ' ';
const char GUriBase::KNodeSep = '/';
const char KGroupStart = '(';
const char KGroupEnd = ')';
const char GUriBase::KCurUnit = '.';
const string KGroupMark = "()";
const string KRelTypesMarks = "/:";

TBool GUriBase::iInit = EFalse;



GUriBase::GUriBase(const string& aUri): iUri(aUri), iErr(EFalse)
{
}

GUriBase::GUriBase(): iUri(), iErr(EFalse)
{
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

const string& GUriBase::Authority() const
{
    return iAuthority;
}

const string& GUriBase::Path() const
{
    return iPath;
}

const string& GUriBase::GetBase() const
{
    return iBase;
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

void GUriBase::Prepend(const GUriBase& aUri)
{
    for (vector<TElem>::const_reverse_iterator it = aUri.Elems().rbegin(); it != aUri.Elems().rend(); it++) {
	PrependElem(*it);
    }
}

void GUriBase::AppendTail(const GUriBase& aUri, const_elem_iter aIter)
{
    for (const_elem_iter it = aIter; it != aUri.Elems().end(); it++) {
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

void GUriBase::PrependElem(const TElem& aElem)
{
    iElems.insert(iElems.begin(), aElem);
}

void GUriBase::PrependElem(const string& aType, const string& aName, char aRelType)
{
    iElems.insert(iElems.begin(), TElem(aType, TRel(aRelType, aName)));
}

void GUriBase::AppendQueryElem(TQueryOpr aOpr, TNodeAttr aAttr, const string& aValue)
{
    iQueryElems.push_back(TQueryElem(aOpr, TQueryCnd(aAttr, aValue)));
}

string GUriBase::GetUri(const_elem_iter aStart, TBool aShort) const
{
    return DoGetUri(aStart, iElems.end(), aShort);
}

string GUriBase::GetUriBody(const_elem_iter aEnd, TBool aShort) const
{
    return DoGetUri(iElems.begin(), aEnd, aShort);
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

TBool GUriBase::IsAbsolute() const
{
    TBool res = EFalse;
    if (iElems.size() > 0) {
	const_elem_iter it = iElems.begin();
	TElem elem = *it;
	res = elem.first.empty() && elem.second.second.empty();
    }
    return res;
}

TBool GUriBase::Compare(const_elem_iter aStart, const GUriBase& aUri, const_elem_iter& aResPos) const
{
    TBool res = EFalse;
    TBool matched = ETrue;
    const_elem_iter itb = aStart;
    for (const_elem_iter itf = aUri.Elems().begin(); itf != aUri.Elems().end() && matched; itf++, itb++) {
	matched = (*itb == *itf);
    }
    if (matched) {
	aResPos = itb;
	res = ETrue;
    }
    return res;
}

GUriBase::const_elem_iter GUriBase::Begin() const
{
    return iElems.begin();
}

void GUri::Parse()
{
    TBool fin = EFalse;
    TBool err = EFalse;
    string frag;
    size_t base_end = iUri.find_first_of(KBaseSep, 0);
    if (base_end != string::npos) {
	// Base part is presented
	iBase = iUri.substr(0, base_end);
	frag = iUri.substr(base_end + 1);
	size_t scheme_end = iBase.find_first_of(KSchemeSep, 0);
	if (scheme_end != string::npos) {
	    iScheme = iBase.substr(0, scheme_end);
	}
	size_t auth_beg = (scheme_end != string::npos) ? scheme_end + 1 : 0;
	size_t auth_end = auth_beg;
	// Checking authority
	if ((iBase.size() > (scheme_end + 2)) && (iBase.at(scheme_end + 1) == KPathDelim)
		&& (iBase.at(scheme_end + 2) == KPathDelim)) {
	    auth_beg = auth_beg + 2;
	    auth_end = iBase.find_first_of(KPathDelim, auth_beg);
	    iAuthority = iBase.substr(auth_beg, (auth_end == string::npos) ? string::npos : auth_end - auth_beg);
	}
	// Checking path
	size_t path_beg = (auth_end != string::npos) ? auth_end + 1 : 0;
	size_t path_end = iBase.find_first_of(KQueryDelim, path_beg);
	iPath = iBase.substr(path_beg, (path_end == string::npos) ? string::npos : path_end - path_beg);
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

string GUri::DoGetUri(const_elem_iter aStart, const_elem_iter aEnd, TBool aShort) const
{
    string res;
    // Hier
    for (vector<GUri::TElem>::const_iterator it = aStart; it != aEnd; it++) {
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
	    res.append(TMut::NodeAttrName(it->second.first));
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

GUri::operator string() const
{
    return GetUri(iElems.begin(), ETrue);
}

