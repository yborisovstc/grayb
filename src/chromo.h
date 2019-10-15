#ifndef __GRAYB_CHROMO_H
#define __GRAYB_CHROMO_H

#include "base.h"
#include "guri.h"
#include "mchromo.h"
#include <map>

// Mutation spec
class TMut
{
    public:
	typedef pair<TNodeAttr, string> TElem;
	typedef map<TNodeAttr, string> TAttrs;
    public:
	TMut();
	TMut(const string& aSpec);
	TMut(TNodeType aType);
	TMut(const ChromoNode& aCnode);
	TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val);
	TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val);
	TMut(TNodeType aType, TNodeAttr aAttr0, const string& aAttr0Val, TNodeAttr aAttr1, const string& aAttr1Val,
		TNodeAttr aAttr2, const string& aAttr2Val);
	TNodeType Type() const { return mType;};
	void SetAttr(TNodeAttr aAttr, const string& aAttrVal);
	void RmAttr(TNodeAttr aAttr);
	string Attr(TNodeAttr aId) const;
	TBool AttrExists(TNodeAttr aId) const;
	const TAttrs& Attrs() const { return mAttrs;};
	operator string() const;
    public:
	static const string& NodeAttrName(TNodeAttr aAttr);
	static const string& NodeTypeName(TNodeType aType);
	static TNodeAttr NodeAttr(const string& aAttrName);
	static TNodeType NodeType(const string& aTypeName);
    private:
	static string EscapeCtrls(const string& aInp);
	static string DeEscCtrls(const string& aInp);
	static map<string, TNodeType> KNodeTypes_Init();
	static map<string, TNodeAttr> KNodeAttrs_Init();
	static TBool IsCtrlSymb(const char aSymb) { return aSymb == KSep || aSymb == KAttrSep;};
    private:
	TNodeType mType;
	TAttrs mAttrs;
	static const char KSep = ',';
	static const char KAttrSep = ':';
	static const char KEsc = '\\';
	static const char KValDelim = '"';
	static const string KCtrls;
	static map<string, TNodeType> KNodeTypes;
	static map<string, TNodeAttr> KNodeAttrs;
	static map<TNodeType, string> KNodeTypesNames;
	static map<TNodeAttr, string> KNodeAttrsNames;
};

// Rank
class Rank: public vector<TInt>
{
    public:
	Rank(): vector<TInt>() {};
	string ToString() const;
	TBool IsRankOf(const Rank& aArg) const;
	TInt Compare(const Rank& aArg) const;
	TBool operator==(const Rank& aArg) const {return Compare(aArg) == 0;};
	TBool operator<(const Rank& aArg) const {return Compare(aArg) == -1;};
	TBool operator>(const Rank& aArg) const {return Compare(aArg) == 1;};
	Rank& operator+=(const Rank& aArg);
};


// Node of chromo
class ChromoNode
{
    public:

