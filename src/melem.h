#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include "mmuta.h"
#include "miface.h"
#include "ifu.h"
#include <set>

class ChromoNode;
class Base;
class MElem;
class RqContext;
class Rank;

class TICacheRCtx: public vector<Base*>
{
    public:
	TICacheRCtx();
	TICacheRCtx(const RqContext* aCtx);
};

// Cloned from MCompsObserver in order to avoid MCompsObserver be MIface
// This would require MIface wrapper for MCompsObserver to distinguish MIface methods in Elem
class MAgentObserver: public MIface
{
    public:
	static const char* Type() { return "MAgentObserver";};
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue) = 0;
	virtual void OnCompAdding(MElem& aComp) = 0;
	// TODO [YB] Return value isn't used, to remove ?
	virtual TBool OnCompChanged(MElem& aComp) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnContentChanged(MElem& aComp) = 0;
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName) = 0;
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

class MCompsObserver
{
    public:
	static const char* Type() { return "MCompsObserver";};
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue) = 0;
	virtual void OnCompAdding(MElem& aComp) = 0;
	virtual TBool OnCompChanged(MElem& aComp) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnContentChanged(MElem& aComp) = 0;
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName) = 0;
};

class MOwner: public MCompsObserver
{
    public:
	static const char* Type() { return "MOwner";};
	virtual TBool IsComp(const MElem* aElem) const = 0;
	virtual TBool AppendComp(MElem* aComp) = 0;
	virtual void RemoveComp(MElem* aComp) = 0;
};

// Interface of parent in native hierarchy
class MParent
{
    public:
	static const char* Type() { return "MParent";};
    public: 
	virtual void OnChildDeleting(MElem* aChild) = 0;
	virtual TBool OnChildRenamed(MElem* aComp, const string& aOldName) = 0;
	virtual TBool AppendChild(MElem* aChild) = 0;
	virtual void RemoveChild(MElem* aChild) = 0;
};

class MChild
{
    public:
	static const char* Type() { return "MChild";};
    public:
	virtual MElem* GetParent() = 0;
	virtual const MElem* GetParent() const = 0;
	virtual void OnParentDeleting(MElem* aParent) = 0;
	virtual void SetParent(const string& aParent) = 0;
	virtual void SetParent(MElem* aParent) = 0;
};

// Cooperative Interface provider
class MIfProv
{
    public:
	static const char* Type() { return "MIfProv";};
    public:
	// Iface of Cooperative Ifaces provider iterator
	class MIfIter : public iterator<input_iterator_tag, void*> {
	    public:
		virtual ~MIfIter() {};
		virtual MIfIter* Clone() const { return NULL;};
		virtual MIfIter& operator=(const MIfIter& aIt) { return *this;};
		virtual MIfIter& operator++() { return *this;};
		virtual TBool operator==(const MIfIter& aIt) { return EFalse;};
		virtual void*  operator*() {return NULL;};
	};

	// Envelop of Cooperative Ifaces provider iterator 
	class TIfIter : public iterator<input_iterator_tag, void*> {
	    public:
		TIfIter(): mImpl(NULL), mCloned(EFalse) {};
		TIfIter(MIfIter& aImpl): mImpl(&aImpl), mCloned(EFalse) {};
		TIfIter(const TIfIter& aIt): mImpl(NULL), mCloned(ETrue) { if (aIt.mImpl != NULL) mImpl = aIt.mImpl->Clone(); };
		~TIfIter() { if (mCloned) delete mImpl; mImpl = NULL;};
		TIfIter& operator=(const TIfIter& aIt) { if (mImpl == NULL) { if (aIt.mImpl != NULL) mImpl = aIt.mImpl->Clone();} 
		    else if (aIt.mImpl == NULL) {delete mImpl; mImpl = NULL;} else mImpl->operator=(*(aIt.mImpl)); return *this;};
		TIfIter& operator++() { mImpl->operator++(); return *this;};
		TIfIter operator++(int) { TIfIter tmp(*this); operator++(); return tmp; };
		TBool operator==(const TIfIter& aIt) { return (mImpl != NULL && aIt.mImpl != NULL) ? mImpl->operator==((*aIt.mImpl)) : mImpl == aIt.mImpl;};
		TBool operator!=(const TIfIter& aIt) { return !operator==(aIt);};
		virtual void*  operator*() { return mImpl->operator*();};
	    protected:
		MIfIter* mImpl;
		TBool mCloned;
	};

	typedef pair<TIfIter, TIfIter> TIfRange;
    public:
	virtual void* GetSIfiC(const string& aName, Base* aRequestor = NULL) = 0;
	virtual void* GetSIfi(const string& aName, const RqContext* aCtx = NULL) = 0;
	virtual void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue) = 0;
	virtual TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL) = 0;
	virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx) = 0;
	virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv = EFalse) = 0;
};

// Agent - comps observer
class MACompsObserver
{
    public:
	static const char* Type() { return "MACompsObserver";};
	virtual TBool HandleCompChanged(MElem& aContext, MElem& aComp) = 0;
};

