#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H


#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"

class Chromo;
class MProvider;

// Element of native hier - mutable
class Elem: public Base, public MMutable, public MCompsObserver, public MChildsObserver, 
    public MChild
{
    public:
	// Regiser keyed by name, multimap
	typedef string TNMKey;
	typedef pair<TNMKey, Elem*> TNMVal;
	typedef multimap<string, Elem*> TNMReg;
	// Ref to mutation
	typedef pair<Elem*, void*> TMutRef;
	// Rank of node
	typedef vector<TInt> TRank;
	// Relation chromo to model
	typedef pair<Elem*, TNodeAttr> TCMRelTo;
	typedef pair<void*, TNodeAttr> TCMRelFrom;
	typedef pair<TCMRelFrom, Elem*> TCMRel;
	typedef map<TCMRelFrom, Elem*> TCMRelReg;
	// Deps Mutations on RT model node
	// Elem of deps mutation on RT node: chromo node handle, type of dep
	typedef pair<TMutRef, TNodeAttr> TMDep;
	typedef vector<TMDep> TMDeps;
	

    public:
	// Request context
	typedef vector<Base*> TICacheRCtx;
	// Iface cache key first part: [iface name, requestor]
	typedef pair<string, TICacheRCtx> TICacheKeyF;
	// Iface cache key: [[iface name, requestor], provider]
	typedef pair<TICacheKeyF, Base*> TICacheKey;
	// Iface cache
	typedef multimap<TICacheKey, void*> TICache;
	// Iface cache query by TICMapKeyF (iface, requestor)
	typedef multimap<TICacheKeyF, TICacheKey> TICacheQF;
	// Iface cache iterator
	typedef TICache::iterator TICacheIter;
	// Iface cache const iterator
	typedef TICache::const_iterator TICacheCIter;
	// Iface cache query F iterator
	typedef TICacheQF::iterator TICacheQFIter;
	// Iface cache range
	typedef pair<TICacheIter, TICacheIter> TICacheRange;
	typedef pair<TICacheCIter, TICacheCIter> TICacheCRange;
	typedef pair<TICacheQFIter, TICacheQFIter> TICacheQFRange;

    public:
	class IfIter: public iterator<input_iterator_tag, void*>
    {
	friend class Elem;
	public:
	IfIter(): iHost(NULL) {};
	IfIter(Elem* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd = EFalse);
	IfIter(const IfIter& aIt);
	IfIter& operator=(const IfIter& aIt);
	IfIter& operator++();
	IfIter operator++(int) { IfIter tmp(*this); operator++(); return tmp; };
	TBool operator==(const IfIter& aIt);
	TBool operator!=(const IfIter& aIt) { return !operator==(aIt);};
	void*  operator*();
	public:
	Elem* iHost;
	string iIName; // Iface name
	TICacheRCtx iReq; // Requestor
	TICacheQFRange iQFRange; // Range of  cache query by TICMapKeyF
	TICacheQFIter iQFIter; // Query result current iterator
	TICacheRange iCacheRange;
	TICacheIter iCacheIter; // Cache current iterator

    };

	typedef pair<IfIter, IfIter> TIfRange;


    public:
	class IterImplBase
	{
	    friend class Elem;
	    public:
	    static const char* Type() { return "IterImplBase";};
	    IterImplBase(Elem& aElem, GUri::TElem aId, TBool aToEnd = EFalse);
	    IterImplBase(Elem& aElem);
	    IterImplBase(const IterImplBase& aImpl);
	    char SRel() const;
	    virtual void Set(const IterImplBase& aImpl);
	    virtual void PostIncr();
	    virtual TBool IsEqual(const IterImplBase& aImpl) const;
	    virtual TBool IsCompatible(const IterImplBase& aImpl) const;
	    virtual Elem*  GetElem();
	    virtual void* DoGetObj(const char *aName);
	    virtual const void* DoGetObj(const char *aName) const;
	    public:
	    Elem& iElem;
	    GUri::TElem iId;
	    char iExtsrel;
	    string iExt;
	    TNMReg::iterator iCIter; // Comps iter
	    pair<TNMReg::iterator, TNMReg::iterator> iCIterRange;
	    TNMReg::iterator iChildsIter;
	    pair<TNMReg::iterator, TNMReg::iterator> iChildsRange;
	};

	class Iterator: public iterator<input_iterator_tag, Elem*>
    {
	friend class Elem;
	public:
	Iterator(): iImpl(NULL) {};
	Iterator(IterImplBase* aImpl): iImpl(aImpl) {};
	~Iterator() { delete iImpl;};
	Iterator(const Iterator& aIt) { iImpl = new IterImplBase(*(aIt.iImpl));};
	Iterator& operator=(const Iterator& aIt) { iImpl->Set(*(aIt.iImpl)); return *this;};
	Iterator& operator++() { iImpl->PostIncr(); return *this;};
	Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; };
	TBool operator==(const Iterator& aIt) { return iImpl->IsEqual((*aIt.iImpl));};
	TBool operator!=(const Iterator& aIt) { return !operator==(aIt);};
	Elem*  operator*() { return iImpl->GetElem();};
	public:
	IterImplBase* iImpl;
    };


    public:
	static const char* Type() { return "Elem";};
	// Predefined extended type, for core elements only
	static string PEType();
	Elem(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem(Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem* GetNode(const GUri& aUri);
	virtual ~Elem();
	TBool IsRemoved() const;
	virtual void SetRemoved();
	void SetEType(const string& aPName, const string& aPEType = string());
	void SetParent(const string& aParent);
	void SetMan(Elem* aMan);
	void SetObserver(MCompsObserver* aObserver);
	void SetMutation(const ChromoNode& aMuta);
	void AppendMutation(const ChromoNode& aMuta);
	TBool AppendMutation(const string& aFileName);
	void Mutate(TBool aRunTimeOnly = EFalse);
	string PName() const;
	const vector<Elem*>& Comps() const;
	static void ToCacheRCtx(const RqContext* aCtx, TICacheRCtx& aCct);
    public:
	// aInitCont - the uri of initial context of element, this is requires to element "understand" that it is
	// in new context now and corrected uris related to initial context
	Elem* CreateHeir(const string& aName, Elem* aMan/*, const GUri& aInitCont*/);
	const MChromo& Chromos() const { return *iChromo;};
	MChromo& Chromos() { return *iChromo;};
	MChromo& Mutation() { return *iMut;};
	// Gets the comp with given type and owning given element
	Elem* GetCompOwning(const string& aParent, Elem* aElem);
	Elem* GetCompOwning(Elem* aElem);
	// Gets the comp with given name and owning given element
	Elem* GetCompOwningN(const string& aParent, Elem* aElem);
	Elem* GetRoot() const;
	Elem* GetInhRoot() const;
	TBool IsComp(Elem* aElem);
	Elem* GetCommonOwner(Elem* aElem);
	// Checks if elements chromo is attached. Ref UC_019 for details
	TBool IsChromoAttached() const;
	Elem* GetAttachingMgr();
	const Elem* GetAttachingMgr() const;
	// Checks if the node is originated vis phenotypical modification
	TBool IsPhenoModif() const;
	TBool IsInheritedComp(const Elem* aNode) const;
	TBool IsCompOfInheritedComp(const Elem* aNode) const;
	// Debug helpers
	Elem* GetNodeS(const char* aUri);
	TBool IsName(const char* aName);
	TBool IsHeirOf(const string& aParent) const;
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	void GetUri(GUri& aUri, Elem* aTop = NULL);
	void GetRUri(GUri& aUri, Elem* aTop = NULL);
	string GetUri(Elem* aTop = NULL);
	string GetRUri(Elem* aTop = NULL);
	virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	// Iface provider
	void* GetSIfiC(const string& aName, Base* aRequestor);
	void* GetSIfi(const string& aName, const RqContext* aCtx = NULL);
	void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue);
	TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MElem
	virtual const string EType(TBool aShort = ETrue) const;
	virtual const set<string>& CompsTypes();
	virtual Elem* GetMan();
	virtual const Elem* GetMan() const;
	Elem* GetParent();
	void GetRank(Rank& aRank, const Elem* aBase = NULL) const;
	void GetRank(Rank& aRank, const ChromoNode& aMut) const;
	void GetLRank(Rank& aRank, TBool aCur = EFalse) const; 
	TInt GetLocalRank() const;
	const Elem* GetParent() const;
	void SetParent(Elem* aParent);
	virtual Elem* GetNode(const string& aUri);
	virtual Elem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	// TODO [YB] The only attr allowed for change is name. To consider replacing
	// of ChangeAttr to Rename 
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	virtual void GetCont(string& aCont); 
	//virtual TBool ChangeCont(const string& aVal); 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue); 
	virtual TBool AddNode(const ChromoNode& aSpec, TBool aRunTime);
	TBool AppendChild(Elem* aChild);
	void RemoveChild(Elem* aChild);
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime);
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime);
	vector<Elem*>& Comps();
	// From MChild
	virtual void OnParentDeleting(Elem* aParent);
	// From MChildsObserver
	virtual void OnChildDeleting(Elem* aChild);
	virtual TBool OnChildRenamed(Elem* aChild, const string& aOldName);
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual TBool OnCompChanged(Elem& aComp);
	virtual TBool OnCompRenamed(Elem& aComp, const string& aOldName);
	virtual TBool OnContentChanged(Elem& aComp);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
	void RmIfCache(IfIter& aIt);
	void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx);
	void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, Elem* aProv, TBool aInv = EFalse);
	void InvalidateIfCache();
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg);
	void InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, TIfRange aRg);
	// Deps
	Elem* GetMajorChild(Rank& rr);
	void GetMajorChild(Elem*& aElem, Rank& rr);
	TBool IsMutSafe(Elem* aRef);
	TBool IsRefSafe(Elem* aRef);
	TMDeps& GetMDeps() { return iMDeps;};
	void AddMDep(Elem* aNode, const ChromoNode& aMut, TNodeAttr aAttr);
	void RemoveMDep(const TMDep& aDep);
	static void GetDepRank(const TMDep& aDep, Rank& aRank);
	// Adding two directions chromo-model dependencies
	void AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, Elem* aNode);
	TBool RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr);
	void RmCMDep(const ChromoNode& aMut);
	Elem* GetCMDep(const ChromoNode& aMut, TNodeAttr aAttr) const;
	void GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly = EFalse) const;
	void GetDepRank(Rank& aRank, TNodeAttr aAttr);
	TMDep GetMajorDep();
	void GetMajorDep(TMDep& aDep, TBool aUp = EFalse, TBool aDown = ETrue);
	TMDep GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel);
	void GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp = ETrue, TBool aDown = ETrue);
	// Chromo
	ChromoNode GetChNode(const GUri& aUri) const;
	void CompactChromo();
	void CompactChromo(const ChromoNode& aNode);
    protected:
	Elem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse);
	static void Init();
	inline MLogRec* Logger() const;
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	TBool RegisterComp(Elem* aComp);
	TBool RegisterChild(Elem* aChild);
	TBool MoveComp(Elem* aComp, Elem* aDest);
	TBool MoveComp(Elem* aComp, const ChromoNode& aDest);
	TBool IsCompRegistered(Elem* aComp);
	// aName is required because the comp can be renamed already. This is the case of 
	// comp renaming: comp is renamed first, then the renaming is handled
	TBool UnregisterComp(Elem* aComp, const string& aName = string());
	TBool UnregisterChild(Elem* aChild, const string& aName = string());
	Elem* GetComp(const string& aParent, const string& aName);
	TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime);
	TBool MergeMutation(const ChromoNode& aSpec);
	TBool MergeMutMove(const ChromoNode& aSpec);
	virtual void DoOnCompChanged(Elem& aComp);
	TBool IsLogeventCreOn();
	void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime);
	TBool HasChilds() const;
	TBool HasInherDeps() const;
	void InsertIfQm(const string& aName, const TICacheRCtx& aReq, Base* aProv);
	void UnregAllIfRel(TBool aInv = EFalse);
	// ICache helpers, for debug only
	Elem* GetIcCtxComp(const TICacheRCtx& aCtx, TInt aInd);
	void LogIfReqs();
	// Chromo modification/repairing utilities
	TBool ShiftComp(Elem* aComp, Elem* aDest = NULL);
	TBool ShiftCompOverDep(Elem* aComp, const TMDep& aDep);
    protected:
	// Element type - parent's chain
	// TODO [YB] Is it needed now after implementing inheritance chain?
	string iEType;
	// Environment
	MEnv* iEnv;
	// Managing (higher) element of hier
	Elem* iMan;
	// Observer, mostly for root - normally elem notifies to Mgr
	MCompsObserver* iObserver;
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
	// Sign of inited
	static bool iInit;
	// Components types
	static set<string> iCompsTypes;
	// Components, owninig container
	vector<Elem*> iComps;
	// Components map, not owning
	TNMReg iMComps;
	// Ifaces cache
	TICache iICache;
	// Ifaces cache query by TICMapKeyF
	TICacheQF iICacheQF;
	// Children register
	TNMReg iChilds;
	// Parent
	Elem* iParent;
	// Dependent nodes, relations to keep model consistent on mutations, ref uc_028, ds_mut
	TMDeps iMDeps;
	// Mutation to model node relation, required for chromo squeezing, ref ds_mut_sqeezing
	TCMRelReg iCMRelReg;
	// Sign of that node is removed
	TBool isRemoved;
};

inline MLogRec* Elem::Logger() const {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }


// Runtime agent - implements interfaces that can be used by the man

class Agent: public Elem
{
    public:
	static const char* Type() { return "Agent";};
	Agent(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

#endif
