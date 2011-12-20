#ifndef __GRAYB_CHROMO_H
#define __GRAYB_CHROMO_H

#include "base.h"
#include "guri.h"
#include "mchromo.h"

class ChromoMdl: public Base, public MChromoMdl
{
    public:
	ChromoMdl(const string &aName): Base(aName) {}
};

// Node of chromo
class ChromoNode
{
    public:
	class Iterator: public iterator<input_iterator_tag, ChromoNode>
    {
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
    public:
	class Const_Iterator: public iterator<input_iterator_tag, ChromoNode>
    {
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

    public:
	ChromoNode(ChromoMdl& aMdl, void* aHandle): iMdl(aMdl), iHandle(aHandle) {};
	ChromoNode(const ChromoNode& aNode): iMdl(aNode.iMdl), iHandle(aNode.iHandle) {};
	ChromoNode& operator=(const ChromoNode& aNode) { iMdl = aNode.iMdl; iHandle = aNode.iHandle; return *this; };
	Iterator Begin() { return Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Const_Iterator Begin() const { return Const_Iterator(iMdl, iMdl.GetFirstChild(iHandle)); };
	Iterator End() { return Iterator(iMdl, NULL); };
	Const_Iterator End() const { return Const_Iterator(iMdl, NULL); };
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
	const void* Handle() { return iHandle;};
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
	// Be careful while removing node got from iterator. Iterator is not cleaned thus it returns wrong node on ++
	void RmChild(const ChromoNode& aChild) { iMdl.RmChild(iHandle, aChild.iHandle); };
	void Rm() { iMdl.Rm(iHandle); };
	void SetAttr(TNodeAttr aType, const string& aVal) { iMdl.SetAttr(iHandle, aType, aVal.c_str()); };
	void SetAttr(TNodeAttr aType, TNodeType aVal) { iMdl.SetAttr(iHandle, aType, aVal); };
	void SetAttr(TNodeAttr aType, TNodeAttr aVal) { iMdl.SetAttr(iHandle, aType, aVal); };
	ChromoNode::Iterator Parent();
	ChromoNode::Iterator Find(TNodeType aType, const string& aName);
	ChromoNode::Const_Iterator Find(TNodeType aType, const string& aName) const;
	ChromoNode::Iterator Find(TNodeType aType, const string& aName, TNodeAttr aAttr, const string& aAttrVal);
	void Dump(MLogRec* aLogRec) const { iMdl.Dump(iHandle, aLogRec);};
	string GetName(const string& aTname);
	void MoveNextTo(Iterator& aDest) { iMdl.MoveNextTo(iHandle, aDest.iHandle);};
    private :
	ChromoMdl& iMdl;
	void* iHandle;
};

class Chromo: public MChromo
{
    public:
	virtual ~Chromo() {};
    public:
	static void GetUriScheme(const string& aUri, string& aScheme);
	static void GetPath(const string& aUri, string& aPath);
	static void GetFrag(const string& aUri, string& aFrag);
};

#endif
