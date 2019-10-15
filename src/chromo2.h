
#pragma once

#include "vector"

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
    public:
	void AddContext(const string& aType, const string& aValue);
    public:
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
	virtual TNodeType GetType(const void* aHandle);
	virtual TNodeType GetType(const string& aId);
	virtual void* Root(const void* aHandle);
	virtual void* Parent(const void* aHandle);
	virtual void* Next(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* NextText(const void* aHandle);
	virtual void* Prev(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* GetFirstChild(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual void* GetLastChild(const void* aHandle, TNodeType aType = ENt_Unknown);
	virtual char* GetAttr(const void* aHandle, TNodeAttr aAttr) const;
	virtual void  GetAttr(const void* aNode, TNodeAttr aType, TInt& aVal) const;
	virtual TBool AttrExists(const void* aHandle, TNodeAttr aAttr) const ;
	virtual void* AddChild(void* aParent, TNodeType aType);
	virtual void* AddChild(void* aParent, const void* aHandle, TBool aCopy = ETrue, TBool aRecursively = ETrue);
	virtual void* AddChildDef(void* aParent, const void* aHandle, TBool aCopy = ETrue);
	virtual void* AddNext(const void* aPrev, const void* aHandle, TBool aCopy = ETrue);
	virtual void* AddNext(const void* aPrev, TNodeType aNode);
	virtual void* AddPrev(const void* aNext, const void* aHandle, TBool aCopy = ETrue);
	virtual void RmChild(void* aParent, void* aChild, TBool aDeattachOnly = EFalse);
	virtual void Rm(void* aHandle);
	virtual void MoveNextTo(void* aHandle, void* aDest);
	virtual void MovePrevTo(void* aHandle, void* aDest);
	virtual void MoveToEnd(void* aHandle);
	virtual void SetAttr(void* aNode, TNodeAttr aType, const char* aVal);
	virtual void SetAttr(void* aNode, TNodeAttr aType, TInt aVal);
	virtual void RmAttr(void* aNode, TNodeAttr aType);
	virtual void Dump(void* aNode, MLogRec* aLogRec);
	virtual TBool ToString(void* aNode, string& aString) const;
	virtual void Save(const string& aFileName) const;
	virtual void* Find(const void* aHandle, const string& aUri);
	virtual TInt GetOrder(void* aHandle, TBool aTree = EFalse) const;
	virtual void SetOrder(void* aHandle, TInt aOrder, TBool aTree = EFalse);
	virtual void DeOrder(void* aHandle);
	virtual TInt GetLineId(void* aHandle) const;
    public:
	int GetAttrInt(void *aHandle, const char *aName);
	void* SetFromFile(const string& aFileName);
	void* Set(const string& aUri);
	void* SetFromSpec(const string& aSpec);
	void* Set(const void* aHandle);
	virtual void* Init(TNodeType aRootType);
	void Reset();
    protected:
	// Helpers
	C2MdlNode CreateNodeMut(const void* aHandle, TNodeType aR, TNodeAttr aP, TNodeAttr aQ);
	C2MdlNode CreateNodeChr(const void* aHandle);
	/** @brief Processes X model node and transform it to the model nodes
	 * */
	void HandleXNode(const void* aHandle);
	/** @brief Processes X model node attrs and transform it to the model node context
	 * */
	void HandleXNodeCtx(C2MdlNode& aMdlNode, const void* aHandle);
    protected:
	// From Base
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
    protected:
	ChromoMdlX *mMdlX;	/*!< Model XML document */
	vector<C2MdlNode> mNodes;
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
	virtual ChromoNode CreateNode(void* aHandle);
	virtual void ReduceToSelection(const ChromoNode& aSelNode);

    protected:
	Chromo2Mdl mMdl;
	ChromoNode mRootNode;
};

