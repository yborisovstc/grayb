
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
	C2Mut() {}
	C2Mut(const C2Mut& aSrc): mP(aSrc.mP), mQ(aSrc.mQ), mR(aSrc.mR) {}
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
	C2MdlNode(C2MdlNode* aOwner);
	C2MdlNode(const C2MdlNode& aSrc);
	C2MdlNode(const C2MdlNode& aSrc, C2MdlNode* aOwner);
    public:
	void CloneFrom(const C2MdlNode& aSrc);
	void AddContext(const string& aType, const string& aValue);
	C2MdlNode* GetNextComp(C2MdlNode* aComp);
	C2MdlNode* GetPrevComp(C2MdlNode* aComp);
	void BindTree(C2MdlNode* aOwner);
    public:
	C2MdlNode* mOwner = NULL;
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

using TC2MdlNodes = vector<C2MdlNode>;
using TC2MdlNodesIter = TC2MdlNodes::iterator;
using TC2MdlNodesCiter = TC2MdlNodes::const_iterator;
using TC2MdlNodesRIter = TC2MdlNodes::reverse_iterator;

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
	virtual THandle Root(const THandle& aHandle);
	virtual THandle Parent(const THandle& aHandle);
	virtual THandle Next(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle Prev(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetFirstChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual THandle GetLastChild(const THandle& aHandle, TNodeType aType = ENt_Unknown);
	virtual string GetAttr(const THandle& aHandle, TNodeAttr aAttr) const;
	virtual TBool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const ;
	virtual THandle AddChild(const THandle& aParent, TNodeType aType);
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy = ETrue, TBool aRecursively = ETrue);
	virtual THandle AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy = ETrue);
	virtual THandle AddNext(const THandle& aPrev, TNodeType aNode);
	virtual THandle AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy = ETrue);
	virtual void RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly = EFalse);
	virtual void Rm(const THandle& aHandle);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal);
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, TInt aVal);
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType);
	virtual void Dump(const THandle& aNode);
	virtual void DumpToLog(const THandle& aNode, MLogRec* aLogRec);
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
	const CError& Error() const { return mErr;};
    protected:
	/** @brief Parses chromo spec
	 * */
	void ParseChromo(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode);
	/** @brief Parses mutation
	 * */
	void ParseCnodeMut(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode);
	/** @brief Parses chromo node
	 * */
	void ParseCnodeChromo(istream& aIs, streampos aStart, streampos aEnd, C2MdlNode& aMnode);
	void ParseContext(vector<string>& aLexs, streampos aPos, C2MdlNode& aMnode);
	/** @brief Sets error */
	void SetErr(streampos);
    protected:
	/** @brief Dumps content of input stream fragment
	 * */
	static void DumpIsFrag(istream& aIs, streampos aStart, streampos aEnd);
	/** @brief Dumps model node
	 * */
	void DumpMnode(const C2MdlNode& aNode, int aLevel) const;
    protected:
	// From Base
	virtual MIface *DoGetObj(const char *aName) override { return NULL;}
    protected:
	C2MdlNode mRoot;
	CError mErr;           //!< Error data
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
	virtual bool IsError() const;
	virtual const CError& Error() const;

    protected:
	Chromo2Mdl mMdl;
	ChromoNode mRootNode;
};

