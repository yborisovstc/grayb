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

// Rank

class Rank: public vector<TInt>
{
    public:
	Rank(): vector<TInt>() {};
	string ToString() const;
//	TBool IsEqual(const Rank& aArg) const;
	TInt Compare(const Rank& aArg) const;
	TBool operator==(const Rank& aArg) const {return Compare(aArg) == 0;};
	TBool operator<(const Rank& aArg) const {return Compare(aArg) == -1;};
	TBool operator>(const Rank& aArg) const {return Compare(aArg) == 1;};
};


// Element of native hier - mutable
class Elem: public Base, public MMutable, public MCompsObserver, public MChildsObserver
{
    public:
	// name, parent
	typedef pair<string, string> TCkey;
	typedef pair<TCkey, Elem*> TCelem;
	typedef multimap<TCkey, Elem*> TMElem;
	// Register of heirs
	typedef pair<string, Elem*> TNKey;
	// Regiser keyed by name, multimap
	typedef multimap<string, Elem*> TNMReg;
	// Rank of node
	typedef vector<TInt> TRank;

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
	    TMElem::iterator iCIter; // Comps iter
	    pair<TMElem::iterator, TMElem::iterator> iCIterRange;
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
	void SetEType(const string& aPName, const string& aPEType = string());
	void SetParent(const string& aParent);
	void SetMan(Elem* aMan);
	void SetObserver(MCompsObserver* aObserver);
	void SetMutation(const ChromoNode& aMuta);
	void AppendMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
	string PName() const;
	const vector<Elem*>& Comps() const;
	static TICacheRCtx ToCacheRCtx(const RqContext* aCtx);
    public:
	// aInitCont - the uri of initial context of element, this is requires to element "understand" that it is
	// in new context now and corrected uris related to initial context
	Elem* CreateHeir(const string& aName, Elem* aMan/*, const GUri& aInitCont*/);
	const MChromo& Chromos() { return *iChromo;};
	MChromo& Mutation() { return *iMut;};
	// Gets the comp with given type and owning given element
	Elem* GetCompOwning(const string& aParent, Elem* aElem);
	Elem* GetCompOwning(Elem* aElem);
	// Gets the comp with given name and owning given element
	Elem* GetCompOwningN(const string& aParent, Elem* aElem);
	Elem* GetRoot() const;
	Elem* GetInhRoot() const;
	TBool IsComp(Elem* aElem);
	// Checks if elements chromo is attached. Ref UC_019 for details
	TBool IsChromoAttached() const;
	Elem* GetAttachingMgr();
	// Debug helpers
	Elem* GetNodeS(const char* aUri);
	TBool IsName(const char* aName);
	TBool IsHeirOf(const string& aParent) const;
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	void GetUri(GUri& aUri, Elem* aTop = NULL);
	string GetUri(Elem* aTop = NULL);
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
	void GetRank(Rank& aRank);
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
	virtual TBool AddNode(const ChromoNode& aSpec);
	TBool AppendChild(Elem* aChild);
	void RemoveChild(Elem* aChild);
	virtual TBool RmNode(const GUri& aUri);
	virtual TBool MoveNode(const ChromoNode& aSpec);
	vector<Elem*>& Comps();
	// From MChildsObserver
	virtual void OnChildDeleting(Elem* aChild);
	virtual TBool OnChildRenamed(Elem* aChild, const string& aOldName);
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	virtual TBool OnCompRenamed(Elem& aComp, const string& aOldName);
	virtual void OnContentChanged(Elem& aComp);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
	void RmIfCache(IfIter& aIt);
	void InvalidateIfCache();
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, Base* aProv, TIfRange aRg);
    protected:
	Elem* AddElem(const ChromoNode& aSpec);
	static void Init();
	inline MLogRec* Logger() const;
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	TBool MoveComp(Elem* aComp, Elem* aDest);
	TBool RegisterComp(Elem* aComp);
	TBool RegisterChild(Elem* aChild);
	TBool IsCompRegistered(Elem* aComp) const;
	// aName is required because the comp can be renamed already. This is the case of 
	// comp renaming: comp is renamed first, then the renaming is handled
	TBool UnregisterComp(Elem* aComp, const string& aName = string());
	TBool UnregisterChild(Elem* aChild, const string& aName = string());
	Elem* GetComp(const string& aParent, const string& aName);
	TBool DoMutChangeCont(const ChromoNode& aSpec);
	TBool MergeMutation(const ChromoNode& aSpec);
	TBool MergeMutMove(const ChromoNode& aSpec);
	Elem* GetComp(TInt aInd);
	virtual void DoOnCompChanged(Elem& aComp);
	TBool IsLogeventCreOn();
	void ChangeAttr(const ChromoNode& aSpec);
    protected:
	// Element type - parent's chain
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
	multimap<TCkey, Elem*> iMComps;
	// Ifaces cache
	TICache iICache;
	// Ifaces cache query by TICMapKeyF
	TICacheQF iICacheQF;
	// Children register
	TNMReg iChilds;
	// Parent
	Elem* iParent;
	
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
