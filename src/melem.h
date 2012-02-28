#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include <set>

class ChromoNode;
class Base;

// TODO [YB] Obsolete?
// Element of native graph hier
class MElem
{
    public:
	virtual const string& EType() const = 0;
	virtual const set<string>& CompsTypes() = 0;
	virtual Base* EBase() = 0;
	virtual const Base* EBase() const = 0;
	virtual MElem* GetNode(const string& aUri) = 0;
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase) = 0;
	virtual TBool ChangeCont(const string& aVal) = 0; 
	virtual TBool AddNode(const ChromoNode& aSpec) = 0;
	virtual void OnCompDeleting(const MElem& aComp) = 0;
	virtual void OnCompAdding(const MElem& aComp) = 0;
	virtual void OnCompChanged(const MElem& aComp) = 0;
};

#endif
