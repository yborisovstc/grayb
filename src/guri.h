#ifndef __GRAYB_GURI_H
#define __GRAYB_GURI_H

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <mchromo.h>

using namespace std;

// Node identification can be done by specifying it's "position" within given type of coordinates.
// There are three coordinate types for now: network hier, native hier and inheritance chain
// Currently non-unique name approach is used, thus all coord types may be required to specify position.
// The "position" can be specifyed in term of relation, which are associated to coord types. 
// One approach is that network hier and native hier is not "orthogonal", i.e the network hier is like
// "continuation" for native hier from root node, so we can consider two these coord aa "extended" native hier
// There are two types of relation: relation owner-owned in native hier and parent-child in inheritance chain.
// So each node relates to two other: owner (relation owner-owned) and parent (relation parent-child)
// If continue the relation from owner and parent we will get the "colored" tree build with two types (color) 
// relation. Currently uri implementaion does't support parsing into the tree, it is "linear" i.e. uses
// only onle relation type. This simplities the parsed representation but needs two type of uri's - one for hier
// and another for inheritance, which is not universal.

// Currently the simplified URI scheme is used, where native hier is not specified for parents, so the
// The idea is that if the hier name is umique for particular node then inher chain is unique also
// thus any parts of inher chain from chain begin can be used for unique identificatio of node ancestor
// strucrure is lienar nw_hier '#' native_hier '/' inheritance_chain ':' name


// URI base
class GUriBase
{
    public:
	enum TQueryOpr {
	    EQop_Unknown,
	    EQop_And
	};
    public:
	// Relation: relation type symbol, name
	typedef pair<char, string> TRel;
	// Element of chain: extenstion, relation
	// Example /(Elem:)Mynode here Elem: - extention, Mynode - name, / - relation symbol
	typedef pair<string, TRel> TElem;
	typedef pair<TNodeAttr, string> TQueryCnd;
	typedef pair<TQueryOpr, TQueryCnd> TQueryElem;
	typedef vector<TElem>::const_iterator const_elem_iter;
	typedef vector<TElem>::iterator elem_iter;
    public:
	GUriBase(const string& aGUri);
	GUriBase();
	const vector<TElem>& Elems() const {return iElems;};
	string GetUri(const_elem_iter aStart, TBool aShort = EFalse) const;
	string GetUriBody(const_elem_iter aEnd, TBool aShort = EFalse) const;
	string GetUri(TBool aShort = EFalse) const { return GetUri(iElems.begin(), aShort);};
	const string& GetLoc() const;
	const string& Scheme() const;
	const string& Authority() const;
	const string& Path() const;
	string GetName() const;
	static string Nil();
	const string& GetBase() const;
	const_elem_iter Begin() const;
	void Append(const GUriBase& aUri);
	void Prepend(const GUriBase& aUri);
	void AppendTail(const GUriBase& aUri, const_elem_iter aIter);
	void AppendElem(const string& aType, const string& aName, char aRelType = KNodeSep);
	void AppendElem(const string& aExt, const char aExtRel, const string& aName, char aRelType = KNodeSep);
	void AppendElem(const TElem& aElem);
	void PrependElem(const TElem& aElem);
	void PrependElem(const string& aType, const string& aName, char aRelType = KNodeSep);
	void AppendQueryElem(TQueryOpr aOpr, TNodeAttr aAttr, const string& aValue);
	//void ToString(string& aRes);
	static TElem Elem(char aRelType, const string& aName, const string& aExt);
	TBool IsErr() const { return iErr;};
	TBool IsAbsolute() const;
	TBool IsNil() const;
	static char GetExtFirstPart(const string& aExt, string& aPart);
	TBool Compare(const_elem_iter aStart, const GUriBase& aUri, const_elem_iter& aResPos) const;
    protected:
	virtual string DoGetUri(const_elem_iter aStart, const_elem_iter aEnd, TBool aShort = EFalse) const = 0;
	static string SelectGroup(const string& aData, int aEndPos);
	static size_t FindGroup(const string& aStr, size_t aPos);
    protected:
	string iUri;
	string iScheme;
	string iAuthority;
	string iPath;
	// Nw hier part
	string iBase;
	vector<TElem> iElems;
	vector<TQueryElem> iQueryElems;
	static TBool iInit;
	TBool iErr;
    public:
	static const string KTypeAny;
	static const string KTypeAnywhere;
	static const string KUpperLevel;
	static const char KParentSep;
	static const char KSchemeSep;
	static const char KPathDelim;
	static const char KQueryDelim;
	static const char KNodeSep;
	static const char KSepNone;
	static const char KBaseSep;
	static const char KIfaceSep;
	static const string KIfaceSepS;
	static const char KCurUnit;
	static const string KNativeNil; // Predefined name of not existent native agent
};

inline string GUriBase::Nil() {return KNativeNil;};

// URI for native hier coord. Inheritnace coord is provided as extended location thru GetLoc
class GUri: public GUriBase
{
    public:
	enum TCoord {
	    EHier,
	    EInher
	};
    public:
	GUri(const string& aGUri);
	GUri();
	GUri operator+(const GUri& aUri);
	GUri& operator+=(const GUri& aUri);
	operator string() const;
    protected:
	virtual string DoGetUri(const_elem_iter  aStart, const_elem_iter aEnd, TBool aShort = EFalse) const;
	void Parse();
};

#if 0
// URI for inheritance coord. Native hier coord is provided as extended location thru GetLoc
// !! Not completed, not used for now
class IUri: public GUriBase
{
    public:
	IUri(const string& aUri);
	IUri();
    protected:
	virtual string DoGetUri(vector<TElem>::const_iterator aStart, TBool aShort = EFalse) const;
	void Parse();
};
#endif

#endif
