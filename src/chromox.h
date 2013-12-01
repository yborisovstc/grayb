
#ifndef __GRAYB_CHROMOX_H
#define __GRAYB_CHROMOX_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include "chromo.h"


// Model of XML based chromo
class ChromoMdlX: public ChromoMdl
{
    public:
	ChromoMdlX();
	virtual ~ChromoMdlX();
    public:
	virtual TNodeType GetType(const string& aId);
	virtual TNodeType GetType(const void* aHandle);
	virtual void* Parent(const void* aHandle);
	virtual void* Next(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* NextText(const void* aHandle);
	virtual void* Prev(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* GetFirstChild(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* GetLastChild(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* GetFirstTextChild(const void* aHandle);
	virtual char *GetAttr(const void* aHandle, TNodeAttr aAttr);
	virtual char* GetContent(const void* aHandle);
	virtual void  SetContent(const void* aHandle, const string& aContent);
	virtual TBool AttrExists(const void* aHandle, TNodeAttr aAttr);
	virtual void* AddChild(void* aParent, TNodeType aNode);
	virtual void* AddChild(void* aParent, const void* aHandle, TBool aCopy = ETrue);
	virtual void* AddChildDef(void* aParent, const void* aHandle, TBool aCopy = ETrue);
	virtual void* AddNext(const void* aPrev, const void* aHandle, TBool aCopy = ETrue);
	virtual void* AddNext(const void* aPrev, TNodeType aNode);
	virtual void RmChild(void* aParent, void* aChild, TBool aDeattachOnly = EFalse);
	virtual void Rm(void* aNode);
	virtual void MoveNextTo(void* aHandle, void* aDest);
	virtual void MovePrevTo(void* aHandle, void* aDest);
	virtual void MoveToEnd(void* aHandle);
	virtual void SetAttr(void* aNode, TNodeAttr aType, const char* aVal);
	virtual void SetAttr(void* aNode, TNodeAttr aType, TNodeType aVal);
	virtual void SetAttr(void* aNode, TNodeAttr aType, TNodeAttr aVal);
	virtual void Dump(void* aNode, MLogRec* aLogRec);
	virtual void Save(const string& aFileName) const;
	virtual void* Find(const void* aHandle, const string& aUri);
    public:
	int GetAttrInt(void *aHandle, const char *aName);
	void* Set(const char* aFileName);
	void* Set(const string& aUri);
	void* Set(const void* aHandle);
	xmlDoc* Doc() { return iDoc;};
	static inline const char *Type() { return "ChromoMdlX";}; 
	virtual void* Init(TNodeType aRootType);
	void Reset();
    protected:
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
    private:
	xmlElementPtr GetElementDecl(void* aHandle);
	xmlElementContent* GetFirstEldeclSecCont(xmlElementPtr aElem);
	xmlElementContent* FindEldeclSecCont(xmlNodePtr aParent, xmlNodePtr aNode);
	xmlElementContent* GetNextEldeclSecCont(xmlElementContent* aContent);
	xmlElementContent* GetPrevEldeclSecCont(xmlElementContent* aContent);
	xmlNodePtr FindNodeEnterigPos(xmlNodePtr aParent, xmlNodePtr aNode);
    private:
	xmlDoc *iDoc;	// XML document
	TBool iDocOwned;
};


// XML based chromo
class ChromoX: public Chromo
{
    public:
	ChromoX();
	ChromoX(const char *aFileName);
	virtual ~ChromoX();
    public:
	// From MChromo
	virtual ChromoNode& Root();
	virtual const ChromoNode& Root() const;
	virtual void Set(const char *aFileName);
	virtual TBool Set(const string& aUri);
	virtual void Set(const ChromoNode& aRoot);
	virtual void Init(TNodeType aRootType);
	virtual void Reset();
	virtual void Save(const string& aFileName) const;
    private:
	ChromoMdlX iMdl;
	ChromoNode iRootNode;
};


#endif
