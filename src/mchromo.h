#ifndef __GRAYB_MCHROMO_H
#define __GRAYB_MCHROMO_H

#include "base.h"
#include "guri.h"

class MLogRec;

// Mutation to model relation type
enum TMutModRel {
    EMrel_Parent = 1, // Parent
    EMrel_Obj = 2, // Object of mutation 
    EMrel_Ref = 3 // Ref to model node, e.g. -id- value in -cont-
};

// Interface of chromo model
class MChromoMdl
{
    public:
	virtual TNodeType GetType(const void* aHandle) = 0;
	virtual TNodeType GetType(const string& aId) = 0;
	virtual void* Parent(const void* aHandle) = 0;
	virtual void* Next(const void* aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual void* NextText(const void* aHandle) = 0;
	virtual void* Prev(const void* aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual void* GetFirstChild(const void* aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual void* GetLastChild(const void* aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual void* GetFirstTextChild(const void* aHandle) = 0;
	virtual char* GetAttr(const void* aHandle, TNodeAttr aAttr) = 0;
	virtual char* GetContent(const void* aHandle) = 0;
	virtual void  SetContent(const void* aHandle, const string& aContent) = 0;
	virtual TBool AttrExists(const void* aHandle, TNodeAttr aAttr) = 0;
	virtual void* AddChild(void* aParent, TNodeType aType) = 0;
	virtual void* AddChild(void* aParent, const void* aHandle, TBool aCopy = ETrue) = 0;
	virtual void* AddChildDef(void* aParent, const void* aHandle, TBool aCopy = ETrue) = 0;
	virtual void* AddNext(const void* aPrev, const void* aHandle, TBool aCopy = ETrue) = 0;
	virtual void* AddNext(const void* aPrev, TNodeType aNode) = 0;
	virtual void* AddPrev(const void* aNext, const void* aHandle, TBool aCopy = ETrue) = 0;
	virtual void RmChild(void* aParent, void* aChild, TBool aDeattachOnly = EFalse) = 0;
	virtual void Rm(void* aHandle) = 0;
	virtual void MoveNextTo(void* aHandle, void* aDest) = 0;
	virtual void MovePrevTo(void* aHandle, void* aDest) = 0;
	virtual void MoveToEnd(void* aHandle) = 0;
	virtual void SetAttr(void* aNode, TNodeAttr aType, const char* aVal) = 0;
	virtual void SetAttr(void* aNode, TNodeAttr aType, TNodeType aVal) = 0;
	virtual void SetAttr(void* aNode, TNodeAttr aType, TNodeAttr aVal) = 0;
	virtual void Dump(void* aNode, MLogRec* aLogRec) = 0;
	virtual void Save(const string& aFileName) const = 0;
	virtual void* Find(const void* aHandle, const string& aUri) = 0;
};

class ChromoNode;

// Interface of chromosome
class MChromo
{
    public:
	// Dependencies levels
	enum TDepsLevel {
	    EDl_Critical = 0x01,
	    EDl_Affecting = 0x02,
	    EDl_Harmless = 0x04,
	    EDl_Any = EDl_Critical | EDl_Affecting | EDl_Harmless
	};
    public:
	virtual ChromoNode& Root() = 0;
	virtual const ChromoNode& Root() const= 0;
	virtual void Set(const char *aFileName) = 0;
	virtual TBool Set(const string& aUri) = 0;
	virtual void Set(const ChromoNode& aRoot) = 0;
	virtual void Init(TNodeType aRootType) = 0;
	virtual void Reset() = 0;
	virtual void Save(const string& aFileName) const = 0;
	virtual ChromoNode CreateNode(void* aHandle) = 0;
};

#endif
