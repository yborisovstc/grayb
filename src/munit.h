#ifndef __GRAYB_MUNIT_H
#define __GRAYB_MUNIT_H

#include "plat.h"
#include "guri.h"
//#include "mmuta.h"
#include "miface.h"
#include "ifu.h"
#include "ifu.h"
#include <set>


class Base;
class MUnit;

/**
 * @brief Events handling result
 */
typedef enum {
    EEHR_Ignored = 0,
    EEHR_Accepted = 1,
    EEHR_Denied = 2
} TEhr;


/** @brief Name spaces
 * */
//using TNs = vector<MUnit*>;
class TNs: public vector<MUnit*>
{
};

class TICacheRCtx: public vector<MUnit*>
{
    public:
	TICacheRCtx(): vector<MUnit*>() {}
	TICacheRCtx(MUnit* aElem): TICacheRCtx() { __ASSERT(aElem != NULL); push_back(aElem);}
	TBool IsInContext(MUnit* aReq) const { 
	    TBool res = EFalse;
	    for (auto* req : *this) {
		if (req == aReq) { res = ETrue; break;}
	    }
	    return res;
	}
	inline void push_back(MUnit* aUnit, TBool aNoCheck = false) {
	    __ASSERT(aNoCheck || !IsInContext(aUnit));
	    vector<MUnit*>::push_back(aUnit);
	}
};


/** @brief Interface of agent - element that handles managing requests
 *
 * TODO How is it uitlized? Base iface provides this functionality 
 */
class MAgent: public MIfaceProv, public MIface
{
    public:
	static const char* Type() { return "MAgent";}
	MIface* DoGetIface(const string& aName) override { return MAgent_DoGetIface(aName);}
	virtual MIface* MAgent_DoGetIface(const string& aName) = 0;
	virtual string MAgent_Mid() const {return "?";}
	virtual MIface* Call(const string& aSpec, string& aRes) override { return NULL;}
	virtual string Mid() const override { return MAgent_Mid();}
};

// Cloned from MCompsObserver in order to avoid MCompsObserver be MIface
// This would require MIface wrapper for MCompsObserver to distinguish MIface methods in Elem
// Added flag for distinguish modif and mutation, ref uc_066
// TODO [YB] To remove flag aModif, it's functionality is covered by OnCompMutated notif
class MAgentObserver: public MIface
{
    public:
	static const char* Type() { return "MAgentObserver";};
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) = 0;
	virtual void OnCompAdding(const MUnit* aComp, TBool aModif = EFalse) = 0;
	// TODO [YB] Return value isn't used, to remove ?
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnChanged(const MUnit* aComp) = 0;
	virtual TBool OnCompRenamed(const MUnit* aComp, const string& aOldName) = 0;
	// Extra notification for the clients handling model's chromo, ref ds_nmm, uc_066
	virtual void OnCompMutated(const MUnit* aNode) = 0;
	// TODO Consider passing error parameters
	virtual void OnError(const MUnit* aComp) = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
};

// TODO [YB] To move OnNodeMutated to MCompsObserver ??
class MCompsObserver
{
    public:
	static const char* Type() { return "MCompsObserver";};
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) = 0;
	virtual void OnCompAdding(const MUnit* aComp, TBool aModif = EFalse) = 0;
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnChanged(const MUnit* aComp) = 0;
	virtual TBool OnCompRenamed(const MUnit* aComp, const string& aOldName) = 0;
	virtual void OnError(const MUnit* aComp) = 0;
};

/** @brief Interface of owning node in native hier tree
 */
class MOwner: public MCompsObserver
{
    public:
	static const char* Type() { return "MOwner";};
	virtual TBool IsComp(const MUnit* aElem) const = 0;
	virtual TBool AppendComp(MUnit* aComp, TBool aRt = EFalse) = 0;
	virtual void RemoveComp(MUnit* aComp) = 0;
};

/** @brief Interface of owned node in native hier tree
 */
class MOwned
{
    public:
};

// Cooperative Interface provider
class MIfProv
{
    public:
	static const char* Type() { return "MIfProv";};
    public:
	// Iface of Cooperative Ifaces provider iterator
	class MIfIter : public iterator<input_iterator_tag, MIface*> {
	    public:
		virtual ~MIfIter() {};
		virtual MIfIter* Clone() const { return NULL;};
		virtual MIfIter& operator=(const MIfIter& aIt) { return *this;};
		virtual MIfIter& operator++() { return *this;};
		virtual TBool operator==(const MIfIter& aIt) { return EFalse;};
		virtual MIface*  operator*() {return NULL;};
	};

