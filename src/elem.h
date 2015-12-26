#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H


#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"
#include <time.h>

class Chromo;
class MProvider;

// Standard element of native hier
class Elem: public MElem
{
    public:

	// Regiser keyed by name, multimap
	typedef string TNMKey;
	typedef pair<TNMKey, MElem*> TNMVal;
	typedef multimap<string, MElem*> TNMReg;
	typedef pair<TNMReg::iterator, TNMReg::iterator> TNMRegItRange;
	// Relation chromo to model
	typedef pair<MElem*, TNodeAttr> TCMRelTo;
	typedef pair<void*, TNodeAttr> TCMRelFrom;
	typedef pair<TCMRelFrom, MElem*> TCMRel;
	typedef map<TCMRelFrom, MElem*> TCMRelReg;

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
	class IfIter: public MIfIter
    {
	friend class Elem;
	public:
	IfIter(): iHost(NULL) {};
	IfIter(Elem* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd = EFalse);
	IfIter(const IfIter& aIt);
	virtual MIfIter* Clone() const;
	virtual MIfIter& operator=(const MIfIter& aIt);
	virtual MIfIter& operator++();
	virtual TBool operator==(const MIfIter& aIt);
	virtual void*  operator*();
	public:
	Elem* iHost;
	string iIName; // Iface name
	TICacheRCtx iReq; // Requestor
	TICacheQFRange iQFRange; // Range of  cache query by TICMapKeyF
	TICacheQFIter iQFIter; // Query result current iterator
	TICacheRange iCacheRange;
	TICacheIter iCacheIter; // Cache current iterator
    };

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
	    virtual MElem*  GetElem();
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
	MElem*  operator*() { return iImpl->GetElem();};
	public:
	IterImplBase* iImpl;
    };

    public:
	// Formatter
	class Fmt 
	{
	    public:
	    // Separator of content inputs info
	    static string mSepContInp;
	    // Separator of parts of content name
	    static string mSepContName;
	};

    public:
	static const char* Type() { return "Elem";};
	virtual const string& Name() const { return iName;}
	// Predefined extended type, for core elements only
	static string PEType();
	Elem(const string &aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	Elem(Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~Elem();
	virtual void SetParent(const string& aParent);
	virtual void SetParent(MElem* aParent);
	virtual void SetMan(MElem* aMan);
	virtual void SetObserver(MCompsObserver* aObserver);
	virtual void SetMutation(const ChromoNode& aMuta);
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta);
	virtual TBool AppendMutation(const string& aFileName);
	string PName() const;
	static void ToCacheRCtx(const RqContext* aCtx, TICacheRCtx& aCct);
    public:
	virtual MElem* CreateHeir(const string& aName, MElem* aMan);
	virtual const MChromo& Chromos() const { return *iChromo;};
	virtual MChromo& Chromos() { return *iChromo;};
	virtual MChromo& Mutation() { return *iMut;};
	// Gets the comp with given type and owning given element
	virtual MElem* GetCompOwning(const string& aParent, MElem* aElem);
	virtual MElem* GetCompOwning(MElem* aElem);
	// Gets acomp that attaches the given node or node itself if this is attaching it
	MElem* GetAcompAttaching(MElem* aElem);
	virtual MElem* GetRoot() const;
	virtual MElem* GetInhRoot() const;
	virtual MElem* GetCommonOwner(MElem* aElem);
	MElem* GetCommonAowner(MElem* aElem);
	virtual TBool IsAownerOf(const MElem* aElem) const;
	// Checks if elements chromo is attached. Ref UC_019 for details
	virtual TBool IsChromoAttached() const;
	TBool IsMutAttached(const ChromoNode& aMut) const;
	virtual MElem* GetAttachingMgr();
	virtual const MElem* GetAttachingMgr() const;
	virtual MElem* GetAowner();
	virtual const MElem* GetAowner() const;
	virtual MElem* GetAcompOwning(MElem* aComp);
	// Checks if the node is originated via phenotypical modification
	virtual TBool IsPhenoModif() const;
	virtual TBool IsInheritedComp(const MElem* aNode) const;
	TBool IsDirectInheritedComp(const MElem* aNode) const;
	TBool IsCompOfInheritedComp(const MElem* aNode) const;
	// Debug helpers
	Elem* GetNodeS(const char* aUri);
	TBool IsName(const char* aName);
	virtual TBool IsHeirOf(const string& aParent) const;
	void RebaseUriToOuterNode(MElem* aOldBase, const GUri& aUri, GUri& aResult);
	virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	// From MIfProv Iface provider
	virtual void* GetSIfiC(const string& aName, Base* aRequestor = NULL);
	virtual void* GetSIfi(const string& aName, const RqContext* aCtx = NULL);
	virtual void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue);
	virtual TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual const string EType(TBool aShort = ETrue) const;
	virtual const string& EName() const { return Name();};
	virtual MElem* GetMan();
	virtual const MElem* GetMan() const;
	virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const;
	virtual void GetLRank(Rank& aRank, TBool aCur = EFalse) const; 
	TInt GetLocalRank() const;
	virtual MElem* GetNode(const string& aUri);
	virtual MElem* GetNode(const GUri& aUri);
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse);
	virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue);
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue);
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MElem* aTop = NULL) const;
	virtual void GetRUri(GUri& aUri, MElem* aTop = NULL);
	virtual string GetUri(MElem* aTop = NULL, TBool aShort = EFalse);
	virtual string GetRUri(MElem* aTop = NULL);
	virtual TBool RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes);
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes);
	// TODO [YB] The only attr allowed for change is name. To consider replacing of ChangeAttr to Rename 
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual void GetCont(string& aCont, const string& aName = string()); 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	virtual TInt GetContCount() const;
	virtual MElem* GetUpperAowner();
	virtual TBool IsRemoved() const;
	virtual void SetRemoved();
	// Debugging
	virtual void GetContactsData(vector<string, string>& aData) const;
	virtual TInt GetCapacity() const;
	// Nodes
	virtual TBool AppendChild(MElem* aChild);
	virtual void RemoveChild(MElem* aChild);
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	virtual TBool ImportNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	virtual vector<MElem*>& Comps();
	virtual const vector<MElem*>& Comps() const;
	// From MChild
	virtual MElem* GetParent();
	virtual const MElem* GetParent() const;
	virtual void OnParentDeleting(MElem* aParent);
	// From MParent
	virtual void OnChildDeleting(MElem* aChild);
	virtual TBool OnChildRenamed(MElem* aChild, const string& aOldName);
	// From MOwner
	virtual TBool IsComp(const MElem* aElem) const;
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual void OnCompAdding(MElem& aComp);
	virtual TBool OnCompChanged(MElem& aComp);
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	virtual TBool OnContentChanged(MElem& aComp);
	virtual TBool MoveComp(MElem* aComp, MElem* aDest);
	virtual TBool MoveComp(MElem* aComp, const ChromoNode& aDest);
	// Gets major dep for referenced node, ref ds_indp_mutord_impl
	virtual void GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse);
	// Resolve owned mutation unsafety via changing mutation position
	virtual TBool ResolveMutUnsafety(MElem* aMutated, const TMDep& aDep);
	virtual TBool ResolveMutsUnsafety();
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx);
	void RmIfCache(IfIter& aIt);
	void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx);
	void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, Elem* aProv, TBool aInv = EFalse);
	void InvalidateIfCache();
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg);
	void InsertIfCache(const string& aName, const RqContext* aCtx, Base* aProv, TIfRange aRg);
	// Deps
	TBool IsMutSafe(MElem* aRef);
	TMDep GetRefDep(MElem* aRef, TNodeAttr aReftype, MElem* aObj = NULL);
	virtual TBool IsRefSafe(MElem* aRef, TNodeAttr aReftype, MElem* aObj = NULL, TMDep* aDep = NULL);
	TBool IsForwardRef(MElem* aRef);
	virtual TMDeps& GetMDeps() { return iMDeps;};
	virtual void AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr);
	virtual void RemoveMDep(const TMDep& aDep, const MElem* aContext = NULL);
	void RmMCDeps();
	static void GetDepRank(const TMDep& aDep, Rank& aRank);
	static TBool IsDepActive(const TMDep& aDep);
	// Adding two directions chromo-model dependencies
	virtual void AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode);
	virtual TBool RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext = NULL);
	void RmCMDep(const ChromoNode& aMut);
	void RmCMDeps();
	virtual void GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly = EFalse, TBool aAnyType = EFalse) const;
	void GetDepRank(Rank& aRank, TNodeAttr aAttr);
	virtual TMDep GetMajorDep();
	virtual void GetMajorDep(TMDep& aDep, TBool aUp = EFalse, TBool aDown = ETrue);
	virtual TMDep GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel);
	virtual void GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp = ETrue, TBool aDown = ETrue);
	// Chromo
	ChromoNode GetChNode(const GUri& aUri) const;
	virtual TBool CompactChromo();
	virtual TBool CompactChromo(const ChromoNode& aNode);
	void UndoCompactChromo();
	inline MLogRec* Logger() const;
	inline TBool IsIftEnabled() const;
	// Transformations
	virtual TBool HasParentModifs() const;
	virtual void CopyModifsFromParent();
	virtual TBool HasModifs(const MElem* aOwner) const;
	virtual void CopyParentModifsToComp(MElem* aComp);
	virtual TBool RebaseUriToIntNode(const GUri& aUri, const MElem* aComp, GUri& aResult);
	// Utils
	void LogComps() const;
	Elem* GetNodeE(const string& aUri) {return ToElem(GetNode(aUri));};
	Elem* GetNodeE(const GUri& aUri) {return ToElem(GetNode(aUri));};
    protected:
	static Elem* ToElem(MElem* aMelem) { Elem* res = (aMelem == NULL) ? NULL: aMelem->GetObj(res); return res;};
	inline MProvider* Provider() const;
	virtual TBool AppendComp(MElem* aComp);
	TBool RegisterComp(MElem* aComp);
	TBool RegisterChild(MElem* aChild);
	TBool IsCompRegistered(MElem* aComp);
	// aName is required because the comp can be renamed already. This is the case of 
	// comp renaming: comp is renamed first, then the renaming is handled
	TBool UnregisterComp(MElem* aComp, const string& aName = string());
	TBool UnregisterChild(MElem* aChild, const string& aName = string());
	MElem* GetComp(const string& aParent, const string& aName);
	TBool MergeMutation(const ChromoNode& aSpec);
	TBool MergeMutMove(const ChromoNode& aSpec);
	virtual void DoOnCompChanged(Elem& aComp);
	TBool IsLogeventCreOn();
	TBool HasChilds() const;
	virtual TBool HasInherDeps(const MElem* aScope) const;
	void InsertIfQm(const string& aName, const TICacheRCtx& aReq, Base* aProv);
	void UnregAllIfRel(TBool aInv = EFalse);
	// ICache helpers, for debug only
	Elem* GetIcCtxComp(const TICacheRCtx& aCtx, TInt aInd);
	void LogIfReqs();
	// Chromo modification/repairing utilities
	TBool ShiftComp(MElem* aComp, MElem* aDest = NULL);
	TBool ShiftCompOverDep(MElem* aComp, const TMDep& aDep);
	virtual ChromoNode GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const;
	// Profiling
	static void Delay(long us);
	static long GetClockElapsed(const timespec& aStart, timespec& aEnd);
	static long GetClockElapsed(long aStart);
	static long GetClock();
	// Debugging
	MElem* GetComp(TInt aInd);
    protected:
	// Environment
	MEnv* iEnv;
	// Managing (higher) element of hier
	MElem* iMan;
	// Observer, mostly for root - normally elem notifies to Mgr
	MCompsObserver* iObserver;
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
	// Components, owninig container
	vector<MElem*> iComps;
	// Components map, not owning
	TNMReg iMComps;
	// Ifaces cache
	TICache iICache;
	// Ifaces cache query by TICMapKeyF
	TICacheQF iICacheQF;
	// Children register
	TNMReg iChilds;
	// Parent
	MElem* iParent;
	// Dependent nodes, relations to keep model consistent on mutations, ref uc_028, ds_mut
	TMDeps iMDeps;
	// Mutation to model node relation, required for chromo squeezing, ref ds_mut_sqeezing
	TCMRelReg iCMRelReg;
	// Sign of that node is removed
	TBool isRemoved;
	string iName;
	static TBool EN_PERF_TRACE;
	static TBool EN_PERF_METR;
	static TBool EN_MUT_LIM;
	static TBool EN_PERF_DBG1;
};

inline MLogRec* Elem::Logger() const {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }

inline TBool Elem::IsIftEnabled() const { return iEnv ? iEnv->GetSBool(MEnv::ESb_EnIfTrace): EFalse;};

#endif
