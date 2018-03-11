#include "base.h"
#include "guri.h"
#include "chromo.h"


// URI
// Uri conforms to RFC 3986 
// Query syntax: (attr_name '=' attr_value) *( ('&' | '|')  (attr_name '=' attr_value))  

const string GUri::KTypeAny = "*";
const string GUri::KTypeAnywhere = "**";
const string KTypeUnknown = "";
const string GUri::KUpperLevel = "..";
const string GUri::KNativeNil = "nil";

const char GUri:: KSchemeSep = ':';
const char GUri::KPathDelim = '/';
const char GUri::KQueryDelim = '?';
const char GUri::KBaseSep = '#';
const char GUri::KIfaceSep = '%';
const string GUri::KIfaceSepS = "%";
const char GUri::KParentSep = ':';
const char GUri::KSepNone = ' ';
const char GUri::KNodeSep = '/';
const char KGroupStart = '(';
const char KGroupEnd = ')';
const char GUri::KCurUnit = '.';
const string KGroupMark = "()";
const string KRelTypesMarks = "/:";

TBool GUri::iInit = EFalse;


GUri::GUri(const GUri& s):
    iUri(s.iUri), iScheme(s.iScheme), iAuthority(s.iAuthority), iPath(s.iPath), iBase(s.iBase), iElems(s.iElems), iErr(s.iErr)
{
}

GUri::GUri(const string& aUri): iUri(aUri), iErr(EFalse)
{
    Parse();
}

GUri::GUri(): iUri(), iErr(EFalse)
{
}

const string& GUri::Scheme() const
{
    return iScheme;
}

const string& GUri::Authority() const
{
    return iAuthority;
}

const string& GUri::Path() const
{
    return iPath;
}

const string& GUri::GetBase() const
{
    return iBase;
}

string GUri::GetName() const
{
    TInt size = iElems.size();
    return size == 0 ? string() : iElems.at(size -1).mName;
}

void GUri::Append(const GUri& aUri)
{
    for (const_elem_iter it = aUri.Elems().begin(); it != aUri.Elems().end(); it++) {
	AppendElem(*it);
    }
}

void GUri::Prepend(const GUri& aUri)
{
    for (vector<TElem>::const_reverse_iterator it = aUri.Elems().rbegin(); it != aUri.Elems().rend(); it++) {
	PrependElem(*it);
    }
}

void GUri::AppendTail(const GUri& aUri, const_elem_iter aIter)
{
    for (const_elem_iter it = aIter; it != aUri.Elems().end(); it++) {
	AppendElem(*it);
    }
}

void GUri::AppendElem(const string& aExt, const string& aName, char aRelChar)
{
    iElems.push_back(TElem(aName, TElem::relFromChar(aRelChar), aExt));
}

void GUri::AppendElem(const string& aExtName, const char aExtRel, const string& aName, char aRel)
{
    AppendElem(aExtName + string(1, aExtRel), aName, aRel);

}

void GUri::AppendElem(const TElem& aElem)
{
    iElems.push_back(aElem);
}

void GUri::PrependElem(const TElem& aElem)
{
    iElems.insert(iElems.begin(), aElem);
}

void GUri::PrependElem(const string& aExt, const string& aName, char aRelChar)
{
    iElems.insert(iElems.begin(), TElem(aName, TElem::relFromChar(aRelChar), aExt));
}

string GUri::toString(const_elem_iter aStart, TBool aShort) const
{
    return toString(aStart, iElems.end(), aShort);
}

char GUri::GetExtFirstPart(const string& aExt, string& aPart)
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

// Finds the end of group. Nested group are taken into account
size_t GUri::FindGroup(const string& aStr, size_t aPos) 
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

TBool GUri::IsAbsolute() const
{
    TBool res = EFalse;
    if (iElems.size() > 0) {
	const_elem_iter it = iElems.begin();
	TElem elem = *it;
	res = elem.mExt.empty() && elem.mName.empty();
    }
    return res;
}

TBool GUri::IsNil() const
{
    TBool res = EFalse;
    if (iElems.size() == 1) {
	const_elem_iter it = iElems.begin();
	TElem elem = *it;
	res = elem.mExt.empty() && elem.mName == Nil();
    }
    return res;
}

TBool GUri::Compare(const_elem_iter aStart, const GUri& aUri, const_elem_iter& aResPos) const
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

GUri::const_elem_iter GUri::Begin() const
{
    return iElems.begin();
}

string GUri::toString(const_elem_iter aStart, const_elem_iter aEnd, TBool aShort) const
{
    string res;
    for (auto it = aStart; it != aEnd; it++) {
	TElem elem = *it;
	if (it == aStart && (!elem.mExt.empty() || !elem.mName.empty()) && elem.mRel != ENone) {
	    res.append(1, KCurUnit);
	    res.append(1, elem.relChar());
	}
	if (!aShort) {
	    if (!elem.mExt.empty() && elem.mExt != KTypeAny) {
		res.append(1, KGroupStart);
		res.append(elem.mExt);
		res.append(1, KGroupEnd);
	    }
	}
	res.append(elem.mName);
	// TODO Is it correct. This means that relation is relation current to next but not curr to prev
	if (it + 1 != iElems.end()) {
	    res.append(1, elem.relChar());
	}
    }
    return res;
}

bool GUri::operator==(const GUri& s) const
{
    return iUri == s.iUri && iScheme == s.iScheme && iAuthority == s.iAuthority &&
	iPath == s.iPath && iBase == s.iBase && iElems == s.iElems && iErr == s.iErr;
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