	// Envelop of Cooperative Ifaces provider iterator 
	class TIfIter : public iterator<input_iterator_tag, MIface*> {
	    public:
		TIfIter(): mImpl(NULL), mCloned(EFalse) {};
		TIfIter(MIfIter& aImpl): mImpl(&aImpl), mCloned(EFalse) {};
		TIfIter(const TIfIter& aIt): mImpl(NULL), mCloned(ETrue) { if (aIt.mImpl != NULL) mImpl = aIt.mImpl->Clone(); };
		~TIfIter() { if (mCloned) delete mImpl; mImpl = NULL;};
		TIfIter& operator=(const TIfIter& aIt);
		TIfIter& operator++() { mImpl->operator++(); return *this;};
		TIfIter operator++(int) { TIfIter tmp(*this); operator++(); return tmp; };
		TBool operator==(const TIfIter& aIt) { return (mImpl != NULL && aIt.mImpl != NULL) ? mImpl->operator==((*aIt.mImpl)) : mImpl == aIt.mImpl;};
		TBool operator!=(const TIfIter& aIt) { return !operator==(aIt);};
		virtual MIface*  operator*() { return mImpl->operator*();};
	    protected:
		MIfIter* mImpl;
		TBool mCloned;
	};

	typedef pair<TIfIter, TIfIter> TIfRange;
    public:
	MIface* GetSIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) { TIfRange rg = GetIfi(aName, aCtx);
	    return (rg.first != rg.second) ? *(rg.first) : NULL;
	}
	template<class T> T* GetSIfit(const T* aIfi, const TICacheRCtx& aCtx = TICacheRCtx()) { TIfRange rg = GetIfi(aIfi->Type(), aCtx);
	    return dynamic_cast<T*>((rg.first != rg.second) ? *(rg.first) : NULL); }
	virtual MIface* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue) = 0;
	virtual TIfRange GetIfi(const string& aName, const TICacheRCtx& aCtx = TICacheRCtx()) = 0;
	/** @brief Gets iface by index
	 * */
	virtual MIface* GetIfind(const string& aName, const TICacheRCtx& aCtx, TInt aInd) = 0;
	/** @brief Register interface provider
	 * @param[in] aIfName  Name of interface
	 * @param[in] aCtx     Context of the request at requester
	 * @param[in] aProv    Ptr to provider
	 * */
	virtual void RegIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv) = 0;
	virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx) = 0;
	virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv) = 0;
	virtual MIface* getLocalIface(const string& aName, const TICacheRCtx& aCtx) = 0;
};

inline MIfProv::TIfIter& MIfProv::TIfIter::operator=(const TIfIter& aIt)
{ 
    if (mImpl == NULL) {
	if (aIt.mImpl != NULL) mImpl = aIt.mImpl->Clone();
    } else if (aIt.mImpl == NULL) {
	delete mImpl; mImpl = NULL;
    } else mImpl->operator=(*(aIt.mImpl));
    return *this;
};

// Agent - comps observer
class MACompsObserver: public MIface
{
    public:
	static const char* Type() { return "MACompsObserver";};
	virtual TBool HandleCompChanged(MUnit* aContext, MUnit* aComp, const string& aContName = string()) = 0;
	virtual TBool HandleChanged(MUnit* aContext, MUnit* aComp) {return ETrue;}
	virtual MIface* MACompsObserver_Call(const string& aSpec, string& aRes) { return NULL;}
	virtual string MACompsObserver_Mid() const { return "?";}
	MIface* Call(const string& aSpec, string& aRes) override {return MACompsObserver_Call(aSpec, aRes);}
	string Mid() const override { return MACompsObserver_Mid();}
};

/** @brief Composite root interface of Uint: node, changeable, cooperative iface reasolver
 * Note that using composite iface helps to minimize resources in distributed models
 * Otherwize many 'small' elementary ifaces proxies need to be created.
 *
 * TODO To consider to defice lower level root iface: just node and changeable
 * TODO To move all API to ortohonal ifaces
 * */
