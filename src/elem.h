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
class Elem: public Base, public MMutable, public MCompsObserver
{
    public:
	// name, parent
	typedef pair<string, string> TCkey;
	typedef pair<TCkey, Elem*> TCelem;
	typedef multimap<TCkey, Elem*> TMElem;

    public:
	// Iface cache key first part: [iface name, requestor]
	typedef pair<string, Base*> TICacheKeyF;
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
	IfIter(Elem* aHost, const string& aIName, Base* aReq, TBool aToEnd = EFalse);
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
	Base* iReq; // Requestor
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
	Elem(const string &aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	Elem* GetNode(const GUri& aUri);
	virtual ~Elem();
	void SetEType(const string& aEType);
	void SetParent(const string& aParent);
	void SetMan(Elem* aMan);
	void SetMutation(const ChromoNode& aMuta);
	void Mutate(TBool aRunTimeOnly = EFalse);
	string PName() const;
	const vector<Elem*>& Comps() const;
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
	TBool IsComp(Elem* aElem);
	// Debug helpers
	Elem* GetNodeS(const char* aUri);
	TBool IsName(const char* aName);
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	void GetUri(GUri& aUri, Elem* aTop = NULL);
	virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem);
	virtual Iterator NodesLoc_End(const GUri::TElem& aElem);
	// Iface provider
	void* GetSIfi(const string& aName, const RqContext* aCtx = NULL);
	void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue);
	TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// From MElem
	virtual const string& EType() const;
	virtual const set<string>& CompsTypes();
	virtual Elem* GetMan();
	virtual Elem* GetNode(const string& aUri);
	virtual Elem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase);
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem);
	virtual TBool ChangeCont(const string& aVal); 
	virtual TBool AddNode(const ChromoNode& aSpec);
	virtual TBool RmNode(const GUri& aUri);
	vector<Elem*>& Comps();
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime);
    protected:
	Elem* AddElem(const ChromoNode& aSpec);
	static void Init();
	inline MLogRec* Logger() const;
	inline MProvider* Provider() const;
	TBool AppendComp(Elem* aComp);
	Elem* GetComp(const string& aParent, const string& aName);
	TBool DoMutChangeCont(const ChromoNode& aSpec);
	TBool MergeMutation(const ChromoNode& aSpec);
	TBool MergeMutMove(const ChromoNode& aSpec);
	Elem* GetComp(TInt aInd);
	virtual void DoOnCompChanged(Elem& aComp);
	TBool IsLogeventCreOn();
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
	void RmIfCache(IfIter& aIt);
	void InvalidateIfCache();
	void InsertIfCache(const string& aName, Base* aReq, Base* aProv, void* aVal);
	void InsertIfCache(const string& aName, Base* aReq, Base* aProv, TIfRange aRg);
    protected:
	// Element type - parent's name
	string iEType;
	// Environment
	MEnv* iEnv;
	// Managed (higher) element of hier
	Elem* iMan;
	// Chromo
	Chromo* iChromo;
	// Mutation
	Chromo* iMut;
	// Components types
	static set<string> iCompsTypes;
	// Sign of inited
	static bool iInit;
	// Components, owninig container
	vector<Elem*> iComps;
	// Components map, not owning
	multimap<TCkey, Elem*> iMComps;
	// Ifaces cache
	TICache iICache;
	// Ifaces cache query by TICMapKeyF
	TICacheQF iICacheQF;
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