// Composite interface of Element (node) of native graph hierarchy
// TODO [YB] Do we need MComp also ?
// Please note that MIface should be on 1-st position. This is required in order to trivial cast to MIface from DoGetObj result
// TODO [YB] actually this is a hack, to re-design (probably MIface needs methos GetIface to use instead DoGetObj)
class MElem : public MIface, public Base, public MMutable, public MOwner, public MParent, public MChild, public MIfProv
{
    friend class Elem;
    friend class ImportsMgr;
    public:
	static const char* Type() { return "MElem";};
    public:
	virtual const string EType(TBool aShort = ETrue) const = 0;
	virtual const string& Name() const = 0;
	virtual TBool IsProvided() const = 0;
	virtual MElem* GetMan() = 0;
	virtual const MElem* GetMan() const = 0;
	virtual void GetCRoot(TMut& aMut) const = 0;
	virtual void SetMan(MElem* aMan) = 0;
	virtual void SetObserver(MAgentObserver* aObserver) = 0;
	virtual vector<MElem*>& Comps() = 0;
	virtual const vector<MElem*>& Comps() const = 0;
	virtual MElem* GetNode(const string& aUri, TBool aInclRm = EFalse) = 0;
	virtual MElem* GetNode(const GUri& aUri, TBool aInclRm = EFalse) = 0;
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse) = 0;
	virtual MElem* GetRoot() const = 0;
	virtual MElem* GetInhRoot() const = 0;
	virtual TInt GetContCount() const = 0;
	virtual TBool IsContChangeable(const string& aName = string()) const = 0; 
	virtual void GetCont(string& aCont, const string& aName=string()) = 0; 
	virtual string GetContent(const string& aName=string()) const = 0; 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const = 0;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()) = 0; 
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse) = 0;
	virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL) = 0;
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL) = 0;
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MElem* aTop = NULL) const = 0;
	virtual void GetRUri(GUri& aUri, MElem* aTop = NULL) = 0;
	virtual string GetUri(MElem* aTop = NULL, TBool aShort = EFalse) const = 0;
	virtual string GetRUri(MElem* aTop = NULL) = 0;
	virtual TBool RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes) = 0;
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes) = 0;
	virtual MElem* GetAowner() = 0;
	virtual const MElem* GetAowner() const = 0;
	virtual MElem* GetCompAowner(const MElem* aComp) = 0;
	virtual const MElem* GetCompAowner(const MElem* aComp) const = 0;
	virtual MElem* GetAcompOwning(MElem* aComp) = 0;
	virtual MElem* GetUpperAowner() = 0;
	virtual MElem* GetCommonOwner(MElem* aElem) = 0;
	virtual TBool IsRemoved() const = 0;
	virtual void SetRemoved() = 0;
	virtual MElem* GetAttachedMgr() = 0;
	virtual TBool IsCompAttached(const MElem* aComp) const = 0;
	virtual const MElem* GetAttachedMgr() const = 0;
	virtual TBool IsAownerOf(const MElem* aElem) const = 0;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal) = 0;
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL) = 0;
	virtual void GetRank(Rank& aRank) const = 0; 
	virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const = 0;
	virtual void GetCompRank(Rank& aRank, const MElem* aComp) const = 0;
	virtual TInt GetCompLrank(const MElem* aComp) const = 0;
	virtual MElem* GetComp(const string& aParent, const string& aName) = 0;
	virtual MElem* GetComp(const string& aParent, const string& aName) const = 0;
	virtual void OnNodeMutated(const MElem* aNode, const TMut& aMut, const MElem* aCtx = NULL) = 0;
	virtual void OnParentMutated(MElem* aParent, const TMut& aMut) = 0;
	// Gets the comp with given type and owning given element
	virtual MElem* GetCompOwning(const string& aParent, MElem* aElem) = 0;
	virtual MElem* GetCompOwning(MElem* aElem) = 0;
	virtual const MElem* GetCompOwning(const MElem* aElem) const = 0;
	virtual TBool IsInheritedComp(const MElem* aNode) const = 0;
	virtual TBool HasInherDeps(const MElem* aScope) const = 0;
	// Debugging
	virtual TInt GetCapacity() const = 0;
	virtual TBool IsHeirOf(const string& aParent) const = 0;
	virtual MElem* GetNodeS(const char* aUri) = 0;
	// TODO [YB] To replace Comps APIs with local nodes APIs (NodesLoc_Begin etc.)
	// or general nodes APIs (GetNode). This will allow proper iteration with excluding removed nodes
	virtual TInt CompsCount() const = 0;
	virtual MElem* GetComp(TInt aInd) = 0;
	virtual void SaveChromo(const char* aPath) const = 0;
	virtual void DumpChilds() const = 0;
	// From MIface
	virtual string Uid() const { return Mid() + "%" + Type();};
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
		static void FromCtx(const TICacheRCtx& aCtx, string& aRes);
		static void ToCtx(MElem* aHost, const string& aStr, TICacheRCtx& aCtx);
	};
	// Interface methods utility
	static EIfu mIfu;
};

#endif
