#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include "mmuta.h"
#include <set>

class ChromoNode;
class Base;
class MElem;
class RqContext;
class Rank;


class MOwner
{
    public:
	static const char* Type() { return "MOwner";};
	virtual TBool IsComp(const MElem* aElem) const = 0;
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue) = 0;
	virtual void OnCompAdding(MElem& aComp) = 0;
	virtual TBool OnCompChanged(MElem& aComp) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnContentChanged(MElem& aComp) = 0;
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName) = 0;
	virtual TBool MoveComp(MElem* aComp, MElem* aDest) = 0;
	virtual TBool MoveComp(MElem* aComp, const ChromoNode& aDest) = 0;
	virtual TBool AppendComp(MElem* aComp) = 0;
};

// Interface of parent in native hierarchy
class MParent
{
    public: 
	virtual void OnChildDeleting(MElem* aChild) = 0;
	virtual TBool OnChildRenamed(MElem* aComp, const string& aOldName) = 0;
	virtual TBool AppendChild(MElem* aChild) = 0;
	virtual void RemoveChild(MElem* aChild) = 0;
};

class MChild
{
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
	class TIfIter : public iterator<input_iterator_tag, void*> {
	    public:
		virtual TIfIter& operator++() {};
		TIfIter operator++(int) { TIfIter tmp(*this); operator++(); return tmp; };
		virtual TBool operator==(const TIfIter& aIt) { return EFalse;};
		TBool operator!=(const TIfIter& aIt) { return !operator==(aIt);};
		virtual void*  operator*() {return NULL;};
	};
	typedef pair<TIfIter, TIfIter> TIfRange;
    public:
	virtual void* GetSIfiC(const string& aName, Base* aRequestor = NULL) = 0;
	virtual void* GetSIfi(const string& aName, const RqContext* aCtx = NULL) = 0;
	virtual void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert = ETrue) = 0;
	virtual TIfRange GetIfi(const string& aName, const RqContext* aCtx = NULL) = 0;
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
class MElem : public Base, public MMutable, public MOwner, public MParent, public MChild, public MIfProv
{
    public:
	virtual const string EType(TBool aShort = ETrue) const = 0;
	virtual const string& Name() const = 0;
	virtual const string& EName() const = 0;
	virtual MElem* GetMan() = 0;
	virtual const MElem* GetMan() const = 0;
	virtual void SetMan(MElem* aMan) = 0;
	virtual vector<MElem*>& Comps() = 0;
	virtual const vector<MElem*>& Comps() const = 0;
	virtual MElem* GetNode(const string& aUri) = 0;
	virtual MElem* GetNode(const GUri& aUri) = 0;
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse) = 0;
	virtual MElem* GetRoot() const = 0;
	virtual MElem* GetInhRoot() const = 0;
	virtual void GetCont(string& aCont, const string& aName=string()) = 0; 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()) = 0; 
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse) = 0;
	virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue) = 0;
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue) = 0;
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MElem* aTop = NULL) const = 0;
	virtual void GetRUri(GUri& aUri, MElem* aTop = NULL) = 0;
	virtual string GetUri(MElem* aTop = NULL, TBool aShort = EFalse) = 0;
	virtual string GetRUri(MElem* aTop = NULL) = 0;
	virtual TBool RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes) = 0;
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes) = 0;
	virtual MElem* GetAowner() = 0;
	virtual const MElem* GetAowner() const = 0;
	virtual MElem* GetAcompOwning(MElem* aComp) = 0;
	virtual MElem* GetUpperAowner() = 0;
	virtual TBool IsRemoved() const = 0;
	virtual void SetRemoved() = 0;
	virtual MElem* GetAttachingMgr() = 0;
	virtual const MElem* GetAttachingMgr() const = 0;
	virtual TBool IsAownerOf(const MElem* aElem) const = 0;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal) = 0;
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse) = 0;
	virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const = 0;
	virtual void GetLRank(Rank& aRank, TBool aCur = EFalse) const = 0; 
	// Gets the comp with given type and owning given element
	virtual MElem* GetCompOwning(const string& aParent, MElem* aElem) = 0;
	virtual MElem* GetCompOwning(MElem* aElem) = 0;
	virtual TBool IsInheritedComp(const MElem* aNode) const = 0;
	virtual TBool HasInherDeps(const MElem* aScope) const = 0;
	// Debugging
	virtual TInt GetCapacity() const = 0;
};

#endif
