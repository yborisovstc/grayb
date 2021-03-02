#ifndef __GRAYB_GURI_H
#define __GRAYB_GURI_H

#include <utility>
#include <vector>
#include <cassert>
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
    //

    /**
     *
     */
    class GUri
    {
	public:
	    /** Unit relation to preceeding unit. ENode is used for native agent */
	    enum TRel {
		ENone = 0,
		EComp,
		EChild
	    };

	    class Unit
	    {
		public:
		    Unit(): mName(), mRel(TRel::EComp), mExt() {}
		    Unit(const string& aName, TRel aRel = TRel::EComp, const string& aExt = string()): mName(aName), mRel(aRel), mExt(aExt) {}
		    Unit(const Unit& src): mName(src.mName), mRel(src.mRel), mExt(src.mExt) {}
		    const string& name() const { return mName;}
		    TRel rel() const { return mRel;}
		    const string& ext() const { return mExt;}
		    bool operator ==(const Unit& p) const { return mName == p.mName && mRel == p.mRel && mExt == p.mExt;}
		    char relChar() const { return mRel == ENone ? KSepNone : (mRel == EComp ? KNodeSep : KParentSep);}
		    static char relToChar(TRel aRel) { return aRel == ENone ? KSepNone : (aRel == EComp ? KNodeSep : KParentSep);}
		    static TRel relFromChar(char aChar) { if (aChar == KSepNone) return ENone; else if (aChar == KNodeSep) return EComp;
			else if (aChar == KParentSep) return EChild; else assert(EFalse);}
		public:
		    string mName;
		    TRel mRel;
		    string mExt; // Extension
	    };


	public:
	    typedef Unit TElem;
	    typedef vector<TElem>::const_iterator const_elem_iter;
	    typedef vector<TElem>::iterator elem_iter;
	public:
	    GUri();
	    GUri(const GUri& aSrc);
	    GUri(const string& aUri);
	    const vector<TElem>& Elems() const {return iElems;};
	    string toString(const_elem_iter aStart, TBool aShort = EFalse) const;
	    string toString(TBool aShort = EFalse) const;
	    const string& Scheme() const;
	    const string& Authority() const;
	    const string& Path() const;
	    string GetName() const;
	    static string Nil();
	    const string& GetBase() const;
	    const_elem_iter Begin() const;
	    void Append(const GUri& aUri);
	    void Prepend(const GUri& aUri);
	    void AppendTail(const GUri& aUri, const_elem_iter aIter);
	    void AppendElem(const string& aExt, const string& aName, char aRelChar = KNodeSep);
	    void AppendElem(const string& aExtName, const char aExtRel, const string& aName, char aRel = KNodeSep);
	    void AppendElem(const TElem& aElem);
	    void PrependElem(const TElem& aElem);
	    void PrependElem(const string& aType, const string& aName, char aRelType = KNodeSep);
	    void RemoveLastElem();
	    void Tail(const GUri& aHead, GUri& aTail) const;
	    /** @brief Reduces path units by merging back steps */
	    void Reduce();
	    //void ToString(string& aRes);
	    TBool IsErr() const { return iErr;};
	    void SetErr(TBool aVal = ETrue) { iErr = aVal;}
	    TBool IsAbsolute() const;
	    TBool IsNil() const;
	    TBool IsContent() const { return !mContent.empty();}
	    string GetContent() const { return mContent;}
	    static char GetExtFirstPart(const string& aExt, string& aPart);
	    TBool Compare(const_elem_iter aStart, const GUri& aUri, const_elem_iter& aResPos) const;
	    GUri operator+(const GUri& aUri);
	    GUri& operator+=(const GUri& aUri);
	    bool operator==(const GUri& aSrc) const;
	    bool operator!=(const GUri& aSrc) const { return !operator ==(aSrc);}
	    bool operator<(const GUri& aSrc) const;
	    bool operator<=(const GUri& aSrc) const { return *this < aSrc || *this == aSrc;}
	    bool operator>(const GUri& aSrc) const { return aSrc < *this;}
	    bool operator>=(const GUri& aSrc) const { return aSrc <= *this;}
	    operator string() const { return toString(iElems.begin());}
	protected:
	    void Parse();
	    // TODO Do we really need convertor of part of the chain?
	    string toString(const_elem_iter aStart, const_elem_iter aEnd, TBool aShort = EFalse) const;
	    static size_t FindGroup(const string& aStr, size_t aPos);
	protected:
	    string iUri;
	    string iScheme;
	    string iAuthority;
	    string iPath;
	    string mContent;
	    // Nw hier part
	    string iBase;
	    vector<TElem> iElems;
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
	    static const char KContentSep;
	    static const string KIfaceSepS;
	    static const char KCurUnit;
	    static const string KNativeNil; // Predefined name of not existent native agent
    };

    inline string GUri::Nil() {return KNativeNil;};


#endif
