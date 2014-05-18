#ifndef __GRAYB_CHROMO_H
#define __GRAYB_CHROMO_H

#include "base.h"
#include "guri.h"
#include "mchromo.h"

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


class ChromoMdl: public Base, public MChromoMdl
{
    public:
	ChromoMdl(const string &aName): Base(aName) {}
};

// Node of chromo
class ChromoNode
{
    public:

	class Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Iterator(ChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Iterator(const Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Iterator& operator=(const Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Iterator& operator++() { iHandle = iMdl.Next(iHandle); return *this; };
	    Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    ChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};

	class Const_Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Const_Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Const_Iterator(ChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Const_Iterator(const Const_Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Const_Iterator& operator=(const Const_Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Const_Iterator& operator++() { iHandle = iMdl.Next(iHandle); return *this; };
	    Const_Iterator operator++(int) { Const_Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Const_Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Const_Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    ChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};

	class Reverse_Iterator: public iterator<input_iterator_tag, ChromoNode> {
	    friend class ChromoNode;
	    public:
	    Reverse_Iterator(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	    Reverse_Iterator(ChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	    Reverse_Iterator(const Reverse_Iterator& aIt): iMdl(aIt.iMdl), iHandle(aIt.iHandle) {};
	    Reverse_Iterator& operator=(const Reverse_Iterator& aIt) { iMdl = aIt.iMdl; iHandle = aIt.iHandle; return *this; };
	    Reverse_Iterator& operator++() { iHandle = iMdl.Prev(iHandle); return *this; };
	    Reverse_Iterator operator++(int) { Reverse_Iterator tmp(*this); operator++(); return tmp; };
	    TBool operator==(const Reverse_Iterator& aIt) { return (iHandle == aIt.iHandle); };
	    TBool operator!=(const Reverse_Iterator& aIt) { return !(iHandle == aIt.iHandle); };
	    ChromoNode operator*() { return ChromoNode(iMdl, iHandle);};
	    public:
	    ChromoMdl& iMdl;
	    void* iHandle; // NULL point to the past-of-the-end element
	};


    public:
	ChromoNode(ChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	ChromoNode(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	ChromoNode& operator=(const ChromoNode& aNode) { iMdl = aNode.iMdl; iHandle = aNode.iHandle; return *this; };
	TBool operator==(const ChromoNode& aNode) { return &iMdl == &(aNode.iMdl) && iHandle == aNode.iHandle;};
	Iterator Begin() { return Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Const_Iterator Begin() const { return Const_Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Iterator End() { return Iterator(iMdl, NULL); };
	Const_Iterator End() const { return Const_Iterator(iMdl, NULL); };
	Reverse_Iterator Rbegin() { return Reverse_Iterator(iMdl, iMdl.GetLastChild(iHandle)); };
	Reverse_Iterator Rend() { return Reverse_Iterator(iMdl, NULL); };
	Iterator Find(TNodeType aNodeType) { return Iterator(iMdl, iMdl.GetFirstChild(iHandle, aNodeType)); };
	Const_Iterator Find(TNodeType aNodeType) const { return Const_Iterator(iMdl, iMdl.GetFirstChild(iHandle, aNodeType)); };
	Iterator FindText() { return Iterator(iMdl, iMdl.GetFirstTextChild(iHandle)); };
	Const_Iterator FindText() const { return Const_Iterator(iMdl, iMdl.GetFirstTextChild(iHandle)); };
	void Reset() { iHandle = NULL;};
    public:
	TNodeType Type() { return iMdl.GetType(iHandle); };
	TNodeType Type() const { return iMdl.GetType(iHandle); };
	const string Name() { return Attr(ENa_Id);};
	const string Name() const { return Attr(ENa_Id);};
	const string Attr(TNodeAttr aAttr);
	const string Attr(TNodeAttr aAttr) const;
	const string Content() { return iMdl.GetContent(iHandle);};
	void  SetContent(const string& aContent) { return iMdl.SetContent(iHandle, aContent);};
	TInt AttrInt(TNodeAttr aAttr) const;
	TBool AttrExists(TNodeAttr aAttr) const { return iMdl.AttrExists(iHandle, aAttr);};
	TBool AttrBool(TNodeAttr aAttr) const;
	void* Handle() { return iHandle;};
	const void* Handle() const { return iHandle;};
	ChromoMdl& Mdl() const { return iMdl;};
	ChromoNode AddChild(TNodeType aType) { return ChromoNode(iMdl, iMdl.AddChild(iHandle, aType)); };
	ChromoNode AddChild(const ChromoNode& aNode, TBool aCopy = ETrue) { return 
	    ChromoNode(iMdl, iMdl.AddChild(iHandle, aNode.Handle(), aCopy)); };
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
	void SetAttr(TNodeAttr aType, const string& aVal) { iMdl.SetAttr(iHandle, aType, aVal.c_str()); };
	void SetAttr(TNodeAttr aType, TNodeType aVal) { iMdl.SetAttr(iHandle, aType, aVal); };
	void SetAttr(TNodeAttr aType, TNodeAttr aVal) { iMdl.SetAttr(iHandle, aType, aVal); };
	ChromoNode::Iterator Parent();
	ChromoNode::Const_Iterator Parent() const;
	ChromoNode::Iterator Find(TNodeType aType, const string& aName);
	ChromoNode::Const_Iterator Find(TNodeType aType, const string& aName) const;
	ChromoNode::Iterator Find(TNodeType aType, const string& aName, TNodeAttr aAttr, const string& aAttrVal);
	void Dump(MLogRec* aLogRec) const { iMdl.Dump(iHandle, aLogRec);};
	string GetName(const string& aTname);
	void MoveNextTo(Iterator& aDest) { iMdl.MoveNextTo(iHandle, aDest.iHandle);};
	void MovePrevTo(Iterator& aDest) { iMdl.MovePrevTo(iHandle, aDest.iHandle);};
	void MoveToEnd() { iMdl.MoveToEnd(iHandle);};
	TInt GetLocalRank();
	void GetRank(Rank& aRank) const;
	void GetRank(Rank& aRank, const ChromoNode& aBase) const;
	// The number of direct childs
	TInt GetLocalSize();
	ChromoNode& GetNode(const GUri& aUri) const;
    private :
	ChromoMdl& iMdl;
	void* iHandle;
};

class Chromo: public MChromo
{
    public:
	virtual ~Chromo() {};
    public:
	static void GetPath(const string& aUri, string& aPath);
	static void GetFrag(const string& aUri, string& aFrag);
};

#endif
