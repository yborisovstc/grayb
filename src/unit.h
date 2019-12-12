#ifndef __GRAYB_UNIT_H
#define __GRAYB_UNIT_H


#include "menv.h"
#include "mlog.h"
#include "base.h"
#include "munit.h"
#include "profiler.h"
#include "ifu.h"
#include <time.h>

#define UNICONTENT

class MProvider;

/** @brief Unit of native hier: hier node, not mutable, supports corporative iface resolution
 * Even not mutable the unit supports native inheritance. So it doesn't have chromosome and
 * doesn't support parent-child relation but keeps native inheritance chain in extended type (EType) 
 * */
class Unit: public MUnit
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
	typedef pair<TNMKey, MUnit*> TNMVal;
	typedef multimap<string, MUnit*> TNMReg;
	typedef map<string, MUnit*> TNReg;
	typedef pair<TNMReg::iterator, TNMReg::iterator> TNMRegItRange;
	// Type of content elements
	/*
	   typedef enum 
	   {
	   ECnt_Comp = 0, // Component of content
	   ECnt_Val,      // Value
	   ECnt_Ctg       // Category
	   } TCnt;
	   */
	// Content key
	//typedef multimap<TCntKey, string> TCntReg;
	typedef pair<string, string> TCntRec;
	typedef multimap<string, string> TCntComps;
	typedef map<string, string> TCntVals;
	typedef pair<string, TCntRel> TContentKey; // Content key: full name, relation
	typedef pair<TContentKey, string> TContentRec; // Content record: key, data
	typedef multimap<TContentKey, string> TContent; // Content, mutlimap needs for categories and comps
    public:
	// Request context
	// Iface cache key first part: [iface name, requestor]
	typedef pair<string, TICacheRCtx> TICacheKeyF;
	// Iface cache key: [[iface name, requestor], provider]
	typedef pair<TICacheKeyF, MUnit*> TICacheKey;
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
	friend class Unit;
	public:
	IfIter(): iHost(NULL) {};
	IfIter(Unit* aHost, const string& aIName, const TICacheRCtx& aReq, TBool aToEnd = EFalse);
	IfIter(const IfIter& aIt);
	virtual MIfIter* Clone() const;
	virtual MIfIter& operator=(const MIfIter& aIt);
	virtual MIfIter& operator++();
	virtual TBool operator==(const MIfIter& aIt);
	MIface* operator*() override;
	public:
	Unit* iHost;
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
	static const char* Type() { return "Unit";};
	virtual const string& Name() const { return iName;}
	// Predefined extended type, for core elements only
	static string PEType();
	Unit(const string &aName = string(), MUnit* aMan = NULL, MEnv* aEnv = NULL);
	virtual void Delete();
	virtual ~Unit();
	virtual TBool IsProvided() const;
	virtual void SetMan(MUnit* aMan);
	string PName() const;
    public:
	virtual MUnit* CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext);
	// Gets the comp with given type and owning given element
	virtual MUnit* GetCompOwning(const string& aParent, MUnit* aElem);
	virtual MUnit* GetCompOwning(MUnit* aElem);
	virtual const MUnit* GetCompOwning(const MUnit* aElem) const;
	virtual MUnit* GetRoot() const;
	virtual MUnit* GetCommonOwner(MUnit* aElem);
	// Debug helpers
	virtual MUnit* GetNodeS(const char* aUri);
	// From MIfProv Iface provider
	virtual MIface* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue);
	TIfRange GetIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) override;
	MIface* getLocalIface(const string& aName, const TICacheRCtx& aCtx) override;
	TInt IfRangeSize(const TIfRange& aRange) const;
	void* GetIfind(TIfRange& aRange, TInt aInd);
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MUnit
	virtual string EType(TBool aShort = ETrue) const;
	virtual MUnit* GetMan();
	virtual const MUnit* GetMan() const;
	virtual MUnit* GetCtx();
	virtual void SetCtx(MUnit* aOwner);
	virtual MUnit* GetNode(const string& aUri, TBool aInclRm = EFalse);
	virtual MUnit* GetNode(const GUri& aUri, TBool aInclRm = EFalse);
	virtual MUnit* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse);
	virtual MUnit* GetNodeByName(const string& aName, const TNs& aNs) override;
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MUnit* aTop = NULL) const;
	virtual void GetRUri(GUri& aUri, MUnit* aTop = NULL);
	virtual string GetUri(MUnit* aTop = NULL, TBool aShort = EFalse) const;
	virtual string GetRUri(MUnit* aTop = NULL);
	virtual TBool RebaseUri(const GUri& aUri, const MUnit* aBase, GUri& aRes);
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MUnit* aBase, GUri& aRes);
	// TODO [YB] The only attr allowed for change is name. To consider replacing of ChangeAttr to Rename
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	//virtual TBool GetCont(string& aValue, const string& aName = string()) const;
	virtual TBool ContentExists(const string& aName) const;
	virtual TBool ContValueExists(const string& aName=string()) const;
	virtual string GetContComp(const string& aOwnerName, TInt aInd) const;
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const;
	virtual string GetContentValue(const string& aName) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string());
	virtual TBool IsContOfCategory(const string& aName, const string& aCategory) const; 
	virtual TInt GetContCount(const string& aName = string()) const;
	virtual TBool IsRemoved() const;
	virtual void SetRemoved(TBool aModif);
	// Debugging
	virtual TInt GetCapacity() const;
	// From MOwner
	virtual TBool IsComp(const MUnit* aElem) const;
	virtual void OnCompDeleting(MUnit& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual void OnCompAdding(MUnit& aComp, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MUnit& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual TBool OnCompRenamed(MUnit& aComp, const string& aOldName);
	virtual TBool OnChanged(MUnit& aComp);
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx());
	void UpdateIfiLocal(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx());
	void RmIfCache(IfIter& aIt);
	virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx);
	virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv, TBool aInv = EFalse);
	void InvalidateIfCache();
	void InvalidateIfCache(const string& aIfName);
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, MUnit* aProv, MIface* aVal);
	void InsertIfCache(const string& aName, const TICacheRCtx& aReq, MUnit* aProv, TIfRange aRg);
	//static TBool InInIfRctx(, TIMUnit* aReq);
	inline MLogRec* Logger() const;
	inline void Log(const TLog& aRec) const;
	inline TBool IsIftEnabled() const;
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
	string Mid() const override { return GetUri(iEnv->Root(), ETrue);}
	virtual TInt CompsCount() const;
	virtual MUnit* GetComp(TInt aInd);
	static string ContentCompId(const string& aOwnerName, const string& aCompName);
    protected:
	static bool IsName(const string& aUri);
	/** Handles agent change */
	virtual TEhr ProcessCompChanged(MUnit& aComp, const string& aContName);
	virtual TEhr ProcessChanged(MUnit& aComp);
	/** Helper. Gets type from extended type */
	string GetType(const string& aEType) const;
	/** Helper. Gets parent extended type from agent extended type */
	string GetPType(const string& aEType) const;
	void InsertContent(const string& aName);
	void RemoveContent(const string& aName);
	void RemoveContentComp(const string& aOwner, const string& aComp);
	void SetContentValue(const string& aName, const string& aValue);
	void SetContentCategory(const string& aName, const string& aCategory);
	void InsertContentComp(const string& aContName, const string& aCompName);
	void InsertContCompsRec(const string& aName, const string& aComp);
	TBool ContentHasComps(const string& aContName) const;
	static TBool IsContentCompOf(const string& aContName, const string& aContOwner);
	TBool ContentHasComp(const string& aContFname, const string& aCompName) const;
	inline MProvider* Provider() const;
	virtual TBool AppendComp(MUnit* aComp, TBool aRt = EFalse);
	virtual void RemoveComp(MUnit* aComp);
	TBool RegisterComp(MUnit* aComp);
	// aName is required because the comp can be renamed already. This is the case of
	// comp renaming: comp is renamed first, then the renaming is handled
	TBool UnregisterComp(MUnit* aComp, const string& aName = string());
	virtual MUnit* GetComp(const string& aParent, const string& aName) const;
	TBool IsLogeventCreOn();
	void InsertIfQm(const string& aName, const TICacheRCtx& aReq, MUnit* aProv);
	void UnregAllIfRel(TBool aInv = EFalse);
	void LogIfReqs();
	static string ContentKey(const string& aBase, const string& aSuffix);
	static string ContentValueKey(const string& aId);
	static string ContentCompsKey(const string& aId);
	// Visual client debugging, ref ds_visdbg
	virtual string GetAssociatedData(const string& aUri) const;
	// Debugging
	virtual void DumpComps(TBool aRecurs = EFalse) const;
	virtual void DumpContent() const;
	virtual void DumpIfReqs() const override;
	virtual void DumpIfCache() const override;
	static void DumpIfCtx(const TICacheRCtx& aCtx);
	static void DumpIfRange(const TIfRange& aCtx);
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
	MUnit* iMan;
	// Components map, not owning
	TNReg iMComps;
	// Ifaces cache
	TICache iICache;
	// Ifaces cache query by TICMapKeyF
	TICacheQF iICacheQF;
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
	MUnit* mContext;

	static TBool EN_MUT_LIM;
	static const string KCont_About;

	/** Profiler helpers */
	void Pclock(PEvent::TId aEventId, MUnit* aNode) { iEnv->Profiler()->Clock()(aEventId, aNode);}
	void Pdur(PEvent::TId aEventId, MUnit* aNode) { iEnv->Profiler()->Dur()(aEventId, aNode);}
	void Pdstat(PEvent::TId aEventId, bool aStart) { iEnv->Profiler()->DurStat()(aEventId, aStart);}
};

inline MLogRec* Unit::Logger() const {return iEnv ? iEnv->Logger(): NULL; }

inline MProvider* Unit::Provider() const {return iEnv ? iEnv->Provider(): NULL; }

inline TBool Unit::IsIftEnabled() const { return iEnv ? iEnv->GetSBool(MEnv::ESb_EnIfTrace): EFalse;};

inline void Unit::Log(const TLog& aRec) const { Logger()->Write(aRec);};


#endif
