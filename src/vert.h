#ifndef __GRAYB_VERT_H
#define __GRAYB_VERT_H

#include "mvert.h"
#include "elem.h"
#include <set>

class Vert: public Elem, public MVert
{
    public:
	static const char* Type() { return "Vert";};
	Vert(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MVert
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual set<MVert*>& Pairs();
	virtual Base* EBase();
	// From Elem
	virtual Elem* Clone(const string& aName, Elem* aMan, MEnv* aEnv) const;
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
    protected:
	set<MVert*> iPairs;
};

#endif
