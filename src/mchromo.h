#ifndef __GRAYB_MCHROMO_H
#define __GRAYB_MCHROMO_H

#include "base.h"
#include <array>

class MLogRec;


enum TNodeType
{
    ENt_Unknown = 0,
    ENt_Node = 1,
    ENt_Move = 3, 	// Mutation - move node
    ENt_Rm = 4,   	// Mutation - removal
    ENt_Change = 5, 	// Change node attribute
    ENt_Cont = 6, 	// Change node content
    ENt_Import = 8, 	// Importing node
    ENt_Seg = 9, 	// Segment of chromo
};

enum TNodeAttr
{
    ENa_Unknown = 0,
    ENa_Id = 1,
    ENa_Parent = 2,
    ENa_Ref = 3,
    ENa_Order = 4,
    ENa_TOrder = 5,
    ENa_MutNode = 10,
    ENa_MutAttr = 11,
    ENa_MutVal = 12,
    ENa_Inactive = 13, // Sign of mutations being inactive, optimized out for instance
    ENa_Targ = 14, // Target node of mutation, is used in OSM mode, ref ds_mut_osm
    ENa_Comp = 15, // Component to be changed, for comps related muts only, is used in OSM mode, ref ds_mut_osm
    ENa_NS = 16,    //!< Name space
};


/** @brief Chromo error
 * */
class CError {
    public:
	CError() {};
	CError(streampos aPos, const string& aDescr): mPos(aPos), mText(aDescr) {};
    public:
	void Set(streampos aPos, const string& aDescr) {mPos = aPos, mText = aDescr; };
    public:
	streampos mPos;     //!< Position of last error
	string mText;       //!< Descrition of last error 
};

/** @brief Handle of Chromo model node
 * */
template<int L> class TTHandle
{
    public:
	TTHandle() {mData.fill(0);}
	TTHandle(const TTHandle& aSrc) { for (int i = 0; i < mData.size(); i++) mData[i] = aSrc.mData[i]; }
	template<class T> TTHandle(const T& aData) { __ASSERT(sizeof(T) <= L); T* pd = reinterpret_cast<T*>(mData.data()); *pd = aData;}
	template<class T> T Data(const T& aData) const { __ASSERT(sizeof(T) <= L); const T* pd = reinterpret_cast<const T*>(mData.data()); return *pd;}
	template<class T> T Data() const { __ASSERT(sizeof(T) <= L); const T* pd = reinterpret_cast<const T*>(mData.data()); return *pd;}
	bool operator ==(const TTHandle& aB) const {return mData == aB.mData;}
	bool operator !=(const TTHandle& aB) const {return !(*this == aB);}
    public:
	array<uint8_t, L> mData;
};

using THandle = TTHandle<8>;

// Interface of chromo model
class MChromoMdl
{
    public:
	virtual TNodeType GetType(const THandle& aHandle) = 0;
	virtual THandle Root(const THandle& aHandle) = 0;
	virtual THandle Parent(const THandle& aHandle) = 0;
	virtual THandle Next(const THandle& aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual THandle Prev(const THandle& aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual THandle GetFirstChild(const THandle& aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual THandle GetLastChild(const THandle& aHandle, TNodeType aType = ENt_Unknown) = 0;
	virtual string GetAttr(const THandle& aHandle, TNodeAttr aAttr) const = 0;
	virtual TBool AttrExists(const THandle& aHandle, TNodeAttr aAttr) const  = 0;
	virtual THandle AddChild(const THandle& aParent, TNodeType aType) = 0;
	virtual THandle AddChild(const THandle& aParent, const THandle& aHandle, TBool aCopy = ETrue, TBool aRecursively = ETrue) = 0;
	virtual THandle AddNext(const THandle& aPrev, const THandle& aHandle, TBool aCopy = ETrue) = 0;
	virtual THandle AddNext(const THandle& aPrev, TNodeType aNode) = 0;
	virtual THandle AddPrev(const THandle& aNext, const THandle& aHandle, TBool aCopy = ETrue) = 0;
	virtual void RmChild(const THandle& aParent, const THandle& aChild, TBool aDeattachOnly = EFalse) = 0;
	virtual void Rm(const THandle& aHandle) = 0;
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, const string& aVal) = 0;
	virtual void SetAttr(const THandle& aNode, TNodeAttr aType, TInt aVal) = 0;
	virtual void RmAttr(const THandle& aNode, TNodeAttr aType) = 0;
	//virtual void SetAttr(THandle aNode, TNodeAttr aType, TNodeType aVal) = 0;
	//virtual void SetAttr(THandle aNode, TNodeAttr aType, TNodeAttr aVal) = 0;
	virtual void Dump(const THandle& aNode) = 0;
	virtual void DumpBackTree(const THandle& aNode) = 0;
	virtual void DumpToLog(const THandle& aNode, MLogRec* aLogRec) = 0;
	virtual TBool ToString(const THandle& aNode, string& aString) const = 0;
	virtual void Save(const string& aFileName) const = 0;
	virtual THandle Find(const THandle& aHandle, const string& aUri) = 0;
	virtual TInt GetOrder(const THandle& aHandle, TBool aTree = EFalse) const = 0;
	virtual void DeOrder(const THandle& aHandle) = 0;
	virtual TInt GetLineId(const THandle& aHandle) const = 0;
};

class ChromoNode;

// Interface of chromosome
class MChromo
{
    public:
	virtual ~MChromo() {};
	virtual ChromoNode& Root() = 0;
	virtual const ChromoNode& Root() const= 0;
	virtual void SetFromFile(const string& aFileName) = 0;
	virtual TBool Set(const string& aUri) = 0;
	virtual TBool SetFromSpec(const string& aSpec) = 0;
	virtual TBool GetSpec(string& aSpec) = 0;
	virtual void Set(const ChromoNode& aRoot) = 0;
	virtual void Init(TNodeType aRootType) = 0;
	virtual void Reset() = 0;
	virtual void Save(const string& aFileName) const = 0;
	virtual ChromoNode CreateNode(const THandle& aHandle) = 0;
	virtual void ReduceToSelection(const ChromoNode& aSelNode) = 0;
	virtual bool IsError() const { return false;}
	virtual const CError& Error() const  = 0;
};

#endif
