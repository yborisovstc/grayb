
#ifndef __GRAYB_CHROMOX_H
#define __GRAYB_CHROMOX_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "chromo.h"

// Model of XML based chromo
class ChromoMdlX: public Base, public MChromoMdl
{
    public:
	ChromoMdlX();
	virtual ~ChromoMdlX();
    public:
	virtual TNodeType GetType(const THandle& aHandle);
	virtual THandle Root(const THandle& aHandle);
	virtual THandle Parent(const THandle& aHandle);
	virtual THandle Next(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle Prev(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetFirstChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetLastChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual string GetAttr(const THandle& aHandle, TNodeAttr aAttr) const;
	virtual TBool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const;
	virtual THandle AddChild(const THandle& aParent, TNodeType aNode);
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy = ETrue, TBool aRecursively = ETrue);
	virtual THandle AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy = ETrue);
	virtual THandle AddNext(const THandle& aPrev, TNodeType aNode);
	virtual THandle AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy = ETrue);
	virtual void RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly = EFalse);
	virtual void Rm(const THandle& aNode);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal);
	virtual void SetAttr(const THandle& aHandle, TNodeAttr aAttr, TInt aVal);
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType);
	virtual void Dump(const THandle& aNode);
	virtual void DumpBackTree(const THandle& aNode);
	virtual void DumpToLog(const THandle& aNode, MLogRec* aLogRec);
	virtual TBool ToString(const THandle& aNode, string& aString) const;
	virtual void Save(const string& aFileName, TInt aIndent = 0) const;
	virtual THandle Find(const THandle& aHandle, const string& aUri);
	virtual TInt GetOrder(const THandle& aHandle, TBool aTree = EFalse) const;
	virtual void DeOrder(const THandle& aHandle);
	virtual TInt GetLineId(const THandle& aHandle) const;
    public:
	int GetAttrInt(void *aHandle, const char *aName);
	THandle SetFromFile(const string& aFileName);
	THandle Set(const string& aUri);
	THandle SetFromSpec(const string& aSpec);
	THandle Set(const THandle& aHandle);
	xmlDoc* Doc() { return iDoc;};
	static inline const char *Type() { return "ChromoMdlX";}; 
	virtual THandle Init(TNodeType aRootType);
	void Reset();
    protected:
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
    private:
	xmlDoc *iDoc;	// XML document
	TBool iDocOwned;
};


// XML based chromo
class ChromoX: public MChromo
{
    public:
	ChromoX();
	ChromoX(const ChromoX& aSrc);
	ChromoX(const char *aFileName);
	virtual ~ChromoX();
    public:
	// From MChromo
	virtual ChromoNode& Root();
	virtual const ChromoNode& Root() const;
	virtual void SetFromFile(const string& aFileName);
	virtual TBool Set(const string& aUri);
	virtual TBool SetFromSpec(const string& aSpec);
	virtual TBool GetSpec(string& aSpec);
	virtual void Set(const ChromoNode& aRoot);
	virtual void Convert(const MChromo& aSrc);
	virtual void Init(TNodeType aRootType);
	virtual void Reset();
	virtual void Save(const string& aFileName, TInt aIndent = 0) const;
	virtual ChromoNode CreateNode(const THandle& aHandle);
	virtual void ReduceToSelection(const ChromoNode& aSelNode);
	virtual const CError& Error() const {return mErr;}
    private:
	ChromoMdlX iMdl;
	ChromoNode iRootNode;
	CError mErr;
};

#endif
