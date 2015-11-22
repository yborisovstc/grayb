#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include <set>

class ChromoNode;
//class Base;
class Elem;


// Ref to mutation: [model node, mut handle]
typedef pair<Elem*, void*> TMutRef;
// Deps Mutations on RT model node
typedef pair<TMutRef, TNodeAttr> TMDep;

class MCompsObserver
{
    public:
	static const char* Type() { return "MCompsObserver";};
	virtual void OnCompDeleting(Elem& aComp, TBool aSoft = ETrue) = 0;
	virtual void OnCompAdding(Elem& aComp) = 0;
	virtual TBool OnCompChanged(Elem& aComp) = 0;
	// For run-time only. Use OnCompChanged when the content is changed via mutation
	virtual TBool OnContentChanged(Elem& aComp) = 0;
	virtual TBool OnCompRenamed(Elem& aComp, const string& aOldName) = 0;
};

class MChildsObserver
{
    public: 
	virtual void OnChildDeleting(Elem* aChild) = 0;
	virtual TBool OnChildRenamed(Elem* aComp, const string& aOldName) = 0;
};

class MChild
{
    public:
	virtual void OnParentDeleting(Elem* aParent) = 0;
};

// Agent - comps observer
class MACompsObserver
{
    public:
	static const char* Type() { return "MACompsObserver";};
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp) = 0;
};

// TODO [YB] Obsolete?
// Element of native graph hier
class MElem
{
    public:
	virtual const string& EType() const = 0;
	virtual const set<string>& CompsTypes() = 0;
//	virtual Base* EBase() = 0;
//	virtual const Base* EBase() const = 0;
	virtual MElem* GetMan() = 0;
	virtual MElem* GetNode(const string& aUri) = 0;
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase) = 0;
	virtual Elem* GetNodeLoc(const GUri::TElem& aElem) = 0;
	virtual void GetCont(string& aCont, const string& aName=string()) = 0; 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue) = 0; 
	virtual TBool AddNode(const ChromoNode& aSpec) = 0;
	virtual TBool MoveNode(const ChromoNode& aSpec) = 0;
};

#endif
