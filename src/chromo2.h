
#pragma once

#include <vector>
#include <list>

#include "mchromo.h"
#include "chromo.h"

using namespace std;

/** @brief Chromo2 mutation base
 * */
class C2Mut
{
    public:
	string mP; /*!< P object */
	string mQ; /*!< Q object */
	string mR; /*!< Relation */
};

/** @brief Chromo2 context unit
 * */
class C2MdlCtxNode
{
    public:
	C2MdlCtxNode(const string& aType, const string& aValue);
    public:
	string mType; /*!< Type of context */
	string mValue;/*!< Value of context */
    public:
	static const string KT_Target; /*!< Id of target context */
	static const string KT_Node; /*!< Id of node context */
	static const string KT_Namespace; /*!< Id of name space context */
};

/** @brief Chromo2 model unit
 * */
class C2MdlNode
{
    public:
	C2MdlNode();
	C2MdlNode(const C2MdlNode* aOwner);
    public:
	void AddContext(const string& aType, const string& aValue);
    public:
	const C2MdlNode* mOwner = NULL;
	vector<C2MdlCtxNode> mContext; /*!< Context */
	C2Mut mMut;                    /*!< Mutation */
	vector<C2MdlNode> mChromo;     /*!< Chromosome */
};

/** @brief Chromo2 model unit with mut
 * */
class C2MdlNodeMut: public C2MdlNode
{
    public:
	C2MdlNodeMut();
    protected:
	C2Mut mMut;                    /*!< Mutation */
};

class ChromoMdlX;

/** @brief Chromo2 model
 * */
class Chromo2Mdl: public Base, public MChromoMdl
{
    public:
	static inline const char *Type() { return "ChromoMdlX";}; 
    public:
	Chromo2Mdl();
	virtual ~Chromo2Mdl();
	// From MChromoMdl
	virtual TNodeType GetType(const THandle& aHandle);
	virtual TNodeType GetType(const string& aId);
	virtual THandle Root(const THandle& aHandle);
	virtual THandle Parent(const THandle& aHandle);
	virtual THandle Next(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle Prev(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetFirstChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetLastChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual char* GetAttr(const THandle& aHandle, TNodeAttr aAttr) const;
	virtual void  GetAttr(const THandle& aNode, TNodeAttr aType, TInt& aVal) const;
	virtual TBool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const ;
	virtual THandle AddChild(const THandle& aParent, TNodeType aType);
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy = ETrue, TBool aRecursively = ETrue);
	virtual THandle AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy = ETrue);
	virtual THandle AddNext(const THandle& aPrev, TNodeType aNode);
	virtual THandle AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy = ETrue);
	virtual void RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly = EFalse);
	virtual void Rm(const THandle& aHandle);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const char* aVal);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, TInt aVal);
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType);
	virtual void Dump(const THandle& aNode, MLogRec* aLogRec);
	virtual TBool ToString(const THandle& aNode, string& aString) const;
	virtual void Save(const string& aFileName) const;
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
	virtual THandle Init(TNodeType aRootType);
	void Reset();
    protected:
	// Helpers
	C2MdlNode CreateNodeMut(const THandle& aHandle, const C2MdlNode& aOwner, TNodeType aR, TNodeAttr aP, TNodeAttr aQ);
	C2MdlNode CreateNodeChr(const THandle& aHandle, const C2MdlNode& aOwner);
	/** @brief Processes X model node and transform it to the model nodes
	 * */
	void HandleXNode(const THandle& aHandle, C2MdlNode& aOwner);
	/** @brief Processes X model node attrs and transform it to the model node context
	 * */
	void HandleXNodeCtx(C2MdlNode& aMdlNode, const THandle& aHandle);
    protected:
	// From Base
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
    protected:
	ChromoMdlX *mMdlX;	/*!< Model XML document */
	C2MdlNode mRoot;
};

/** @brief Chromo2
 * */
class Chromo2: public MChromo
{
    public:
	Chromo2();
	Chromo2(const Chromo2& aSrc);
	Chromo2(const string& aFileName);
	virtual ~Chromo2();
    public:
	// From MChromo
	virtual ChromoNode& Root();
	virtual const ChromoNode& Root() const;
	virtual void SetFromFile(const string& aFileName);
	virtual TBool Set(const string& aUri);
	virtual TBool SetFromSpec(const string& aSpec);
	virtual TBool GetSpec(string& aSpec);
	virtual void Set(const ChromoNode& aRoot);
	virtual void Init(TNodeType aRootType);
	virtual void Reset();
	virtual void Save(const string& aFileName) const;
	virtual ChromoNode CreateNode(const THandle& aHandle);
	virtual void ReduceToSelection(const ChromoNode& aSelNode);

    protected:
	Chromo2Mdl mMdl;
	ChromoNode mRootNode;
};

