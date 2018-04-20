#ifndef __GRAYB_ELEM_H
#define __GRAYB_ELEM_H


#include "melem.h"
#include "menv.h"
#include "mlog.h"
#include "mmuta.h"
#include "base.h"
#include "chromo.h"
#include "ifu.h"
#include <time.h>

#define UNICONTENT

    class Chromo;
    class MProvider;

    // Standard element of native hier
    class Elem: public MElem
    {
	public:
	    // Predefined content data relations
	    typedef enum {
		ECrl_Undef = 0, 
		ECrl_Value,
		ECrl_Component,
		ECrl_Category
	    } TCntRel;

	    // Regiser keyed by name, multimap
	    typedef string TNMKey;
	    typedef pair<TNMKey, MElem*> TNMVal;
	    typedef multimap<string, MElem*> TNMReg;
	    typedef map<string, MElem*> TNReg;
	    typedef pair<TNMReg::iterator, TNMReg::iterator> TNMRegItRange;
	    // Type of content elements
	    typedef enum 
	    {
		ECnt_Comp = 0, // Component of content
		ECnt_Val,      // Value
		ECnt_Ctg       // Category
	    } TCnt;
	    // Content key
	    //typedef multimap<TCntKey, string> TCntReg;
	    typedef pair<string, string> TCntRec;
	    typedef multimap<string, string> TCntComps;
	    typedef map<string, string> TCntVals;
	    typedef pair<string, TCntRel> TContentKey; // Content key: full name, relation
	    typedef pair<TContentKey, string> TContentRec; // Content record: key, data
	    typedef multimap<TContentKey, string> TContent;
	public:
	    // Request context
	    //typedef vector<Base*> TICacheRCtx;
	    // Iface cache key first part: [iface name, requestor]
	    typedef pair<string, TICacheRCtx> TICacheKeyF;
	    // Iface cache key: [[iface name, requestor], provider]
	    typedef pair<TICacheKeyF, MElem*> TICacheKey;
	    // Iface cache
	    typedef multimap<TICacheKey, MIface*> TICache;
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
	    MIface* operator*() override;
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
	    virtual void Delete();
	    virtual ~Elem();
	    virtual TBool IsProvided() const;
	    virtual void SetParent(const string& aParent);
	    virtual void SetParent(MElem* aParent);
	    virtual void SetMan(MElem* aMan);
	    virtual void SetObserver(MAgentObserver* aObserver);
	    virtual void SetMutation(const ChromoNode& aMuta);
	    virtual void SetMutation(const string& aMutSpec);
	    virtual ChromoNode AppendMutation(const ChromoNode& aMuta);
	    virtual ChromoNode AppendMutation(TNodeType aType);
	    virtual TBool AppendMutation(const string& aFileName);
	    virtual void AppendMutation(const TMut& aMut);
	    string PName() const;
	public:
	    virtual MElem* CreateHeir(const string& aName, MElem* aMan, MElem* aContext);
	    virtual unique_ptr<MChromo> GetFullChromo() const;
	    virtual string GetChromoSpec() const;
	    virtual const MChromo& Chromos() const { return *iChromo;};
	    virtual MChromo& Chromos() { return *iChromo;};
	    // Gets the comp with given type and owning given element
	    virtual MElem* GetCompOwning(const string& aParent, MElem* aElem);
	    virtual MElem* GetCompOwning(MElem* aElem);
	    virtual const MElem* GetCompOwning(const MElem* aElem) const;
	    virtual MElem* GetRoot() const;
	    virtual MElem* GetInhRoot() const;
	    virtual MElem* GetCommonOwner(MElem* aElem);
	    virtual MElem* GetCompAowner(const MElem* aComp);
	    virtual const MElem* GetCompAowner(const MElem* aComp) const;
	    virtual TBool IsAownerOf(const MElem* aElem) const;
	    virtual TBool IsCompAttached(const MElem* aComp) const;
	    // Checks if elements chromo is attached. Ref UC_019 for details
	    virtual TBool IsChromoAttached() const;
	    TBool IsMutAttached(const ChromoNode& aMut) const;
	    virtual MElem* GetAttachedMgr();
	    virtual const MElem* GetAttachedMgr() const;
	    virtual MElem* GetAowner();
	    virtual const MElem* GetAowner() const;
	    virtual MElem* GetAcompOwning(MElem* aComp);
	    // Checks if the node is originated via phenotypical modification
	    virtual TBool IsInheritedComp(const MElem* aNode) const;
	    TBool IsDirectInheritedComp(const MElem* aNode) const;
	    TBool IsCompOfInheritedComp(const MElem* aNode) const;
	    // Debug helpers
	    virtual MElem* GetNodeS(const char* aUri);
	    TBool IsName(const char* aName);
	    virtual TBool IsHeirOf(const string& aParent) const;
	    // From MIfProv Iface provider
	    virtual MIface* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue);
	    TIfRange GetIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	    MIface* getLocalIface(const string& aName, const TICacheRCtx& aCtx) override;
	    TInt IfRangeSize(const TIfRange& aRange) const;
	    void* GetIfind(TIfRange& aRange, TInt aInd);
	    // From Base
	    virtual MIface *DoGetObj(const char *aName);
	    // From MElem
	    virtual string EType(TBool aShort = ETrue) const;
	    virtual MElem* GetMan();
	    virtual const MElem* GetMan() const;
	    virtual MElem* GetCtx();
	    virtual void SetCtx(MElem* aOwner);
	    virtual void GetCRoot(TMut& aMut) const;
	    virtual void GetRank(Rank& aRank) const;
	    virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const;
	    virtual void GetCompRank(Rank& aRank, const MElem* aComp) const;
	    virtual TInt GetCompLrank(const MElem* aComp) const;
	    TInt GetLocalRank() const;
	    virtual MElem* GetNode(const string& aUri, TBool aInclRm = EFalse);
	    virtual MElem* GetNode(const GUri& aUri, TBool aInclRm = EFalse);
	    virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse);
	    virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL);
	    virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL);
	    // Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	    virtual void GetUri(GUri& aUri, MElem* aTop = NULL) const;
	    virtual void GetRUri(GUri& aUri, MElem* aTop = NULL);
	    virtual string GetUri(MElem* aTop = NULL, TBool aShort = EFalse) const;
	    virtual string GetRUri(MElem* aTop = NULL);
	    virtual TBool RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes);
	    virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes);
	    // TODO [YB] The only attr allowed for change is name. To consider replacing of ChangeAttr to Rename
	    virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	    virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	    //virtual TBool GetCont(string& aValue, const string& aName = string()) const;
	    virtual TBool ContentExists(const string& aName) const;
	    virtual TBool ContValueExists(const string& aName=string()) const;
	    virtual string GetContComp(const string& aOwnerName, TInt aInd) const;
	    virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const;
	    virtual string GetContentValue(const string& aName) const;
	    virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string());
	    virtual TBool IsContOfCategory(const string& aName, const string& aCategory) const; 
	    virtual TInt GetContCount(const string& aName = string()) const;
	    virtual MElem* GetUpperAowner();
	    virtual TBool IsRemoved() const;
	    virtual void SetRemoved(TBool aModif);
	    // Debugging
	    virtual TInt GetCapacity() const;
	    // Nodes
	    virtual TBool AppendChild(MElem* aChild);
	    virtual void RemoveChild(MElem* aChild);
	    virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	    virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	    virtual TBool ImportNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	    // From MChild
	    virtual MElem* GetParent();
	    virtual const MElem* GetParent() const;
	    virtual void OnParentDeleting(MElem* aParent);
	    // From MParent
	    virtual void OnChildDeleting(MElem* aChild);
	    virtual TBool OnChildRenamed(MElem* aChild, const string& aOldName);
	    // From MOwner
	    virtual TBool IsComp(const MElem* aElem) const;
	    virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	    virtual void OnCompAdding(MElem& aComp, TBool aModif = EFalse);
	    virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	    virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	    virtual TBool OnChanged(MElem& aComp);
	    // From MMutable
	    virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	    virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	    virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	    virtual void OnNodeMutated(const MElem* aNode, const TMut& aMut, const MElem* aCtx);
	    virtual void OnParentMutated(MElem* aParent, const TMut& aMut);
	    // Ifaces cache
	    virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx());
	    void UpdateIfiLocal(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx());
	    void RmIfCache(IfIter& aIt);
	    virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx);
	    virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv = EFalse);
	    void InvalidateIfCache();
	    void InvalidateIfCache(const string& aIfName);
	    void InsertIfCache(const string& aName, const TICacheRCtx& aReq, MElem* aProv, MIface* aVal);
	    void InsertIfCache(const string& aName, const TICacheRCtx& aReq, MElem* aProv, TIfRange aRg);
	    //static TBool InInIfRctx(, TIMElem* aReq);
	    // Chromo
	    ChromoNode GetChNode(const GUri& aUri) const;
	    virtual TBool CompactChromo() override { return EFalse;}
	    virtual TBool CompactChromo(const ChromoNode& aNode) override { return EFalse;}
	    void UndoCompactChromo() {}
	    inline MLogRec* Logger() const;
	    inline void Log(const TLog& aRec) const;
	    inline TBool IsIftEnabled() const;
	    // From MIface
	    virtual MIface* Call(const string& aSpec, string& aRes);
	    string Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	    virtual TInt CompsCount() const;
	    virtual MElem* GetComp(TInt aInd);
	    static string ContentCompId(const string& aOwnerName, const string& aCompName);
	protected:
	    void InsertContent(const string& aName);
	    void RemoveContent(const string& aName);
	    void RemoveContentComp(const string& aOwner, const string& aComp);
	    void SetContentValue(const string& aName, const string& aValue);
	    void SetContentCategory(const string& aName, const string& aCategory);
	    void InsertContentComp(const string& aContName, const string& aCompName);
	    void InsertContCompsRec(const string& aName, const string& aComp);
	    TBool ContentHasComps(const string& aContName) const;
	    inline MProvider* Provider() const;
	    virtual TBool AppendComp(MElem* aComp, TBool aRt = EFalse);
	    virtual void RemoveComp(MElem* aComp);
	    TBool RegisterComp(MElem* aComp);
	    TBool RegisterChild(MElem* aChild);
	    void NotifyNodeMutated(const ChromoNode& aMut, const MElem* aCtx);
	    void NotifyParentMutated(const TMut& aMut);
	    // aName is required because the comp can be renamed already. This is the case of
	    // comp renaming: comp is renamed first, then the renaming is handled
	    TBool UnregisterComp(MElem* aComp, const string& aName = string());
	    TBool UnregisterChild(MElem* aChild, const string& aName = string());
	    //virtual MElem* GetComp(const string& aParent, const string& aName);
	    virtual MElem* GetComp(const string& aParent, const string& aName) const;
	    TBool IsLogeventCreOn();
	    TBool HasChilds() const;
	    virtual TBool HasInherDeps(const MElem* aScope) const;
	    void InsertIfQm(const string& aName, const TICacheRCtx& aReq, MElem* aProv);
	    void UnregAllIfRel(TBool aInv = EFalse);
	    void LogIfReqs();
	    static string ContentKey(const string& aBase, const string& aSuffix);
	    static string ContentValueKey(const string& aId);
	    static string ContentCompsKey(const string& aId);
	    // Profiling
	    static void Delay(long us);
	    static long GetClockElapsed(const timespec& aStart, timespec& aEnd);
	    static long GetClockElapsed(long aStart);
	    static long GetClock();
	    // Visual client debugging, ref ds_visdbg
	    virtual string GetAssociatedData(const string& aUri) const;
	    // Debugging
	    virtual void SaveChromo(const char* aPath) const;
	    virtual void DumpComps() const;
	    virtual void DumpChilds() const;
	    virtual void DumpContent() const;
	    virtual void DumpIfReqs() const override;
	    virtual void DumpIfCache() const override;
	public:
	    // TODO [YB] To move to Ifu ?
	    static const char KContentStart = '{';
	    static const char KContentEnd = '}';
	    static const char KContentValSep = ':';
	    static const char KContentCompsSep = ' ';
	    static const char KContentValQuote = '\'';
	    static const char KContentSep = '.';
	    static const char KContentKey_Value = '#';
	    static const char KContentKey_Comps = '*';
	    static const char KContentPref_Category = '@';
	    static const char KContentDeletion = '-';
	protected:
	    // Environment
	    MEnv* iEnv;
	    // Managing (higher) element of hier
	    MElem* iMan;
	    // Observer, mostly for root - normally elem notifies to Mgr
	    MAgentObserver* iObserver;
	    // Chromo
	    Chromo* iChromo;
	    // Mutation
	    Chromo* iMut;
	    // Components map, not owning
	    TNReg iMComps;
	    // Ifaces cache
	    TICache iICache;
	    // Ifaces cache query by TICMapKeyF
	    TICacheQF iICacheQF;
	    // Children register
	    TNMReg iChilds;
	    // Parent
	    MElem* iParent;
	    // Sign of that node is removed
	    TBool isRemoved;
	    string iName;
	    // Content register
#ifdef UNICONTENT
	    TContent mContent;
#else 
	    TCntComps mCntComps;
	    TCntVals mCntVals;
#endif
	    // Context, ref ds_daa_itn_sfo
	    MElem* mContext;

	    static TBool EN_PERF_TRACE;
	    static TBool EN_PERF_METR;
	    static TBool EN_MUT_LIM;
	    static TBool EN_PERF_DBG1;
	    static const string KCont_About;
	    /*
	       protected:
	       class EIfu: public Ifu {
	       public:
	       EIfu();
	       };
	    // Interface methods utility
	    static EIfu mIfu;
	    */
    };

    inline MLogRec* Elem::Logger() const {return iEnv ? iEnv->Logger(): NULL; }

    inline MProvider* Elem::Provider() const {return iEnv ? iEnv->Provider(): NULL; }

    inline TBool Elem::IsIftEnabled() const { return iEnv ? iEnv->GetSBool(MEnv::ESb_EnIfTrace): EFalse;};

    inline void Elem::Log(const TLog& aRec) const { Logger()->Write(aRec);};


#endif
