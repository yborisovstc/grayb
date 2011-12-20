#ifndef __GRAYB_MELEM_H
#define __GRAYB_MELEM_H

#include "plat.h"
#include "guri.h"
#include <set>

class ChromoNode;

// Element of native graph hier
class MElem
{
    public:
	MElem* GetNode(const GUri& aUri) { return GetNode(aUri, aUri.Elems().begin()); }
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter aPathBase) = 0;
	virtual TBool AddNode(const ChromoNode& aSpec) = 0;
	virtual const set<TNodeType>& CompsTypes() = 0;
};

#endif