class MUnit : public MIface, public Base, public MOwner, public MIfProv
{
    public:
	// Predefined content categories
	static const string KCont_Categories;
	static const string KCont_Ctg_Readonly;
	static const string KCont_Ctg_Debug;
    public:
	static const char* Type() { return "MUnit";};
	/** Extended type
	 * @param aShort Directs to get short version of type
	 * @return Extended type
	 * */
	virtual string EType(TBool aShort = ETrue) const = 0;
	/** Dedicated request of deletion, ref ds_daa_powrd
	 * */
	virtual void Delete() = 0;
	virtual const string& Name() const = 0;
	virtual TBool IsProvided() const = 0;
	virtual MUnit* GetMan() = 0;
	virtual const MUnit* GetMan() const = 0;
	virtual void SetMan(MUnit* aMan) = 0;
	virtual TBool IsHeirOf(const string& aParent) const = 0;
	/** @brief Creating heir
	 * In simplest case it creates the instance of the unit
	 */
	virtual MUnit* CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext) = 0;
	// Support of final owner, ref ds_daa_itn_sfo
	// TODO [YB] Is the context actually used?
	virtual MUnit* GetCtx() = 0;
	virtual void SetCtx(MUnit* aOwner) = 0;
	virtual MUnit* GetNode(const string& aUri, TBool aInclRm = EFalse) = 0;
	virtual MUnit* GetNode(const GUri& aUri, TBool aInclRm = EFalse) = 0;
	virtual MUnit* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse) = 0;
	virtual MUnit* GetRoot() const = 0;
	/** @brief Gets node by name. Name is resolved with name spaces
	 * @param[out] aNs    Name space
	 * */
	virtual MUnit* GetNodeByName(const string& aName, const TNs& aNs) = 0;
	virtual MUnit* GetCommonOwner(MUnit* aElem) = 0;
	virtual TInt GetContCount(const string& aName = string()) const = 0;
	virtual TBool IsContOfCategory(const string& aName, const string& aCategory) const = 0; 
	virtual TBool ContentExists(const string& aName) const = 0;
	virtual TBool ContValueExists(const string& aName=string()) const = 0;
	// TODO To use content URI instead of key
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const = 0; 
	// TODO To add also method GetContentComp(baseId, compId)
	virtual string GetContComp(const string& aOwnerName, TInt aInd) const = 0;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()) = 0; 
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MUnit* aTop = NULL) const = 0;
	virtual void GetRUri(GUri& aUri, MUnit* aTop = NULL) = 0;
	virtual string GetUri(MUnit* aTop = NULL, TBool aShort = EFalse) const = 0;
	virtual string GetRUri(MUnit* aTop = NULL) = 0;
	virtual TBool RebaseUri(const GUri& aUri, const MUnit* aBase, GUri& aRes) = 0;
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MUnit* aBase, GUri& aRes) = 0;
	virtual TBool IsRemoved() const = 0;
	virtual void SetRemoved(TBool aModif) = 0;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal) = 0;
	virtual MUnit* GetComp(const string& aParent, const string& aName) const = 0;
	// Gets the comp with given type and owning given element
	virtual MUnit* GetCompOwning(const string& aParent, MUnit* aElem) = 0;
	/** Gets the direct comp of this owning given unit
	 * @param aElem Given unit
	 * @return The direct comp owning aElem
	 * */
	virtual MUnit* GetCompOwning(MUnit* aElem) = 0;
	virtual const MUnit* GetCompOwning(const MUnit* aElem) const = 0;
	// Visual client debugging, ref ds_visdbg
	virtual string GetAssociatedData(const string& aUri) const = 0;
	// Debugging
	virtual TInt GetCapacity() const = 0;
	virtual MUnit* GetNodeS(const char* aUri) = 0;
	virtual TInt CompsCount() const = 0;
	virtual MUnit* GetComp(TInt aInd) = 0;
	virtual void DumpComps(TBool aRecurs = EFalse) const = 0;
	virtual void DumpContent() const = 0;
	virtual void DumpIfProv(const string& aName, const TICacheRCtx& aCtx, const MIface* aIface) const = 0;
	/** @brief Dumps iface resolution paths */
	virtual void DumpIfPaths(const char* aIfName = NULL) const = 0;
	virtual void DumpIfCache() const = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
	// Helpers
	static string GetContentOwner(const string& aCont);
	static string GetContentLName(const string& aName);
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
		static void ToCtx(MUnit* aHost, const string& aStr, TICacheRCtx& aCtx);
	};
	// Interface methods utility
	// TODO [YB] To redesign: to separate iface metadata and utility. To add miface method to get metadata.
	static EIfu mIfu;
};

#endif