	class Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Iterator(MChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Iterator(const Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Iterator& operator=(const Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Iterator& operator++() { iHandle = iMdl.Next(iHandle); return *this; };
	    Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    MChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};

	class Const_Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Const_Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Const_Iterator(MChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Const_Iterator(const Const_Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Const_Iterator& operator=(const Const_Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Const_Iterator& operator++() { iHandle = iMdl.Next(iHandle); return *this; };
	    Const_Iterator operator++(int) { Const_Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Const_Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Const_Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    MChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};

	class Reverse_Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Reverse_Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Reverse_Iterator(MChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Reverse_Iterator(const Reverse_Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Reverse_Iterator& operator=(const Reverse_Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Reverse_Iterator& operator++() { iHandle = iMdl.Prev(iHandle); return *this; };
	    Reverse_Iterator operator++(int) { Reverse_Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Reverse_Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Reverse_Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    MChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};

    public:
	ChromoNode(): iMdl(*(MChromoMdl*) NULL), iHandle(NULL) {};
	ChromoNode(MChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	ChromoNode(const ChromoNode& aNode);
	ChromoNode& operator=(const ChromoNode& aNode);
	TBool operator==(const ChromoNode& aNode);
	TBool operator==(const ChromoNode& aNode) const;
	TBool operator!=(const ChromoNode& aNode) { return !this->operator==(aNode);};
	TBool operator!=(const ChromoNode& aNode) const { return !this->operator==(aNode);};
	Iterator Begin() { return Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Const_Iterator Begin() const { return Const_Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Iterator End() { return Iterator(iMdl, NULL); };
	Const_Iterator End() const { return Const_Iterator(iMdl, NULL); };
	Reverse_Iterator Rbegin() { return Reverse_Iterator(iMdl, iMdl.GetLastChild(iHandle)); };
	Reverse_Iterator Rend() { return Reverse_Iterator(iMdl, NULL); };
	Iterator Find(TNodeType aNodeType) { return Iterator(iMdl, iMdl.GetFirstChild(iHandle, aNodeType)); };
	Const_Iterator Find(TNodeType aNodeType) const { return Const_Iterator(iMdl, iMdl.GetFirstChild(iHandle, aNodeType)); };
	Iterator FindNextSibling(TNodeType aNodeType) { return Iterator(iMdl, iMdl.Next(iHandle, aNodeType)); };
	Const_Iterator FindNextSibling(TNodeType aNodeType) const { return Const_Iterator(iMdl, iMdl.Next(iHandle, aNodeType)); };
	void Reset() { iHandle = NULL;};
    public:
	TNodeType Type() { return iMdl.GetType(iHandle); };
	TNodeType Type() const { return iMdl.GetType(iHandle); };
	TInt LineId() const { return iMdl.GetLineId(iHandle); };
	const string Name() { return Attr(ENa_Id);};
	const string Name() const { return Attr(ENa_Id);};
	const string Attr(TNodeAttr aAttr);
	const string Attr(TNodeAttr aAttr) const;
	void GetAttr(TNodeAttr aAttr, TInt& aVal) const { iMdl.GetAttr(iHandle, aAttr, aVal);};
	void RmAttr(TNodeAttr aAttr) const { iMdl.RmAttr(iHandle, aAttr);};
	TInt AttrInt(TNodeAttr aAttr) const;
	TBool AttrExists(TNodeAttr aAttr) const { return iMdl.AttrExists(iHandle, aAttr);};
	TBool AttrBool(TNodeAttr aAttr) const;
	void* Handle() { return iHandle;};
	const void* Handle() const { return iHandle;};
	MChromoMdl& Mdl() const { return iMdl;};
	ChromoNode AddChild(TNodeType aType);
	ChromoNode AddChild(const TMut& aMut);
	ChromoNode AddChild(const ChromoNode& aNode, TBool aCopy = ETrue, TBool aRecursively = ETrue);
	ChromoNode AddChildDef(const ChromoNode& aNode, TBool aCopy = ETrue) { return 
	    ChromoNode(iMdl, iMdl.AddChildDef(iHandle, aNode.Handle(), aCopy)); };
	ChromoNode AddNext(const ChromoNode& aPrev, const ChromoNode& aNode, TBool aCopy = ETrue) { return 
	    ChromoNode(iMdl, iMdl.AddNext(aPrev.Handle(), aNode.Handle(), aCopy)); };
	ChromoNode AddNext(TNodeType aType) { return ChromoNode(iMdl, iMdl.AddNext(iHandle, aType));};
	ChromoNode AddPrev(const ChromoNode& aNext, const ChromoNode& aNode, TBool aCopy = ETrue) { return 
	    ChromoNode(iMdl, iMdl.AddPrev(aNext.Handle(), aNode.Handle(), aCopy)); };
	// Be careful while removing node got from iterator. Iterator is not cleaned thus it returns wrong node on ++
	void RmChild(const ChromoNode& aChild, TBool aDeattachOnly = EFalse) { iMdl.RmChild(iHandle, aChild.iHandle, aDeattachOnly); };
	void Rm() { iMdl.Rm(iHandle); };
	TBool IsActive() const { return !AttrExists(ENa_Inactive);};
	void Deactivate() { SetAttr(ENa_Inactive, ""); };
	void Activate();
	void SetAttr(TNodeAttr aType, const string& aVal) { iMdl.SetAttr(iHandle, aType, aVal.c_str()); };
	void SetAttr(TNodeAttr aType, TInt aVal) { iMdl.SetAttr(iHandle, aType, aVal);};
	ChromoNode::Iterator Parent();
	ChromoNode::Iterator Root();
	ChromoNode::Const_Iterator Parent() const;
	ChromoNode::Const_Iterator Root() const;
	ChromoNode::Iterator Find(TNodeType aType, const string& aName);
	ChromoNode::Const_Iterator Find(TNodeType aType, const string& aName) const;
	ChromoNode::Iterator Find(TNodeType aType, const string& aName, TNodeAttr aAttr, const string& aAttrVal);
	ChromoNode::Iterator FindNodeInMhUri(const GUri& aMhUri, const GUri::const_elem_iter& aMhUriPos, GUri::const_elem_iter& aResPos);
	ChromoNode::Iterator GetChildOwning(const ChromoNode& aNode) const;
	void Dump(MLogRec* aLogRec) const { iMdl.Dump(iHandle, aLogRec);};
	string GetName(const string& aTname);
	void MoveNextTo(Iterator& aDest) { iMdl.MoveNextTo(iHandle, aDest.iHandle);};
	void MoveNextTo(ChromoNode& aDest) { iMdl.MoveNextTo(iHandle, aDest.iHandle);};
	void MovePrevTo(Iterator& aDest) { iMdl.MovePrevTo(iHandle, aDest.iHandle);};
	void MovePrevTo(ChromoNode& aDest) { iMdl.MovePrevTo(iHandle, aDest.iHandle);};
	void MoveToEnd() { iMdl.MoveToEnd(iHandle);};
	TInt GetLocalRank() const;
	TInt GetOrder(TBool aTree = EFalse) const { return iMdl.GetOrder(iHandle, aTree);};
	void SetOrder(TInt aOrder, TBool aTree = EFalse) { iMdl.SetOrder(iHandle, aOrder, aTree);};
	void DeOrder() { iMdl.DeOrder(iHandle);};
	// The number of direct childs
	TInt Count() { return GetLocalSize();};
	TInt Count() const;
	TInt Count(TNodeType aType) const { return GetLocalSize(aType);};
	TInt GetLocalSize();
	TInt GetLocalSize(TNodeType aType) const;
	ChromoNode GetNode(const GUri& aUri);
	ChromoNode GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	// Get node by model hier uri
	ChromoNode GetNodeByMhUri(const GUri& aUri);
	ChromoNode GetNodeByMhUri(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	void GetUri(GUri& aUri) const;
	void GetUri(GUri& aUri, const ChromoNode& aBase) const;
	void ReduceToSelection(const ChromoNode& aSelNode);
	void ToString(string& aString) const { iMdl.ToString(iHandle, aString);};
	operator string() const;
    private :
	MChromoMdl& iMdl;
	void* iHandle;
};

class ChromoUtils
{
    public:
	static void GetPath(const string& aUri, string& aPath);
	static void GetFrag(const string& aUri, string& aFrag);
};

#endif
